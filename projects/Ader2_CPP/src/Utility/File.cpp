#include "File.h"

// ifstream
#include <fstream>

std::string readFile(const std::string& path)
{
	std::string result;

	// Open file
	std::ifstream file(path);

	// Get size of the file
	std::streampos fileSize;

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Resize the string and copy file contents
	result.resize(fileSize);
	file.read(result.data(), fileSize);

	// Return as string
	return result;
}
