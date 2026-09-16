/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_uni_hwc_prevalidate_util.h"

#include <dlfcn.h>
#include <memory>

#include "common/rs_singleton.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "engine/rs_base_render_util.h"
#include "feature/hwc/rs_uni_hwc_compute_util.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_param_util.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "utils/rect.h"

#undef LOG_TAG
#define LOG_TAG "RSUniHwcPrevalidateUtil"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t ROTATION_360 = 360;
constexpr uint64_t USAGE_SOLID_LAYER_ENABLE = 1ULL << 59;
constexpr uint64_t USAGE_HARDWARE_CURSOR = 1ULL << 61;
constexpr uint64_t USAGE_UNI_LAYER = 1ULL << 60;
constexpr uint64_t USAGE_NONE_PREMULTIPLIED = 1ULL << 62;
constexpr uint64_t USAGE_DUAL_PREVIEW = 1ULL << 50;
constexpr uint64_t USAGE_UNI_RENDER_LAYER_NULL = 1ULL << 51;

// RAII wrapper for dlopen handles. Calling dlclose automatically on scope exit
struct DlCloser {
    void operator()(void* handle) const {
        if (handle) {
            dlclose(handle);
        }
    }
};
using DlHandle = std::unique_ptr<void, DlCloser>;

inline void LogPrevalidateLayerInfo(const char* nodeName, uint64_t nodeId,
    const RequestLayerInfo& info, const RSScreenProperty& screenProperty)
{
    RS_LOGD_IF(DEBUG_PREVALIDATE, "PrevalidateLayerInfo %{public}s, %{public}" PRIu64
        ", src: %{public}u,%{public}u,%{public}u,%{public}u"
        " dst: %{public}u,%{public}u,%{public}u,%{public}u, z: %{public}" PRIu32 ", bufferUsage: %{public}" PRIu64 ","
        " layerUsage: %{public}" PRIu64 ", format: %{public}d, transform: %{public}d, fps: %{public}d,"
        " isRogResolution: %{public}d, isHdiRogEnable: %{public}d, rogRatio: %{public}f, %{public}f",
        nodeName, nodeId, info.srcRect.x, info.srcRect.y, info.srcRect.w, info.srcRect.h,
        info.dstRect.x, info.dstRect.y, info.dstRect.w, info.dstRect.h, info.zOrder, info.bufferUsage, info.layerUsage,
        info.format, info.transform, info.fps, screenProperty.IsRogResolution(), screenProperty.GetHdiRogEnable(),
        screenProperty.GetRogWidthRatio(), screenProperty.GetRogHeightRatio());
}
}
RSUniHwcPrevalidateUtil& RSUniHwcPrevalidateUtil::GetInstance()
{
    static RSUniHwcPrevalidateUtil instance;
    return instance;
}

RSUniHwcPrevalidateUtil::RSUniHwcPrevalidateUtil()
{
    DlHandle handle(dlopen("libprevalidate_client.z.so", RTLD_NOW));
    if (!handle) {
        RS_LOGW("Load library failed, reason: %{public}s", dlerror());
        return;
    }
    PreValidateInitFunc initFunc = reinterpret_cast<PreValidateInitFunc>(dlsym(handle.get(), "InitPrevalidate"));
    if ((initFunc == nullptr) || (initFunc() != 0)) {
        RS_LOGW("Prevalidate init failed");
        return;
    }
    auto preValidateFunc = reinterpret_cast<PreValidateFunc>(dlsym(handle.get(), "RequestLayerStrategy"));
    auto handleEventFunc = reinterpret_cast<HandleEventFunc>(dlsym(handle.get(), "HandleHWCEvent"));
    if (preValidateFunc == nullptr || handleEventFunc == nullptr) {
        RS_LOGW("Load func failed, reason: %{public}s", dlerror());
        return;
    }
    // GetVcldEnabled is an optional symbol: its absence is not a load failure;
    auto getVcldEnabledInfoFunc = reinterpret_cast<GetVcldEnabledInfoFunc>(dlsym(handle.get(), "GetVcldEnabled"));
    if (getVcldEnabledInfoFunc != nullptr) {
        bool isVcldEnabled = false;
        (void)getVcldEnabledInfoFunc(isVcldEnabled);
        RSUniHwcPrevalidateParamUtil::SetIsVcldEnabled(isVcldEnabled);
        RS_LOGI("Load vcld enable success: %{public}d", isVcldEnabled);
    }
    preValidateFunc_ = preValidateFunc;
    handleEventFunc_ = handleEventFunc;
    loadSuccess_ = true;
    preValidateHandle_ = handle.release(); // transfer ownership to the member on success
    RS_LOGI("Load success");
}

