newoption {
	trigger		= "with-mk",
	description	= "Enable the Minko MK plugin."
}

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	kind "StaticLib"
	language "C++"

	files {
		"lib/**.hpp",
		"lib/**.h",
		"lib/**.cpp",
		"lib/**.c",
		"src/**.cpp",
		"src/**.hpp",
		"include/**.hpp"
	}

	excludes {
		"lib/msgpack-c/test/**.cpp",
		"lib/msgpack-c/test/**.c"
	}

	includedirs {
		"include",
		"src",
		"lib/msgpack-c/src"
	}

	configuration { "win" }
		-- msgpack
		defines {
			"_LIB",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			"__STDC_VERSION__=199901L",
			"__STDC__",
			"WIN32"
		}
		buildoptions {
			"/wd4028",
			"/wd4244",
			"/wd4267",
			"/wd4996",
			"/wd4273",
			"/wd4503"
		}

	configuration { "linux or macosx" }
		buildoptions {
			"-Wno-deprecated-declarations"
		}