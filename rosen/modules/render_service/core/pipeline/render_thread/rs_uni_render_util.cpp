/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_uni_render_util.h"

#include <cstdint>
#include <memory>
#include <parameter.h>
#include <parameters.h>
#include <string>
#include <unordered_set>

#include "rs_trace.h"
#include "scene_board_judgement.h"

#include "common/rs_optional_trace.h"
#include "common/rs_rectangles_merger.h"
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/anco_manager/rs_anco_manager.h"
#include "feature/dirty/rs_uni_dirty_compute_util.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "memory/rs_tag_tracker.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"
#include "rs_base_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_maskcolor_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"

#ifdef RS_ENABLE_VK
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#else
#include "include/gpu/GrBackendSurface.h"
#endif
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_hdr_vulkan_task.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "render_frame_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr float GAMMA2_2 = 2.2f;
constexpr int64_t PERF_TIME_OUT = 950;
constexpr uint32_t PERF_LEVEL_INTERVAL = 10;
constexpr uint32_t PERF_LAYER_START_NUM = 12;
constexpr uint32_t PERF_LEVEL_0 = 0;
constexpr uint32_t PERF_LEVEL_1 = 1;
constexpr uint32_t PERF_LEVEL_2 = 2;
constexpr int32_t PERF_LEVEL_1_REQUESTED_CODE = 10013;
constexpr int32_t PERF_LEVEL_2_REQUESTED_CODE = 10014;
constexpr int32_t PERF_LEVEL_3_REQUESTED_CODE = 10015;
void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSProcessor::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
}
void RSUniRenderUtil::ExpandDamageRegionToSingleRect(Occlusion::Region& damageRegion)
{
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    if (clipRectThreshold < 1.f) {
        auto bound = damageRegion.GetBound();
        // Multi-rects damage region will lead to clip path, which is performance-affecting.
        // Within reasonable threshold, consider expanding multi-rects into one single rect for performance improvement.
        if (damageRegion.GetSize() > 1 && !bound.IsEmpty() && damageRegion.Area() > bound.Area() * clipRectThreshold) {
            RS_OPTIONAL_TRACE_NAME_FMT("dirty expand: %s to %s",
                damageRegion.GetRegionInfo().c_str(), bound.GetRectInfo().c_str());
            damageRegion = Occlusion::Region { bound };
        }
    }
}

std::vector<RectI> RSUniRenderUtil::MergeDirtyHistory(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx, RSScreenRenderParams& params)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    // renderThreadParams/dirtyManager not null in caller
    auto dirtyManager = screenDrawable.GetSyncDirtyManager();
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(screenDrawable, bufferAge, params, false);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(
        curAllSurfaceDrawables, RSUniRenderThread::Instance().GetDrawStatusVec());

    RectI screenRectI(0, 0, static_cast<int32_t>(screenInfo.phyWidth), static_cast<int32_t>(screenInfo.phyHeight));

    Occlusion::Region globalDirtyRegion;
    for (const auto& rect : dirtyManager->GetAdvancedDirtyRegion()) {
        Occlusion::Region region = Occlusion::Region(Occlusion::Rect(rect));
        globalDirtyRegion.OrSelf(region);
        GpuDirtyRegionCollection::GetInstance().UpdateGlobalDirtyInfoForDFX(rect.IntersectRect(screenRectI));
    }
    rsDirtyRectsDfx.SetDirtyRegion(dirtyRegion);
    Occlusion::Region damageRegion;
    RS_TRACE_NAME_FMT("AdvancedDirtyRegionType is [%d]", static_cast<int>(uniParam->GetAdvancedDirtyType()));
    switch (uniParam->GetAdvancedDirtyType()) {
        case AdvancedDirtyRegionType::DISABLED:
            damageRegion = dirtyRegion.Or(globalDirtyRegion);
            break;
        case AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY:
            damageRegion = RSUniRenderUtil::MergeDirtyRects(dirtyRegion.Or(globalDirtyRegion));
            break;
        case AdvancedDirtyRegionType::SET_ADVANCED_DISPLAY:
            damageRegion = RSUniRenderUtil::MergeDirtyRects(globalDirtyRegion);
            damageRegion.OrSelf(dirtyRegion);
            break;
        default:
            damageRegion = dirtyRegion.Or(globalDirtyRegion);
            RS_LOGI("RSUniRenderUtil::MergeDirtyHistory unsupported advanced dirty region type");
            RS_TRACE_NAME_FMT("RSUniRenderUtil::MergeDirtyHistory unsupported advanced dirty region type");
            break;
    }
    if (!uniParam->IsDirtyAlignEnabled()) {
        ExpandDamageRegionToSingleRect(damageRegion);
    }
    // [Attention]: Filter dirty must be the last. If sampling is needed, sample after filter dirty processing.
    Occlusion::Region drawnRegion;
    if (screenInfo.isSamplingOn && screenInfo.samplingScale > 0) {
        GetSampledDamageAndDrawnRegion(screenInfo, damageRegion, uniParam->IsDirtyAlignEnabled(),
            damageRegion, drawnRegion);
    } else {
        drawnRegion = uniParam->IsDirtyAlignEnabled() ?
            damageRegion.GetAlignedRegion(MAX_DIRTY_ALIGNMENT_SIZE) : damageRegion;
        RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(
            damageRegion, drawnRegion, screenDrawable, std::nullopt, uniParam->IsDirtyAlignEnabled());
    }
    // [Attention]: filter dirty process must be the last step.
    RSUniFilterDirtyComputeUtil::DealWithFilterDirtyRegion(
        damageRegion, drawnRegion, screenDrawable, std::nullopt, uniParam->IsDirtyAlignEnabled());
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    // overlay display expand dirty region
    RSOverlayDisplayManager::Instance().ExpandDirtyRegion(*dirtyManager, screenInfo, drawnRegion, damageRegion);
#endif
    RSUniRenderUtil::SetDrawRegionForQuickReject(curAllSurfaceDrawables, drawnRegion);
    rsDirtyRectsDfx.SetMergedDirtyRegion(drawnRegion);
    params.SetDrawnRegion(drawnRegion);
    auto damageRegionRects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(damageRegion, screenInfo);
    if (damageRegionRects.empty()) {
        // When damageRegionRects is empty, SetDamageRegion function will not take effect and buffer will
        // full screen refresh. Therefore, we need to insert an empty rect into the damageRegionRects array
        damageRegionRects.emplace_back(RectI(0, 0, 0, 0));
    }
    return damageRegionRects;
}

std::vector<RectI> RSUniRenderUtil::MergeDirtyHistoryInVirtual(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, int32_t bufferAge,
    const ScreenInfo& screenInfo, bool isSecScreen)
{
    auto params = static_cast<RSScreenRenderParams*>(screenDrawable.GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams || !params) {
        return {};
    }
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    auto dirtyManager = screenDrawable.GetSyncDirtyManager();
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(screenDrawable, bufferAge);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(
        curAllSurfaceDrawables, *params, isSecScreen);

    RectI rect = dirtyManager->GetRectFlipWithinSurface(dirtyManager->GetDirtyRegionInVirtual());
    auto rects = RSUniDirtyComputeUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }
    if (screenInfo.isSamplingOn && screenInfo.samplingScale > 0) {
        std::vector<RectI> dstDamageRegionrects;
        for (const auto& rect : rects) {
            Drawing::Matrix scaleMatrix;
            scaleMatrix.SetScaleTranslate(screenInfo.samplingScale, screenInfo.samplingScale,
                screenInfo.samplingTranslateX, screenInfo.samplingTranslateY);
            RectI mappedRect = RSObjAbsGeometry::MapRect(rect.ConvertTo<float>(), scaleMatrix);
            const Vector4<int> expandSize{screenInfo.samplingDistance, screenInfo.samplingDistance,
                screenInfo.samplingDistance, screenInfo.samplingDistance};
            dstDamageRegionrects.emplace_back(mappedRect.MakeOutset(expandSize));
        }
        return dstDamageRegionrects;
    }
    return rects;
}

