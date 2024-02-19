#include "ElfFile.hpp"

#include <cstring>
#include <fstream>

using namespace CoreInject;

ElfFile::ElfFile(std::filesystem::path path)
	: path(std::move(path))
	, header()
	, architecture()
{
	std::fstream exe{ this->path, std::fstream::in | std::fstream::binary };
	exe.read(reinterpret_cast<char*>(this->header), SELFMAG);
	exe.read(reinterpret_cast<char*>(&this->architecture), sizeof(this->architecture));
	exe.close();
}

bool ElfFile::isHeaderValid() const
{
	return std::strncmp(header, ELFMAG, SELFMAG) == 0;
}

ElfFile::Architecture ElfFile::getArchitecture() const
{
	return architecture;
}