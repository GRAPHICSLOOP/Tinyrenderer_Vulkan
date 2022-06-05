#include<fstream>
#include "VulkanUtil.h"
#include "core/base/macro.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint32_t tiny::VulkanUtil::findMemoryType(vk::PhysicalDevice phyDevice, uint32_t typeFilter, vk::MemoryPropertyFlags property)
{
    vk::PhysicalDeviceMemoryProperties memProperties;
    memProperties = phyDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & property) == property)
        {
            return i;
        }
    }

    throw std::runtime_error("find memory faild");
}

void tiny::VulkanUtil::createImage(
    vk::PhysicalDevice phyDevice,
    vk::Device device,
    uint32_t imageWidth,
    uint32_t imageHeight,
    vk::Format format,
    vk::ImageTiling tilling,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Image& image,
    vk::DeviceMemory& imageMemory,
    uint32_t mipLevel,
    vk::SampleCountFlagBits numSamples)
{
    vk::ImageCreateInfo imageInfo;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = mipLevel;
    imageInfo.samples = numSamples;
    imageInfo.extent = vk::Extent3D(imageWidth, imageHeight, 1);
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = format;
    imageInfo.tiling = tilling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    image = device.createImage(imageInfo);
    CHECK_NULL(image);

    vk::MemoryRequirements memRequirements;
    device.getImageMemoryRequirements(image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setAllocationSize(memRequirements.size);
    allocInfo.memoryTypeIndex = findMemoryType(phyDevice,memRequirements.memoryTypeBits, properties);

    vk::Result result = device.allocateMemory(&allocInfo, nullptr, &imageMemory);
    CHECK_NULL(imageMemory);

    device.bindImageMemory(image, imageMemory, 0);
}

vk::ImageView tiny::VulkanUtil::createImageView(
    vk::Device device,
    vk::ImageAspectFlags aspectMask,
    vk::Format fomat,
    vk::Image image)
{
    vk::ImageSubresourceRange range;
    range.aspectMask = aspectMask;
    range.baseArrayLayer = 0;
    range.baseMipLevel = 0;
    range.layerCount = 1;
    range.levelCount = 1;

    vk::ImageViewCreateInfo info;
    info.components = vk::ComponentMapping();
    info.format = fomat;
    info.image = image;
    info.viewType = vk::ImageViewType::e2D;
    info.subresourceRange = range;

    vk::ImageView imageView = device.createImageView(info);
    CHECK_NULL(imageView);

    return imageView;
}

std::vector<char> tiny::VulkanUtil::readFile(const char* fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary); // 末尾且二进制读取

    if (!file.is_open())
    {
        throw std::runtime_error("open file error");
    }

    size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0); // 回到头部
    file.read(buffer.data(), size);

    file.close();
    return buffer;
}

vk::ShaderModule tiny::VulkanUtil::createShaderModule(vk::Device device, const std::vector<char>& code)
{
    vk::ShaderModuleCreateInfo shaderInfo;
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    shaderInfo.codeSize = code.size();

    vk::ShaderModule shaderModule = device.createShaderModule(shaderInfo);
    CHECK_NULL(shaderModule);
    return shaderModule;
}

vk::ShaderModule tiny::VulkanUtil::loadShaderModuleFromFile(vk::Device device, const char* fileName)
{
    std::vector<char> code = readFile(fileName);
    return createShaderModule(device, code);
}

