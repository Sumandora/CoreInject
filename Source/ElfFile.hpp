#ifndef COREINJECT_ELFFILE_HPP
#define COREINJECT_ELFFILE_HPP

#include <cstdint>
#include <filesystem>

namespace CoreInject {
	class ElfFile {
		std::filesystem::path path;

	public:
		explicit ElfFile(std::filesystem::path path);

		using Architecture = uint8_t;
		[[nodiscard]] Architecture getArchitecture() const;
	};
}

#endif
