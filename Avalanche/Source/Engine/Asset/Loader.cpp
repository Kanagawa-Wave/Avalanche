#include "Loader.h"

#include <fstream>

bool Loader::SaveBinaryBlob(const std::string& path, const Asset& asset)
{
    std::ofstream outfile;
    outfile.open(path, std::ios::binary | std::ios::out);

    std::string assetType = asset.GetTypeName();
    outfile.write(assetType.c_str(), 4);

    //json length
    uint32_t length = (uint32_t)asset.GetJson().size();
    outfile.write((const char*)&length, sizeof(uint32_t));

    //blob length
    uint32_t blobLength = (uint32_t)asset.GetBlob().size();
    outfile.write((const char*)&blobLength, sizeof(uint32_t));

    //json stream
    outfile.write(asset.GetJson().data(), length);
    //blob data
    outfile.write(asset.GetBlob().data(), (std::streamsize)asset.GetBlob().size());

    outfile.close();

    return true;
}

bool Loader::LoadBinaryBlob(const std::string& path, Asset& outAsset)
{
    std::ifstream infile;
    infile.open(path, std::ios::binary);

    if (!infile.is_open()) return false;

    //move file cursor to beginning
    infile.seekg(0);

    std::string type;
    infile.read(type.data(), 4);

    uint32_t jsonlen = 0;
    infile.read((char*)&jsonlen, sizeof(uint32_t));

    uint32_t bloblen = 0;
    infile.read((char*)&bloblen, sizeof(uint32_t));

    std::string json;
    json.resize(jsonlen);
    infile.read(json.data(), jsonlen);

    std::vector<char> blob;
    blob.resize(bloblen);
    infile.read(blob.data(), bloblen);

    outAsset = Asset(json, blob, type);
    
    return true;
}
