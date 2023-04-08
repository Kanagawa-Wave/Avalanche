#pragma once
#include "Asset.h"

class Loader
{
public:
    static bool SaveBinaryBlob(const std::string& path, const Asset& asset);
    static bool LoadBinaryBlob(const std::string& path, Asset& outAsset);
};
