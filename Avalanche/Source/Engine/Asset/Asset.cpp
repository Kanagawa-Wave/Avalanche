#include "Asset.h"

#include "Engine/Core/Log.h"

Asset::Asset(const std::string& json, const std::vector<char>& binaryBlob, const std::string& type)
    : Asset(json, binaryBlob, TypeNameToAssetType(type))
{
}

Asset::Asset(const std::string& json, const std::vector<char>& binaryBlob, AssetType type)
    : m_Json(json), m_Blob(binaryBlob), m_Type(type)
{
}

std::string Asset::GetTypeName() const
{
    switch (m_Type)
    {
    case AssetType::eNone:
        return "None";
    case AssetType::eTexture:
        return "Text";
    case AssetType::eMesh:
        return "Mesh";
    }
    return {};
}

AssetType Asset::TypeNameToAssetType(const std::string& type)
{
    std::unordered_map<std::string, AssetType> converter = {
        {"None", AssetType::eNone}, {"Text", AssetType::eTexture}, {"Mesh", AssetType::eMesh}
    };

    return converter[type];
}
