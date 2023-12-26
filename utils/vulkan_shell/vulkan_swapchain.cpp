/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vulkan_swapchain.h"

#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/vk/GrVkTypes.h"
#include "vulkan_backbuffer.h"
#include "vulkan_device.h"
#include "vulkan_hilog.h"
#include "vulkan_image.h"
#include "vulkan_proc_table.h"
#include "vulkan_surface.h"

namespace OHOS::Rosen::vulkan {

namespace {
struct FormatInfo {
    VkFormat format_;
    SkColorType colorType_;
    sk_sp<SkColorSpace> colorSpace_;
};
} // namespace

static std::vector<FormatInfo> DesiredFormatInfos()
{
    return { { VK_FORMAT_R8G8B8A8_UNORM, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB() },
        { VK_FORMAT_B8G8R8A8_UNORM, kRGBA_8888_SkColorType, SkColorSpace::MakeSRGB() },
        { VK_FORMAT_R8G8B8A8_SRGB, kSRGBA_8888_SkColorType, SkColorSpace::MakeSRGBLinear() },
        { VK_FORMAT_B8G8R8A8_SRGB, kBGRA_8888_SkColorType, SkColorSpace::MakeSRGBLinear() },
        { VK_FORMAT_R16G16B16A16_SFLOAT, kRGBA_F16_SkColorType, SkColorSpace::MakeSRGBLinear() } };
}

std::mutex RSVulkanSwapchain::mapMutex_;
std::unordered_map<std::thread::id, RSVulkanSwapchain*> RSVulkanSwapchain::toBePresent_;

RSVulkanSwapchain::RSVulkanSwapchain(const RSVulkanProcTable& procVk, const RSVulkanDevice& device,
    const RSVulkanSurface& surface, GrDirectContext* skiaContext, std::unique_ptr<RSVulkanSwapchain> oldSwapchain,
    uint32_t queueFamilyIndex)

    : vk(procVk), device_(device), capabilities_(), surfaceFormat_(),
      currentPipelineStage_(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT), currentBackbufferIndex_(0), currentImageIndex_(0),
      valid_(false)
{
    if (!device_.IsValid() || !surface.IsValid() || skiaContext == nullptr) {
        LOGE("Device or surface is invalid.");
        return;
    }

    if (!device_.GetSurfaceCapabilities(surface, &capabilities_)) {
        LOGE("Could not find surface capabilities.");
        return;
    }

    const auto formatInfos = DesiredFormatInfos();
    std::vector<VkFormat> desired_formats(formatInfos.size());
    for (size_t i = 0; i < formatInfos.size(); ++i) {
        if (skiaContext->colorTypeSupportedAsSurface(formatInfos[i].colorType_)) {
            desired_formats[i] = formatInfos[i].format_;
        } else {
            desired_formats[i] = VK_FORMAT_UNDEFINED;
        }
    }

    int formatIndex = device_.ChooseSurfaceFormat(surface, desired_formats, &surfaceFormat_);
    if (formatIndex < 0) {
        LOGE("Could not choose surface format.");
        return;
    }

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!device_.ChoosePresentMode(surface, &presentMode)) {
        LOGE("Could not choose present mode.");
        return;
    }

    // Check if the surface can present.

    VkBool32 supported = VK_FALSE;
    if (VK_CALL_LOG_ERROR(vk.GetPhysicalDeviceSurfaceSupportKHR(device_.GetPhysicalDeviceHandle(), // physical device
        queueFamilyIndex,                                                                    // queue family
        surface.Handle(),                                                                      // surface to test
        &supported)) != VK_SUCCESS) {
        LOGE("Could not get physical device surface support.");
        return;
    }

    if (supported != VK_TRUE) {
        LOGE("Surface was not supported by the physical device.");
        return;
    }

    // Construct the Swapchain

    VkSwapchainKHR oldSwapchainHandle = VK_NULL_HANDLE;

    if (oldSwapchain != nullptr && oldSwapchain->IsValid()) {
        oldSwapchainHandle = oldSwapchain->swapchain_;
    }

    VkSurfaceKHR surfaceHandle = surface.Handle();