void RSUniRenderUtil::MergeDirtyHistoryForDrawable(DrawableV2::RSScreenRenderNodeDrawable& screenDrawable,
    int32_t bufferAge, RSScreenRenderParams& params, bool useAlignedDirtyRegion)
{
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    // update all child surfacenode history
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->IsLeashOrMainWindow()) {
            continue;
        }
        // for cross-display surface, only merge dirty history once.
        if (surfaceParams->IsFirstLevelCrossNode() && !params.IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderUtil::MergeDirtyHistory for surfaceNode %" PRIu64"",
            surfaceParams->GetId());
        if (!surfaceDirtyManager->SetBufferAge(bufferAge)) {
            ROSEN_LOGW("RSUniRenderUtil::MergeDirtyHistory with invalid buffer age %{public}d", bufferAge);
        }
        surfaceDirtyManager->IntersectDirtyRect(surfaceParams->GetOldDirtyInSurface());
        surfaceDirtyManager->UpdateDirty(useAlignedDirtyRegion);
    }

    // update display dirtymanager
    if (auto dirtyManager = screenDrawable.GetSyncDirtyManager()) {
        dirtyManager->SetBufferAge(bufferAge);
        dirtyManager->UpdateDirty(useAlignedDirtyRegion);
    }
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegion(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
    std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec, bool useAlignedDirtyRegion)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = allSurfaceNodeDrawables.rbegin(); it != allSurfaceNodeDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("MergeVisibleDirtyRegion surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (!surfaceParams || !surfaceDirtyManager) {
            RS_LOGI("RSUniRenderUtil::MergeVisibleDirtyRegion node(%{public}" PRIu64") params or "
                "dirty manager is nullptr", surfaceNodeDrawable->GetId());
            continue;
        }
        if (!surfaceParams->IsLeashOrMainWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        // for cross-display surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() &&
            !RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegion();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_, surfaceDirtyRect.GetRight(),
            surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceParams->GetVisibleRegion();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);

        surfaceNodeDrawable->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        if (!surfaceVisibleDirtyRegion.IsEmpty()) {
            hasVisibleDirtyRegionSurfaceVec.emplace_back(surfaceParams->GetId());
        }
        if (useAlignedDirtyRegion) {
            Occlusion::Region alignedRegion = AlignedDirtyRegion(surfaceVisibleDirtyRegion);
            surfaceNodeDrawable->SetAlignedVisibleDirtyRegion(alignedRegion);
            allSurfaceVisibleDirtyRegion.OrSelf(alignedRegion);
            GpuDirtyRegionCollection::GetInstance().UpdateActiveDirtyInfoForDFX(surfaceParams->GetId(),
                surfaceNodeDrawable->GetName(), alignedRegion.GetRegionRectIs());
        } else {
            allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
            GpuDirtyRegionCollection::GetInstance().UpdateActiveDirtyInfoForDFX(surfaceParams->GetId(),
                surfaceNodeDrawable->GetName(), surfaceVisibleDirtyRegion.GetRegionRectIs());
        }
    }
    return allSurfaceVisibleDirtyRegion;
}

Occlusion::Region RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
    std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    for (auto it = allSurfaceNodeDrawables.rbegin(); it != allSurfaceNodeDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("RSUniRenderUtil::MergeVisibleDirtyRegion surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (!surfaceParams || !surfaceDirtyManager) {
            RS_LOGI("RSUniRenderUtil::MergeVisibleDirtyRegion node(%{public}" PRIu64") params or"
                "dirty manager is nullptr", surfaceNodeDrawable->GetId());
            continue;
        }
        if (!surfaceParams->IsLeashOrMainWindow()) {
            continue;
        }
        //for cross-display surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() &&
            !RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        Occlusion::Region surfaceAdvancedDirtyRegion = Occlusion::Region();
        for (const auto& rect : surfaceDirtyManager->GetAdvancedDirtyRegion()) {
            Occlusion::Region region = Occlusion::Region(Occlusion::Rect(rect));
            surfaceAdvancedDirtyRegion.OrSelf(region);
        }
        auto visibleRegion = surfaceParams->GetVisibleRegion();
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceAdvancedDirtyRegion.And(visibleRegion);
        surfaceNodeDrawable->SetVisibleDirtyRegion(surfaceVisibleDirtyRegion);
        if (!surfaceVisibleDirtyRegion.IsEmpty()) {
            hasVisibleDirtyRegionSurfaceVec.emplace_back(surfaceParams->GetId());
        }
        allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
        GpuDirtyRegionCollection::GetInstance().UpdateActiveDirtyInfoForDFX(surfaceParams->GetId(),
            surfaceNodeDrawable->GetName(), surfaceVisibleDirtyRegion.GetRegionRectIs());
    }
    return allSurfaceVisibleDirtyRegion;
}

Occlusion::Region RSUniRenderUtil::MergeDirtyRects(Occlusion::Region dirtyRegion)
{
    RectsMerger merger(RSAdvancedDirtyConfig::RECT_NUM_MERGING_ALL, RSAdvancedDirtyConfig::RECT_NUM_MERGING_BY_LEVEL);
    auto mergedRects = merger.MergeAllRects(dirtyRegion.GetRegionRects(),
        RSAdvancedDirtyConfig::EXPECTED_OUTPUT_NUM, RSAdvancedDirtyConfig::MAX_TOLERABLE_COST);
    Occlusion::Region results = {};
    for (const auto& rect : mergedRects) {
        Occlusion::Region region = Occlusion::Region(rect);
        results.OrSelf(region);
    }
    return results;
}

void RSUniRenderUtil::SetDrawRegionForQuickReject(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceDrawables,
    const Occlusion::Region mergedDirtyRects)
{
    auto rectsForQuickReject = mergedDirtyRects.GetRegionRectIs();
    for (const auto& rect : mergedDirtyRects.GetRegionRects()) {
        rectsForQuickReject.push_back(rect.ToRectI());
    }
    for (auto it = allSurfaceDrawables.rbegin(); it != allSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (!surfaceParams || (!surfaceParams->IsMainWindowType() && !surfaceParams->IsLeashWindow())) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        surfaceDirtyManager->SetDirtyRegionForQuickReject(rectsForQuickReject);
    }
}

void RSUniRenderUtil::MergeDirtyHistoryInVirtual(
    DrawableV2::RSScreenRenderNodeDrawable& screenDrawable, int32_t bufferAge, bool renderParallel)
{
    (void)renderParallel;
    auto& params = screenDrawable.GetRenderParams();
    if (!params) {
        RS_LOGE("RSUniRenderUtil::MergeDirtyHistory params is nullptr");
        return;
    }
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->IsAppWindow()) {
            continue;
        }
        RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderUtil::MergeDirtyHistory for surfaceNode %" PRIu64"",
            surfaceParams->GetId());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        surfaceDirtyManager->MergeDirtyHistoryInVirtual(bufferAge);
    }
    // update display dirtymanager
    auto displayDirtyManager = screenDrawable.GetSyncDirtyManager();
    displayDirtyManager->MergeDirtyHistoryInVirtual(bufferAge);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
    RSScreenRenderParams& screenParams, bool isSecScreen)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual, failed to get screen manager!");
        return allSurfaceVisibleDirtyRegion;
    }
    auto curBlackList = screenManager->GetVirtualScreenBlackList(screenParams.GetScreenId());
    for (auto it = allSurfaceNodeDrawables.rbegin(); it != allSurfaceNodeDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("MergeVisibleDirtyRegion surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGI("RSUniRenderUtil::MergeVisibleDirtyRegion surface params is nullptr");
            continue;
        }
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        if (surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP) && !isSecScreen) {
            continue;
        }
        if (curBlackList.find(surfaceParams->GetId()) != curBlackList.end()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        auto surfaceDirtyRect = surfaceDirtyManager->GetDirtyRegionInVirtual();
        Occlusion::Rect dirtyRect { surfaceDirtyRect.left_, surfaceDirtyRect.top_,
            surfaceDirtyRect.GetRight(), surfaceDirtyRect.GetBottom() };
        auto visibleRegion = surfaceParams->GetVisibleRegionInVirtual();
        Occlusion::Region surfaceDirtyRegion { dirtyRect };
        Occlusion::Region surfaceVisibleDirtyRegion = surfaceDirtyRegion.And(visibleRegion);
        allSurfaceVisibleDirtyRegion = allSurfaceVisibleDirtyRegion.Or(surfaceVisibleDirtyRegion);
    }
    return allSurfaceVisibleDirtyRegion;
}

void RSUniRenderUtil::SrcRectScaleFit(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
    const sptr<IConsumerSurface>& surface, RectF& localBounds)
{
    if (buffer == nullptr || surface == nullptr) {
        RS_LOGE("buffer or surface is nullptr");
        return;
    }
    uint32_t srcWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t srcHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
    float newWidth = 0.0f;
    float newHeight = 0.0f;
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());
    if (boundsWidth == 0 || boundsHeight == 0 || srcWidth == 0 || srcHeight == 0) {
        return;
    }

    if (srcWidth * boundsHeight > srcHeight * boundsWidth) {
        newWidth = boundsWidth;
        newHeight = srcHeight * newWidth / srcWidth;
    } else if (srcWidth * boundsHeight < srcHeight * boundsWidth) {
        newHeight = boundsHeight;
        newWidth = newHeight * srcWidth / srcHeight;
    } else {
        newWidth = boundsWidth;
        newHeight = boundsHeight;
    }
    newHeight = newHeight * srcHeight / boundsHeight;
    newWidth = newWidth * srcWidth / boundsWidth;
    if (newWidth < srcWidth) {
        float halfdw = (srcWidth - newWidth) / 2;
        params.dstRect =
            Drawing::Rect(params.srcRect.GetLeft() + halfdw, params.srcRect.GetTop(),
                params.srcRect.GetLeft() + halfdw + newWidth,
                params.srcRect.GetTop() + params.srcRect.GetHeight());
    } else if (newHeight < srcHeight) {
        float halfdh = (srcHeight - newHeight) / 2;
        params.dstRect =
            Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + halfdh,
                params.srcRect.GetLeft() + params.srcRect.GetWidth(),
                params.srcRect.GetTop() + halfdh + newHeight);
    }
    RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleFit name:%{public}s,"
        " dstRect [%{public}f %{public}f %{public}f %{public}f]",
        surface->GetName().c_str(), params.dstRect.GetLeft(), params.dstRect.GetTop(),
        params.dstRect.GetWidth(), params.dstRect.GetHeight());
}

