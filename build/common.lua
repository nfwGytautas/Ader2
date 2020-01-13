-- Folder where to store the projects relative to the executing script
project_root = "../projects"

-- Root folder where all code will be built to
out_root = "../out"

-- Output folder name
out_cfg_folder = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Output directory
out_dir = out_root .. "/bin/" .. out_cfg_folder

-- Intermediate object directory
out_dir_intermediate = out_root .. "/bin-int/" .. out_cfg_folder

-- Import libraries
local libraries = require "libraries"

local engine_paths = 
{
	project = project_root,

	out_target = out_dir,
	out_object = out_dir_intermediate,

	lib = libraries,
}

return engine_paths
