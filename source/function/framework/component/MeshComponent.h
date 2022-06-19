#pragma once
#include <string>
#include "Component.h"
#include "function/framework/assets/MeshAsset.h"

namespace tiny
{

	class MeshComponent : public Component
	{
	public:
		MeshComponent(std::shared_ptr<class Object> obj);

		virtual void tick(float delta);

		void TempLoadMeshAsset(std::string name, std::string path);
		void setMeshAsset(std::shared_ptr<MeshAsset> asset);

	private:
		std::shared_ptr<MeshAsset> mMeshAsset;
		std::weak_ptr<RenderResource> mRenderResource;
	};
}



