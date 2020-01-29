#include "File.h"

// ifstream
#include <fstream>

// Logging
#include "Utility/Log.h"

// STB image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

Memory::reference<ImageFileContents> readImage(const std::string& path)
{
	// Result image
	Memory::reference<ImageFileContents> result = new ImageFileContents();

	// Flip the image
	stbi_set_flip_vertically_on_load(true);

	// Load the image
	result->Buffer = stbi_load(path.c_str(), &result->Width, &result->Height, &result->BPP, 0);

	// Check if load was successful
	if (!result->Buffer)
	{
		// The loading has failed
		LOG_ERROR("Failed to load '{0}', error:\n {1}", path, stbi_failure_reason());
		return nullptr;
	}

	// Return the image
	return result;
}

ImageFileContents::~ImageFileContents()
{
	// Check if the image has a valid buffer and then free it
	if (Buffer)
	{
		stbi_image_free(Buffer);
	}
}