void tiny::VulkanUtil::createBuffer(
    vk::PhysicalDevice phyDevice,
    vk::Device device,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties,
    vk::Buffer& buffer,
    vk::DeviceMemory& bufferMemory)
{
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    buffer = device.createBuffer(bufferInfo);
    CHECK_NULL(buffer);

    vk::MemoryRequirements memRequirements;
    memRequirements = device.getBufferMemoryRequirements(buffer);

    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setAllocationSize(memRequirements.size);
    allocInfo.setMemoryTypeIndex(findMemoryType(phyDevice, memRequirements.memoryTypeBits, properties));
    bufferMemory = device.allocateMemory(allocInfo);
    CHECK_NULL(bufferMemory);

    // 绑定buffer和内存
    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void tiny::VulkanUtil::transitionImageLayout(
    VulkanRHI* vulkanRHI,
    vk::Image& image,
    vk::Format format,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    uint32_t mipLevel)
{
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    vk::CommandBuffer commandBuffer = vulkanRHI->beginSingleTimeBuffer();
    vk::ImageSubresourceRange range;
    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        range.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
    else
    {
        range.aspectMask = vk::ImageAspectFlagBits::eColor;

    }
    range.setLayerCount(1);
    range.setBaseArrayLayer(0);
    range.setLevelCount(mipLevel);
    range.setBaseMipLevel(0);

    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(oldLayout);
    barrier.setNewLayout(newLayout);
    barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setImage(image);
    barrier.setSubresourceRange(range);

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    }
    else
    {
        throw std::runtime_error("unsupported layout transition!");
    }

    vk::DependencyFlags dependencyFlag;
    commandBuffer.pipelineBarrier(sourceStage, destinationStage,
        dependencyFlag,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    vulkanRHI->endSingleTimeBuffer(commandBuffer);
}

void tiny::VulkanUtil::copyBuffer(
    VulkanRHI* vulkanRHI,
    vk::Buffer srcBuffer,
    vk::Buffer dstBuffer,
    vk::DeviceSize size)
{
    vk::CommandBuffer buffer = vulkanRHI->beginSingleTimeBuffer();

    vk::BufferCopy bufferCopy;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;
    bufferCopy.srcOffset = 0;
    buffer.copyBuffer(srcBuffer, dstBuffer, bufferCopy);

    vulkanRHI->endSingleTimeBuffer(buffer);
}

tiny::Textrure2D tiny::VulkanUtil::createTexture2D(
    VulkanRHI* vulkanRHI,
    uint32_t width,
    uint32_t height,
    void* pixels,
    PIXEL_FORMAT pixelFormat,
    uint32_t miplevels)
{
    tiny::Textrure2D texture2D;
    texture2D.mFormat = pixelFormat;
    texture2D.mWidth = width;
    texture2D.mHeight = height;
    texture2D.mMiplevels = miplevels;

    size_t pixelSize = 0;
    vk::Format vulkanImageFormat;
    switch (pixelFormat)
    {
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_UNORM:
        pixelSize = (size_t)width * height * 3;
        vulkanImageFormat = vk::Format::eR8G8B8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8_SRGB:
        pixelSize = (size_t)width * height * 3;
        vulkanImageFormat = vk::Format::eR8G8B8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM:
        pixelSize = (size_t)width * height * 4;
        vulkanImageFormat = vk::Format::eR8G8B8A8Unorm;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_SRGB:
        pixelSize = (size_t)width * height * 4;
        vulkanImageFormat = vk::Format::eR8G8B8A8Srgb;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32_FLOAT:
        pixelSize = (size_t)width * height * 4 * 2;
        vulkanImageFormat = vk::Format::eR32G32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32_FLOAT:
        pixelSize = (size_t)width * height * 4 * 3;
        vulkanImageFormat = vk::Format::eR32G32B32Sfloat;
        break;
    case PIXEL_FORMAT::PIXEL_FORMAT_R32G32B32A32_FLOAT:
        pixelSize = (size_t)width * height * 4 * 4;
        vulkanImageFormat = vk::Format::eR32G32B32A32Sfloat;
        break;
    default:
        throw std::runtime_error("invalid pixelSize");
        break;
    }

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    createBuffer(
        vulkanRHI->mPhyDevice,
        vulkanRHI->mDevice,
        pixelSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        stagingBuffer,
        stagingBufferMemory);

    void* data;
    vkMapMemory(vulkanRHI->mDevice, stagingBufferMemory, 0, pixelSize, 0, &data);
    memcpy(data, pixels, pixelSize);
    vkUnmapMemory(vulkanRHI->mDevice, stagingBufferMemory);

    // create image
    createImage(
        vulkanRHI->mPhyDevice,
        vulkanRHI->mDevice,
        width,
        height,
        vulkanImageFormat,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        texture2D.mImage,
        texture2D.mMemory,
        miplevels,
        vk::SampleCountFlagBits::e1);

    transitionImageLayout(
        vulkanRHI,
        texture2D.mImage,
        vulkanImageFormat,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        miplevels);

    copyBufferToImage(vulkanRHI, stagingBuffer, texture2D.mImage, width, height);

    vulkanRHI->mDevice.destroyBuffer(stagingBuffer);
    vulkanRHI->mDevice.freeMemory(stagingBufferMemory);

    generateMipmaps(vulkanRHI, texture2D.mImage, width, height, miplevels);

    texture2D.mImageView = createImageView(vulkanRHI->mDevice, vk::ImageAspectFlagBits::eColor, vulkanImageFormat, texture2D.mImage);

    // check
    CHECK_NULL(texture2D.mImage);
    CHECK_NULL(texture2D.mImageView);
    CHECK_NULL(texture2D.mMemory);

    return texture2D;
}

void tiny::VulkanUtil::copyBufferToImage(VulkanRHI* vulkanRHI, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
    vk::ImageSubresourceLayers subresource;
    subresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresource.setLayerCount(1);
    subresource.setBaseArrayLayer(0);
    subresource.setMipLevel(0);

    vk::CommandBuffer commandBuffer = vulkanRHI->beginSingleTimeBuffer();
    vk::BufferImageCopy region;
    region.setBufferOffset(0);
    region.setBufferRowLength(0);
    region.setBufferImageHeight(0);
    region.setImageSubresource(subresource);
    region.setImageOffset({ 0,0,0 });
    region.setImageExtent({ width,height,1 });

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    vulkanRHI->endSingleTimeBuffer(commandBuffer);
}

void tiny::VulkanUtil::generateMipmaps(VulkanRHI* vulkanRHI, vk::Image image, uint32_t width, uint32_t hegith, uint32_t miplevels)
{
    vk::CommandBuffer commandBuffer = vulkanRHI->beginSingleTimeBuffer();

    vk::DependencyFlags dependency;
    vk::ImageSubresourceRange range;
    range.setBaseArrayLayer(0);
    range.setLayerCount(1);
    range.setLevelCount(1);
    range.setAspectMask(vk::ImageAspectFlagBits::eColor);

    vk::ImageMemoryBarrier barrier;
    barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setImage(image);

    uint32_t mipWidth = width;
    uint32_t mipHeight = hegith;
    for (uint32_t i = 1; i < miplevels; i++)
    {
        range.setBaseMipLevel(i - 1);

        barrier.setSubresourceRange(range);
        barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
        barrier.setNewLayout(vk::ImageLayout::eTransferSrcOptimal);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferRead);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, dependency, nullptr, nullptr, barrier);


        // 将源图 进行 mip
        vk::ImageBlit imageBlit;
        imageBlit.srcOffsets[0] = vk::Offset3D(0, 0, 0);
        imageBlit.srcOffsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
        imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageBlit.srcSubresource.baseArrayLayer = 0;
        imageBlit.srcSubresource.layerCount = 1;
        imageBlit.srcSubresource.mipLevel = i - 1;

        imageBlit.dstOffsets[0] = vk::Offset3D(0, 0, 0);
        imageBlit.dstOffsets[1] = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1);
        imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageBlit.dstSubresource.baseArrayLayer = 0;
        imageBlit.dstSubresource.layerCount = 1;
        imageBlit.dstSubresource.mipLevel = i;

        commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal,
            image, vk::ImageLayout::eTransferDstOptimal,
            imageBlit,
            vk::Filter::eLinear);

        barrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal);
        barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, dependency, nullptr, nullptr, barrier);

        if (mipHeight > 1) mipHeight /= 2;
        if (mipWidth > 1) mipWidth /= 2;
    }

    range.setBaseMipLevel(miplevels - 1);
    barrier.setSubresourceRange(range);
    barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
    barrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
    barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, dependency, nullptr, nullptr, barrier);

    vulkanRHI->endSingleTimeBuffer(commandBuffer);
}

tiny::Textrure2D tiny::VulkanUtil::loadTexture2DFormFile(VulkanRHI* vulkanRHI, const char* filePath)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    CHECK_NULL(pixels);
    vk::DeviceSize DeviceSize = (uint64_t)texWidth * texHeight * 4;
    uint32_t miplevels = (uint32_t)std::floor(std::log2(std::max(texWidth, texHeight))) + 1;

    tiny::Textrure2D texture2D = createTexture2D(
        vulkanRHI,
        texWidth,
        texHeight,
        pixels, 
        tiny::PIXEL_FORMAT::PIXEL_FORMAT_R8G8B8A8_UNORM, 
        miplevels);

    stbi_image_free(pixels);

    return texture2D;
}
