#pragma once

// For string
#include <string>

// For data buffers
#include <vector>

// Reference
#include "CommonTypes/reference.h"

/**
 * @file 
 * Utility file containing methods for reading and writing files
 */ 

/**
 * Reads entire file contents and returns it as a c++ string
 */
std::string readFile(const std::string& path);

/**
 * Struct containing the loaded image file data
 */
class ImageFileContents
{
public:
    /**
     * Free buffer memory
     */
    ~ImageFileContents();

    int Width = 0;
    int Height = 0;
    int BPP = 0;
    unsigned char* Buffer = nullptr;
};

/**
 * Reads image data from the specified file
 *
 * @param path Path to the image file
 *
 * @return Reference to ImageFileContents
 */
Memory::reference<ImageFileContents> readImage(const std::string& path);

/**
 * Struct representing a wave audio file contents
 */
struct WaveFileContents
{
    /**
     * Struct representing a wave audio file header
     */
    struct WaveFileHeader
    {
        char ChunkId[4];
        unsigned long ChunkSize;
        char FormatTag[4];
        char SubChunkId[4];
        unsigned long SubChunkSize;
        unsigned short AudioFormat;
        unsigned short NumChannels;
        unsigned long SampleRate;
        unsigned long BytesPerSecond;
        unsigned short BlockAlign;
        unsigned short BitsPerSample;
        char DataChunkId[4];
        unsigned long DataSize;
    } Header;

    /// Numeric format representation
    unsigned int Format;

    /// Actual data buffer
    std::vector<unsigned char> Data;
};

/**
 * Reads wave audio file data from the specified file
 *
 * @param path Path to the audio file
 *
 * @return Reference to WaveFileContents
 */
Memory::reference<WaveFileContents> readAudio(const std::string& path);