    const VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surfaceHandle,
        .minImageCount = capabilities_.minImageCount,
        .imageFormat = surfaceFormat_.format,
        .imageColorSpace = surfaceFormat_.colorSpace,
        .imageExtent = capabilities_.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_FALSE,
        .oldSwapchain = oldSwapchainHandle,
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    if (VK_CALL_LOG_ERROR(vk.CreateSwapchainKHR(device_.GetHandle(), &createInfo, nullptr, &swapchain)) !=
        VK_SUCCESS) {
        LOGE("Could not create the swapchain.");
        return;
    }

    swapchain_ = { swapchain, [this](VkSwapchainKHR swapchain) {
        device_.WaitIdle();
        vk.DestroySwapchainKHR(device_.GetHandle(), swapchain, nullptr);
    } };

    if (!CreateSwapchainImages(
        skiaContext, formatInfos[formatIndex].colorType_, formatInfos[formatIndex].colorSpace_)) {
        LOGE("Could not create swapchain images.");
        return;
    }

    valid_ = true;
}

RSVulkanSwapchain::~RSVulkanSwapchain() = default;

bool RSVulkanSwapchain::IsValid() const
{
    return valid_;
}

std::vector<VkImage> RSVulkanSwapchain::GetImages() const
{
    uint32_t count = 0;
    if (VK_CALL_LOG_ERROR(vk.GetSwapchainImagesKHR(device_.GetHandle(), swapchain_, &count, nullptr)) != VK_SUCCESS) {
        return {};
    }

    if (count == 0) {
        return {};
    }

    std::vector<VkImage> images;

    images.resize(count);

    if (VK_CALL_LOG_ERROR(vk.GetSwapchainImagesKHR(device_.GetHandle(), swapchain_, &count, images.data())) !=
        VK_SUCCESS) {
        return {};
    }

    return images;
}

SkISize RSVulkanSwapchain::GetSize() const
{
    VkExtent2D vkExtents = capabilities_.currentExtent;

    if (vkExtents.width < capabilities_.minImageExtent.width) {
        vkExtents.width = capabilities_.minImageExtent.width;
    } else if (vkExtents.width > capabilities_.maxImageExtent.width) {
        vkExtents.width = capabilities_.maxImageExtent.width;
    }

    if (vkExtents.height < capabilities_.minImageExtent.height) {
        vkExtents.height = capabilities_.minImageExtent.height;
    } else if (vkExtents.height > capabilities_.maxImageExtent.height) {
        vkExtents.height = capabilities_.maxImageExtent.height;
    }

    return SkISize::Make(vkExtents.width, vkExtents.height);
}