void RSUniRenderUtil::SrcRectScaleDown(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
    const sptr<IConsumerSurface>& surface, RectF& localBounds)
{
    if (buffer == nullptr || surface == nullptr) {
        return;
    }
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());
    if (boundsWidth == 0 || boundsHeight == 0) {
        RS_LOGE("RSUniRenderUtil::SrcRectScaleDown: boundsWidth or boundsHeight is 0");
        return;
    }
    uint32_t newWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t newHeight = static_cast<uint32_t>(params.srcRect.GetHeight());

    uint32_t newWidthBoundsHeight = newWidth * boundsHeight;
    uint32_t newHeightBoundsWidth = newHeight * boundsWidth;

    if (newWidthBoundsHeight > newHeightBoundsWidth) {
        newWidth = boundsWidth * newHeight / boundsHeight;
    } else if (newWidthBoundsHeight < newHeightBoundsWidth) {
        newHeight = boundsHeight * newWidth / boundsWidth;
    } else {
        return;
    }

    uint32_t currentWidth = static_cast<uint32_t>(params.srcRect.GetWidth());
    uint32_t currentHeight = static_cast<uint32_t>(params.srcRect.GetHeight());
    if (newWidth < currentWidth) {
        // the crop is too wide
        uint32_t dw = currentWidth - newWidth;
        auto halfdw = dw / 2;
        params.srcRect =
            Drawing::Rect(params.srcRect.GetLeft() + static_cast<int32_t>(halfdw), params.srcRect.GetTop(),
                params.srcRect.GetLeft() + static_cast<int32_t>(halfdw) + static_cast<int32_t>(newWidth),
                params.srcRect.GetTop() + params.srcRect.GetHeight());
    } else {
        // thr crop is too tall
        uint32_t dh = currentHeight - newHeight;
        auto halfdh = dh / 2;
        params.srcRect =
            Drawing::Rect(params.srcRect.GetLeft(), params.srcRect.GetTop() + static_cast<int32_t>(halfdh),
                params.srcRect.GetLeft() + params.srcRect.GetWidth(),
                params.srcRect.GetTop() + static_cast<int32_t>(halfdh) + static_cast<int32_t>(newHeight));
    }
    RS_LOGD("RsDebug RSUniRenderUtil::SrcRectScaleDown name:%{public}s,"
        " srcRect [%{public}f %{public}f %{public}f %{public}f]",
        surface->GetName().c_str(), params.srcRect.GetLeft(), params.srcRect.GetTop(),
        params.srcRect.GetWidth(), params.srcRect.GetHeight());
}

void RSUniRenderUtil::SetSrcRect(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer)
{
    Rect crop = {0, 0, 0, 0};
    params.hasCropMetadata = buffer->GetCropMetadata(crop);

    if (UNLIKELY(params.hasCropMetadata)) {
        RS_LOGD("buffer has crop metadata, "
            "left = %{public}u, right = %{public}u, width = %{public}u, height = %{public}u",
            crop.x, crop.y, crop.w, crop.h);
        params.srcRect = Drawing::Rect(crop.x, crop.y, crop.x + crop.w, crop.y + crop.h);
    } else {
        params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    }
}

Drawing::Matrix RSUniRenderUtil::GetMatrixOfBufferToRelRect(const RSSurfaceRenderNode& node)
{
    const sptr<SurfaceBuffer> buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (buffer == nullptr) {
        return Drawing::Matrix();
    }

    auto consumer = node.GetRSSurfaceHandler()->GetConsumer();
    if (consumer == nullptr) {
        return Drawing::Matrix();
    }

    BufferDrawParam params;
    params.buffer = buffer;
    params.srcRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    const RSProperties& property = node.GetRenderProperties();
    params.dstRect = Drawing::Rect(0, 0, property.GetBoundsWidth(), property.GetBoundsHeight());
    auto transform = RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, buffer);
    RectF localBounds = { 0.0f, 0.0f, property.GetBoundsWidth(), property.GetBoundsHeight() };
    auto& renderParams = const_cast<RSSurfaceRenderNode&>(node).GetStagingRenderParams();
    auto surfaceRenderParams = renderParams == nullptr
                            ? nullptr
                            : static_cast<RSSurfaceRenderParams*>(renderParams.get());
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, property.GetFrameGravity(), localBounds, params,
        surfaceRenderParams);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    return params.matrix;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(
    const RSSurfaceRenderNode& node, bool forceCPU, uint32_t threadIndex, bool useRenderParams)
{
    BufferDrawParam params;

    auto drawable = node.GetRenderDrawable();
    if (useRenderParams && !drawable) {
        return params;
    }
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
    auto& nodeParams = surfaceDrawable->GetRenderParams();
    if (useRenderParams && !nodeParams) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam RenderThread nodeParams is nullptr");
        return params;
    }
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeParams.get());
    const RSProperties& property = node.GetRenderProperties();

    params.threadIndex = threadIndex;
    params.useBilinearInterpolation = useRenderParams ?
        surfaceParams->NeedBilinearInterpolation() : node.NeedBilinearInterpolation();
    params.useCPU = forceCPU;
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    auto boundWidth = useRenderParams ? nodeParams->GetBounds().GetWidth() : property.GetBoundsWidth();
    auto boundHeight = useRenderParams ? nodeParams->GetBounds().GetHeight() : property.GetBoundsHeight();
    params.dstRect = Drawing::Rect(0, 0, boundWidth, boundHeight);

    const sptr<SurfaceBuffer> buffer = useRenderParams ? surfaceParams->GetBuffer() : surfaceHandler->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = useRenderParams ? nodeParams->GetAcquireFence() : surfaceHandler->GetAcquireFence();
    SetSrcRect(params, buffer);
    auto consumer = useRenderParams ? surfaceDrawable->GetConsumerOnDraw() : surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return params;
    }

    GraphicAlphaType alphaType = GraphicAlphaType::GRAPHIC_ALPHATYPE_PREMUL;
    if (consumer->GetAlphaType(alphaType) == GSERROR_OK) {
        params.alphaType = static_cast<Drawing::AlphaType>(alphaType);
    }
    
    auto transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transform) != GSERROR_OK) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam GetSurfaceBufferTransformType failed");
    }
    RectF localBounds = { 0.0f, 0.0f, boundWidth, boundHeight };
    auto gravity = useRenderParams ? nodeParams->GetFrameGravity() : property.GetFrameGravity();
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, gravity, localBounds, params, surfaceParams);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateBufferDrawParam(RSSurfaceRenderNode): Scaling mode is %{public}d", scalingMode);
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, consumer, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, consumer, localBounds);
    }
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateBufferDrawParam(RSSurfaceRenderNode): Parameters creation completed");
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(
    const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, bool forceCPU, uint32_t threadIndex)
{
    BufferDrawParam params;
    auto& nodeParams = surfaceDrawable.GetRenderParams();
    if (!nodeParams) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam RenderThread nodeParams is nullptr");
        return params;
    }
    auto surfaceNodeParams = static_cast<RSSurfaceRenderParams*>(nodeParams.get());
    params.threadIndex = threadIndex;
    params.useBilinearInterpolation = surfaceNodeParams->NeedBilinearInterpolation();
    params.useCPU = forceCPU;
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    auto boundWidth = nodeParams->GetBounds().GetWidth();
    auto boundHeight = nodeParams->GetBounds().GetHeight();
    params.dstRect = Drawing::Rect(0, 0, boundWidth, boundHeight);

    const sptr<SurfaceBuffer> buffer = nodeParams->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = nodeParams->GetAcquireFence();
    SetSrcRect(params, buffer);
    auto consumer = surfaceDrawable.GetConsumerOnDraw();
    if (consumer == nullptr) {
        return params;
    }

    GraphicAlphaType alphaType = GraphicAlphaType::GRAPHIC_ALPHATYPE_PREMUL;
    if (consumer->GetAlphaType(alphaType) == GSERROR_OK) {
        params.alphaType = static_cast<Drawing::AlphaType>(alphaType);
    }

    auto transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transform) != GSERROR_OK) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam GetSurfaceBufferTransformType failed");
    }
    params.preRotation = consumer->GetSurfaceAppFrameworkType() == "fixed-rotation" ? true : false;

    RectF localBounds = { 0.0f, 0.0f, boundWidth, boundHeight };
    auto gravity = nodeParams->GetFrameGravity();
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, gravity, localBounds, params, surfaceNodeParams);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    RSAncoManager::UpdateCropRectForAnco(surfaceNodeParams->GetAncoFlags(), surfaceNodeParams->GetAncoSrcCrop(),
        { buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), buffer->GetFormat() }, params.srcRect);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, consumer, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, consumer, localBounds);
    }
    RS_LOGD_IF(DEBUG_COMPOSER, "RSUniRenderUtil::CreateBufferDrawParam(DrawableV2::RSSurfaceRenderNodeDrawable):"
        " Parameters creation completed");
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParamForRotationFixed(
    const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable,
    RSSurfaceRenderParams& renderParams, uint32_t threadIndex)
{
    BufferDrawParam params;
    params.threadIndex = threadIndex;
    params.useBilinearInterpolation = renderParams.NeedBilinearInterpolation();
    params.useCPU = false;
    params.targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
#ifdef USE_VIDEO_PROCESSING_ENGINE
    params.sdrNits = renderParams.GetSdrNit();
    params.tmoNits = renderParams.GetDisplayNit();
    params.displayNits = params.tmoNits / std::pow(renderParams.GetBrightnessRatio(), GAMMA2_2); // gamma 2.2
    params.layerLinearMatrix = renderParams.GetLayerLinearMatrix();
#endif

    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    auto buffer = renderParams.GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = renderParams.GetAcquireFence();
    auto srcRect = renderParams.GetLayerInfo().srcRect;
    params.srcRect = Drawing::Rect(srcRect.x, srcRect.y, srcRect.x + srcRect.w, srcRect.y + srcRect.h);

    auto dstRect = renderParams.GetLayerInfo().dstRect;
    params.matrix = Drawing::Matrix();
    params.matrix.PreTranslate(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y));

    auto layerTransform = renderParams.GetLayerInfo().transformType;
    int realRotation = RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform));
    auto flip = RSBaseRenderUtil::GetFlipTransform(layerTransform);
    // calculate transform in anti-clockwise
    auto transform = RSBaseRenderUtil::RotateEnumToInt(realRotation, flip);

    RectF localBounds = { 0.0f, 0.0f,
        static_cast<float>(dstRect.w), static_cast<float>(dstRect.h) };
    DealWithRotationAndGravityForRotationFixed(transform, renderParams.GetFrameGravity(), localBounds, params);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateBufferDrawParamForRotationFixed(DrawableV2::RSSurfaceRenderNodeDrawable):"
        " Parameters creation completed");
    return params;
}

