#include "MeshComponent.h"

void tiny::MeshComponent::TempLoadMeshAsset(std::string name, std::string path)
{
	std::shared_ptr<MeshAsset> asset = std::make_shared<MeshAsset>();
	asset->initialize(name, path);
	setMeshAsset(asset);
}

void tiny::MeshComponent::setMeshAsset(std::shared_ptr<MeshAsset> asset)
{
	mMeshAsset = asset;
}
