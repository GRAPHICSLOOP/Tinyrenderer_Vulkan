#include "TextureResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "function/global/GlobalContext.h"
#include "core/base/macro.h"

tiny::TextureResource::TextureResource()
{
    mId = 0;
    mInit = false;
}

void tiny::TextureResource::initialize(
    size_t id,
    uint32_t width,
    uint32_t height,
    void* pixels,
    PIXEL_FORMAT pixelFormat,
    uint32_t miplevels)
{
    mId = id;
    mRenderResource = gRuntimeGlobalContext.mRenderSystem->mRenderResource;
    mVulkanRHI = gRuntimeGlobalContext.mRenderSystem->mVulkanRHI;

    if (mRenderResource->mGlobalTextureResources[mId].lock())
    {
        TINYLOG_INFO("重复贴图不加载：{}", mId);
        return;
    }
    mRenderResource->mGlobalTextureResources[mId] = shared_from_this();
    mTextureBufferResource = VulkanUtil::createTextureBufferResource(width, height, pixels, pixelFormat, miplevels);
    mTextureBufferResource.mTextureSampler = createTextureSampler(miplevels);
    createDescriptorSet();
    mInit = true;
}

tiny::TextureResource::~TextureResource()
{
    if (!mInit)
        return;

    auto iter = mRenderResource->mGlobalTextureResources.find(mId);
    mRenderResource->mGlobalTextureResources.erase(iter);

    mVulkanRHI->mDevice.destroyImage(mTextureBufferResource.mImage);
    mVulkanRHI->mDevice.destroyImageView(mTextureBufferResource.mImageView);
    mVulkanRHI->mDevice.freeMemory(mTextureBufferResource.mMemory);
    mVulkanRHI->mDevice.destroySampler(mTextureBufferResource.mTextureSampler);
}

vk::Sampler tiny::TextureResource::createTextureSampler(uint32_t mipLevels)
{
    vk::SamplerCreateInfo info;
    info.minFilter = vk::Filter::eLinear;
    info.magFilter = vk::Filter::eLinear;
    info.addressModeU = vk::SamplerAddressMode::eRepeat;
    info.addressModeV = vk::SamplerAddressMode::eRepeat;
    info.addressModeW = vk::SamplerAddressMode::eRepeat;
    info.anisotropyEnable = VK_FALSE;
    info.maxAnisotropy = 1;
    info.borderColor = vk::BorderColor::eIntOpaqueBlack;
    info.unnormalizedCoordinates = VK_FALSE;
    info.compareEnable = VK_FALSE;
    info.compareOp = vk::CompareOp::eAlways;
    info.mipmapMode = vk::SamplerMipmapMode::eLinear;
    info.mipLodBias = 0;
    info.minLod = 0;
    info.maxLod = (float)mipLevels;

    vk::Sampler sample = mVulkanRHI->mDevice.createSampler(info);
    CHECK_NULL(sample);

    return sample;
}

void tiny::TextureResource::createDescriptorSet()
{
    vk::DescriptorSetLayout setLayout = mRenderResource->getDescriptorSetLayout(DESCRIPTOR_TYPE_SAMPLE);

    vk::DescriptorSetAllocateInfo info;
    info.descriptorPool = mVulkanRHI->mDescriptorPool;
    info.descriptorSetCount = 1;
    info.pSetLayouts = &setLayout;
    mTextureBufferResource.mDescriptorSet = mVulkanRHI->mDevice.allocateDescriptorSets(info)[0];

    vk::DescriptorImageInfo imageInfo;
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo.imageView = mTextureBufferResource.mImageView;
    imageInfo.sampler = mTextureBufferResource.mTextureSampler;

    // 更新描述符
    std::array<vk::WriteDescriptorSet, 1> writeSet;
    writeSet[0].dstArrayElement = 0;
    writeSet[0].dstBinding = 0;
    writeSet[0].dstSet = mTextureBufferResource.mDescriptorSet;
    writeSet[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writeSet[0].descriptorCount = 1;
    writeSet[0].pImageInfo = &imageInfo;

    mVulkanRHI->mDevice.updateDescriptorSets(writeSet, nullptr);
}