void RSUniRenderUtil::DealWithRotationAndGravityForRotationFixed(GraphicTransformType transform, Gravity gravity,
    RectF& localBounds, BufferDrawParam& params)
{
    auto rotationTransform = RSBaseRenderUtil::GetRotateTransform(transform);
    params.matrix.PreConcat(RSBaseRenderUtil::GetSurfaceTransformMatrixForRotationFixed(
        rotationTransform, localBounds));
    if (rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        rotationTransform == GraphicTransformType::GRAPHIC_ROTATE_270) {
        // after rotate, we should swap dstRect and bound's width and height.
        std::swap(localBounds.width_, localBounds.height_);
    }
    params.dstRect = Drawing::Rect(0, 0, params.srcRect.GetWidth(), params.srcRect.GetHeight());

    Drawing::Matrix gravityMatrix;
    if (!RSPropertiesPainter::GetGravityMatrix(gravity, localBounds,
        params.srcRect.GetWidth(), params.srcRect.GetHeight(), gravityMatrix)) {
        RS_LOGD("RSUniRenderUtil::DealWithRotationAndGravityForRotationFixed did not obtain gravity matrix.");
    }
    params.matrix.PreConcat(gravityMatrix);
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSScreenRenderNode& node, bool forceCPU)
{
    BufferDrawParam params;
    params.useCPU = forceCPU;
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);

    auto drawable = node.GetRenderDrawable();
    if (!drawable) {
        return params;
    }
    auto screenDrawable = std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(drawable);
    auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
    const sptr<SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
    if (!buffer) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam buffer is null.");
        return params;
    }
    const sptr<IConsumerSurface> consumer = surfaceHandler->GetConsumer();
    GraphicAlphaType alphaType = GraphicAlphaType::GRAPHIC_ALPHATYPE_PREMUL;
    if (consumer && consumer->GetAlphaType(alphaType) == GSERROR_OK) {
        params.alphaType = static_cast<Drawing::AlphaType>(alphaType);
    }
    params.buffer = buffer;
    params.acquireFence = surfaceHandler->GetAcquireFence();
    SetSrcRect(params, buffer);
    params.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateBufferDrawParam(RSScreenRenderNode): Parameters creation completed");
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSSurfaceHandler& surfaceHandler, bool forceCPU)
{
    BufferDrawParam bufferDrawParam;
    bufferDrawParam.useCPU = forceCPU;
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    bufferDrawParam.paint.SetFilter(filter);

    const sptr<SurfaceBuffer> buffer = surfaceHandler.GetBuffer();
    if (!buffer) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam buffer is null.");
        return bufferDrawParam;
    }
    const sptr<IConsumerSurface> consumer = surfaceHandler.GetConsumer();
    GraphicAlphaType alphaType = GraphicAlphaType::GRAPHIC_ALPHATYPE_PREMUL;
    if (consumer && consumer->GetAlphaType(alphaType) == GSERROR_OK) {
        bufferDrawParam.alphaType = static_cast<Drawing::AlphaType>(alphaType);
    }
    bufferDrawParam.buffer = buffer;
    bufferDrawParam.acquireFence = surfaceHandler.GetAcquireFence();
    SetSrcRect(bufferDrawParam, buffer);
    bufferDrawParam.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateBufferDrawParam(RSSurfaceHandler): Parameters creation completed");
    return bufferDrawParam;
}

BufferDrawParam RSUniRenderUtil::CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU)
{
    BufferDrawParam params;
    if (layer == nullptr) {
        return params;
    }
    params.useCPU = forceCPU;
    params.useBilinearInterpolation = layer->GetNeedBilinearInterpolation();
    Drawing::Filter filter;
    filter.SetFilterQuality(Drawing::Filter::FilterQuality::LOW);
    params.paint.SetFilter(filter);
    params.paint.SetAlpha(layer->GetAlpha().gAlpha);
    sptr<SurfaceBuffer> buffer = layer->GetBuffer();
    if (buffer == nullptr) {
        return params;
    }
    params.acquireFence = layer->GetAcquireFence();
    params.buffer = buffer;
    SetSrcRect(params, buffer);
    auto boundRect = layer->GetBoundSize();
    params.dstRect = Drawing::Rect(0, 0, boundRect.w, boundRect.h);

    auto layerMatrix = layer->GetMatrix();
    params.matrix = Drawing::Matrix();
    bool rotationFixed = layer->GetRotationFixed();
    auto dstRect = layer->GetLayerSize();
    if (rotationFixed) {
        // if rotation fixed, not use [total matrix + bounds] to draw buffer, use [src + dst + transform]
        params.matrix.PreTranslate(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y));
        auto srcRect = layer->GetCropRect();
        params.srcRect = Drawing::Rect(srcRect.x, srcRect.y, srcRect.x + srcRect.w, srcRect.y + srcRect.h);
    } else {
        params.matrix.SetMatrix(layerMatrix.scaleX, layerMatrix.skewX, layerMatrix.transX, layerMatrix.skewY,
            layerMatrix.scaleY, layerMatrix.transY, layerMatrix.pers0, layerMatrix.pers1, layerMatrix.pers2);
    }
    // rotation degree anti-clockwise
    int nodeRotation = rotationFixed ? 0 : RSUniRenderUtil::GetRotationFromMatrix(params.matrix);
    auto layerTransform = layer->GetTransformType();
    // calculate clockwise rotation degree excluded rotation in total matrix
    int realRotation = (nodeRotation +
        RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(layerTransform))) % 360;
    auto flip = RSBaseRenderUtil::GetFlipTransform(layerTransform);
    // calculate transform in anti-clockwise
    auto transform = RSBaseRenderUtil::RotateEnumToInt(realRotation, flip);

    RectF localBounds = { 0.0f, 0.0f,
        rotationFixed ? static_cast<float>(dstRect.w) : static_cast<float>(boundRect.w),
        rotationFixed ? static_cast<float>(dstRect.h) : static_cast<float>(boundRect.h) };
    if (rotationFixed) {
        DealWithRotationAndGravityForRotationFixed(transform, static_cast<Gravity>(layer->GetGravity()), localBounds,
            params);
    } else {
        RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, static_cast<Gravity>(layer->GetGravity()),
            localBounds, params);
    }
    RSBaseRenderUtil::FlipMatrix(transform, params);
    if (rotationFixed) {
        // if rotation fixed, no need to calculate scaling mode, it is contained in dstRect
        return params;
    }
    const auto& surface = layer->GetSurface();
    if (surface == nullptr) {
        RS_LOGE("buffer or surface is nullptr");
        return params;
    }
    RSAncoManager::UpdateCropRectForAnco(layer->GetAncoFlags(), layer->GetCropRect(),
        { buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight(), buffer->GetFormat() }, params.srcRect);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, surface, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, surface, localBounds);
    }
    RS_LOGD_IF(DEBUG_COMPOSER,
        "RSUniRenderUtil::CreateLayerBufferDrawParam(LayerInfoPtr): Parameters creation completed");
    return params;
}

