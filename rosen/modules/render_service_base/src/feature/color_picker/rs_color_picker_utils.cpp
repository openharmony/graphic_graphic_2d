/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "feature/color_picker/rs_color_picker_utils.h"

#include "feature/color_picker/i_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_thread.h"
#include "feature/color_picker/rs_hetero_color_picker.h"

#include "common/rs_common_def.h"
#include "common/rs_optional_trace.h"
#include "draw/surface.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "image/gpu_context.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"

#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"

#endif

namespace OHOS::Rosen::RSColorPickerUtils {
namespace {
constexpr float RED_LUMINANCE_COEFF = 0.299f;
constexpr float GREEN_LUMINANCE_COEFF = 0.587f;
constexpr float BLUE_LUMINANCE_COEFF = 0.114f;

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
bool WaitFence(const sptr<SyncFence>& fence)
{
    RS_OPTIONAL_TRACE_FUNC();
    if (fence == nullptr) {
        return false;
    }
    constexpr uint32_t WAIT_TIME = 30; // ms
    int ret = fence->Wait(WAIT_TIME);
    auto fenceFd = fence->Get();
    if (ret < 0 && fenceFd != -1) {
        return false;
    }
    return true;
}

bool ExecColorPick(const std::weak_ptr<IColorPickerManager>& weakManager, ColorPickerInfo& info)
{
    RS_OPTIONAL_TRACE_NAME("ColorPicker::ExecColorPick");
    auto manager = weakManager.lock();
    if (!manager) {
        RS_LOGE("ColorPicker: manager is null");
        return false;
    }

    std::shared_ptr<Drawing::GPUContext> gpuCtx;
    gpuCtx = RSColorPickerThread::Instance().GetShareGPUContext();
    if (!gpuCtx || !info.oldImage_) {
        RS_LOGE("ColorPicker: invalid GPU context or image");
        return false;
    }

    // Wait for GPU to finish writing to the texture before accessing it
    sptr<SyncFence> fence = new SyncFence(info.fenceFd_);
    if (!WaitFence(fence)) {
        RS_LOGE("ColorPicker: fence wait failed");
        return false;
    }

    auto image = std::make_shared<Drawing::Image>();
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    if (!image->BuildFromTexture(
        *gpuCtx, info.backendTexture_.GetTextureInfo(), origin, info.bitmapFormat_, info.colorSpace_)) {
        RS_LOGE("ColorPicker: BuildFromTexture failed");
        return false;
    }

    Drawing::ColorQuad colorPicked;
    if (RSPropertyDrawableUtils::PickColor(gpuCtx, image, colorPicked)) {
        manager->HandleColorUpdate(colorPicked);
        return true;
    }
    RS_LOGE("ColorPicker: PickColor failed");
    return false;
}
#endif
} // namespace

std::unique_ptr<ColorPickerInfo> CreateColorPickerInfo(Drawing::Surface* drawingSurface,
    const std::shared_ptr<Drawing::Image>& snapshot, std::weak_ptr<IColorPickerManager> manager)
{
    if (!drawingSurface || !snapshot) {
        RS_LOGE("CreateColorPickerInfo: drawingSurface or snapshot is null");
        return nullptr;
    }
    Drawing::TextureOrigin origin = Drawing::TextureOrigin::BOTTOM_LEFT;
    auto backendTexture = snapshot->GetBackendTexture(false, &origin);
    if (!backendTexture.IsValid()) {
        RS_LOGE("CreateColorPickerInfo: backendTexture is invalid");
        return nullptr;
    }

    auto imageInfo = drawingSurface->GetImageInfo();
    auto colorSpace = imageInfo.GetColorSpace();
    Drawing::BitmapFormat bitmapFormat = { imageInfo.GetColorType(), imageInfo.GetAlphaType() };
    return std::make_unique<ColorPickerInfo>(colorSpace, bitmapFormat, backendTexture, snapshot, manager);
}

float CalculateLuminance(Drawing::ColorQuad color)
{
    auto red = Drawing::Color::ColorQuadGetR(color);
    auto green = Drawing::Color::ColorQuadGetG(color);
    auto blue = Drawing::Color::ColorQuadGetB(color);
    return red * RED_LUMINANCE_COEFF + green * GREEN_LUMINANCE_COEFF + blue * BLUE_LUMINANCE_COEFF;
}

void ScheduleColorPickWithSemaphore(Drawing::Surface& surface, std::weak_ptr<IColorPickerManager> weakManager,
    std::unique_ptr<ColorPickerInfo> info, Drawing::GPUContext& gpuCtx)
{
    RS_OPTIONAL_TRACE_NAME("ColorPicker::ScheduleColorPickWithSemaphore");
    if (!info) {
        RS_LOGE("ScheduleColorPickWithSemaphore: ColorPickerInfo is null");
        return;
    }

    // Create semaphore and fence for GPU task chaining
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    VkSemaphore semaphore;
    NativeBufferUtils::CreateVkSemaphore(semaphore);
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore);
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    auto* destroyInfo = new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);

    Drawing::FlushInfo flushInfo;
    flushInfo.backendSurfaceAccess = true;
    flushInfo.numSemaphores = 1;
    flushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
    flushInfo.finishedProc = [](void* context) { DestroySemaphoreInfo::DestroySemaphore(context); };
    flushInfo.finishedContext = destroyInfo;
    surface.Flush(&flushInfo);
    gpuCtx.Submit();
    // Get fence fd from semaphore right after flush
    NativeBufferUtils::GetFenceFdFromSemaphore(semaphore, info->fenceFd_);
    if (info->fenceFd_ == -1) {
        RS_LOGE("ScheduleColorPickWithSemaphore: failed to get fence fd from semaphore");
        DestroySemaphoreInfo::DestroySemaphore(destroyInfo);
        return;
    }

    // Post task directly to ColorPickerThread with fence for GPU synchronization
    auto infoPtr = info.release();
    RSColorPickerThread::Instance().PostTask([infoPtr, destroyInfo]() {
        ExecColorPick(infoPtr->manager_, *infoPtr);
        DestroySemaphoreInfo::DestroySemaphore(destroyInfo); // semaphore inits with ref count = 2
        delete infoPtr;
        }, false);
