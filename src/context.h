#pragma once

#include "Assignment.h"
#include "contextframe.h"

/**
 * Local handle to a all context objects. This is used to maintain the
 * dynamic scoping stack using object lifetime.
 * The Context objects can hang around for longer, e.g. in case of closures.
 */
template<typename T>
class ContextHandle : ContextFrameHandle
{
public:
	ContextHandle(std::shared_ptr<T>&& context):
		ContextFrameHandle(context.get()),
		context(std::move(context))
	{
		this->context->init();
	}
	
	ContextHandle(const ContextHandle&) = delete;
	ContextHandle& operator=(const ContextHandle&) = delete;
	ContextHandle(ContextHandle&&) = default;
	ContextHandle& operator=(ContextHandle&&) = delete;
	
	// Valid only if context is on the top of the stack.
	ContextHandle& operator=(std::shared_ptr<T>&& other)
	{
		context = std::move(other);
		ContextFrameHandle::operator=(context.get());
		return *this;
	}
	
	const T* operator->() const { return context.get(); }
	T* operator->() { return context.get(); }
	const std::shared_ptr<T>& operator*() const { return context; }

private:
	std::shared_ptr<T> context;
};

class Context : public ContextFrame, public std::enable_shared_from_this<Context>
{
protected:
	Context(EvaluationSession* session);
	Context(const std::shared_ptr<Context>& parent);

public:
    template<typename C, typename ... T>
    static ContextHandle<C> create(T&& ... t) {
        return ContextHandle<C>{std::shared_ptr<C>(new C(std::forward<T>(t)...))};
    }

	virtual void init() { }

	std::shared_ptr<Context> get_shared_ptr() const { return const_cast<Context*>(this)->shared_from_this(); }
	virtual const class Children* user_module_children() const;

	boost::optional<const Value&> try_lookup_variable(const std::string &name) const;
	const Value& lookup_variable(const std::string &name, const Location &loc) const;
	boost::optional<CallableFunction> lookup_function(const std::string &name, const Location &loc) const;
	boost::optional<InstantiableModule> lookup_module(const std::string &name, const Location &loc) const;

	const std::shared_ptr<Context> &getParent() const { return this->parent; }
	// This modifies the semantics of the context in an error-prone way. Use with caution.
	void setParent(const std::shared_ptr<Context>& parent) { this->parent = parent; }

protected:
	std::shared_ptr<Context> parent;

public:
#ifdef DEBUG
	std::string dump();
#endif
};
