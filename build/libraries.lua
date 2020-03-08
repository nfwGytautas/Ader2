-- Root of all third party libraries used
lib_root = "../libraries"

-- Library table
local libraries = {}

-- Mono library variables
lib_mono = {}
lib_mono["include"] = lib_root .. "/mono/include/mono-2.0"
lib_mono["lib"] = lib_root .. "/mono/lib"
lib_mono["dll"] = "mono-2.0-sgen.lib"
lib_mono["copy_dll"] = "../" .. lib_mono["lib"] .. "/mono-2.0-sgen.dll"
lib_mono["copy_etc"] = "../" .. lib_root .. "/mono/etc"
lib_mono["copy_lib"] = "../" .. lib_root .. "/mono/lib"

-- spdlog library variables
lib_spdlog = {}
lib_spdlog["include"] = lib_root .. "/spdlog/include"
lib_spdlog["lib"] = lib_root .. "/spdlog/lib"
lib_spdlog["dll"] = "r_spdlog.lib"

-- GLFW library variables
lib_GLFW = {}
lib_GLFW["include"] = lib_root .. "/GLFW/include"
lib_GLFW["lib"] = lib_root .. "/GLFW/lib"
lib_GLFW["dll"] = "glfw3.lib"

-- Glad library variables
lib_Glad = {}
lib_Glad["include"] = lib_root .. "/Glad/include"
lib_Glad["lib"] = lib_root .. "/Glad/lib"
lib_Glad["dll"] = "Glad.lib"

-- GLM library variables
lib_GLM = {}
lib_GLM["include"] = lib_root .. "/glm"

-- stbImage library variables
lib_stbimage = {}
lib_stbimage["include"] = lib_root .. "/stbimage"

-- OpenAL library variables
lib_OpenAL = {}
lib_OpenAL["include"] = lib_root .. "/OpenAL/include"
lib_OpenAL["lib"] = lib_root .. "/OpenAL/lib"
lib_OpenAL["dll"] = "OpenAL32.lib"
lib_OpenAL["copy_dll"] = "../" .. lib_OpenAL["lib"] .. "/OpenAL32.dll"

-- FreeType library variables
lib_FreeType = {}
lib_FreeType["include"] = lib_root .. "/FreeType/include"
lib_FreeType["lib"] = lib_root .. "/FreeType/win64"
lib_FreeType["dll"] = "freetype.lib"
lib_FreeType["copy_dll"] = "../" .. lib_FreeType["lib"] .. "/freetype.dll"

-- Add library tables to libraries
libraries["mono"] = lib_mono
libraries["spdlog"] = lib_spdlog
libraries["GLFW"] = lib_GLFW
libraries["Glad"] = lib_Glad
libraries["glm"] = lib_GLM
libraries["stb_image"] = lib_stbimage
libraries["OpenAL"] = lib_OpenAL
libraries["FreeType"] = lib_FreeType

return libraries