#else
    return;
#endif
}

std::pair<Drawing::ColorQuad, Drawing::ColorQuad> GetColorForPlaceholder(ColorPlaceholder placeholder)
{
    static const std::unordered_map<ColorPlaceholder, std::pair<Drawing::ColorQuad, Drawing::ColorQuad>>
        PLACEHOLDER_TO_COLOR {
#define COLOR_PLACEHOLDER_ENTRY(name, dark, light) \
            { ColorPlaceholder::name, { dark, light } },
#include "feature/color_picker/rs_color_placeholder_mapping_def.in"
#undef COLOR_PLACEHOLDER_ENTRY
        };
    if (auto it = PLACEHOLDER_TO_COLOR.find(placeholder); it != PLACEHOLDER_TO_COLOR.end()) {
        return it->second;
    }
    return std::make_pair(Drawing::Color::COLOR_BLACK, Drawing::Color::COLOR_WHITE);
}

Drawing::ColorQuad InterpolateColor(Drawing::ColorQuad start, Drawing::ColorQuad end, float fraction)
{
    if (ROSEN_NE(std::clamp(fraction, 0.f, 1.f), fraction)) {
        return end;
    }
    auto lerp = [](uint32_t a, uint32_t b, float t) {
        return static_cast<uint32_t>(static_cast<float>(a) + (static_cast<float>(b) - static_cast<float>(a)) * t);
    };
    auto a =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetA(start), Drawing::Color::ColorQuadGetA(end), fraction));
    auto r =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetR(start), Drawing::Color::ColorQuadGetR(end), fraction));
    auto g =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetG(start), Drawing::Color::ColorQuadGetG(end), fraction));
    auto b =
        static_cast<uint32_t>(lerp(Drawing::Color::ColorQuadGetB(start), Drawing::Color::ColorQuadGetB(end), fraction));
    return Drawing::Color::ColorQuadSetARGB(a, r, g, b);
}

// render thread
bool ExtractSnapshotAndScheduleColorPick(
    RSPaintFilterCanvas& canvas, const Drawing::Rect* rect, const std::shared_ptr<IColorPickerManager>& manager)
{
    RS_OPTIONAL_TRACE_NAME("ColorPicker::ExtractSnapshotAndScheduleColorPick");
    if (!rect) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: rect is null");
        return false;
    }

    canvas.Save();
    canvas.ClipRect(*rect, Drawing::ClipOp::INTERSECT, false);
    Drawing::RectI snapshotIBounds = canvas.GetRoundInDeviceClipBounds();
    canvas.Restore();

    auto drawingSurface = canvas.GetSurface();
    if (!drawingSurface) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: surface is null");
        return false;
    }

