#pragma once
#include <vector>
#include "function/render/renderResource/MeshResource.h"
#include "Texture2D.h"

namespace tiny
{
	struct MeshPart
	{
		std::shared_ptr<MeshResource> mMeshResource;
		std::shared_ptr<Texture2D> mTexture2D;
	};

	class MeshAsset
	{
	public:
		void initialize(std::string name, std::string path);
		void loadModel();

	public:
		std::string mName;
		std::string mSourcePath;

	private:
		void processNode(struct aiNode* node,const struct aiScene* scene);
		void processMesh(struct aiMesh* mesh, const struct aiScene* scene);
		std::shared_ptr<tiny::Texture2D> loadMaterialTextures(struct aiMaterial* mat,enum aiTextureType type, std::string typeName);

	private:
		std::vector<MeshPart> mMeshParts;
		std::string mDirectory;
	};
}


