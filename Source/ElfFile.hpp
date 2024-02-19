#ifndef COREINJECT_ELFFILE_HPP
#define COREINJECT_ELFFILE_HPP

#include <cstdint>
#include <filesystem>
#include <elf.h>

namespace CoreInject {
	class ElfFile {
	public:
		using Architecture = uint8_t;

	private:
		std::filesystem::path path;

		char header[SELFMAG];
		Architecture architecture;

	public:

		explicit ElfFile(std::filesystem::path path);
		[[nodiscard]] bool isHeaderValid() const;
		[[nodiscard]] Architecture getArchitecture() const;
	};
}

#endif
