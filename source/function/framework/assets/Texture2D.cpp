#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "core/base/macro.h"
#include "function/global/GlobalContext.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "function/render/renderResource/RenderResource.h"

void tiny::Texture2D::initialize(std::string path)
{
	std::hash<std::string> hash_fn;
	mId = hash_fn(path);
    mPath = path;
	
    loadTexture();
}

void tiny::Texture2D::loadTexture()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(mPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    CHECK_NULL(pixels);
    vk::DeviceSize DeviceSize = (uint64_t)texWidth * texHeight * 4;
    uint32_t miplevels = (uint32_t)std::floor(std::log2(std::max(texWidth, texHeight))) + 1;
    
    mFormat = tiny::PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM;
    mWidth = texWidth;
    mHeight = texHeight;
    mMiplevels = miplevels;

    mTextureResource = std::make_shared<TextureResource>();
    mTextureResource->initialize(
        mId,
        mWidth,
        mHeight,
        pixels, 
        mFormat,
        mMiplevels);

    stbi_image_free(pixels);
    return;
}

std::shared_ptr<tiny::TextureResource> tiny::Texture2D::getTextureResource()
{
    return gRuntimeGlobalContext.mRenderSystem->mRenderResource->mGlobalTextureResources[mId].lock();
}
