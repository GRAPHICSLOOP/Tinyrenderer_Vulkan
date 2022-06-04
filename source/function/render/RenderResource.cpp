#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "models/viking_room.png";

tiny::RenderResource::~RenderResource()
{
    mVulkanRHI->mDevice.destroyImage(mTextureResource.mImage);
    mVulkanRHI->mDevice.destroyImageView(mTextureResource.mImageView);
    mVulkanRHI->mDevice.freeMemory(mTextureResource.mMemory);
    mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResource.mIndexBuffer);
    mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResource.mVertexBuffer);
    mVulkanRHI->mDevice.freeMemory(mMeshBufferResource.mIndexBufferMemory);
    mVulkanRHI->mDevice.freeMemory(mMeshBufferResource.mVertexBufferMemory);
    mVulkanRHI->mDevice.destroyBuffer(mTransfromResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mTransfromResource.mMemory);
	mVulkanRHI->mDevice.destroySampler(mSampleResource.mTextureSampler);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;

	createTransfromUniformBuffer();
    tempLoadResource();
	createTextureSampler(mTextureResource.mMiplevels);
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

void tiny::RenderResource::tempLoadResource()
{
    tempLoadModel();
    tempLoadImage();
    createVertexBuffer();
    createIndexBuffer();
}

void tiny::RenderResource::createVertexBuffer()
{
    vk::DeviceSize deviceSize = sizeof(Vertex) * mMeshBufferResource.mVertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        deviceSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(mVulkanRHI->mDevice, stagingBufferMemory, 0, deviceSize, 0, &data);
    memcpy(data, mMeshBufferResource.mVertices.data(), deviceSize);
    vkUnmapMemory(mVulkanRHI->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        deviceSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mMeshBufferResource.mVertexBuffer,
        mMeshBufferResource.mVertexBufferMemory);

    VulkanUtil::copyBuffer(mVulkanRHI.get(),stagingBuffer, mMeshBufferResource.mVertexBuffer, deviceSize);

    mVulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    mVulkanRHI->mDevice.freeMemory(stagingBufferMemory);
}

void tiny::RenderResource::createIndexBuffer()
{
    vk::DeviceSize deviceSize = sizeof(Vertex) * mMeshBufferResource.mIndices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        deviceSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);


    void* data;
    vkMapMemory(mVulkanRHI->mDevice, stagingBufferMemory, 0, deviceSize, 0, &data);
    memcpy(data, mMeshBufferResource.mIndices.data(), deviceSize);
    vkUnmapMemory(mVulkanRHI->mDevice, stagingBufferMemory);

    VulkanUtil::createBuffer(
        mVulkanRHI->mPhyDevice,
        mVulkanRHI->mDevice,
        deviceSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mMeshBufferResource.mIndexBuffer,
        mMeshBufferResource.mIndexBufferMemory);

    VulkanUtil::copyBuffer(mVulkanRHI.get(),stagingBuffer, mMeshBufferResource.mIndexBuffer, deviceSize);

    mVulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    mVulkanRHI->mDevice.freeMemory(stagingBufferMemory);
}

void tiny::RenderResource::tempLoadModel()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    for (const auto& shape : shapes)
    {
        size_t indexOffset = 0;

        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex;
            vertex.mColor = { 1,1,1 };
            vertex.mPosition = {
                attrib.vertices[(uint64_t)3 * index.vertex_index],
                attrib.vertices[(uint64_t)3 * index.vertex_index + 1],
                attrib.vertices[(uint64_t)3 * index.vertex_index + 2]
            };

            vertex.mTexCoord = {
                attrib.texcoords[(uint64_t)2 * index.texcoord_index],
                1.f - attrib.texcoords[(uint64_t)2 * index.texcoord_index + 1]
            };

            mMeshBufferResource.mVertices.push_back(vertex);
            mMeshBufferResource.mIndices.push_back(indexOffset++);
        }
    }
}

void tiny::RenderResource::tempLoadImage()
{
    mTextureResource = VulkanUtil::loadTexture2DFormFile(mVulkanRHI.get(), TEXTURE_PATH.c_str());
}

