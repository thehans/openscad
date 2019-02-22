#include "printutils.h"
#include <sstream>
#include <stdio.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include "exceptions.h"

namespace fs = boost::filesystem;

std::list<std::string> print_messages_stack;
OutputHandlerFunc *outputhandler = nullptr;
void *outputhandler_data = nullptr;
std::string OpenSCAD::debug("");
bool OpenSCAD::quiet = false;
bool OpenSCAD::hardwarnings = false;
bool OpenSCAD::parameterCheck = true;
bool OpenSCAD::rangeCheck = false;

std::string lastmessage;
int lastmessage_count = 0;

void set_output_handler(OutputHandlerFunc *newhandler, void *userdata)
{
	outputhandler = newhandler;
	outputhandler_data = userdata;
}

void PRINT(const std::string &msg)
{
	PRINT_NOCACHE(msg);
}

void PRINT_NOCACHE(const std::string &msg)
{
	if (msg.empty()) return;

	if (boost::starts_with(msg, "WARNING") || boost::starts_with(msg, "ERROR") || boost::starts_with(msg, "TRACE")) {
		if (lastmessage == msg) {
			// Suppress output after 5 equal ERROR or WARNING outputs.
			if (lastmessage_count == 5) return; 
			++lastmessage_count;
		} else {
			lastmessage_count = 1;
			lastmessage = msg;
		}
	}

	if (!OpenSCAD::quiet || boost::starts_with(msg, "ERROR")) {
		if (!outputhandler) {
			fprintf(stderr, "%s\n", msg.c_str());
		} else {
			outputhandler(msg, outputhandler_data);
		}
	}
	if(OpenSCAD::hardwarnings && !std::current_exception() && boost::starts_with(msg, "WARNING")){
		throw HardWarningException(msg);
	}
}

void PRINTDEBUG(const std::string &filename, const std::string &msg)
{
	// see printutils.h for usage instructions
	if (OpenSCAD::debug=="") return;
	std::string shortfname = fs::path(filename).stem().generic_string();
	std::string lowshortfname(shortfname);
	boost::algorithm::to_lower(lowshortfname);
	std::string lowdebug(OpenSCAD::debug);
	boost::algorithm::to_lower(lowdebug);
	if (OpenSCAD::debug=="all" ||
			lowdebug.find(lowshortfname) != std::string::npos) {
		PRINT_NOCACHE( shortfname+": "+ msg );
	}
}

std::string two_digit_exp_format( std::string doublestr )
{
#ifdef _WIN32
	size_t exppos = doublestr.find('e');
	if ( exppos != std::string::npos) {
		exppos += 2;
		if ( doublestr[exppos] == '0' ) doublestr.erase(exppos,1);
	}
#endif
	return doublestr;
}

std::string two_digit_exp_format(double x)
{
	return two_digit_exp_format(std::to_string(x));
}

#include <set>

std::set<std::string> printedDeprecations;

void printDeprecation(const std::string &str)
{
	if (printedDeprecations.find(str) == printedDeprecations.end()) {
		printedDeprecations.insert(str);
		std::string msg = "DEPRECATED: " + str;
		PRINT(msg);
	}
}

void resetSuppressedMessages()
{
	printedDeprecations.clear();
	lastmessage = "";
	lastmessage_count = 0;
}