void RSUniHwcPrevalidateUtil::HandleHwcEvent(
    uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    if (!handleEventFunc_) {
        RS_LOGI("RSUniHwcPrevalidateUtil::HandleEvent handleEventFunc is null");
        return;
    }
    RS_LOGI("RSUniHwcPrevalidateUtil::HandleEvent deviceId:%{public}" PRIu32 ", eventId:%{public}" PRIu32 "",
        deviceId, eventId);
    handleEventFunc_(deviceId, eventId, eventData);
}

bool RSUniHwcPrevalidateUtil::IsPrevalidateEnable()
{
    return loadSuccess_;
}

bool RSUniHwcPrevalidateUtil::PreValidate(
    ScreenId id, std::vector<RequestLayerInfo> infos, std::map<uint64_t, RequestCompositionType> &strategy)
{
    if (!preValidateFunc_) {
        RS_LOGI_IF(DEBUG_PREVALIDATE, "PreValidate preValidateFunc is null");
        return false;
    }
    int32_t ret = preValidateFunc_(id, infos, strategy);
    return ret == 0;
}

bool RSUniHwcPrevalidateUtil::CreateSurfaceNodeLayerInfo(uint32_t& zOrder,
    RSSurfaceRenderNode::SharedPtr node,
    const RSScreenProperty& screenProperty, RequestLayerInfo &info)
{
    if (!node || !node->GetRSSurfaceHandler()->GetConsumer()) {
        return false;
    }
    auto buffer = node->GetRSSurfaceHandler()->GetBuffer();
    if (!buffer) {
        return false;
    }
    auto params = static_cast<RSSurfaceRenderParams *>(node->GetStagingRenderParams().get());
    if (params == nullptr) {
        return false;
    }
    node->SetDeviceOfflineEnable(false);
    uint32_t appZOrder = node->IsHardwareEnabledTopSurface() ? node->GetAppWindowZOrder() : zOrder++;
    auto transform = RSUniHwcComputeUtil::GetLayerTransform(*node);
    uint32_t fps = screenProperty.GetRefreshRate();
    FillCommonFields(info, node->GetId(), appZOrder, fps, buffer);
    info.bufferHandle = buffer->GetBufferHandle();
    info.transform = static_cast<int>(transform);
    ComputeSurfaceSrcRect(info, node, buffer);
    ComputeSurfaceDstRect(info, node, screenProperty);
    UpdateSurfaceLayerUsage(node, info, params->GetIsHwcEnabledBySolidLayer());
    RSUniHwcPrevalidateParamUtil::ApplySourceCropTuning(buffer, info);
    RSUniHwcPrevalidateParamUtil::ApplyArsrEnhance(node, info);
    RSUniHwcPrevalidateParamUtil::ApplyVcldParam(node->GetVcldInfo(), info);
    RSUniHwcPrevalidateParamUtil::ApplyCopybit(buffer, transform, info, node);
    RSUniHwcPrevalidateParamUtil::ApplyLayerLinearMatrix(params->GetLayerLinearMatrix(), info);
    LogPrevalidateLayerInfo(node->GetName().c_str(), node->GetId(), info, screenProperty);
    return true;
}

