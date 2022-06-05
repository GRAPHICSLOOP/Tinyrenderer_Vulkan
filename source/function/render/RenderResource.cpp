#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"

const std::string TEXTURE_PATH = "models/viking_room.png";

tiny::RenderResource::~RenderResource()
{
    mVulkanRHI->mDevice.destroyImage(mTextureResource.mImage);
    mVulkanRHI->mDevice.destroyImageView(mTextureResource.mImageView);
    mVulkanRHI->mDevice.freeMemory(mTextureResource.mMemory);
    for (uint32_t i = 0; i < mMeshBufferResources.size(); i++)
    {
        mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResources[i].mIndexBuffer);
        mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResources[i].mVertexBuffer);
        mVulkanRHI->mDevice.freeMemory(mMeshBufferResources[i].mIndexBufferMemory);
        mVulkanRHI->mDevice.freeMemory(mMeshBufferResources[i].mVertexBufferMemory);
    }
    mVulkanRHI->mDevice.destroyBuffer(mTransfromResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mTransfromResource.mMemory);
	mVulkanRHI->mDevice.destroySampler(mSampleResource.mTextureSampler);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;
	createTransfromUniformBuffer();
    tempLoadImage();
	createTextureSampler(mTextureResource.mMiplevels);
}

void tiny::RenderResource::addRenderData(const void* VerticesData, uint32_t VerticesCount, const void* indicesData, uint32_t indicesCount)
{
    MeshBufferResource meshBufferResource;
    createVertexBuffer(meshBufferResource,VerticesData, VerticesCount);
    createIndexBuffer(meshBufferResource,indicesData, indicesCount);
    mMeshBufferResources.push_back(meshBufferResource);
}

void tiny::RenderResource::createTransfromUniformBuffer()
{
	vk::DeviceSize bufferSize = sizeof(TransfromUniform);
	VulkanUtil::createBuffer(
		mVulkanRHI->mPhyDevice,
		mVulkanRHI->mDevice,
		bufferSize,
		vk::BufferUsageFlagBits::eUniformBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		mTransfromResource.mBuffer,
        mTransfromResource.mMemory
	);
}

void tiny::RenderResource::createTextureSampler(uint32_t mipLevels)
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

	mSampleResource.mTextureSampler = mVulkanRHI->mDevice.createSampler(info);
	CHECK_NULL(mSampleResource.mTextureSampler);
}

void tiny::RenderResource::createVertexBuffer(MeshBufferResource& bufferResouce,const void* VerticesData, uint32_t count)
{
    bufferResouce.mVertexCount = count;
    size_t size = count * sizeof(Vertex);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(mVulkanRHI->mDevice, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, VerticesData, size);
    vkUnmapMemory(mVulkanRHI->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mVertexBuffer,
        bufferResouce.mVertexBufferMemory);

    VulkanUtil::copyBuffer(mVulkanRHI.get(),stagingBuffer, bufferResouce.mVertexBuffer, size);

    mVulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    mVulkanRHI->mDevice.freeMemory(stagingBufferMemory);
}

void tiny::RenderResource::createIndexBuffer(MeshBufferResource& bufferResouce,const void* indicesData, uint32_t count)
{
    bufferResouce.mIndexCount = count;
    size_t size = count * sizeof(uint32_t);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(mVulkanRHI->mDevice, stagingBufferMemory, 0, size, 0, &data);
    memcpy(data, indicesData, size);
    vkUnmapMemory(mVulkanRHI->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mIndexBuffer,
        bufferResouce.mIndexBufferMemory);

    VulkanUtil::copyBuffer(mVulkanRHI.get(),stagingBuffer, bufferResouce.mIndexBuffer, size);

    mVulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    mVulkanRHI->mDevice.freeMemory(stagingBufferMemory);
}

void tiny::RenderResource::tempLoadImage()
{
    mTextureResource = VulkanUtil::loadTexture2DFormFile(mVulkanRHI.get(), TEXTURE_PATH.c_str());
}

