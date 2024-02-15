#ifndef COREINJECT_NEEDSROOT_HPP
#define COREINJECT_NEEDSROOT_HPP

namespace CoreInject {

	enum class PtraceScope {
		UNRESTRICTED,
		CHILD_PROCESS_ONLY,
		ROOT_ONLY,
		DISABLED
	};

	PtraceScope getPtraceScope();

	// Will return false if ptrace_scope is UNRESTRICTED
	bool needsRoot();

}

#endif
