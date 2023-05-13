﻿#pragma once
#include "Image.h"

class Texture
{
public:
    Texture(const std::string& path);
    ~Texture();

    operator Image*() const
    {
        return m_Image.get();
    }
    vk::Sampler GetSampler() const { return m_Sampler; }
    vk::ImageView GetView() const { return m_Image->GetView(); }
    
private:
    int m_Width = 0, m_Height = 0, m_Channels = 0;
    std::unique_ptr<Image> m_Image;
    vk::Sampler m_Sampler;
    vk::DescriptorSet m_DescriptorSet;
};