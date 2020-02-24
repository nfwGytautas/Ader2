#include "File.h"

// ifstream
#include <fstream>

// Logging
#include "Utility/Log.h"

// STB image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// OpenAL includes
#include <AL/al.h>

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

ImageFileContents::~ImageFileContents()
{
	// Check if the image has a valid buffer and then free it
	if (Buffer)
	{
		stbi_image_free(Buffer);
	}
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

Memory::reference<WaveFileContents> readAudio(const std::string& path)
{
	// Result audio
	Memory::reference<WaveFileContents> result = new WaveFileContents();

	// Some helper variables
	int error;
	FILE* filePtr;
	unsigned int count;

	// Open the wave file in binary.
	error = fopen_s(&filePtr, path.c_str(), "rb");
	if (error != 0)
	{
		LOG_ERROR("Can't open wave file '{0}'!", path);
		return nullptr;
	}

	// Read in the wave file header.
	count = fread(
		&result->Header, 
		sizeof(WaveFileContents::WaveFileHeader), 
		1, 
		filePtr);
	if (count != 1)
	{
		LOG_ERROR("The file '{0}' is corrupted or not valid .wav file!", path);
		return nullptr;
	}

	// Reserve data
	result->Data.reserve(result->Header.DataSize);

	// Seek to the start of the contents
	fseek(filePtr, sizeof(WaveFileContents::WaveFileHeader), SEEK_SET);

	// Read buffer
	count = fread(
		result->Data.data(), 
		1, // unsigned char
		result->Header.DataSize, 
		filePtr);

	// Close the file
	fclose(filePtr);

	// Something went wrong
	if (count != result->Header.DataSize)
	{
		LOG_ERROR("The file '{0}' contents are corrupted read {1} bytes, expected {2}!", 
			path, count, result->Header.DataSize);
		return nullptr;
	}

	// Configure numeric format
	if (result->Header.NumChannels == 1)
	{
		result->Format = result->Header.BitsPerSample == 8 ? 
			AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
	}
	else {
		result->Format = result->Header.BitsPerSample == 8 ?
			AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
	}

	// Return audio data
	return result;
}