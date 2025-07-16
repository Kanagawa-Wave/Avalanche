-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

rule "CompileGLSLShaders"
    location "Rules"
    display "GLSLC Shader Compiler"
    fileextension
    {
        ".frag",
        ".vert",
        ".comp"
    }
    
    buildmessage "Compiling %(Filename) (GLSL->SPIRV)"
    buildcommands "%{VULKAN_SDK}/Bin/glslangValidator.exe -V -o $(OutDir)%(Identity).spv %(Identity)"
    buildoutputs "$(OutDir)/%(Identity).spv"
    
rule "CompileHLSLShaders"
    location "Rules"
    display "DXC Shader Compiler"
    fileextension
    {
        ".hlsl",
    }
    
    buildmessage "Compiling %(Filename) (HLSL-SPIRV)"
    buildcommands "%{VULKAN_SDK}/Bin/dxc.exe -spirv -T [ShaderType] -E main %(Identity) -Fo $(OutDir)%(Identity).spv"
    buildoutputs "$(OutDir)/%(Identity).spv"
    
    propertydefinition {
        name = "ShaderType",
        kind = "string",
        display = "ShaderType Target",
    }


workspace "Avalanche"
    architecture "x64"
    startproject "Editor"
    
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
    
    debugdir ("Binaries/" .. outputdir .. "/Editor")

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Source"] = "Avalanche/Source"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["GLFW"] = "Avalanche/ThirdParty/glfw/include"
IncludeDir["GLM"] = "Avalanche/ThirdParty/glm"
IncludeDir["SPDLOG"] = "Avalanche/ThirdParty/spdlog/include"
IncludeDir["IMGUI"] = "Avalanche/ThirdParty/imgui"
IncludeDir["STB"] = "Avalanche/ThirdParty/stb"
IncludeDir["TINYOBJLOADER"] = "Avalanche/ThirdParty/tinyobjloader"
IncludeDir["ENTT"] = "Avalanche/ThirdParty/entt/single_include"
IncludeDir["VKBOOTSTRAP"] = "Avalanche/ThirdParty/vk-bootstrap/src"
IncludeDir["VMA"] = "Avalanche/ThirdParty/VulkanMemoryAllocator/include"
IncludeDir["Assimp"] = "Avalanche/ThirdParty/Assimp/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["Assimp"] = "Avalanche/ThirdParty/Assimp/lib/x64"

Libraries = {}
Libraries["OpenGL"] = "opengl32.lib"
Libraries["Vulkan"] = "vulkan-1.lib"
Libraries["Assimp"] = "assimp-vc143-mt.lib"

project "Editor"
    kind "ConsoleApp"
    location "Editor"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {  
        "%{prj.name}/Source/**.h",
        "%{prj.name}/Source/**.cpp",
    }

    includedirs
    {
        "%{IncludeDir.Source}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.SPDLOG}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.VMA}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.Assimp}",
    }

    links
    {
        "Shcore.lib",
        "Avalanche",
    }

    ignoredefaultlibraries
    {
        "LIBCMTD",
    }
    
    filter "system:windows"
        systemversion "latest"
        postbuildcommands
        {
            "{COPYDIR} %{wks.location}Avalanche/Content %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}/Content",
            "{COPYDIR} %{wks.location}Binaries/" .. outputdir .. "/Avalanche/Shaders %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}/Shaders",
            --"{COPY} %{wks.location}Avalanche/ThirdParty/Assimp/bin/assimp-vc143-mt.dll %{wks.location}Binaries/" .. outputdir .. "/%{prj.name}",
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

project "Avalanche"
    kind "StaticLib"
    location "Avalanche"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    
    rules { "CompileGLSLShaders", "CompileHLSLShaders"}

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/Shaders/**.vert",
        "%{prj.name}/Shaders/**.frag",
        "%{prj.name}/Shaders/**.comp",
        "%{prj.name}/Shaders/**.hlsl",
        "%{prj.name}/Shaders/**.hlsli",
        
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
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.VKBOOTSTRAP}",
        "%{IncludeDir.VMA}",
        "%{IncludeDir.TINYOBJLOADER}",
        "%{IncludeDir.STB}",
        "%{IncludeDir.ENTT}",
        "%{IncludeDir.Assimp}",
    }
    
    links
    {
        "Shcore.lib",
        "Imm32.lib",
        "%{Libraries.Vulkan}",
        "%{Libraries.Assimp}",
        "GLFW",
        "ImGui",
        "VkBootstrap",
    }

    ignoredefaultlibraries
    {
        "LIBCMTD",
    }

    libdirs
    {
        "%{LibraryDir.VulkanSDK}",
        "%{LibraryDir.Assimp}",
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

        defines
        {
            "_PLATFORM_WINDOWS"
        }
        
    filter "files:**.vert.hlsl"
        compileHLSLShadersVars {
            ShaderType = "vs_6_0"
        }
        
    filter "files:**.frag.hlsl"
        compileHLSLShadersVars {
            ShaderType = "ps_6_0"
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

include "Avalanche/ThirdParty/imgui"
include "Avalanche/ThirdParty/glfw"
include "Avalanche/ThirdParty/vk-bootstrap"