sk_sp<SkSurface> RSVulkanSwapchain::CreateSkiaSurface(GrDirectContext* gr_context, VkImage image, const SkISize& size,
    SkColorType color_type, sk_sp<SkColorSpace> color_space) const
{
    if (gr_context == nullptr) {
        LOGE("gr_context == nullptr");
        return nullptr;
    }

    if (color_type == kUnknown_SkColorType) {
        // Unexpected Vulkan format.
        LOGE("color_type == kUnknown_SkColorType");
        return nullptr;
    }

    GrVkImageInfo imageInfo;
    imageInfo.fImage = image;
    imageInfo.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.fFormat = surfaceFormat_.format;
    imageInfo.fImageUsageFlags =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.fSampleCount = 1;
    imageInfo.fLevelCount = 1;

    GrBackendRenderTarget backendRenderTarget(size.fWidth, size.fHeight, 0, imageInfo);
    SkSurfaceProps props(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

    LOGI("SkSurface::MakeFromBackendRenderTarget");
    return SkSurface::MakeFromBackendRenderTarget(gr_context, // context
        backendRenderTarget,                                // backend render target
        kTopLeft_GrSurfaceOrigin,                             // origin
        color_type,                                           // color type
        std::move(color_space),                               // color space
        &props);                                              // surface properties
}

bool RSVulkanSwapchain::CreateSwapchainImages(
    GrDirectContext* skiaContext, SkColorType color_type, sk_sp<SkColorSpace> color_space)
{
    std::vector<VkImage> images = GetImages();

    if (images.size() == 0) {
        LOGE("images.size() == 0");
        return false;
    }

    const SkISize surface_size = GetSize();

    for (const VkImage& image : images) {
        // Populate the backbuffer.
        auto backbuffer = std::make_unique<RSVulkanBackbuffer>(vk, device_.GetHandle(), device_.GetCommandPool());
        if (!backbuffer || !backbuffer->IsValid()) {
            return false;
        }

        backbuffers_.emplace_back(std::move(backbuffer));

        // Populate the image.
        auto vulkan_image = std::make_unique<RSVulkanImage>(image);

        if (!vulkan_image->IsValid()) {
            LOGE("vulkan_image Not Valid()");
            return false;
        }

        images_.emplace_back(std::move(vulkan_image));

        // Populate the surface.
        auto surface = CreateSkiaSurface(skiaContext, image, surface_size, color_type, color_space);
        if (surface == nullptr) {
            LOGE("surface is nullptr");
            return false;
        }

        surfaces_.emplace_back(std::move(surface));
    }

    if (backbuffers_.size() != images_.size()) {
        LOGI("backbuffers_.size() != images_.size()");
    }
    if (images_.size() != surfaces_.size()) {
        LOGI("images_.size() != surfaces_.size()");
    }

    return true;
}

RSVulkanBackbuffer* RSVulkanSwapchain::GetNextBackbuffer()
{
    auto availableBackbuffers = backbuffers_.size();
    if (availableBackbuffers == 0) {
        return nullptr;
    }

    auto nextBackbufferIndex = (currentBackbufferIndex_ + 1) % backbuffers_.size();

    auto& backbuffer = backbuffers_[nextBackbufferIndex];

    if (!backbuffer->IsValid()) {
        return nullptr;
    }

    currentBackbufferIndex_ = nextBackbufferIndex;
    return backbuffer.get();
}

RSVulkanSwapchain::AcquireResult RSVulkanSwapchain::AcquireSurface(int bufferCount)
{
    AcquireResult error = { AcquireStatus::ERROR_SURFACE_LOST, nullptr };

    if (!IsValid()) {
        LOGE("Swapchain was invalid.");
        return error;
    }

    auto backbuffer = GetNextBackbuffer();
    if (backbuffer == nullptr) {
        LOGE("Could not get the next backbuffer.");
        return error;
    }

    if (!backbuffer->IsMultiThreading()) {
        if (!backbuffer->WaitFences()) {
            LOGE("Failed waiting on fences.");
            return error;
        }

        if (!backbuffer->ResetFences()) {
            LOGE("Could not reset fences.");
            return error;
        }
    }
    uint32_t nextImageIndex = 0;

    VkResult acquireResult = VK_CALL_LOG_ERROR(vk.AcquireNextImageKHR(device_.GetHandle(), //
        swapchain_,                                                                         //
        std::numeric_limits<uint64_t>::max(),                                               //
        backbuffer->GetUsageSemaphore(),                                                    //
        VK_NULL_HANDLE,                                                                     //
        &nextImageIndex));

    if (bufferCount == 1) {
        nextImageIndex = 0;
    }

    switch (acquireResult) {
        case VK_SUCCESS:
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            return { AcquireStatus::ERROR_SURFACE_LOST, nullptr };
        case VK_ERROR_OUT_OF_DATE_KHR:
            return { AcquireStatus::ERROR_SURFACE_OUT_OF_DATE, nullptr };
        default:
            LOGE("Unexpected result from AcquireNextImageKHR: %d", acquireResult);
            return { AcquireStatus::ERROR_SURFACE_LOST, nullptr };
    }

    // Simple sanity checking of image index.
    if (nextImageIndex >= images_.size()) {
        LOGE("Image index returned was out-of-bounds.");
        return error;
    }

    auto& image = images_[nextImageIndex];
    if (!image->IsValid()) {
        LOGE("Image at index was invalid.");
        return error;
    }

    if (!backbuffer->GetUsageCommandBuffer().Begin()) {
        LOGE("Could not begin recording to the command buffer.");
        return error;
    }

    VkPipelineStageFlagBits destinationPipelineStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkImageLayout destinationImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    if (!image->InsertImageMemoryBarrier(backbuffer->GetUsageCommandBuffer(), // command buffer
        currentPipelineStage_,                                            // src_pipeline_bits
        destinationPipelineStage,                                       // dest_pipeline_bits
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                             // destAccessFlags
        destinationImageLayout)) {                                          // destLayout
        LOGE("Could not insert image memory barrier.");
        return error;
    } else {
        currentPipelineStage_ = destinationPipelineStage;
    }

    if (!backbuffer->GetUsageCommandBuffer().End()) {
        LOGE("Could not end recording to the command buffer.");
        return error;
    }

    std::vector<VkSemaphore> waitSemaphores = { backbuffer->GetUsageSemaphore() };
    std::vector<VkSemaphore> signalSemaphores = {};
    std::vector<VkCommandBuffer> commandBuffers = { backbuffer->GetUsageCommandBuffer().Handle() };

    if (!device_.QueueSubmit({ destinationPipelineStage }, // waitDestPipelineStages
        waitSemaphores,                                 // waitSemaphores
        signalSemaphores,                               // signalSemaphores
        commandBuffers,                                 // commandBuffers
        backbuffer->GetUsageFence())) {                      // fence
        LOGE("Could not submit to the device queue.");
        return error;
    }

    // reset to not under multi-threading by default
    // the reality will be judged later in flush stage
    backbuffer->UnsetMultiThreading();
    sk_sp<SkSurface> surface = surfaces_[nextImageIndex];

    if (surface == nullptr) {
        LOGE("Could not access surface at the image index.");
        return error;
    }

    GrBackendRenderTarget backendRT = surface->getBackendRenderTarget(SkSurface::kFlushRead_BackendHandleAccess);
    if (!backendRT.isValid()) {
        LOGE("Could not get backend render target.");
        return error;
    }
    backendRT.setVkImageLayout(destinationImageLayout);

    currentImageIndex_ = nextImageIndex;

    return { AcquireStatus::SUCCESS, surface };
}

bool RSVulkanSwapchain::FlushCommands()
{
    if (!IsValid()) {
        LOGE("Swapchain was invalid.");
        return false;
    }

    sk_sp<SkSurface> surface = surfaces_[currentImageIndex_];
    const std::unique_ptr<RSVulkanImage>& image = images_[currentImageIndex_];
    auto backbuffer = backbuffers_[currentBackbufferIndex_].get();

    surface->flush();

    if (!backbuffer->GetRenderCommandBuffer().Begin()) {
        LOGE("Could not start recording to the command buffer.");
        return false;
    }

    VkPipelineStageFlagBits destinationPipelineStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkImageLayout destinationImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    if (!image->InsertImageMemoryBarrier(backbuffer->GetRenderCommandBuffer(), // command buffer
        currentPipelineStage_,                                             // src_pipeline_bits
        destinationPipelineStage,                                        // dest_pipeline_bits
        VK_ACCESS_MEMORY_READ_BIT,                                         // destAccessFlags
        destinationImageLayout)) {                                         // destLayout
        LOGE("Could not insert memory barrier.");
        return false;
    } else {
        currentPipelineStage_ = destinationPipelineStage;
    }

    if (!backbuffer->GetRenderCommandBuffer().End()) {
        LOGE("Could not end recording to the command buffer.");
        return false;
    }
    return true;
}

void RSVulkanSwapchain::AddToPresent()
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    toBePresent_[std::this_thread::get_id()] = this;
}