Occlusion::Region RSUniRenderUtil::AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits)
{
    Occlusion::Region alignedRegion;
    if (alignedBits <= 1) {
        return dirtyRegion;
    }
    for (const auto& dirtyRect : dirtyRegion.GetRegionRects()) {
        int32_t left = (dirtyRect.left_ / alignedBits) * alignedBits;
        int32_t top = (dirtyRect.top_ / alignedBits) * alignedBits;
        int32_t width = ((dirtyRect.right_ + alignedBits - 1) / alignedBits) * alignedBits - left;
        int32_t height = ((dirtyRect.bottom_ + alignedBits - 1) / alignedBits) * alignedBits - top;
        Occlusion::Rect rect = { left, top, left + width, top + height };
        Occlusion::Region singleAlignedRegion(rect);
        alignedRegion.OrSelf(singleAlignedRegion);
    }
    return alignedRegion;
}

int RSUniRenderUtil::TransferToAntiClockwiseDegrees(int angle)
{
    static const std::map<int, int> supportedDegrees = { { 90, 270 }, { 180, 180 }, { -90, 90 }, { -180, 180 },
        { 270, 90 }, { -270, 270 } };
    auto iter = supportedDegrees.find(angle);
    return iter != supportedDegrees.end() ? iter->second : 0;
}

int RSUniRenderUtil::GetRotationFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);

    int rAngle = static_cast<int>(-round(atan2(value[Drawing::Matrix::Index::SKEW_X],
        value[Drawing::Matrix::Index::SCALE_X]) * (180 / PI)));
    // transfer the result to anti-clockwise degrees
    // only rotation with 90°, 180°, 270° are composed through hardware,
    // in which situation the transformation of the layer needs to be set.
    return TransferToAntiClockwiseDegrees(rAngle);
}

int RSUniRenderUtil::GetRotationDegreeFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    return static_cast<int>(-round(atan2(value[Drawing::Matrix::Index::SKEW_X],
        value[Drawing::Matrix::Index::SCALE_X]) * (RS_ROTATION_180 / PI)));
}

void RSUniRenderUtil::ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface>&& cacheSurface,
    std::shared_ptr<Drawing::Surface>&& cacheCompletedSurface,
    uint32_t cacheSurfaceThreadIndex, uint32_t completedSurfaceThreadIndex)
{
    PostReleaseSurfaceTask(std::move(cacheSurface), cacheSurfaceThreadIndex);
    PostReleaseSurfaceTask(std::move(cacheCompletedSurface), completedSurfaceThreadIndex);
}

