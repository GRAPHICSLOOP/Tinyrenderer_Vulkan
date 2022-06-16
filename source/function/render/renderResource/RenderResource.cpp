#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"


tiny::RenderResource::~RenderResource()
{
    for (uint32_t i = 0; i < mDescSetLayouts.size(); i++)
    {
        mVulkanRHI->mDevice.destroyDescriptorSetLayout(mDescSetLayouts[i]);
    }
    mVulkanRHI->mDevice.destroyBuffer(mObjectBufferResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mObjectBufferResource.mMemory);
    mVulkanRHI->mDevice.destroyBuffer(mCameraBufferResource.mBuffer);
    mVulkanRHI->mDevice.freeMemory(mCameraBufferResource.mMemory);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;
    createUniformBuffer();
    createDescriptorSetLayout();
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

vk::DescriptorSetLayout tiny::RenderResource::getDescriptorSetLayout(DESCRIPTOR_TYPE type)
{
    CHECK_NULL(mDescSetLayouts[type]);
    return mDescSetLayouts[type];
}

void tiny::RenderResource::createUniformBuffer()
{
    vk::DeviceSize bufferSize = sizeof(ObjectUniform);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mObjectBufferResource.mBuffer,
        mObjectBufferResource.mMemory
    );

    bufferSize = sizeof(CameraUniform);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mCameraBufferResource.mBuffer,
        mCameraBufferResource.mMemory
    );
}

void tiny::RenderResource::createDescriptorSetLayout()
{
    vk::DescriptorSetLayoutCreateInfo info;

    // mesh固定的uniform DescriptorSetLayout
    std::array<vk::DescriptorSetLayoutBinding,2> uniformBinding;
    uniformBinding[0].binding = 0;
    uniformBinding[0].descriptorCount = 1;
    uniformBinding[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    uniformBinding[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    uniformBinding[1].binding = 1;
    uniformBinding[1].descriptorCount = 1;
    uniformBinding[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    uniformBinding[1].stageFlags = vk::ShaderStageFlagBits::eVertex;
    info.bindingCount = 2;
    info.pBindings = uniformBinding.data();
    mDescSetLayouts[DESCRIPTOR_TYPE_UNIFORM] = mVulkanRHI->mDevice.createDescriptorSetLayout(info);

    // 变动较多的 sample DescriptorSetLayout
    vk::DescriptorSetLayoutBinding sampleBinding;
    sampleBinding.binding = 0;
    sampleBinding.descriptorCount = 1;
    sampleBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    sampleBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
    info.bindingCount = 1;
    info.pBindings = &sampleBinding;
    mDescSetLayouts[DESCRIPTOR_TYPE_SAMPLE] = mVulkanRHI->mDevice.createDescriptorSetLayout(info);
}