void RSVulkanSwapchain::PresentAll(RSVulkanHandle<VkFence>& shared_fence)
{
    if (toBePresent_.empty()) {
        LOGE("nothing to be presented");
        return;
    }

    std::lock_guard<std::mutex> lock(mapMutex_);
    // ---------------------------------------------------------------------------
    // Submit all the command buffer to the device queue. Tell it to signal the render
    // semaphore.
    // ---------------------------------------------------------------------------
    std::vector<VkSemaphore> waitSemaphores = {};
    std::vector<VkSemaphore> queueSignalSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSwapchainKHR> swapchains;
    std::vector<uint32_t> present_image_indices;
    queueSignalSemaphores.reserve(toBePresent_.size());
    commandBuffers.reserve(toBePresent_.size());
    swapchains.reserve(toBePresent_.size());
    present_image_indices.reserve(toBePresent_.size());
    RSVulkanSwapchain* tmpSwapChain = nullptr;
    for (const auto& entry : toBePresent_) {
        auto swapchain = entry.second;
        if (!tmpSwapChain)
            tmpSwapChain = swapchain;
        auto backbuffer = swapchain->backbuffers_[swapchain->currentBackbufferIndex_].get();
        backbuffer->SetMultiThreading();
        queueSignalSemaphores.push_back(backbuffer->GetRenderSemaphore());
        commandBuffers.push_back(backbuffer->GetRenderCommandBuffer().Handle());
        swapchains.push_back(swapchain->swapchain_);
        present_image_indices.push_back(static_cast<uint32_t>(swapchain->currentImageIndex_));
    }

    const RSVulkanProcTable& tmpVk = tmpSwapChain->vk;
    const RSVulkanDevice& device = tmpSwapChain->device_;

    if (!device.QueueSubmit({}, waitSemaphores, queueSignalSemaphores,
        commandBuffers, shared_fence)) {
        LOGE("Could not submit to the device queue");
        return;
    }

    // ----------------------------------------
    //  present multiple swapchain all at once
    // ----------------------------------------
    const VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(queueSignalSemaphores.size()),
        .pWaitSemaphores = queueSignalSemaphores.data(),
        .swapchainCount = static_cast<uint32_t>(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = present_image_indices.data(),
        .pResults = nullptr,
    };
    if (VK_CALL_LOG_ERROR(tmpVk.QueuePresentKHR(device.GetQueueHandle(), &presentInfo)) != VK_SUCCESS) {
        LOGE("Could not submit the present operation");
        return;
    }

    toBePresent_.clear();
}

