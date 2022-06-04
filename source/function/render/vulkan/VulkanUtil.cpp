#include "VulkanUtil.h"
#include "core/base/macro.h"

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