bool RSUniHwcPrevalidateUtil::CreateScreenNodeLayerInfo(uint32_t zorder,
    RSScreenRenderNode::SharedPtr node, const RSScreenProperty& screenProperty, RequestLayerInfo &info)
{
    if (!node) {
        return false;
    }
    auto drawable = node->GetRenderDrawable();
    if (!drawable) {
        return false;
    }
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
    if (!surfaceHandler) {
        return false;
    }
    auto buffer = surfaceHandler->GetBuffer();
    if (!buffer || !surfaceHandler->GetConsumer()) {
        return false;
    }
    uint32_t fps = screenProperty.GetRefreshRate();
    FillCommonFields(info, node->GetId(), zorder, fps, buffer);
    info.srcRect = {0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight()};
    if (screenProperty.GetHdiRogEnable()) {
        info.dstRect = {0, 0, screenProperty.GetWidth(), screenProperty.GetHeight()};
    } else {
        info.dstRect = {0, 0, screenProperty.GetPhyWidth(), screenProperty.GetPhyHeight()};
    }
    bool isUniRenderLayerNull = node->GetDynamicLayerSkipController()->MeetsPreliminarySkipCriteria();
    UpdateScreenUsage(screenProperty.GetDualScreenState(), isUniRenderLayerNull, info);
    LayerRotate(info, surfaceHandler->GetConsumer());
    LogPrevalidateLayerInfo("ScreenNode", node->GetId(), info, screenProperty);
    return true;
}

bool RSUniHwcPrevalidateUtil::CreateRCDLayerInfo(RSRcdSurfaceRenderNode::SharedPtr node,
    const RSScreenProperty& screenProperty, RequestLayerInfo &info)
{
    if (!node || !node->GetConsumer()) {
        return false;
    }
    auto buffer = node->GetBuffer();
    if (!buffer) {
        return false;
    }

    uint32_t fps = screenProperty.GetRefreshRate();
    FillCommonFields(info, node->GetId(), static_cast<uint32_t>(node->GetGlobalZOrder()), fps, buffer);
    auto src = node->GetSrcRect();
    info.srcRect = {src.left_, src.top_, src.width_, src.height_};
    auto dst = node->GetDstRect();
    float widthRatio = screenProperty.GetRogWidthRatio();
    float heightRatio = screenProperty.GetRogHeightRatio();
    info.dstRect.x = static_cast<uint32_t>(static_cast<float>(dst.left_) * widthRatio);
    info.dstRect.y = static_cast<uint32_t>(static_cast<float>(dst.top_) * heightRatio);
    info.dstRect.w = static_cast<uint32_t>(static_cast<float>(dst.width_) * widthRatio);
    info.dstRect.h = static_cast<uint32_t>(static_cast<float>(dst.height_) * heightRatio);
    CopyCldInfo(node->GetCldInfo(), info);
    LayerRotate(info, node->GetConsumer());
    LogPrevalidateLayerInfo("RCDNode", node->GetId(), info, screenProperty);
    return true;
}

std::vector<RequestLayerInfo> RSUniHwcPrevalidateUtil::CollectLayerInfo(
    const RSScreenRenderNode::SharedPtr screenRenderNode, uint32_t zOrder)
{
    std::vector<RequestLayerInfo> prevalidLayers;
    if (!screenRenderNode) {
        return prevalidLayers;
    }
    const auto& screenProperty = screenRenderNode->GetScreenProperty();
    CollectSurfaceNodeLayerInfo(prevalidLayers, screenRenderNode, zOrder, screenProperty);
    if (prevalidLayers.empty()) {
        return prevalidLayers;
    }
    RequestLayerInfo screenLayer;
    if (CreateScreenNodeLayerInfo(zOrder++, screenRenderNode, screenProperty, screenLayer)) {
        prevalidLayers.emplace_back(screenLayer);
    }
    AddRcdLayers(prevalidLayers, screenRenderNode->GetId(), screenProperty);
    return prevalidLayers;
}

