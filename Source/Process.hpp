#ifndef COREINJECT_PROCESS_HPP
#define COREINJECT_PROCESS_HPP

#include "ElfFile.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

namespace CoreInject {
	extern std::filesystem::path selfMount;
	extern std::filesystem::path procfs;

	class Process {
	private:
		std::size_t pid;

	public:
		explicit Process(std::size_t pid);

		[[nodiscard]] std::filesystem::path getProcFS() const;

		[[nodiscard]] bool hasModule(const std::string& moduleName) const;
		[[nodiscard]] bool isSandboxed() const;
		[[nodiscard]] ElfFile getExecutable() const;
		[[nodiscard]] std::filesystem::path intoRoot(std::filesystem::path path) const;
		[[nodiscard]] std::filesystem::path getCwd() const;
		[[nodiscard]] size_t getId() const;
	};

}

#endif