void RSUniRenderUtil::PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex)
{
    if (surface == nullptr) {
        return;
    }

    if (threadIndex == UNI_MAIN_THREAD_INDEX || threadIndex == UNI_RENDER_THREAD_INDEX) {
        if (RSUniRenderJudgement::IsUniRender()) {
            auto instance = &(RSUniRenderThread::Instance());
            instance->AddToReleaseQueue(std::move(surface));
            instance->PostTask(([instance] () { instance->ReleaseSurface(); }),
                RELEASE_SURFACE_TASK, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        } else {
            auto instance = RSMainThread::Instance();
            instance->AddToReleaseQueue(std::move(surface));
            instance->PostTask(([instance] () { instance->ReleaseSurface(); }),
                RELEASE_SURFACE_TASK, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    } else {
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
        auto instance = RSSubThreadManager::Instance();
        instance->AddToReleaseQueue(std::move(surface), threadIndex);
        instance->ReleaseSurface(threadIndex);
#endif
    }
}

void RSUniRenderUtil::DrawRectForDfx(RSPaintFilterCanvas& canvas, const RectI& rect, Drawing::Color color,
    float alpha, const std::string& extraInfo)
{
    if (rect.width_ <= 0 || rect.height_ <= 0) {
        RS_LOGD("DrawRectForDfx rect is invalid.");
        return;
    }
    RS_LOGD("DrawRectForDfx current rect = %{public}s", rect.ToString().c_str());
    auto dstRect = Drawing::Rect(rect.left_, rect.top_,
        rect.left_ + rect.width_, rect.top_ + rect.height_);

    std::string position = rect.ToString() + extraInfo;

    const int defaultTextOffsetX = 6; // text position is 6 pixelSize right side of the Rect
    const int defaultTextOffsetY = 30; // text position has 30 pixelSize under the Rect
    Drawing::Brush rectBrush;
    std::shared_ptr<Drawing::Typeface> typeFace = nullptr;

    // font size: 24
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString(position.c_str(), Drawing::Font(typeFace, 24.0f, 1.0f, 0.0f));

    rectBrush.SetColor(color);
    rectBrush.SetAntiAlias(true);
    rectBrush.SetAlphaF(alpha);
    canvas.AttachBrush(rectBrush);
    canvas.DrawRect(dstRect);
    canvas.DetachBrush();
    canvas.AttachBrush(Drawing::Brush());
    canvas.DrawTextBlob(textBlob.get(), rect.left_ + defaultTextOffsetX, rect.top_ + defaultTextOffsetY);
    canvas.DetachBrush();
}

#ifdef RS_ENABLE_VK

std::vector<GrBackendSemaphore> RSUniRenderUtil::PrepareHdrSemaphoreVector(GrBackendSemaphore& backendSemaphore,
    std::shared_ptr<Drawing::Surface>& surface)
{
    VkSemaphore notifySemaphore;
    std::vector<GrBackendSemaphore> semphoreVec = {backendSemaphore};
    std::vector<uint64_t> frameIdVec = RSHDRPatternManager::Instance().MHCGetFrameIdForGpuTask();
    
    for (auto frameId : frameIdVec) {
        if (RSHDRVulkanTask::GetHTSNotifySemaphore(notifySemaphore, frameId)) {
            GrBackendSemaphore htsSemaphore;
            htsSemaphore.initVulkan(notifySemaphore);
            semphoreVec.emplace_back(std::move(htsSemaphore));
        }
        RSHDRVulkanTask::InsertHTSWaitSemaphore(surface, frameId);
    }

    return semphoreVec;
}
#endif

void RSUniRenderUtil::OptimizedFlushAndSubmit(std::shared_ptr<Drawing::Surface>& surface,
    Drawing::GPUContext* const grContext, bool optFenceWait)
{
    if (!surface || !grContext) {
        RS_LOGE("RSUniRenderUtil::OptimizedFlushAndSubmit cacheSurface or grContext are nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("Render surface flush and submit");
    RS_LOGD("RSUniRenderUtil::optimized flush and submit GpuApiType:%{public}d",
        RSSystemProperties::GetGpuApiType());
#ifdef RS_ENABLE_VK
    if ((RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) && optFenceWait) {
        auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();

        VkExportSemaphoreCreateInfo exportSemaphoreCreateInfo;
        exportSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO;
        exportSemaphoreCreateInfo.pNext = nullptr;
        exportSemaphoreCreateInfo.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;

        VkSemaphoreCreateInfo semaphoreInfo;
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = &exportSemaphoreCreateInfo;
        semaphoreInfo.flags = 0;
        VkSemaphore semaphore;
        vkContext.vkCreateSemaphore(vkContext.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
#ifdef USE_M133_SKIA
        GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore);
#else
        GrBackendSemaphore backendSemaphore;
        backendSemaphore.initVulkan(semaphore);
#endif

        DestroySemaphoreInfo* destroyInfo =
            new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);

        auto semphoreVec = PrepareHdrSemaphoreVector(backendSemaphore, surface);

        Drawing::FlushInfo drawingFlushInfo;
        drawingFlushInfo.backendSurfaceAccess = true;
        drawingFlushInfo.numSemaphores = semphoreVec.size();
        drawingFlushInfo.backendSemaphore = static_cast<void*>(semphoreVec.data());
        drawingFlushInfo.finishedProc = [](void *context) {
            DestroySemaphoreInfo::DestroySemaphore(context);
        };
        drawingFlushInfo.finishedContext = destroyInfo;
        surface->Flush(&drawingFlushInfo);
        grContext->Submit();
        DestroySemaphoreInfo::DestroySemaphore(destroyInfo);
        
        std::vector<uint64_t> frameIdVec = RSHDRPatternManager::Instance().MHCGetFrameIdForGpuTask();
        for (auto frameId : frameIdVec) {
            RSHDRVulkanTask::SubmitWaitEventToGPU(frameId);
        }
    } else {
        surface->FlushAndSubmit(true);
    }
#else
    surface->FlushAndSubmit(true);
#endif
}

SecRectInfo RSUniRenderUtil::GenerateSecRectInfoFromNode(RSRenderNode& node, RectI rect)
{
    SecRectInfo uiExtensionRectInfo;
    uiExtensionRectInfo.relativeCoords = rect;
    uiExtensionRectInfo.scale = node.GetRenderProperties().GetScale();
    return uiExtensionRectInfo;
}

SecSurfaceInfo RSUniRenderUtil::GenerateSecSurfaceInfoFromNode(
    NodeId uiExtensionId, NodeId hostId, SecRectInfo uiExtensionRectInfo)
{
    SecSurfaceInfo secSurfaceInfo;
    secSurfaceInfo.uiExtensionRectInfo = uiExtensionRectInfo;
    secSurfaceInfo.uiExtensionPid = ExtractPid(uiExtensionId);
    secSurfaceInfo.hostPid = ExtractPid(hostId);
    secSurfaceInfo.uiExtensionNodeId = uiExtensionId;
    secSurfaceInfo.hostNodeId = hostId;
    return secSurfaceInfo;
}

void RSUniRenderUtil::UIExtensionFindAndTraverseAncestor(
    const RSRenderNodeMap& nodeMap, UIExtensionCallbackData& callbackData, bool isUnobscured)
{
    const auto& secUIExtensionNodes = RSSurfaceRenderNode::GetSecUIExtensionNodes();
    for (auto it = secUIExtensionNodes.begin(); it != secUIExtensionNodes.end(); ++it) {
        currentUIExtensionIndex_ = -1;
        // only traverse host node one time, even if it has multiple uiextension children.
        if (callbackData.find(it->second) != callbackData.end()) {
            continue;
        }
        auto hostNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(nodeMap.GetRenderNode(it->second));
        if (!hostNode || !hostNode->GetSortedChildren()) {
            RS_LOGE("RSUniRenderUtil::UIExtensionFindAndTraverseAncestor failed to get host node or its children.");
            return;
        }
        for (const auto& child : *hostNode->GetSortedChildren()) {
            TraverseAndCollectUIExtensionInfo(child, Drawing::Matrix(), hostNode->GetId(), callbackData, isUnobscured);
        }
    }
}

void RSUniRenderUtil::TraverseAndCollectUIExtensionInfo(std::shared_ptr<RSRenderNode> node,
    Drawing::Matrix parentMatrix, NodeId hostId, UIExtensionCallbackData& callbackData, bool isUnobscured)
{
    if (!node) {
        return;
    }
    // update position relative to host app window node.
    std::optional<Drawing::Point> offset;
    auto parent = node->GetParent().lock();
    if (parent && !(node->IsInstanceOf<RSSurfaceRenderNode>())) {
        const auto& parentRenderProperties = parent->GetRenderProperties();
        offset = Drawing::Point { parentRenderProperties.GetFrameOffsetX(), parentRenderProperties.GetFrameOffsetY() };
    }
    const auto& nodeRenderProperties = node->GetRenderProperties();
    RSObjAbsGeometry boundsGeo = nodeRenderProperties.GetBoundsGeometry() == nullptr ?
        RSObjAbsGeometry() : *(nodeRenderProperties.GetBoundsGeometry());
    boundsGeo.UpdateMatrix(&parentMatrix, offset);
    auto rect = boundsGeo.MapAbsRect(node->GetSelfDrawRect().JoinRect(node->GetChildrenRect().ConvertTo<float>()));
    // if node is UIExtension type, update its own info, and skip its children.
    if (auto surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(node)) {
        if ((surfaceNode->IsSecureUIExtension() && !isUnobscured) ||
            (surfaceNode->IsUnobscuredUIExtensionNode() && isUnobscured)) {
            currentUIExtensionIndex_++;
            // if host node is not recorded in callbackData, insert it.
            auto [iter, inserted] = callbackData.insert(std::pair(hostId, std::vector<SecSurfaceInfo>{}));
            if (iter != callbackData.end()) {
                iter->second.push_back(GenerateSecSurfaceInfoFromNode(
                    surfaceNode->GetId(), hostId, GenerateSecRectInfoFromNode(*surfaceNode, rect)));
            }
            if (surfaceNode->ChildrenHasUIExtension()) {
                RS_LOGW("RSUniRenderUtil::TraverseAndCollectUIExtensionInfo UIExtension node [%{public}" PRIu64 "]"
                    " has children UIExtension, not surpported!", surfaceNode->GetId());
            }
            return;
        }
    }
    // if the node is traversed after a UIExtension, collect it and skip its children (except it has UIExtension child.)
    auto iter = callbackData.find(hostId);
    if (iter != callbackData.end() && currentUIExtensionIndex_ != -1 &&
        currentUIExtensionIndex_ < static_cast<int>((iter->second).size())) {
        (iter->second)[currentUIExtensionIndex_].upperNodes.push_back(GenerateSecRectInfoFromNode(*node, rect));
        if (!node->ChildrenHasUIExtension()) {
            return;
        }
    }
    // continue to traverse.
    for (const auto& child : *node->GetSortedChildren()) {
        TraverseAndCollectUIExtensionInfo(child, boundsGeo.GetAbsMatrix(), hostId, callbackData, isUnobscured);
    }
}

void RSUniRenderUtil::ProcessCacheImage(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    // Be cautious when changing FilterMode and MipmapMode that may affect clarity
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
    canvas.DrawImage(cacheImageProcessed, 0, 0, sampling);
    canvas.DetachBrush();
}

void RSUniRenderUtil::ProcessCacheImageRect(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed,
    const Drawing::Rect& src, const Drawing::Rect& dst)
{
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    // Be cautious when changing FilterMode and MipmapMode that may affect clarity
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
    canvas.DrawImageRect(cacheImageProcessed, src, dst, sampling, Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
}

void RSUniRenderUtil::ProcessCacheImageForMultiScreenView(RSPaintFilterCanvas& canvas,
    Drawing::Image& cacheImageProcessed, const RectF& rect)
{
    if (cacheImageProcessed.GetWidth() == 0 || cacheImageProcessed.GetHeight() == 0) {
        RS_TRACE_NAME("ProcessCacheImageForMultiScreenView cacheImageProcessed is invalid");
        return;
    }
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    // Be cautious when changing FilterMode and MipmapMode that may affect clarity
    auto sampling = Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::NEAREST);
    canvas.Save();
    // Use Fill Mode
    const float scaleX = rect.GetWidth() / cacheImageProcessed.GetWidth();
    const float scaleY = rect.GetHeight() / cacheImageProcessed.GetHeight();
    canvas.Scale(scaleX, scaleY);
    canvas.DrawImage(cacheImageProcessed, 0, 0, sampling);
    canvas.Restore();
    canvas.DetachBrush();
}

void RSUniRenderUtil::FlushDmaSurfaceBuffer(Media::PixelMap* pixelMap)
{
    if (!pixelMap || pixelMap->GetAllocatorType() != Media::AllocatorType::DMA_ALLOC) {
        return;
    }
    SurfaceBuffer* surfaceBuffer = reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
    if (surfaceBuffer && (surfaceBuffer->GetUsage() & BUFFER_USAGE_MEM_MMZ_CACHE)) {
        GSError err = surfaceBuffer->Map();
        if (err != GSERROR_OK) {
            RS_LOGE("RSUniRenderUtil::FlushDmaSurfaceBuffer Map failed, GSError=%{public}d", err);
            return;
        }
        err = surfaceBuffer->InvalidateCache();
        if (err != GSERROR_OK) {
            RS_LOGE("RSUniRenderUtil::FlushDmaSurfaceBuffer InvalidateCache failed, GSError=%{public}d", err);
        }
    }
}

bool RSUniRenderUtil::CheckRenderSkipIfScreenOff(bool extraFrame, std::optional<ScreenId> screenId)
{
    if (!RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        return false;
    }
    auto screenManager = CreateOrGetScreenManager();
    if (!screenManager) {
        RS_LOGE("RSUniRenderUtil::CheckRenderSkipIfScreenOff, failed to get screen manager!");
        return false;
    }
    // in certain cases such as wireless display, render skipping may be disabled.
    auto disableRenderControlScreensCount = screenManager->GetDisableRenderControlScreensCount();
    auto isScreenOff = screenId.has_value() ?
        screenManager->IsScreenPowerOff(screenId.value()) : screenManager->IsAllScreensPowerOff();
    RS_TRACE_NAME_FMT("CheckRenderSkipIfScreenOff disableRenderControl:[%d], PowerOff:[%d]",
        disableRenderControlScreensCount, isScreenOff);
    if (disableRenderControlScreensCount != 0 || !isScreenOff) {
        return false;
    }
    if (extraFrame && screenManager->GetPowerOffNeedProcessOneFrame()) {
        RS_LOGI("RSUniRenderUtil::CheckRenderSkipIfScreenOff screen power off, one more frame.");
        screenManager->ResetPowerOffNeedProcessOneFrame();
        return false;
    } else {
        return !screenManager->GetPowerOffNeedProcessOneFrame();
    }
}

#ifdef FRAME_AWARE_TRACE
bool RSUniRenderUtil::FrameAwareTraceBoost(size_t layerNum)
{
    using namespace FRAME_TRACE;
    constexpr uint32_t FRAME_TRACE_LAYER_NUM_1 = 11;
    constexpr uint32_t FRAME_TRACE_LAYER_NUM_2 = 13;
    constexpr int32_t FRAME_TRACE_PERF_REQUESTED_CODE = 10024;
    RenderFrameTrace& ft = RenderFrameTrace::GetInstance();
    if (layerNum != FRAME_TRACE_LAYER_NUM_1 && layerNum != FRAME_TRACE_LAYER_NUM_2) {
        if (ft.RenderFrameTraceIsOpen()) {
            ft.RenderFrameTraceClose();
            PerfRequest(FRAME_TRACE_PERF_REQUESTED_CODE, false);
            RS_LOGD("RsDebug RSUniRenderUtil::Perf: FrameTrace 0");
        }
        return false;
    }

    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (isTimeOut || !ft.RenderFrameTraceIsOpen()) {
        if (!ft.RenderFrameTraceOpen()) {
            return false;
        }
        PerfRequest(FRAME_TRACE_PERF_REQUESTED_CODE, true);
        RS_LOGD("RsDebug RSProcessor::Perf: FrameTrace 1");
        lastRequestPerfTime = currentTime;
    }
    return true;
}
#endif

void RSUniRenderUtil::RequestPerf(uint32_t layerLevel, bool onOffTag)
{
    switch (layerLevel) {
        case PERF_LEVEL_0: {
            // do nothing
            RS_LOGD("RsDebug RSProcessor::perf do nothing");
            break;
        }
        case PERF_LEVEL_1: {
            PerfRequest(PERF_LEVEL_1_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level1 %{public}d", onOffTag);
            break;
        }
        case PERF_LEVEL_2: {
            PerfRequest(PERF_LEVEL_2_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level2 %{public}d", onOffTag);
            break;
        }
        default: {
            PerfRequest(PERF_LEVEL_3_REQUESTED_CODE, onOffTag);
            RS_LOGD("RsDebug RSProcessor::Perf: level3 %{public}d", onOffTag);
            break;
        }
    }
}

void RSUniRenderUtil::MultiLayersPerf(size_t layerNum)
{
    RS_LOGD("FrameAwareTraceBoost return false");
    static uint32_t lastLayerLevel = 0;
    constexpr uint32_t PERF_LEVEL_INTERVAL = 10;
    static std::chrono::steady_clock::time_point lastRequestPerfTime = std::chrono::steady_clock::now();
    auto curLayerLevel = layerNum / PERF_LEVEL_INTERVAL;
    if (curLayerLevel == 0 && layerNum >= PERF_LAYER_START_NUM) {
        curLayerLevel = 1;
    }
    auto currentTime = std::chrono::steady_clock::now();
    bool isTimeOut = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRequestPerfTime).
        count() > PERF_TIME_OUT;
    if (curLayerLevel != lastLayerLevel || isTimeOut) {
        if (!isTimeOut) {
            RequestPerf(lastLayerLevel, false);
        }
        RequestPerf(curLayerLevel, true);
        lastLayerLevel = curLayerLevel;
        lastRequestPerfTime = currentTime;
    }
}

Drawing::Rect RSUniRenderUtil::GetImageRegions(float screenWidth, float screenHeight,
    float realImageWidth, float realImageHeight)
{
    auto dstRect = Drawing::Rect(0, 0, screenWidth, screenHeight);
    if (ROSEN_EQ(realImageWidth, 0.0f) || ROSEN_EQ(realImageHeight, 0.0f)) {
        return dstRect;
    }
    float imageScaleWidth = screenWidth / static_cast<float>(realImageWidth);
    float imageScaleHeight = screenHeight / static_cast<float>(realImageHeight);
    auto imageWidth = realImageWidth * imageScaleHeight;
    auto imageHeight = realImageHeight * imageScaleWidth;
    // Ensure that the security mask is located in the middle of the virtual screen.
    if (imageScaleWidth > imageScaleHeight) {
        // Left and right set black
        float halfBoundWidthLeft = (screenWidth - imageWidth) / 2;
        float halfBoundWidthRight = halfBoundWidthLeft + imageWidth;
        dstRect = Drawing::Rect(halfBoundWidthLeft, 0, halfBoundWidthRight, screenHeight);
        return dstRect;
    }

    if (imageScaleWidth < imageScaleHeight) {
        // Up and down set black
        float halfBoundHeightTop = (screenHeight - imageHeight) / 2;
        float halfBoundHeightBottom = halfBoundHeightTop + imageHeight;
        dstRect = Drawing::Rect(0, halfBoundHeightTop, screenWidth, halfBoundHeightBottom);
        return dstRect;
    }
    return dstRect;
}

void RSUniRenderUtil::GetSampledDamageAndDrawnRegion(const ScreenInfo& screenInfo,
    const Occlusion::Region& srcDamageRegion, bool isDirtyAlignEnabled, Occlusion::Region& sampledDamageRegion,
    Occlusion::Region& sampledDrawnRegion)
{
    Drawing::Matrix scaleMatrix;
    scaleMatrix.SetScaleTranslate(screenInfo.samplingScale, screenInfo.samplingScale,
        screenInfo.samplingTranslateX, screenInfo.samplingTranslateY);
    const Vector4<int> expandSize{screenInfo.samplingDistance, screenInfo.samplingDistance,
        screenInfo.samplingDistance, screenInfo.samplingDistance};
    auto rects = srcDamageRegion.GetRegionRectIs();
    sampledDamageRegion.Reset();
    for (const auto& rect : rects) {
        RectI mappedRect = RSObjAbsGeometry::MapRect(rect.ConvertTo<float>(), scaleMatrix);
        Occlusion::Region mappedAndExpandedRegion{mappedRect.MakeOutset(expandSize)};
        sampledDamageRegion.OrSelf(mappedAndExpandedRegion);
    }

    Occlusion::Region drawnRegion = isDirtyAlignEnabled ?
        sampledDamageRegion.GetAlignedRegion(MAX_DIRTY_ALIGNMENT_SIZE) : sampledDamageRegion;

    Drawing::Matrix invertedScaleMatrix;
    if (!scaleMatrix.Invert(invertedScaleMatrix)) {
        RS_LOGW("%{public}s, invert scaleMatrix failed", __func__);
        sampledDrawnRegion = drawnRegion;
        return;
    }
    sampledDrawnRegion.Reset();
    for (const auto& rect : drawnRegion.GetRegionRectIs()) {
        RectI mappedRect = RSObjAbsGeometry::MapRect(rect.ConvertTo<float>(), invertedScaleMatrix);
        Occlusion::Region mappedRegion{mappedRect};
        sampledDrawnRegion.OrSelf(mappedRegion);
    }
}

float RSUniRenderUtil::GetYawFromQuaternion(const Quaternion& q)
{
    // Normalized quaternion
    float x = q[0];
    float y = q[1];
    float z = q[2];
    float w = q[3];
    float norm = std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
    if (ROSEN_EQ(norm, 0.f)) {
        RS_LOGW("RSUniRenderUtil::GetYawFromQuaternion, norm is 0");
        return 0;
    }
    x = x / norm;
    y = y / norm;
    z = z / norm;
    w = w / norm;

    // Hamilton(right-hand), Formula: atan2(2(wz + xy), w² + x² - y² - z²)
    float sinyCosp = 2.f * (w * z + x * y);
    float cosyCosp = (w * w + x * x) - (y * y + z * z);
    float yaw = std::atan2(sinyCosp, cosyCosp);

    return yaw * (180.f / PI);
}

void RSUniRenderUtil::CollectHardwareEnabledNodeByScreenNodeId(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& enableNodes,
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& topEnabledNodes, NodeId id)
{
    enableNodes.clear();
    topEnabledNodes.clear();

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGD("RSUniRenderUtil::CollectHardCursorDrawables uniParam is null");
        return;
    }

    auto& hardwareDrawables = uniParam->GetHardwareEnabledTypeDrawables();
    for (const auto& [screenNodeId, _, drawable] : hardwareDrawables) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceNodeDrawable || !surfaceNodeDrawable->ShouldPaint() ||
            screenNodeId != id) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc enabled 0"
        auto buffer = surfaceParams->GetBuffer();
        RSBaseRenderUtil::WriteSurfaceBufferToPng(buffer, surfaceParams->GetId());
        if (surfaceNodeDrawable->IsHardwareEnabledTopSurface() || surfaceParams->IsLayerTop()) {
            // surfaceNode which should be drawn above displayNode like pointer window
            topEnabledNodes.emplace_back(drawable);
        } else {
            // surfaceNode which should be drawn below displayNode
            enableNodes.emplace_back(drawable);
        }
    }

    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    auto iter = std::find_if(hardCursorDrawables.begin(), hardCursorDrawables.end(), [id](const auto& node) {
        return std::get<0>(node) == id;
    });
    if (iter == hardCursorDrawables.end()) {
        return;
    }
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(std::get<2>(*iter));
    if (!surfaceDrawable) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (surfaceParams == nullptr) {
        return;
    }
    if (!RSUniRenderThread::GetCaptureParam().isSnapshot_ && surfaceParams->GetHardCursorStatus()) {
        topEnabledNodes.emplace_back(surfaceDrawable);
    }
}

void RSUniRenderUtil::CollectHardwareEnabledNodesByDisplayNodeId(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& enableNodes,
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& topEnabledNodes, NodeId id)
{
    enableNodes.clear();
    topEnabledNodes.clear();

    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!uniParam) {
        RS_LOGD("RSUniRenderUtil::CollectHardCursorDrawables uniParam is null");
        return;
    }

    auto& hardwareDrawables = uniParam->GetHardwareEnabledTypeDrawables();
    for (const auto& [_, displayNodeId, drawable] : hardwareDrawables) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        if (!surfaceNodeDrawable || !surfaceNodeDrawable->ShouldPaint() ||
            displayNodeId != id) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->GetHardwareEnabled()) {
            continue;
        }
        // To get dump image
        // execute "param set rosen.dumpsurfacetype.enabled 4 && setenforce 0 && param set rosen.afbc enabled 0"
        auto buffer = surfaceParams->GetBuffer();
        RSBaseRenderUtil::WriteSurfaceBufferToPng(buffer, surfaceParams->GetId());
        if (surfaceNodeDrawable->IsHardwareEnabledTopSurface() || surfaceParams->IsLayerTop()) {
            // surfaceNode which should be drawn above displayNode like pointer window
            topEnabledNodes.emplace_back(drawable);
        } else {
            // surfaceNode which should be drawn below displayNode
            enableNodes.emplace_back(drawable);
        }
    }

    auto& hardCursorDrawables = uniParam->GetHardCursorDrawables();
    auto iter = std::find_if(hardCursorDrawables.begin(), hardCursorDrawables.end(), [id](const auto& node) {
        return std::get<1>(node) == id;
    });
    if (iter == hardCursorDrawables.end()) {
        return;
    }
    auto surfaceDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(std::get<2>(*iter));
    if (!surfaceDrawable) {
        return;
    }
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable->GetRenderParams().get());
    if (surfaceParams == nullptr) {
        return;
    }
    if (!RSUniRenderThread::GetCaptureParam().isSnapshot_ && surfaceParams->GetHardCursorStatus()) {
        topEnabledNodes.emplace_back(surfaceDrawable);
    }
}

void RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& drawables,
    Drawing::Canvas& canvas, RSScreenRenderParams& params)
{
    if (!RSSystemProperties::GetHardwareComposerEnabled()) {
        RS_LOGW("RSUniRenderUtil::AdjustZOrderAndDrawSurfaceNode: \
            HardwareComposer is not enabled.");
        return;
    }
    if (drawables.empty()) {
        return;
    }

    // sort the surfaceNodes by ZOrder
    std::stable_sort(drawables.begin(), drawables.end(), [](const auto& first, const auto& second) -> bool {
        auto firstDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(first);
        auto secondDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(second);
        if (!firstDrawable || !firstDrawable->GetRenderParams() ||
            !secondDrawable || !secondDrawable->GetRenderParams()) {
            return false;
        }
        return firstDrawable->GetRenderParams()->GetLayerInfo().zOrder <
                    secondDrawable->GetRenderParams()->GetLayerInfo().zOrder;
    });

    Drawing::AutoCanvasRestore acr(canvas, true);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(&canvas);
    if (!rscanvas) {
        RS_LOGE("RSScreenRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode, rscanvas is nullptr");
        return;
    }
    // draw hardware-composition nodes
    for (auto& drawable : drawables) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        if (!drawable || !drawable->GetRenderParams()) {
            RS_LOGE("RSScreenRenderNodeDrawable::AdjustZOrderAndDrawSurfaceNode surfaceParams is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->GetRenderParams().get());
        // SelfDrawingNodes need to use LayerMatrix(totalMatrix) when doing capturing
        auto matrix = surfaceParams->GetLayerInfo().matrix;
        // Use for mirror screen visible rect projection
        if (RSUniRenderThread::Instance().GetEnableVisibleRect()) {
            auto rect = surfaceParams->GetLayerInfo().dstRect;
            auto dstRect = Drawing::RectI(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
            const auto& visibleRect = RSUniRenderThread::Instance().GetVisibleRect();
            // samplingScale is 1.f if subsampling is off
            float samplingScale = params.GetScreenInfo().samplingScale;
            if (dstRect.Intersect(visibleRect) && ROSEN_NE(samplingScale, 0.f)) {
                // correct position, devided by samplingScale when subsampling is on
                matrix.PostTranslate(-visibleRect.GetLeft() / samplingScale, -visibleRect.GetTop() / samplingScale);
            } else {
                continue;
            }
        }
        canvas.ConcatMatrix(matrix);
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
        surfaceNodeDrawable->DealWithSelfDrawingNodeBuffer(*rscanvas, *surfaceParams);
    }
}

void RSUniRenderUtil::SwitchColorFilter(RSPaintFilterCanvas& canvas, float hdrBrightnessRatio, bool displayP3Enable)
{
    const auto& renderEngine = RSUniRenderThread::Instance().GetRenderEngine();
    if (!renderEngine) {
        RS_LOGE("RSScreenRenderNodeDrawable::SwitchColorFilter renderEngine is null");
        return;
    }
    ColorFilterMode colorFilterMode = renderEngine->GetColorFilterMode();
    if (colorFilterMode == ColorFilterMode::INVERT_COLOR_DISABLE_MODE ||
        colorFilterMode >= ColorFilterMode::DALTONIZATION_NORMAL_MODE) {
        return;
    }

    if (displayP3Enable) {
        SwitchColorFilterWithP3(canvas, colorFilterMode, hdrBrightnessRatio);
        return;
    }

    Drawing::AutoCanvasRestore acr(canvas, true);
    RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable::SetColorFilterModeToPaint mode:%d",
        static_cast<int32_t>(colorFilterMode));
    Drawing::Brush brush;
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush, hdrBrightnessRatio);
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    RSTagTracker tagTacker(
        renderEngine->GetRenderContext()->GetSharedDrGPUContext(),
        RSTagTracker::TAG_SAVELAYER_COLOR_FILTER);
#endif
    Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::INIT_WITH_PREVIOUS);
    canvas.SaveLayer(slr);
}