#ifdef RS_ENABLE_GPU
    RSTagTracker tracker(canvas.GetGPUContext(), RSTagTracker::TAG_COLOR_PICKER_SNAPSHOT);
#endif
    auto snapshot = drawingSurface->GetImageSnapshot(snapshotIBounds, false);
    if (!snapshot) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: snapshot is null");
        return false;
    }

    // Try accelerated (hetero) color picker first
    if (RSHeteroColorPicker::Instance().GetColor(
        [manager](Drawing::ColorQuad& newColor) { manager->HandleColorUpdate(newColor); }, canvas, snapshot)) {
        return true;
    }

    // Fall back to standard color picker with semaphore
#ifdef RS_ENABLE_GPU
    auto gpuCtx = canvas.GetGPUContext();
#else
    std::shared_ptr<Drawing::GPUContext> gpuCtx = nullptr;
#endif
    if (!gpuCtx) {
        RS_LOGE("ExtractSnapshotAndScheduleColorPick: gpuContext is null");
        return false;
    }
    auto weakManager = std::weak_ptr<IColorPickerManager>(manager);
    auto colorPickerInfo = CreateColorPickerInfo(drawingSurface, snapshot, weakManager);
    ScheduleColorPickWithSemaphore(*drawingSurface, weakManager, std::move(colorPickerInfo), *gpuCtx);
    return true;
}

std::unordered_set<NodeId> CollectColorPickerNodeIds(
    const std::vector<std::shared_ptr<RSRenderNode>>& surfaces, const RSRenderNodeMap& nodeMap)
{
    RS_OPTIONAL_TRACE_NAME("ColorPicker::CollectColorPickerNodeIds");
    std::unordered_set<NodeId> colorPickerNodeIds;
    std::for_each(surfaces.begin(), surfaces.end(), [&nodeMap, &colorPickerNodeIds](auto& node) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node);
        if (!surfaceNode) {
            return;
        }
        for (auto& child : surfaceNode->GetVisibleFilterChild()) {
            auto filterNode = nodeMap.GetRenderNode<RSRenderNode>(child);
            if (filterNode && filterNode->GetColorPickerDrawable()) {
                colorPickerNodeIds.insert(child);
            }
        }
    });
    return colorPickerNodeIds;
}

bool IsColorPickerDirty(const RSRenderNode& filterNode, const std::vector<std::shared_ptr<RSRenderNode>>& surfaces)
{
    auto drawable = filterNode.GetColorPickerDrawable();
    if (!drawable || drawable->GetState() != DrawableV2::ColorPickerState::PREPARING) {
        return false;
    }
    const RectI& nodeAbsRect = filterNode.GetAbsRect();
    RectI colorPickerRect = nodeAbsRect;
    // handle custom rect
    auto params = filterNode.GetRenderProperties().GetColorPicker();
    if (params && params->rect.has_value()) {
        const auto& customRect = params->rect.value();
        colorPickerRect.SetAll(static_cast<int32_t>(customRect.GetLeft()), static_cast<int32_t>(customRect.GetTop()),
            static_cast<int32_t>(customRect.GetWidth()), static_cast<int32_t>(customRect.GetHeight()));
        colorPickerRect.Move(nodeAbsRect.left_, nodeAbsRect.top_);
    }

    for (auto it = surfaces.rbegin(); it != surfaces.rend(); ++it) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (!surfaceNode) {
            continue;
        }
        auto dirtyManager = surfaceNode->GetDirtyManager();
        if (!dirtyManager) {
            continue;
        }

        const auto dirtyRegion = dirtyManager->GetCurrentFrameDirtyRegion();
        if (dirtyRegion.Intersect(colorPickerRect)) {
            RS_OPTIONAL_TRACE_NAME_FMT("color picker rect %s intersects with dirty surface %s, dirty rect = %s",
                colorPickerRect.ToString().c_str(), surfaceNode->GetName().c_str(), dirtyRegion.ToString().c_str());
            return true;
        }
        if (surfaceNode == filterNode.GetInstanceRootNode()) {
            break; // only check surfaces below the node
        }
    }
    return false;
}
} // namespace OHOS::Rosen::RSColorPickerUtils
