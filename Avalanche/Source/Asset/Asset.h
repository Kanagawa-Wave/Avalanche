#pragma once
#include <string>
#include <vector>

enum class AssetType
{
    eNone,
    eTexture,
    eMesh,
};

class Asset
{
public:
    Asset() = default;
    ~Asset() = default;
    Asset(const std::string& json, const std::vector<char>& binaryBlob, const std::string& type);
    Asset(const std::string& json, const std::vector<char>& binaryBlob, AssetType type);

    std::string GetJson() const { return m_Json; }
    std::vector<char> GetBlob() const { return m_Blob; }
    AssetType GetType() const { return m_Type; }
    std::string GetTypeName() const;

private:
    static AssetType TypeNameToAssetType(const std::string& type);
    
    std::string m_Json;
    std::vector<char> m_Blob;
    AssetType m_Type = AssetType::eNone;
};
