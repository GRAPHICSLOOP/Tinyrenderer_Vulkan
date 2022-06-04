#include "RenderResource.h"
#include "function/render/vulkan/VulkanUtil.h"
#include "core/base/macro.h"

tiny::RenderResource::~RenderResource()
{
	mVulkanRHI->mDevice.destroyBuffer(mMeshResource.mBuffer);
	mVulkanRHI->mDevice.freeMemory(mMeshResource.mMemory);
	mVulkanRHI->mDevice.destroySampler(mSampleResource.mTextureSampler);
}

void tiny::RenderResource::initialize(const RenderResourceConfigParams& params)
{
	mVulkanRHI = params.mVulkanRHI;

	createMeshUniformBuffer();
	createTextureSampler(1);
}

void tiny::RenderResource::createMeshUniformBuffer()
{
	vk::DeviceSize bufferSize = sizeof(MeshBufferObject);
	VulkanUtil::createBuffer(
		mVulkanRHI->mPhyDevice,
		mVulkanRHI->mDevice,
		bufferSize,
		vk::BufferUsageFlagBits::eUniformBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		mMeshResource.mBuffer,
		mMeshResource.mMemory
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

