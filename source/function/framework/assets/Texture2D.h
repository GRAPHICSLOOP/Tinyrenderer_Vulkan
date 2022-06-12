#pragma once
#include "function/render/renderResource/TextureResource.h"

namespace tiny
{
	class Texture2D
	{
	public:
		void initialize(std::string path);
		void loadTexture();
		std::shared_ptr<TextureResource> getTextureResource();
		size_t getId() { return mId; }
	public:
		uint32_t mWidth;
		uint32_t mHeight;

	private:
		std::shared_ptr<TextureResource> mTextureResource;
		std::string mPath;
		uint32_t mMiplevels;
		PIXEL_FORMAT mFormat;
		size_t mId;
	};
}


