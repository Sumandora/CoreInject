#include "CoreInject/NeedsRoot.hpp"

#include <fstream>
#include <unistd.h>

CoreInject::PtraceScope CoreInject::getPtraceScope()
{
	auto fs = std::fstream{ "/proc/sys/kernel/yama/ptrace_scope", std::fstream::in };
	char c;
	fs >> c;
	fs.close();
	return static_cast<PtraceScope>(c - '0');
}

bool CoreInject::needsRoot()
{
	bool isRoot = getuid() == 0;
	bool needsRoot = getPtraceScope() > PtraceScope::UNRESTRICTED; // CHILD_PROCESS_ONLY requires root in practice, because we don't inject into ourselves
	return !isRoot && needsRoot;
}