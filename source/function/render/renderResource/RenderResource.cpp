#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"


tiny::RenderResource::~RenderResource()
{
    for (uint32_t i = 0; i < mDescSetLayouts.size(); i++)
    {
        mVulkanRHI->mDevice.destroyDescriptorSetLayout(mDescSetLayouts[i]);
    }

    vkUnmapMemory(mVulkanRHI->mDevice, mObjectBufferResource.mMemory);
    vkUnmapMemory(mVulkanRHI->mDevice, mCameraBufferResource.mMemory);
    mVulkanRHI->mDevice.destroyBuffer(mObjectBufferResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mObjectBufferResource.mMemory);
    mVulkanRHI->mDevice.destroyBuffer(mCameraBufferResource.mBuffer);
    mVulkanRHI->mDevice.freeMemory(mCameraBufferResource.mMemory);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;
    createBufferResource();
    createDescriptorSetLayout();
}

void tiny::RenderResource::createVertexBuffer(MeshBufferResource& bufferResouce,const void* VerticesData, uint32_t count)
{
    bufferResouce.mVertexCount = count;
    size_t size = count * sizeof(VertexBufferData);

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

void tiny::RenderResource::updatePerFrameBuffer(std::shared_ptr<RenderCamera> camera)
{
    //(*(ObjectBufferData*)mObjectBufferResource.mData).mModel = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));
    (*(CameraBufferData*)mCameraBufferResource.mData).mView = camera->getViewMatrix();
    (*(CameraBufferData*)mCameraBufferResource.mData).mProj = glm::perspectiveRH(glm::radians(45.f), mVulkanRHI->mSwapchainSupportDetails.mExtent2D.width / (float)mVulkanRHI->mSwapchainSupportDetails.mExtent2D.height, 0.1f, 10.f);
    (*(CameraBufferData*)mCameraBufferResource.mData).mProj[1][1] *= -1;
    (*(CameraBufferData*)mCameraBufferResource.mData).mViewPorj = (*(CameraBufferData*)mCameraBufferResource.mData).mProj * (*(CameraBufferData*)mCameraBufferResource.mData).mView;
}

vk::DescriptorSetLayout tiny::RenderResource::getDescriptorSetLayout(DESCRIPTOR_TYPE type)
{
    CHECK_NULL(mDescSetLayouts[type]);
    return mDescSetLayouts[type];
}

void tiny::RenderResource::createBufferResource()
{
    vk::DeviceSize bufferSize = sizeof(ObjectBufferData);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mObjectBufferResource.mBuffer,
        mObjectBufferResource.mMemory
    );

    bufferSize = sizeof(CameraBufferData);
    VulkanUtil::createBuffer(
        bufferSize,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        mCameraBufferResource.mBuffer,
        mCameraBufferResource.mMemory
    );

    // 不立马解除映射 销毁RenderResource的时候才解除
    vkMapMemory(mVulkanRHI->mDevice, mObjectBufferResource.mMemory, 0, sizeof(ObjectBufferData), 0, &mObjectBufferResource.mData);
    vkMapMemory(mVulkanRHI->mDevice, mCameraBufferResource.mMemory, 0, sizeof(CameraBufferData), 0, &mCameraBufferResource.mData);
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


