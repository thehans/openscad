#pragma once

#include <string>
#include <map>
#include <list>
#include "NodeVisitor.h"
#include "node.h"
#include "nodecache.h"

class NodeDumper : public NodeVisitor
{
public:
        /*! If idPrefix is true, we will output "n<id>:" in front of each node,
          which is useful for debugging. */
        NodeDumper(NodeCache &cache, const std::string& indent = "", bool idPrefix = false) :
                cache(cache), indent(indent), idprefix(idPrefix), currindent(0), root(nullptr) { 
                cache.set_root_stream(this->dump);
        }
        ~NodeDumper() {}

        Response visit(State &state, const AbstractNode &node) override;
        Response visit(State &state, const RootNode &node) override;

private:
        bool isCached(const AbstractNode &node) const;

        NodeCache &cache;
        std::string indent;
        bool idprefix;

        int currindent;
        const AbstractNode *root;
        std::shared_ptr<std::stringstream> dump;

};
