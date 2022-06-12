#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"


tiny::RenderResource::~RenderResource()
{
    mVulkanRHI->mDevice.destroyBuffer(mTransfromResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mTransfromResource.mMemory);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;
	createTransfromUniformBuffer();
}

void tiny::RenderResource::createVertexBuffer(MeshBufferResource& bufferResouce,const void* VerticesData, uint32_t count)
{
    bufferResouce.mVertexCount = count;
    size_t size = count * sizeof(Vertex);

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    VulkanUtil::createBuffer(
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
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mVertexBuffer,
        bufferResouce.mVertexBufferMemory);

    VulkanUtil::copyBuffer(stagingBuffer, bufferResouce.mVertexBuffer, size);

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
        size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        bufferResouce.mIndexBuffer,
        bufferResouce.mIndexBufferMemory);

    VulkanUtil::copyBuffer(stagingBuffer, bufferResouce.mIndexBuffer, size);

    mVulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    mVulkanRHI->mDevice.freeMemory(stagingBufferMemory);
}

void tiny::RenderResource::createTransfromUniformBuffer()
{
    vk::DeviceSize bufferSize = sizeof(TransfromUniform);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mTransfromResource.mBuffer,
        mTransfromResource.mMemory
    );
}

void tiny::RenderResource::createDescriptorSetLayout()
{
}