void RSUniHwcPrevalidateUtil::CollectSurfaceNodeLayerInfo(std::vector<RequestLayerInfo>& prevalidLayers,
    const RSScreenRenderNode::SharedPtr screenRenderNode, uint32_t& zOrder,
    const RSScreenProperty& screenProperty)
{
    if (!screenRenderNode) {
        return;
    }
    auto collectSurfaceNodeLayer = [&prevalidLayers, &zOrder, &screenProperty](
        const RSSurfaceRenderNode::SharedPtr& node) {
        RequestLayerInfo surfaceLayer;
        if (CreateSurfaceNodeLayerInfo(zOrder, node, screenProperty, surfaceLayer)) {
            prevalidLayers.emplace_back(surfaceLayer);
        }
    };
    auto& surfaceNodes = screenRenderNode->GetAllMainAndLeashSurfaces();
    for (auto it = surfaceNodes.rbegin(); it != surfaceNodes.rend(); it++) {
        auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(*it);
        if (surfaceNode && CheckHwcNode(surfaceNode)) {
            collectSurfaceNodeLayer(surfaceNode);
        }
    }
    for (const auto& hwcNode : screenRenderNode->GetChildHwcNodes()) {
        auto hwcNodePtr = hwcNode.lock();
        if (CheckHwcNode(hwcNodePtr)) {
            collectSurfaceNodeLayer(hwcNodePtr);
        }
    }
}

void RSUniHwcPrevalidateUtil::AddRcdLayers(std::vector<RequestLayerInfo>& prevalidLayers,
    NodeId screenId, const RSScreenProperty& screenProperty)
{
    if (!RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        return;
    }
    RequestLayerInfo rcdLayer;
    auto rcdSurface = RSRcdRenderManager::GetInstance().GetBottomSurfaceNode(screenId);
    if (CreateRCDLayerInfo(rcdSurface, screenProperty, rcdLayer)) {
        prevalidLayers.emplace_back(rcdLayer);
    }
    rcdSurface = RSRcdRenderManager::GetInstance().GetTopSurfaceNode(screenId);
    if (CreateRCDLayerInfo(rcdSurface, screenProperty, rcdLayer)) {
        prevalidLayers.emplace_back(rcdLayer);
    }
}

bool RSUniHwcPrevalidateUtil::CheckHwcNode(const RSSurfaceRenderNode::SharedPtr& node)
{
    if (!node || !node->IsOnTheTree()) {
        return false;
    }
    auto isHardCursor = node->GetHardCursorStatus();
    if ((!isHardCursor && node->IsHardwareForcedDisabled()) || node->GetAncoForceDoDirect()) {
        return false;
    }
    return true;
}

void RSUniHwcPrevalidateUtil::LayerRotate(RequestLayerInfo& info, const sptr<IConsumerSurface>& surface)
{
    if (!surface) {
        return;
    }
    int totalRotation = RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(
        surface->GetTransform()));
    GraphicTransformType rotateEnum = RSBaseRenderUtil::RotateEnumToInt(totalRotation,
        RSBaseRenderUtil::GetFlipTransform(surface->GetTransform()));
    info.transform = rotateEnum;
}

void RSUniHwcPrevalidateUtil::CopyCldInfo(const CldInfo& src, RequestLayerInfo& info)
{
    info.cldInfo.cldDataOffset = src.cldDataOffset;
    info.cldInfo.cldSize = src.cldSize;
    info.cldInfo.cldWidth = src.cldWidth;
    info.cldInfo.cldHeight = src.cldHeight;
    info.cldInfo.cldStride = src.cldStride;
    info.cldInfo.exWidth = src.exWidth;
    info.cldInfo.exHeight = src.exHeight;
    info.cldInfo.baseColor = src.baseColor;
}

void RSUniHwcPrevalidateUtil::FillCommonFields(RequestLayerInfo& info, uint64_t id, uint32_t zOrder,
    uint32_t fps, const sptr<SurfaceBuffer>& buffer)
{
    info.id = id;
    info.zOrder = zOrder;
    info.bufferUsage = buffer->GetUsage();
    info.format = buffer->GetFormat();
    info.fps = fps;
}