void RSUniRenderUtil::SwitchColorFilterWithP3(RSPaintFilterCanvas& canvas,
    ColorFilterMode colorFilterMode, float hdrBrightnessRatio)
{
    RS_TRACE_NAME_FMT("RSScreenRenderNodeDrawable::SwitchColorFilterWithP3 mode:%d",
        static_cast<int32_t>(colorFilterMode));
    
    int32_t offscreenWidth = canvas.GetWidth();
    int32_t offscreenHeight = canvas.GetHeight();

    Drawing::ImageInfo info = Drawing::ImageInfo { offscreenWidth, offscreenHeight,
        Drawing::COLORTYPE_RGBA_F16, Drawing::ALPHATYPE_PREMUL, Drawing::ColorSpace::CreateSRGB()};
    auto offscreenSurface = canvas.GetSurface()->MakeSurface(info);
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());

    Drawing::Brush brush;
    auto originSurfaceImage = canvas.GetSurface()->GetImageSnapshot();
    RSBaseRenderUtil::SetColorFilterModeToPaint(colorFilterMode, brush, hdrBrightnessRatio);
    offscreenCanvas->AttachBrush(brush);
    offscreenCanvas->DrawImage(*originSurfaceImage, 0.f, 0.f, Drawing::SamplingOptions());
    offscreenCanvas->DetachBrush();

    auto offscreenImage = offscreenCanvas->GetSurface()->GetImageSnapshot();
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ResetMatrix();
    canvas.DrawImage(*offscreenImage, 0.f, 0.f, Drawing::SamplingOptions());
}

} // namespace Rosen
} // namespace OHOS
