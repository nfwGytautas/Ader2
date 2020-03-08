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

#include <ft2build.h>
#include FT_FREETYPE_H

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

Memory::reference<FontFileContents> readFont(const std::string& path)
{
	FT_Library  library;   // Handle to library
	FT_Face     face;      // Handle to face object
	FT_Error	error;

	Memory::reference<FontFileContents> result = new FontFileContents();

	// Default values
	result->PixelWidth = 0;
	result->PixelHeight = 32;
	result->Width = 128;
	result->Height = 1;

	// Init FreeType
	error = FT_Init_FreeType(&library);
	if (error)
	{
		LOG_ERROR("FreeType failed to initialize");
		FT_Done_FreeType(library);
		return nullptr;
	}

	// Create new face from the specified path
	error = FT_New_Face(library,
		path.c_str(),
		0,
		&face);

	// Check for errors
	if (error == FT_Err_Unknown_File_Format)
	{
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		LOG_ERROR("The font file could be opened and read, but it appears that its font format is unsupported");
		return nullptr;
	}
	else if (error)
	{
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		LOG_ERROR("The font file could not be opened and read");
		return nullptr;
	}

	// Set the size of generated glyph data, resolution could take a parameter but for now it's
	// enough
	error = FT_Set_Char_Size(
		face,							// Handle to face object
		result->PixelWidth,				// char_width in 1/64th of points
		result->PixelHeight * 64,		// char_height in 1/64th of points
		96,								// Horizontal device resolution
		96);							// Vertical device resolution

	// If any of these values were 0 than the FreeType library set them to the one that isn't 0
	if (result->PixelWidth == 0)
	{
		result->PixelWidth = result->PixelHeight;
	}
	else if (result->PixelHeight == 0)
	{
		result->PixelHeight = result->PixelWidth;
	}

	// Additional variables for reserving buffer data and getting width and height correctly
	int max_dim = (1 + (face->size->metrics.height >> 6))* ceilf(sqrtf(128));
	int tex_width = 1;
	while (tex_width < max_dim) tex_width <<= 1;
	int tex_height = tex_width;

	// Width and height
	result->Width = tex_width;
	result->Height = tex_height;

	// Texture buffer
	result->Buffer = std::vector<unsigned char>(tex_width * tex_height, 0);

	// More variables to help generate text
	int off = 0;
	int pen_x = 0, pen_y = 0;

	for (int i = 0; i < 128; ++i)
	{
		result->Metrics[i] = {};

		FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
		FT_Bitmap* bmp = &face->glyph->bitmap;

		if (pen_x + bmp->width >= result->Width) {
			pen_x = 0;
			pen_y += ((face->size->metrics.height >> 6) + 1);
		}

		for (int row = 0; row < bmp->rows; ++row) {
			for (int col = 0; col < bmp->width; ++col) {
				int x = pen_x + col;
				int y = pen_y + row;
				result->Buffer[y * result->Width + x] = bmp->buffer[row * bmp->pitch + col];
			}
		}

		result->Metrics[i].Start.x = pen_x;
		result->Metrics[i].Start.y = pen_y;
		result->Metrics[i].End.x = pen_x + bmp->width;
		result->Metrics[i].End.y = pen_y + bmp->rows;

		result->Metrics[i].Offset.x = face->glyph->bitmap_left;
		result->Metrics[i].Offset.y = face->glyph->bitmap_top;
		result->Metrics[i].Advance = face->glyph->advance.x >> 6;

		pen_x += bmp->width + 1;
	}

	// Clean up FreeType
	FT_Done_Face(face);
	FT_Done_FreeType(library);

	// Return the result
	return result;
}