void RSUniHwcPrevalidateUtil::ComputeSurfaceSrcRect(RequestLayerInfo& info,
    const RSSurfaceRenderNode::SharedPtr node, const sptr<SurfaceBuffer>& buffer)
{
    auto src = node->GetSrcRect();
    Rect crop{0, 0, 0, 0};
    auto bufferWidth = buffer->GetWidth();
    auto bufferHeight = buffer->GetHeight();
    if (buffer->GetCropMetadata(crop) && bufferWidth > 0 && bufferHeight > 0) {
        float scaleX = static_cast<float>(crop.w) / bufferWidth;
        float scaleY = static_cast<float>(crop.h) / bufferHeight;
        info.srcRect = {
            static_cast<uint32_t>(std::max(0.f, std::ceil(src.left_ * scaleX))),
            static_cast<uint32_t>(std::max(0.f, std::ceil(src.top_ * scaleY))),
            static_cast<uint32_t>(std::max(0.f, std::floor(src.width_ * scaleX))),
            static_cast<uint32_t>(std::max(0.f, std::floor(src.height_ * scaleY)))
        };
    } else {
        info.srcRect = {
            static_cast<uint32_t>(std::max(0, src.left_)),
            static_cast<uint32_t>(std::max(0, src.top_)),
            static_cast<uint32_t>(std::max(0, src.width_)),
            static_cast<uint32_t>(std::max(0, src.height_))
        };
    }
}

void RSUniHwcPrevalidateUtil::ComputeSurfaceDstRect(RequestLayerInfo& info,
    const RSSurfaceRenderNode::SharedPtr node, const RSScreenProperty& screenProperty)
{
    auto dst = node->GetDstRect();
    if (!screenProperty.IsRogResolution() || screenProperty.GetHdiRogEnable() || node->IsHardwareEnabledTopSurface()) {
        info.dstRect = {
            static_cast<uint32_t>(std::max(0, dst.left_)),
            static_cast<uint32_t>(std::max(0, dst.top_)),
            static_cast<uint32_t>(std::max(0, dst.width_)),
            static_cast<uint32_t>(std::max(0, dst.height_))
        };
    } else {
        auto rogWidthRatio = screenProperty.GetRogWidthRatio();
        auto rogHeightRatio = screenProperty.GetRogHeightRatio();
        Drawing::Rect adjustedDstRect(
            std::floor(dst.GetLeft() * rogWidthRatio),
            std::floor(dst.GetTop() * rogHeightRatio),
            std::ceil(dst.GetRight() * rogWidthRatio),
            std::ceil(dst.GetBottom() * rogHeightRatio));
        Drawing::Rect screenRect(0.f, 0.f,
            static_cast<float>(screenProperty.GetPhyWidth()),
            static_cast<float>(screenProperty.GetPhyHeight()));
        adjustedDstRect.Intersect(screenRect);
        info.dstRect = {
            static_cast<uint32_t>(std::max(0.f, adjustedDstRect.GetLeft())),
            static_cast<uint32_t>(std::max(0.f, adjustedDstRect.GetTop())),
            static_cast<uint32_t>(adjustedDstRect.GetWidth()),
            static_cast<uint32_t>(adjustedDstRect.GetHeight())
        };
    }
}

void RSUniHwcPrevalidateUtil::UpdateSurfaceLayerUsage(const RSSurfaceRenderNode::SharedPtr node,
    RequestLayerInfo& info, bool isHwcEnabledBySolidLayer)
{
    if (node->IsHardwareEnabledTopSurface() && node->GetHardCursorStatus()) {
        info.layerUsage |= USAGE_HARDWARE_CURSOR;
    }
    if (isHwcEnabledBySolidLayer) {
        info.layerUsage |= USAGE_SOLID_LAYER_ENABLE;
    }
    if (node->GetBlendType() != GraphicBlendType::GRAPHIC_BLEND_SRCOVER) {
        info.layerUsage |= USAGE_NONE_PREMULTIPLIED;
    }
}

void RSUniHwcPrevalidateUtil::UpdateScreenUsage(const DualScreenStatus dualScreenState,
    const bool isUniRenderLayerNull, RequestLayerInfo& info)
{
    info.layerUsage = info.layerUsage | USAGE_UNI_LAYER;
    if (dualScreenState == DualScreenStatus::DUAL_SCREEN_ENTER) {
        info.layerUsage |= USAGE_DUAL_PREVIEW;
    }
    if (isUniRenderLayerNull) {
        info.layerUsage |= USAGE_UNI_RENDER_LAYER_NULL;
    }
}
} //Rosen
} //OHOS