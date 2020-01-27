#pragma once

// For string
#include <string>

/**
 * @file 
 * Utility file containing methods for reading and writing files
 */ 

/**
 * Reads entire file contents and returns it as a c++ string
 */
std::string readFile(const std::string& path);