bool RSVulkanSwapchain::Submit()
{
    LOGI("RSVulkanSwapchain::Submit()");
    if (!IsValid()) {
        LOGE("Swapchain was invalid.");
        return false;
    }

    sk_sp<SkSurface> surface = surfaces_[currentImageIndex_];
    const std::unique_ptr<RSVulkanImage>& image = images_[currentImageIndex_];
    auto backbuffer = backbuffers_[currentBackbufferIndex_].get();

    surface->flushAndSubmit(true);

    if (!backbuffer->GetRenderCommandBuffer().Begin()) {
        LOGE("Could not start recording to the command buffer.");
        return false;
    }

    VkPipelineStageFlagBits destinationPipelineStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkImageLayout destinationImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    if (!image->InsertImageMemoryBarrier(backbuffer->GetRenderCommandBuffer(), // command buffer
        currentPipelineStage_,                                             // src_pipeline_bits
        destinationPipelineStage,                                        // dest_pipeline_bits
        VK_ACCESS_MEMORY_READ_BIT,                                         // destAccessFlags
        destinationImageLayout)) {                                           // destLayout
        LOGE("Could not insert memory barrier.");
        return false;
    } else {
        currentPipelineStage_ = destinationPipelineStage;
    }

    if (!backbuffer->GetRenderCommandBuffer().End()) {
        LOGE("Could not end recording to the command buffer.");
        return false;
    }

    std::vector<VkSemaphore> waitSemaphores = {};
    std::vector<VkSemaphore> queueSignalSemaphores = { backbuffer->GetRenderSemaphore() };
    std::vector<VkCommandBuffer> commandBuffers = { backbuffer->GetRenderCommandBuffer().Handle() };

    if (!device_.QueueSubmit({}, // waitDestPipelineStages
        waitSemaphores,                                       // waitSemaphores
        queueSignalSemaphores,                               // signalSemaphores
        commandBuffers,                                       // commandBuffers
        backbuffer->GetRenderFence()                           // fence
        )) {
        LOGE("Could not submit to the device queue.");
        return false;
    }

    backbuffer->UnsetMultiThreading();
    // ---------------------------------------------------------------------------
    // Step 5:
    // Submit the present operation and wait on the render semaphore.
    // ---------------------------------------------------------------------------
    VkSwapchainKHR swapchain = swapchain_;
    uint32_t presentImageIndex = static_cast<uint32_t>(currentImageIndex_);
    const VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = static_cast<uint32_t>(queueSignalSemaphores.size()),
        .pWaitSemaphores = queueSignalSemaphores.data(),
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &presentImageIndex,
        .pResults = nullptr,
    };
    if (VK_CALL_LOG_ERROR(vk.QueuePresentKHR(device_.GetQueueHandle(), &presentInfo)) != VK_SUCCESS) {
        LOGE("Could not submit the present operation.");
        return false;
    }

    return true;
}

} // namespace OHOS::Rosen::vulkan
