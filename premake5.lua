workspace "Elit3d"
	configurations { "Debug", "Release", "DistPortable", "DistInstall" }
	platforms { "x86", "x64" }
	
project "Elit3d"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	targetdir "Elit3d/bin/%{cfg.architecture}/%{cfg.buildcfg}"
	location "Elit3d"
	
	characterset "MBCS"
	
	flags { "MultiProcessorCompile" }
	
	staticruntime "on"
	
	includedirs	{
		"%{wks.location}/Elit3d/src/ExternalTools/GLEW/include",
		"%{wks.location}/Elit3d/src/ExternalTools/SDL2/include",
		"%{wks.location}/Elit3d/src/ExternalTools/DevIL/include",
		"%{wks.location}/Elit3d/src/ExternalTools/efsw/src",
		"%{wks.location}/Elit3d/src"
	}
	
	links {
		"SDL2.lib",
		"SDL2main.lib",
		"SDL2test.lib",
		"glew32.lib",
		"glew32s.lib",
		"opengl32.lib",
		"assimp.lib",
		"DevIL.lib",
		"ILU.lib",
		"ILUT.lib",
		"gdi32.lib",
		"version.lib",
		"Ole32.lib",
		"OleAut32.lib",
		"wbemuuid.lib",
		"Advapi32.lib",
		"efsw.lib"
	}

	files { "**.hpp", "**.cpp", "**.h", "**.c", "**.rc", "**.ico" }
	excludes { 
		"%{wks.location}/Elit3d/src/ExternalTools/ImGui/misc/**.cpp", 
		"%{wks.location}/Elit3d/src/ExternalTools/ImGui/misc/**.h",
		"%{wks.location}/Elit3d/src/ExternalTools/DevIL/include/IL/luadevil.c",
		"**/example/**"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	  
	filter "configurations:DistPortable"
		defines { "NDEBUG", "DISTP" }
		optimize "On"
	filter "configurations:DistInstall"
		defines { "NDEBUG", "DISTI" }
		optimize "On"
	  
	filter "platforms:x86"
		system "Windows"
		architecture "x86"
		libdirs {
			"%{wks.location}/Elit3d/src/ExternalTools/SDL2/libx86",
			"%{wks.location}/Elit3d/src/ExternalTools/GLEW/libx86",
			"%{wks.location}/Elit3d/src/ExternalTools/Assimp/libx86",
			"%{wks.location}/Elit3d/src/ExternalTools/DevIL/libx86",
			"%{wks.location}/Elit3d/src/ExternalTools/efsw/libx86"
		}
		
		postbuildcommands { 
			"{COPY} src/ExternalTools/SDL2/libx86/SDL2.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/Assimp/libx86/assimp.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx86/DevIL.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx86/ILU.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx86/ILUT.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/GLEW/libx86/glew32.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/efsw/libx86/efsw.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}"
			}
	
	filter "platforms:x64"
		system "Windows"
		architecture "x64"
		libdirs {
			"%{wks.location}/Elit3d/src/ExternalTools/GLEW/libx64",
			"%{wks.location}/Elit3d/src/ExternalTools/SDL2/libx64",
			"%{wks.location}/Elit3d/src/ExternalTools/Assimp/libx64",
			"%{wks.location}/Elit3d/src/ExternalTools/DevIL/libx64",
			"%{wks.location}/Elit3d/src/ExternalTools/efsw/libx64"
		}
		
		postbuildcommands { 
			"{COPY} src/ExternalTools/SDL2/libx64/SDL2.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/Assimp/libx64/assimp.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx64/DevIL.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx64/ILU.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/DevIL/libx64/ILUT.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/GLEW/libx64/glew32.dll 	bin/%{cfg.architecture}/%{cfg.buildcfg}",
			"{COPY} src/ExternalTools/efsw/libx64/efsw.dll 		bin/%{cfg.architecture}/%{cfg.buildcfg}"
		}
	