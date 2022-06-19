#include "MeshComponent.h"
#include "function/global/GlobalContext.h"
#include "function/framework/object/Object.h"
#include "TransformComponent.h"

tiny::MeshComponent::MeshComponent(std::shared_ptr<class Object> obj)
	:Component(obj)
{
	mRenderResource = gRuntimeGlobalContext.mRenderSystem->mRenderResource;
}

void tiny::MeshComponent::tick(float delta)
{
	TransformComponent* tran = mObject.lock()->mRootComponent->getComponent<TransformComponent>();
	if (!tran)
		return;

	ObjectBufferData data;
	data.mModel = tran->getModelMatrix();
	mRenderResource.lock()->mObjectBufferDatas[mObject.lock()->getID()] = data;
}

void tiny::MeshComponent::TempLoadMeshAsset(std::string name, std::string path)
{
	std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
	asset->initialize(name, path);
	setMeshAsset(asset);
	mRenderResource.lock()->mModelRenderResource.insert(std::make_pair(mObject.lock()->getID(),asset->getMeshResource()));
}

void tiny::MeshComponent::setMeshAsset(std::shared_ptr<MeshAsset> asset)
{
	mMeshAsset = asset;
}
