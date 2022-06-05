#pragma once
#include <string>
#include "Component.h"
#include "function/framework/assets/MeshAsset.h"

namespace tiny
{

	class MeshComponent : public Component
	{
	public:
		void TempLoadMeshAsset(std::string name, std::string path);
		void setMeshAsset(std::shared_ptr<MeshAsset> asset);

	private:
		std::shared_ptr<MeshAsset> mMeshAsset;
	};
}



