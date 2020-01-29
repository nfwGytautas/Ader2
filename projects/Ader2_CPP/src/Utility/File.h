#pragma once

// For string
#include <string>

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
