#include "ElfFile.hpp"

#include <fstream>

using namespace CoreInject;

ElfFile::ElfFile(std::filesystem::path path)
	: path(std::move(path))
{
}

ElfFile::Architecture ElfFile::getArchitecture() const
{
	std::fstream exe{ path, std::fstream::in | std::fstream::binary };
	exe.ignore(4);
	Architecture arch;
	exe.read(reinterpret_cast<char*>(&arch), sizeof(arch));
	exe.close();
	return arch;
}