#include "MeshResource.h"
#include "function/global/GlobalContext.h"

tiny::MeshResource::MeshResource(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	mVulkanRHI = gRuntimeGlobalContext.mRenderSystem->mVulkanRHI;
	gRuntimeGlobalContext.mRenderSystem->mRenderResource->createVertexBuffer(mMeshBufferResource, vertices.data(), (uint32_t)vertices.size());
	gRuntimeGlobalContext.mRenderSystem->mRenderResource->createIndexBuffer(mMeshBufferResource, indices.data(), (uint32_t)indices.size());
}

tiny::MeshResource::~MeshResource()
{
	mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResource.mVertexBuffer);
	mVulkanRHI->mDevice.destroyBuffer(mMeshBufferResource.mIndexBuffer);
	mVulkanRHI->mDevice.freeMemory(mMeshBufferResource.mVertexBufferMemory);
	mVulkanRHI->mDevice.freeMemory(mMeshBufferResource.mIndexBufferMemory);
}
