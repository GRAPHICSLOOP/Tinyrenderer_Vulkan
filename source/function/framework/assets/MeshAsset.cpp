#include "MeshAsset.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "function/global/GlobalContext.h"

void tiny::MeshAsset::initialize(std::string name, std::string path)
{
	mName = name;
	mSourcePath = path;

    loadVertexData();

    gRuntimeGlobalContext.mRenderSystem->mRenderResource->addRenderData(
        mVertices.data(),
        (uint32_t)mVertices.size(),
        mIndices.data(),
        (uint32_t)mIndices.size());

}

void tiny::MeshAsset::loadVertexData()
{

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mSourcePath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    for (const auto& shape : shapes)
    {
        uint32_t indexOffset = 0;

        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex;
            vertex.mColor = { 1,1,1 };
            vertex.mPosition = {
                attrib.vertices[(uint64_t)3 * index.vertex_index],
                attrib.vertices[(uint64_t)3 * index.vertex_index + 1],
                attrib.vertices[(uint64_t)3 * index.vertex_index + 2]
            };

            vertex.mTexCoord = {
                attrib.texcoords[(uint64_t)2 * index.texcoord_index],
                1.f - attrib.texcoords[(uint64_t)2 * index.texcoord_index + 1]
            };

            mVertices.push_back(vertex);
            mIndices.push_back(indexOffset++);
        }
    }
}
