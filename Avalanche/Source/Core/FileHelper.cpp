#include "FileHelper.h"

#include "Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// Code snippet from https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Platform/Windows/WindowsPlatformUtils.cpp
// with some minor change to the original code.

std::string FileHelper::OpenFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    CHAR currentDir[256] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(Application::Instance()->GetWindow()->GetGLFWWindow());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return {};
}

std::string FileHelper::SaveFile(const char* filter)
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    CHAR currentDir[256] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = glfwGetWin32Window(Application::Instance()->GetWindow()->GetGLFWWindow());
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    // Sets the default extension by extracting it from the filter
    ofn.lpstrDefExt = strchr(filter, '\0') + 1;

    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;

    return {};
}
