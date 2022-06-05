#pragma once
#include "function/render/RenderResource.h"

namespace tiny
{
	class MeshAsset
	{


	public:
		void initialize(std::string name, std::string path);
		void loadVertexData();

	public:
		std::string mName;
		std::string mSourcePath;
	private:
		std::vector<Vertex> mVertices;
		std::vector<uint32_t> mIndices;
		std::shared_ptr<RenderResource> mRenderResource;
	};
}


