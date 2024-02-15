#include "Process.hpp"

#include <fstream>
#include <functional>
#include <glob.h>
#include <iostream>
#include <sstream>

using namespace CoreInject;

std::filesystem::path CoreInject::procfs{ "/proc" };
std::filesystem::path CoreInject::selfMount = std::filesystem::read_symlink(procfs / "self" / "ns" / "mnt");

Process::Process(std::size_t pid)
	: pid(pid)
{
}

bool Process::hasModule(const std::string& moduleName) const
{
	std::fstream maps{ getProcFS() / "maps", std::fstream::in };

	for(std::string line; std::getline(maps, line);) {
		std::stringstream ss{line};
		ss.ignore(std::numeric_limits<std::streamsize>::max(), '/');

		if(!ss.eof()) {
			std::string name = "/";
			std::string token;
			while (!ss.eof()) {
				ss >> token;
				if (token == "(deleted)")
					break;
				name += token + " ";
			}

			if (name.substr(0, name.length() - 1 /* trailing space */).ends_with(moduleName)) {
				maps.close();
				return true;
			}
		}
	}

	maps.close();
	return false;
}

std::filesystem::path Process::getProcFS() const
{
	return procfs / std::to_string(pid);
}

bool Process::isSandboxed() const
{
	return std::filesystem::read_symlink(getProcFS() / "ns" / "mnt") != selfMount;
}

ElfFile Process::getExecutable() const {
	return ElfFile{ getProcFS() / "exe" };
}

std::filesystem::path Process::intoRoot(std::filesystem::path path) const {
	if(path.string()[0] == '/')
		path = path.string().substr(1);
	return getProcFS() / "root" / path;
}

std::filesystem::path Process::getCwd() const {
	return std::filesystem::read_symlink(getProcFS() / "cwd");
}

size_t Process::getId() const
{
	return pid;
}
