#include "VulkanUtil.h"

vk::ImageView tiny::VulkanUtil::createImageView(vk::Device device,vk::ImageAspectFlags aspectMask,vk::Format fomat,vk::Image image)
{
    vk::ImageSubresourceRange range;
    range.setAspectMask(aspectMask);
    range.setBaseArrayLayer(0);
    range.setBaseMipLevel(0);
    range.setLayerCount(1);
    range.setLevelCount(1);

    vk::ImageViewCreateInfo info;
    info.setComponents(vk::ComponentMapping());
    info.setFormat(fomat);
    info.setImage(image);
    info.setViewType(vk::ImageViewType::e2D);
    info.setSubresourceRange(range);

    return device.createImageView(info);
}
