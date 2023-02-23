-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

rule "CompileShaders"
    location "Rules"
    display "GLSL Shader Compiler"
    fileextension
    {
        ".frag",
        ".vert",
        ".comp"
    }
    
    buildmessage "Compiling %(Filename) with GLSL-SPV"
    buildcommands "glslangValidator -G -o $(OutDir)/%(Identity).spv %(Identity)"
    buildoutputs "$(OutDir)/%(Identity).spv"

workspace "Avalanche"
    architecture "x64"
    startproject "Avalanche"
    
    configurations
    {
        "Debug",
        "Release",
        "Dist",
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
    debugdir ("Binaries/" .. outputdir .. "/Avalanche")

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Source"] = "Avalanche/Source"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["GLFW"] = "Avalanche/ThirdParty/glfw/include"
IncludeDir["GLM"] = "Avalanche/ThirdParty/glm"
IncludeDir["SPDLOG"] = "Avalanche/ThirdParty/spdlog/include"
IncludeDir["IMGUI"] = "Avalanche/ThirdParty/imgui"
IncludeDir["STB_IMAGE"] = "Avalanche/ThirdParty/stb"
IncludeDir["TINYOBJLOADER"] = "Avalanche/ThirdParty/tinyobjloader"
IncludeDir["ENTT"] = "Avalanche/ThirdParty/entt"
IncludeDir["VKBOOTSTRAP"] = "Avalanche/ThirdParty/vk-bootstrap/src"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Libraries = {}
Libraries["OpenGL"] = "opengl32.lib"
Libraries["Vulkan"] = "vulkan-1.lib"

project "Avalanche"
    kind "ConsoleApp"
    location "Avalanche"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/Shaders/**.vert",
        "%{prj.name}/Shaders/**.frag",
        "%{prj.name}/Shaders/**.comp",
        
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.cpp",

        "%{prj.name}/ThirdParty/glm/glm/**.hpp",
        "%{prj.name}/ThirdParty/glm/glm/**.inl",
    }
    
    includedirs
    {
        "%{IncludeDir.Source}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.SPDLOG}",
        -- "%{IncludeDir.IMGUI}",
        "%{IncludeDir.VKBOOTSTRAP}",
    }
    
    links
    {
        "Shcore.lib",
        "%{Libraries.Vulkan}",
        "GLFW",
        -- "ImGui",
        "VkBootstrap",
    }

    ignoredefaultlibraries
    {
        "LIBCMTD",
    }

    libdirs
    {
        "%{LibraryDir.VulkanSDK}",
    }
    
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }

    postbuildcommands
    {
        "{COPYDIR} %{wks.location}Avalanche/Content %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}/Content",
        --"{COPYDIR} %{wks.location}Binaries/" .. outputdir .. "/Avalanche/Shaders %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}/Shaders"
    }
    
    filter "system:windows"
        systemversion "latest"
        rules { "CompileShaders" }

        defines
        {
            "_PLATFORM_WINDOWS"
        }

    
    filter "configurations:Debug"
        defines "_DEBUG"
        runtime "Debug"
        symbols "on"
    
    filter "configurations:Release"
        defines "_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "_DIST"
        runtime "Release"
        optimize "on"

-- include "Avalanche/ThirdParty/imgui"
include "Avalanche/ThirdParty/glfw"
include "Avalanche/ThirdParty/vk-bootstrap"