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
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#include "feature/uifirst/rs_uifirst_manager.h"
#include "info_collection/rs_gpu_dirty_region_collection.h"
#include "params/rs_display_render_params.h"
#include "params/rs_surface_render_params.h"
#include "rs_base_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_maskcolor_shader_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"

#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

#ifdef SOC_PERF_ENABLE
#include "socperf_client.h"
#endif
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
constexpr int MAX_DIRTY_ALIGNMENT_SIZE = 128;
constexpr int32_t NO_SPECIAL_LAYER = 0;
void PerfRequest(int32_t perfRequestCode, bool onOffTag)
{
#ifdef SOC_PERF_ENABLE
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(perfRequestCode, onOffTag, "");
    RS_LOGD("RSProcessor::soc perf info [%{public}d %{public}d]", perfRequestCode, onOffTag);
#endif
}
}
void RSUniRenderUtil::MergeDirtyRectAfterMergeHistory(
    std::shared_ptr<RSDirtyRegionManager> dirtyManager, Occlusion::Region& dirtyRegion)
{
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    if (clipRectThreshold < 1.f) {
        Occlusion::Region allDirtyRegion{ Occlusion::Rect{ dirtyManager->GetDirtyRegion() } };
        allDirtyRegion.OrSelf(dirtyRegion);
        auto bound = allDirtyRegion.GetBound();
        if (allDirtyRegion.GetSize() > 1 && !bound.IsEmpty() &&
            allDirtyRegion.Area() > bound.Area() * clipRectThreshold) {
            dirtyManager->MergeDirtyRectAfterMergeHistory(bound.ToRectI());
            RS_OPTIONAL_TRACE_NAME_FMT("dirty expand: %s to %s",
                allDirtyRegion.GetRegionInfo().c_str(), bound.GetRectInfo().c_str());
        }
    }
}

std::vector<RectI> RSUniRenderUtil::MergeDirtyHistory(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx, RSDisplayRenderParams& params)
{
    auto& uniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    // renderThreadParams/dirtyManager not null in caller
    auto dirtyManager = displayDrawable.GetSyncDirtyManager();
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(displayDrawable, bufferAge, params, false);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(
        curAllSurfaceDrawables, RSUniRenderThread::Instance().GetDrawStatusVec());
    if (uniParam->GetAdvancedDirtyType() == AdvancedDirtyRegionType::DISABLED &&
        !uniParam->IsDirtyAlignEnabled()) {
        MergeDirtyRectAfterMergeHistory(dirtyManager, dirtyRegion);
    }
    RectI screenRectI(0, 0, static_cast<int32_t>(screenInfo.phyWidth), static_cast<int32_t>(screenInfo.phyHeight));
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    // overlay display expand dirty region
    RSOverlayDisplayManager::Instance().ExpandDirtyRegion(*dirtyManager, screenInfo, dirtyRegion);
#endif
    Occlusion::Region globalDirtyRegion;
    for (const auto& rect : dirtyManager->GetAdvancedDirtyRegion()) {
        Occlusion::Region region = Occlusion::Region(Occlusion::Rect(rect));
        globalDirtyRegion.OrSelf(region);
        GpuDirtyRegionCollection::GetInstance().UpdateGlobalDirtyInfoForDFX(rect.IntersectRect(screenRectI));
    }
    Occlusion::Region damageRegion;
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
    Occlusion::Region drawnRegion;
    if (screenInfo.isSamplingOn && screenInfo.samplingScale > 0) {
        GetSampledDamageAndDrawnRegion(screenInfo, damageRegion, uniParam->IsDirtyAlignEnabled(),
            damageRegion, drawnRegion);
    } else {
        drawnRegion = uniParam->IsDirtyAlignEnabled() ?
            damageRegion.GetAlignedRegion(MAX_DIRTY_ALIGNMENT_SIZE) : damageRegion;
    }
    RSUniRenderUtil::SetDrawRegionForQuickReject(curAllSurfaceDrawables, drawnRegion);
    rsDirtyRectsDfx.SetDirtyRegion(drawnRegion);
    params.SetDrawnRegion(drawnRegion);
    auto damageRegionRects = RSUniRenderUtil::ScreenIntersectDirtyRects(damageRegion, screenInfo);
    if (damageRegionRects.empty()) {
        // When damageRegionRects is empty, SetDamageRegion function will not take effect and buffer will
        // full screen refresh. Therefore, we need to insert an empty rect into the damageRegionRects array
        damageRegionRects.emplace_back(RectI(0, 0, 0, 0));
    }
    return damageRegionRects;
}

std::vector<RectI> RSUniRenderUtil::MergeDirtyHistoryInVirtual(
    DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, int32_t bufferAge, ScreenInfo& screenInfo)
{
    auto params = static_cast<RSDisplayRenderParams*>(displayDrawable.GetRenderParams().get());
    auto& renderThreadParams = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    if (!renderThreadParams || !params) {
        return {};
    }
    auto& curAllSurfaceDrawables = params->GetAllMainAndLeashSurfaceDrawables();
    auto dirtyManager = displayDrawable.GetSyncDirtyManager();
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayDrawable, bufferAge);
    Occlusion::Region dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(curAllSurfaceDrawables);

    RectI rect = dirtyManager->GetRectFlipWithinSurface(dirtyManager->GetDirtyRegionInVirtual());
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
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

void RSUniRenderUtil::MergeDirtyHistoryForDrawable(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    int32_t bufferAge, RSDisplayRenderParams& params, bool useAlignedDirtyRegion)
{
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    // update all child surfacenode history
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (surfaceParams == nullptr || !surfaceParams->IsAppWindow()) {
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
    if (auto dirtyManager = displayDrawable.GetSyncDirtyManager()) {
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
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
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
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
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
        if (!surfaceParams || !surfaceParams->IsMainWindowType()) {
            continue;
        }
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (surfaceDirtyManager == nullptr) {
            continue;
        }
        surfaceDirtyManager->SetDirtyRegionForQuickReject(rectsForQuickReject);
    }
}

void RSUniRenderUtil::MergeDirtyHistoryInVirtual(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
    int32_t bufferAge, bool renderParallel)
{
    (void)renderParallel;
    auto& params = displayDrawable.GetRenderParams();
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
    auto displayDirtyManager = displayDrawable.GetSyncDirtyManager();
    displayDirtyManager->MergeDirtyHistoryInVirtual(bufferAge);
}

Occlusion::Region RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables)
{
    Occlusion::Region allSurfaceVisibleDirtyRegion;
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
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty() ||
            surfaceParams->GetName().find(CAPTURE_WINDOW_NAME) != std::string::npos ||
            surfaceParams->GetSpecialLayerMgr().Find(SpecialLayerType::SKIP)) {
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

std::vector<RectI> RSUniRenderUtil::GetCurrentFrameVisibleDirty(
    DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, ScreenInfo& screenInfo, RSDisplayRenderParams& params)
{
    Occlusion::Region damageRegions;
    auto& curAllSurfaceDrawables = params.GetAllMainAndLeashSurfaceDrawables();
    // update all child surfacenode history
    for (auto it = curAllSurfaceDrawables.rbegin(); it != curAllSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            RS_LOGI("GetCurrentFrameVisibleDirty surfaceNodeDrawable is nullptr");
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        auto surfaceDirtyManager = surfaceNodeDrawable->GetSyncDirtyManager();
        if (!surfaceParams || !surfaceDirtyManager) {
            RS_LOGI("RSUniRenderUtil::GetCurrentFrameVisibleDirty node(%{public}" PRIu64") params or "
                "dirty manager is nullptr", surfaceNodeDrawable->GetId());
            continue;
        }
        if (!surfaceParams->IsAppWindow() || surfaceParams->GetDstRect().IsEmpty()) {
            continue;
        }
        // for cross-display surface, only consider the dirty region on the first display (use global dirty for others).
        if (surfaceParams->IsFirstLevelCrossNode() &&
            !RSUniRenderThread::Instance().GetRSRenderThreadParams()->IsFirstVisitCrossNodeDisplay()) {
            continue;
        }
        auto visibleRegion = surfaceParams->GetVisibleRegion();
        auto surfaceCurrentFrameDirtyRegion = surfaceDirtyManager->GetCurrentFrameDirtyRegion();
        Occlusion::Region currentFrameDirtyRegion { Occlusion::Rect {
            surfaceCurrentFrameDirtyRegion.left_, surfaceCurrentFrameDirtyRegion.top_,
            surfaceCurrentFrameDirtyRegion.GetRight(), surfaceCurrentFrameDirtyRegion.GetBottom() } };
        Occlusion::Region damageRegion = currentFrameDirtyRegion.And(visibleRegion);
        damageRegions.OrSelf(damageRegion);
    }
    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(damageRegions, screenInfo);
    RectI rect = displayDrawable.GetSyncDirtyManager()->GetDirtyRegionFlipWithinSurface();
    if (!rect.IsEmpty()) {
        rects.emplace_back(rect);
    }
    return rects;
}

void RSUniRenderUtil::SetAllSurfaceDrawableGlobalDirtyRegion(
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceDrawables,
    const Occlusion::Region& globalDirtyRegion)
{
    // Set Surface Global Dirty Region
    for (auto it = allSurfaceDrawables.rbegin(); it != allSurfaceDrawables.rend(); ++it) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(*it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGW("RSUniRenderUtil::MergeVisibleDirtyRegion surface params is nullptr");
            continue;
        }
        if (!surfaceParams->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNodeDrawable
        surfaceNodeDrawable->SetGlobalDirtyRegion(globalDirtyRegion);
        surfaceNodeDrawable->SetDirtyRegionAlignedEnable(false);
    }
    Occlusion::Region curVisibleDirtyRegion;
    for (auto& it : allSurfaceDrawables) {
        auto surfaceNodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(it);
        if (surfaceNodeDrawable == nullptr) {
            continue;
        }
        auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceNodeDrawable->GetRenderParams().get());
        if (!surfaceParams) {
            RS_LOGE("RSUniRenderUtil::MergeVisibleDirtyRegion surface params is nullptr");
            continue;
        }
        if (!surfaceParams->IsMainWindowType()) {
            continue;
        }
        // set display dirty region to surfaceNodeDrawable
        surfaceNodeDrawable->SetDirtyRegionBelowCurrentLayer(curVisibleDirtyRegion);
        auto visibleDirtyRegion = surfaceNodeDrawable->GetVisibleDirtyRegion();
        curVisibleDirtyRegion = curVisibleDirtyRegion.Or(visibleDirtyRegion);
    }
}

std::vector<RectI> RSUniRenderUtil::ScreenIntersectDirtyRects(const Occlusion::Region &region, ScreenInfo& screenInfo)
{
    const std::vector<Occlusion::Rect>& rects = region.GetRegionRects();
    std::vector<RectI> retRects;
    for (const Occlusion::Rect& rect : rects) {
        // origin transformation
#ifdef RS_ENABLE_VK
        if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
            RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
            retRects.emplace_back(RectI(rect.left_, rect.top_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        } else {
            retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
                rect.right_ - rect.left_, rect.bottom_ - rect.top_));
        }
#else
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.bottom_,
            rect.right_ - rect.left_, rect.bottom_ - rect.top_));
#endif
    }
    RS_LOGD("ScreenIntersectDirtyRects size %{public}d %{public}s", region.GetSize(), region.GetRegionInfo().c_str());
    return retRects;
}

std::vector<RectI> RSUniRenderUtil::GetFilpDirtyRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return srcRects;
    }
#endif

    return FilpRects(srcRects, screenInfo);
}

std::vector<RectI> RSUniRenderUtil::FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo)
{
    std::vector<RectI> retRects;
    for (const RectI& rect : srcRects) {
        retRects.emplace_back(RectI(rect.left_, screenInfo.GetRotatedHeight() - rect.top_ - rect.height_,
            rect.width_, rect.height_));
    }
    return retRects;
}

GraphicIRect RSUniRenderUtil::IntersectRect(const GraphicIRect& first, const GraphicIRect& second)
{
    int left = std::max(first.x, second.x);
    int top = std::max(first.y, second.y);
    int right = std::min(first.x + first.w, second.x + second.w);
    int bottom = std::min(first.y + first.h, second.y + second.h);
    int width = right - left;
    int height = bottom - top;

    if (width <= 0 || height <= 0) {
        return GraphicIRect { 0, 0, 0, 0 };
    } else {
        return GraphicIRect { left, top, width, height };
    }
}

void RSUniRenderUtil::UpdateVirtualExpandDisplayAccumulatedParams(
    RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    // All other factors that may prevent skipping virtual expand display need to be considered
    // update accumulated dirty region
    params.SetAccumulatedDirty(params.GetAccumulatedDirty() ||
        (displayDrawable.GetSyncDirtyManager()->IsCurrentFrameDirty() || params.GetMainAndLeashSurfaceDirty()));
}

bool RSUniRenderUtil::CheckVirtualExpandDisplaySkip(
    RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable)
{
    // Regardless of whether the current frame is skipped, the state needs to be accumulated
    if (!RSSystemProperties::GetVirtualExpandScreenSkipEnabled()) {
        return false;
    }
    if (displayDrawable.GetSpecialLayerType(params) != NO_SPECIAL_LAYER) {
        RS_TRACE_NAME("CheckVirtualExpandDisplaySkip has special layer can not skip");
        return false;
    }
    RS_TRACE_NAME_FMT("CheckVirtualExpandDisplaySkip isAccumulatedDirty: %d", params.GetAccumulatedDirty());
    return !params.GetAccumulatedDirty();
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
    auto transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transform) != GSERROR_OK) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam GetSurfaceBufferTransformType failed");
    }
    RectF localBounds = { 0.0f, 0.0f, boundWidth, boundHeight };
    auto gravity = useRenderParams ? nodeParams->GetFrameGravity() : property.GetFrameGravity();
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, gravity, localBounds, params, surfaceParams);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, consumer, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, consumer, localBounds);
    }
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
    auto transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer->GetSurfaceBufferTransformType(buffer, &transform) != GSERROR_OK) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam GetSurfaceBufferTransformType failed");
    }
    params.preRotation = consumer->GetSurfaceAppFrameworkType() == "fixed-rotation" ? true : false;

    RectF localBounds = { 0.0f, 0.0f, boundWidth, boundHeight };
    auto gravity = nodeParams->GetFrameGravity();
    RSBaseRenderUtil::DealWithSurfaceRotationAndGravity(transform, gravity, localBounds, params, surfaceNodeParams);
    RSBaseRenderUtil::FlipMatrix(transform, params);
    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, consumer, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, consumer, localBounds);
    }
    return params;
}

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParamForRotationFixed(
    const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, RSSurfaceRenderParams& renderParams)
{
    BufferDrawParam params;
    params.threadIndex = static_cast<uint32_t>(gettid());
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

BufferDrawParam RSUniRenderUtil::CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU)
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
    auto displayDrawable = std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(drawable);
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    const sptr<SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
    if (!buffer) {
        RS_LOGE("RSUniRenderUtil::CreateBufferDrawParam buffer is null.");
        return params;
    }
    params.buffer = buffer;
    params.acquireFence = surfaceHandler->GetAcquireFence();
    SetSrcRect(params, buffer);
    params.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
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
    bufferDrawParam.buffer = buffer;
    bufferDrawParam.acquireFence = surfaceHandler.GetAcquireFence();
    SetSrcRect(bufferDrawParam, buffer);
    bufferDrawParam.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
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

    ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_CROP) {
        SrcRectScaleDown(params, buffer, surface, localBounds);
    } else if (scalingMode == ScalingMode::SCALING_MODE_SCALE_FIT) {
        SrcRectScaleFit(params, buffer, surface, localBounds);
    }
    return params;
}

bool RSUniRenderUtil::IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info)
{
    if (RSSystemProperties::IsForceClient()) {
        RS_LOGD("RSUniRenderUtil::IsNeedClient: force client.");
        return true;
    }
    const auto& property = node.GetRenderProperties();
    if (!ROSEN_EQ(property.GetRotation(), 0.f) || !ROSEN_EQ(property.GetRotationX(), 0.f) ||
        !ROSEN_EQ(property.GetRotationY(), 0.f) || property.GetQuaternion() != Quaternion()) {
        RS_LOGD("RSUniRenderUtil::IsNeedClient need client with RSSurfaceRenderNode rotation");
        return true;
    }
    return false;
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

float RSUniRenderUtil::GetFloatRotationDegreeFromMatrix(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    return atan2(value[Drawing::Matrix::Index::SKEW_X], value[Drawing::Matrix::Index::SCALE_X]) *
        (RS_ROTATION_180 / PI);
}

bool RSUniRenderUtil::HasNonZRotationTransform(Drawing::Matrix matrix)
{
    Drawing::Matrix::Buffer value;
    matrix.GetAll(value);
    if (!ROSEN_EQ(value[Drawing::Matrix::Index::PERSP_0], 0.f) ||
        !ROSEN_EQ(value[Drawing::Matrix::Index::PERSP_1], 0.f)) {
        return true;
    }
    int rotation = static_cast<int>(round(value[Drawing::Matrix::Index::SCALE_X] *
        value[Drawing::Matrix::Index::SKEW_Y] +
        value[Drawing::Matrix::Index::SCALE_Y] *
        value[Drawing::Matrix::Index::SKEW_X]));
    if (rotation != 0) {
        return true;
    }
    int vectorZ = value[Drawing::Matrix::Index::SCALE_X] * value[Drawing::Matrix::Index::SCALE_Y] -
        value[Drawing::Matrix::Index::SKEW_Y] * value[Drawing::Matrix::Index::SKEW_X];
    return vectorZ < 0;
}

void RSUniRenderUtil::SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    // sort subThreadNodes by priority and z-order
    subThreadNodes.sort([](const auto& first, const auto& second) -> bool {
        auto node1 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(first);
        auto node2 = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(second);
        if (node1 == nullptr || node2 == nullptr) {
            ROSEN_LOGE(
                "RSUniRenderUtil::SortSubThreadNodes sort nullptr found in subThreadNodes, this should not happen");
            return false;
        }
        if (node1->GetPriority() == node2->GetPriority()) {
            return node2->GetRenderProperties().GetPositionZ() < node1->GetRenderProperties().GetPositionZ();
        } else {
            return node1->GetPriority() < node2->GetPriority();
        }
    });
}

void RSUniRenderUtil::CacheSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& oldSubThreadNodes,
    std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes)
{
    std::unordered_set<std::shared_ptr<RSSurfaceRenderNode>> nodes(subThreadNodes.begin(), subThreadNodes.end());
    for (auto node : oldSubThreadNodes) {
        if (nodes.count(node) > 0) {
            continue;
        }
        // The node being processed by sub thread may have been removed.
        if (node->GetCacheSurfaceProcessedStatus() == CacheProcessStatus::DOING) {
            subThreadNodes.emplace_back(node);
        }
    }
    oldSubThreadNodes.clear();
    oldSubThreadNodes = subThreadNodes;
}

void RSUniRenderUtil::HandleHardwareNode(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr || !node->HasHardwareNode()) {
        return;
    }
    auto appWindow = node;
    if (node->IsLeashWindow()) {
        for (auto& child : *node->GetSortedChildren()) {
            auto surfaceNodePtr = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(child);
            if (surfaceNodePtr && surfaceNodePtr->IsAppWindow()) {
                appWindow = surfaceNodePtr;
                break;
            }
        }
    }
    auto hardwareEnabledNodes = appWindow->GetChildHardwareEnabledNodes();
    for (auto& hardwareEnabledNode : hardwareEnabledNodes) {
        auto hardwareEnabledNodePtr = hardwareEnabledNode.lock();
        if (hardwareEnabledNodePtr) {
            hardwareEnabledNodePtr->SetHardwareDisabledByCache(false);
        }
    }
}

void RSUniRenderUtil::ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface)
{
    RS_LOGD("ClearCacheSurface node: [%{public}" PRIu64 "]", node.GetId());
    uint32_t cacheSurfaceThreadIndex = node.GetCacheSurfaceThreadIndex();
    uint32_t completedSurfaceThreadIndex = node.GetCompletedSurfaceThreadIndex();
    if (cacheSurfaceThreadIndex == threadIndex && completedSurfaceThreadIndex == threadIndex) {
        node.ClearCacheSurface(isClearCompletedCacheSurface);
        return;
    }
    std::shared_ptr<Drawing::Surface> completedCacheSurface = isClearCompletedCacheSurface ?
        node.GetCompletedCacheSurface(threadIndex, false, true) : nullptr;
    ClearNodeCacheSurface(node.GetCacheSurface(threadIndex, false, true),
        std::move(completedCacheSurface), cacheSurfaceThreadIndex, completedSurfaceThreadIndex);
    node.ClearCacheSurface(isClearCompletedCacheSurface);
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
uint32_t RSUniRenderUtil::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::VULKAN &&
        OHOS::Rosen::RSSystemProperties::GetGpuApiType() != OHOS::Rosen::GpuApiType::DDGR) {
        return UINT32_MAX;
    }
    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();

    VkPhysicalDeviceMemoryProperties memProperties;
    vkContext.vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return UINT32_MAX;
}

void RSUniRenderUtil::SetVkImageInfo(std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo,
    const VkImageCreateInfo& imageInfo)
{
    if (vkImageInfo == nullptr) {
        return;
    }
    vkImageInfo->imageTiling = imageInfo.tiling;
    vkImageInfo->imageLayout = imageInfo.initialLayout;
    vkImageInfo->format = imageInfo.format;
    vkImageInfo->imageUsageFlags = imageInfo.usage;
    vkImageInfo->levelCount = imageInfo.mipLevels;
    vkImageInfo->currentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    vkImageInfo->ycbcrConversionInfo = {};
    vkImageInfo->sharingMode = imageInfo.sharingMode;
}

Drawing::BackendTexture RSUniRenderUtil::MakeBackendTexture(uint32_t width, uint32_t height, VkFormat format)
{
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageCreateInfo imageInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkDevice device = vkContext.GetDevice();
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    if (width * height > OHOS::Rosen::NativeBufferUtils::VKIMAGE_LIMIT_SIZE) {
        ROSEN_LOGE(
            "RSUniRenderUtil::MakeBackendTexture failed, image is too large, width:%{public}u, height::%{public}u",
            width, height);
        return {};
    }

    if (vkContext.vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        return {};
    }

    VkMemoryRequirements memRequirements;
    vkContext.vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (allocInfo.memoryTypeIndex == UINT32_MAX) {
        return {};
    }

    if (vkContext.vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        return {};
    }

    vkContext.vkBindImageMemory(device, image, memory, 0);

    OHOS::Rosen::Drawing::BackendTexture backendTexture(true);
    OHOS::Rosen::Drawing::TextureInfo textureInfo;
    textureInfo.SetWidth(width);
    textureInfo.SetHeight(height);

    std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo =
        std::make_shared<OHOS::Rosen::Drawing::VKTextureInfo>();
    vkImageInfo->vkImage = image;
    vkImageInfo->vkAlloc.memory = memory;
    vkImageInfo->vkAlloc.size = memRequirements.size;

    SetVkImageInfo(vkImageInfo, imageInfo);
    textureInfo.SetVKTextureInfo(vkImageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    return backendTexture;
}
#endif

GraphicTransformType RSUniRenderUtil::GetRotateTransformForRotationFixed(RSSurfaceRenderNode& node,
    sptr<IConsumerSurface> consumer)
{
    auto transformType = RSBaseRenderUtil::GetRotateTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(
        node.GetRSSurfaceHandler()->GetConsumer(), node.GetRSSurfaceHandler()->GetBuffer()));
    int extraRotation = 0;
    int degree = static_cast<int>(round(node.GetAbsRotation()));
    auto surfaceParams = node.GetStagingRenderParams() == nullptr
                             ? nullptr
                             : static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    int32_t rotationDegree = RSBaseRenderUtil::GetScreenRotationOffset(surfaceParams);
    extraRotation = (degree - rotationDegree) % ROUND_ANGLE;
    transformType = static_cast<GraphicTransformType>(
        (transformType + extraRotation / RS_ROTATION_90 + SCREEN_ROTATION_NUM) % SCREEN_ROTATION_NUM);
    return transformType;
}

void RSUniRenderUtil::UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    auto buffer = surfaceHandler->GetBuffer();
    if (!consumer || !buffer) {
        return;
    }
    auto bufferRotateTransformType = node.GetFixRotationByUser() ?
        RSUniRenderUtil::GetRotateTransformForRotationFixed(node, consumer) :
        RSBaseRenderUtil::GetRotateTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, buffer));
    const auto& property = node.GetRenderProperties();
    auto bufferWidth = buffer->GetSurfaceBufferWidth();
    auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto boundsWidth = property.GetBoundsWidth();
    const auto boundsHeight = property.GetBoundsHeight();
    auto srcRect = node.GetSrcRect();
    if (bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    if ((bufferWidth != boundsWidth || bufferHeight != boundsHeight) &&
        node.GetRenderProperties().GetFrameGravity() != Gravity::TOP_LEFT) {
        float xScale = (ROSEN_EQ(boundsWidth, 0.0f) ? 1.0f : bufferWidth / boundsWidth);
        float yScale = (ROSEN_EQ(boundsHeight, 0.0f) ? 1.0f : bufferHeight / boundsHeight);
        if (absRect == node.GetDstRect()) {
            // If the SurfaceRenderNode is completely in the DisplayRenderNode,
            // we do not need to crop the buffer.
            srcRect.width_ = bufferWidth;
            srcRect.height_ = bufferHeight;
        } else {
            srcRect.left_ = srcRect.left_ * xScale;
            srcRect.top_ = srcRect.top_ * yScale;
            srcRect.width_ = std::min(static_cast<int32_t>(std::ceil(srcRect.width_ * xScale)), bufferWidth);
            srcRect.height_ = std::min(static_cast<int32_t>(std::ceil(srcRect.height_ * yScale)), bufferHeight);
        }
    }
    RectI bufferRect(0, 0, bufferWidth, bufferHeight);
    RectI calibratedSrcRect = srcRect.IntersectRect(bufferRect);
    if (bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        bufferRotateTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(calibratedSrcRect.left_, calibratedSrcRect.top_);
        std::swap(calibratedSrcRect.width_, calibratedSrcRect.height_);
    }
    auto newSrcRect = SrcRectRotateTransform(*buffer, bufferRotateTransformType, calibratedSrcRect);
    node.SetSrcRect(newSrcRect);
}

GraphicTransformType RSUniRenderUtil::GetConsumerTransform(const RSSurfaceRenderNode& node,
    const sptr<SurfaceBuffer> buffer, const sptr<IConsumerSurface> consumer)
{
    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    if (consumer != nullptr && buffer != nullptr) {
        if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
            RS_LOGE("RSUniRenderUtil::GetConsumerTransform GetSurfaceBufferTransformType failed");
        }
    }
    GraphicTransformType consumerTransform = RSBaseRenderUtil::GetRotateTransform(transformType);
    return consumerTransform;
}

void RSUniRenderUtil::CalcSrcRectByBufferFlip(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    auto buffer = surfaceHandler->GetBuffer();
    if (!consumer || !buffer) {
        return;
    }
    const auto bufferWidth = buffer->GetSurfaceBufferWidth();
    const auto bufferHeight = buffer->GetSurfaceBufferHeight();
    const auto dstRect = node.GetDstRect();
    RectI srcRect = node.GetSrcRect();
    auto bufferFlipTransformType =
        RSBaseRenderUtil::GetFlipTransform(RSBaseRenderUtil::GetSurfaceBufferTransformType(consumer, buffer));
    switch (bufferFlipTransformType) {
        case GraphicTransformType::GRAPHIC_FLIP_H: {
            if (srcRect.left_ >= 0) {
                srcRect.left_ = bufferWidth - srcRect.left_ - srcRect.width_;
            } else if (dstRect.left_ + dstRect.width_ >= static_cast<int32_t>(screenInfo.width)) {
                srcRect.left_ = bufferWidth - srcRect.width_;
            }
            break;
        }
        case GraphicTransformType::GRAPHIC_FLIP_V: {
            if (srcRect.top_ >= 0) {
                srcRect.top_ = bufferHeight - srcRect.top_ - srcRect.height_;
            } else if (dstRect.top_ + dstRect.height_ >= static_cast<int32_t>(screenInfo.height)) {
                srcRect.top_ = bufferHeight - srcRect.height_;
            }
            break;
        }
        default:
            break;
    }
    node.SetSrcRect(srcRect);
}

RectI RSUniRenderUtil::SrcRectRotateTransform(const SurfaceBuffer& buffer,
    const GraphicTransformType bufferRotateTransformType, const RectI& newSrcRect)
{
    const auto bufferWidth = buffer.GetSurfaceBufferWidth();
    const auto bufferHeight = buffer.GetSurfaceBufferHeight();
    int left = newSrcRect.GetLeft();
    int top = newSrcRect.GetTop();
    int width = newSrcRect.GetWidth();
    int height = newSrcRect.GetHeight();
    RectI srcRect(newSrcRect);
    switch (bufferRotateTransformType) {
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            srcRect = RectI {bufferWidth - width - left, top, width, height};
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            srcRect = RectI {bufferWidth - width - left, bufferHeight - height - top, width, height};
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            srcRect = RectI {left, bufferHeight - height - top, width, height};
            break;
        }
        default:
            break;
    }
    return srcRect;
}

Drawing::Rect RSUniRenderUtil::CalcSrcRectByBufferRotation(const SurfaceBuffer& buffer,
    const GraphicTransformType consumerTransformType, Drawing::Rect newSrcRect)
{
    const float frameWidth = buffer.GetSurfaceBufferWidth();
    const float frameHeight = buffer.GetSurfaceBufferHeight();
    int left = std::clamp<int>(newSrcRect.GetLeft(), 0, frameWidth);
    int top = std::clamp<int>(newSrcRect.GetTop(), 0, frameHeight);
    int width = std::clamp<int>(newSrcRect.GetWidth(), 0, frameWidth - left);
    int height = std::clamp<int>(newSrcRect.GetHeight(), 0, frameHeight - top);
    switch (consumerTransformType) {
        case GraphicTransformType::GRAPHIC_ROTATE_NONE: {
            newSrcRect = Drawing::Rect(left, top, left + width, top + height);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            newSrcRect = Drawing::Rect(frameWidth - width - left, top, frameWidth - left, top + height);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            newSrcRect = Drawing::Rect(frameWidth - width - left, frameHeight - height - top,
                frameWidth - left, frameHeight - top);
            break;
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            newSrcRect = Drawing::Rect(left, frameHeight - height - top, left + width, frameHeight - top);
            break;
        }
        default:
            break;
    }
    return newSrcRect;
}

bool RSUniRenderUtil::IsHwcEnabledByGravity(RSSurfaceRenderNode& node, const Gravity frameGravity)
{
    // When renderfit mode is not Gravity::RESIZE or Gravity::TOP_LEFT,
    // we currently disable hardware composer.
    if (frameGravity != Gravity::RESIZE && frameGravity != Gravity::TOP_LEFT) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 "disabled by frameGravity[%d]",
            node.GetName().c_str(), node.GetId(), static_cast<int>(frameGravity));
        node.SetHardwareForcedDisabledState(true);
        return false;
    }
    return true;
}

void RSUniRenderUtil::DealWithNodeGravity(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return;
    }
    const auto buffer = surfaceHandler->GetBuffer();
    const auto consumer = surfaceHandler->GetConsumer();
    if (!consumer || !buffer) {
        return;
    }
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = node.GetRenderProperties().GetBoundsWidth();
    const float boundsHeight = node.GetRenderProperties().GetBoundsHeight();
    const Gravity frameGravity = node.GetRenderProperties().GetFrameGravity();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    CheckForceHardwareAndUpdateDstRect(node);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(frameGravity,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, gravityMatrix);
    Drawing::Matrix inverseTotalMatrix;
    Drawing::Matrix inverseGravityMatrix;
    // We don't have to do additional works when renderfit mode is Gravity::RESIZE or frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE ||
        (ROSEN_EQ(bufferWidth, boundsWidth) && ROSEN_EQ(bufferHeight, boundsHeight)) ||
        !totalMatrix.Invert(inverseTotalMatrix) || !gravityMatrix.Invert(inverseGravityMatrix) ||
        !IsHwcEnabledByGravity(node, frameGravity)) {
        return;
    }
    Drawing::Rect bound = Drawing::Rect(0.f, 0.f, boundsWidth, boundsHeight);
    Drawing::Rect frame = Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight);
    Drawing::Rect localIntersectRect;
    gravityMatrix.MapRect(localIntersectRect, frame);
    localIntersectRect.Intersect(bound);
    Drawing::Rect absIntersectRect;
    totalMatrix.MapRect(absIntersectRect, localIntersectRect);
    const RectI dstRect = node.GetDstRect();
    Drawing::Rect newDstRect(dstRect.left_, dstRect.top_, dstRect.GetRight(), dstRect.GetBottom());
    newDstRect.Intersect(absIntersectRect);
    node.SetDstRect({std::floor(newDstRect.GetLeft()), std::floor(newDstRect.GetTop()),
        std::ceil(newDstRect.GetWidth()), std::ceil(newDstRect.GetHeight())});
    Drawing::Rect newSrcRect;
    inverseTotalMatrix.MapRect(newSrcRect, newDstRect);
    inverseGravityMatrix.MapRect(newSrcRect, newSrcRect);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(newSrcRect.left_, newSrcRect.top_);
        std::swap(newSrcRect.right_, newSrcRect.bottom_);
    }
    newSrcRect = CalcSrcRectByBufferRotation(*buffer, consumerTransformType, newSrcRect);
    node.SetSrcRect({newSrcRect.GetLeft(), newSrcRect.GetTop(), newSrcRect.GetWidth(), newSrcRect.GetHeight()});
}

void RSUniRenderUtil::DealWithNodeGravityOldVersion(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (!buffer) {
        return;
    }
    const auto& property = node.GetRenderProperties();
    const float frameWidth = buffer->GetSurfaceBufferWidth();
    const float frameHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = property.GetBoundsWidth();
    const float boundsHeight = property.GetBoundsHeight();
    const Gravity frameGravity = property.GetFrameGravity();

    CheckForceHardwareAndUpdateDstRect(node);
    // we do not need to do additional works for Gravity::RESIZE and if frameSize == boundsSize.
    if (frameGravity == Gravity::RESIZE || (ROSEN_EQ(frameWidth, boundsWidth) && ROSEN_EQ(frameHeight, boundsHeight)) ||
        frameGravity == Gravity::TOP_LEFT) {
        return;
    }
 
    // get current node's translate matrix and calculate gravity matrix.
    auto translateMatrix = Drawing::Matrix();
    translateMatrix.Translate(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_X),
        std::ceil(node.GetTotalMatrix().Get(Drawing::Matrix::Index::TRANS_Y)));
    Drawing::Matrix gravityMatrix;
    (void)RSPropertiesPainter::GetGravityMatrix(frameGravity,
        RectF {0.0f, 0.0f, boundsWidth, boundsHeight}, frameWidth, frameHeight, gravityMatrix);
    // create a canvas to calculate new dstRect and new srcRect
    int32_t screenWidth = screenInfo.phyWidth;
    int32_t screenHeight = screenInfo.phyHeight;
    const auto screenRotation = screenInfo.rotation;
    if (screenRotation == ScreenRotation::ROTATION_90 || screenRotation == ScreenRotation::ROTATION_270) {
        std::swap(screenWidth, screenHeight);
    }
 
    auto canvas = std::make_unique<Drawing::Canvas>(screenWidth, screenHeight);
    canvas->ConcatMatrix(translateMatrix);
    canvas->ConcatMatrix(gravityMatrix);
    Drawing::Rect clipRect;
    gravityMatrix.MapRect(clipRect, Drawing::Rect(0, 0, frameWidth, frameHeight));
    canvas->ClipRect(Drawing::Rect(0, 0, clipRect.GetWidth(), clipRect.GetHeight()), Drawing::ClipOp::INTERSECT);
    Drawing::RectI newDstRect = canvas->GetDeviceClipBounds();
    auto dstRect = node.GetDstRect();
    // we make the newDstRect as the intersection of new and old dstRect,
    // to deal with the situation that frameSize > boundsSize.
    newDstRect.Intersect(Drawing::RectI(
        dstRect.left_, dstRect.top_, dstRect.width_ + dstRect.left_, dstRect.height_ + dstRect.top_));
    auto localRect = canvas->GetLocalClipBounds();
    int left = std::clamp<int>(localRect.GetLeft(), 0, frameWidth);
    int top = std::clamp<int>(localRect.GetTop(), 0, frameHeight);
    int width = std::clamp<int>(localRect.GetWidth(), 0, frameWidth - left);
    int height = std::clamp<int>(localRect.GetHeight(), 0, frameHeight - top);
 
    node.SetDstRect({newDstRect.GetLeft(), newDstRect.GetTop(), newDstRect.GetWidth(), newDstRect.GetHeight()});
    node.SetSrcRect({left, top, width, height});
}

void RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler->GetConsumer() || !node.IsInFixedRotation() || !surfaceHandler->GetBuffer()) {
        return;
    }
    node.SetSrcRect(node.GetOriginalSrcRect());
    auto dstRect = node.GetDstRect();
    auto originalDstRect = node.GetOriginalDstRect();
    dstRect.left_ += (dstRect.width_ - originalDstRect.width_) / 2;
    dstRect.top_ += (dstRect.height_ - originalDstRect.height_) / 2;
    dstRect.width_ = originalDstRect.width_;
    dstRect.height_ = originalDstRect.height_;
    node.SetDstRect(dstRect);
}

void RSUniRenderUtil::LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    const auto screenWidth = static_cast<int32_t>(screenInfo.width);
    const auto screenHeight = static_cast<int32_t>(screenInfo.height);
    const auto screenRotation = screenInfo.rotation;
    const auto rect = node.GetDstRect();
    switch (screenRotation) {
        case ScreenRotation::ROTATION_90: {
            node.SetDstRect({rect.top_, screenHeight - rect.left_ - rect.width_, rect.height_, rect.width_});
            break;
        }
        case ScreenRotation::ROTATION_180: {
            node.SetDstRect({screenWidth - rect.left_ - rect.width_, screenHeight - rect.top_ - rect.height_,
                rect.width_, rect.height_});
            break;
        }
        case ScreenRotation::ROTATION_270: {
            node.SetDstRect({screenWidth - rect.top_ - rect.height_, rect.left_, rect.height_, rect.width_});
            break;
        }
        default:  {
            break;
        }
    }
}

GraphicTransformType RSUniRenderUtil::GetLayerTransform(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return GraphicTransformType::GRAPHIC_ROTATE_NONE;
    }
    auto consumer = surfaceHandler->GetConsumer();
    auto surfaceParams = node.GetStagingRenderParams() == nullptr
                             ? nullptr
                             : static_cast<RSSurfaceRenderParams*>(node.GetStagingRenderParams().get());
    int32_t rotationDegree = RSBaseRenderUtil::GetScreenRotationOffset(surfaceParams);
    int surfaceNodeRotation = 0;
    if (node.GetFixRotationByUser()) {
        surfaceNodeRotation = -1 * rotationDegree;
    } else {
        surfaceNodeRotation =
            TransferToAntiClockwiseDegrees(static_cast<int>(round(node.GetAbsRotation())) % ROUND_ANGLE);
    }
    auto transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    auto buffer = node.GetRSSurfaceHandler()->GetBuffer();
    if (consumer != nullptr && buffer != nullptr) {
        if (consumer->GetSurfaceBufferTransformType(buffer, &transformType) != GSERROR_OK) {
            RS_LOGE("RSUniRenderUtil::GetLayerTransform GetSurfaceBufferTransformType failed");
        }
    }
    int consumerTransform = RSBaseRenderUtil::RotateEnumToInt(RSBaseRenderUtil::GetRotateTransform(transformType));
    GraphicTransformType consumerFlip = RSBaseRenderUtil::GetFlipTransform(transformType);
    int totalRotation =
        (RSBaseRenderUtil::RotateEnumToInt(screenInfo.rotation) + surfaceNodeRotation + consumerTransform + 360) % 360;
    GraphicTransformType rotateEnum = RSBaseRenderUtil::RotateEnumToInt(totalRotation, consumerFlip);

    RS_OPTIONAL_TRACE_NAME_FMT("RSUniRenderUtil::GetLayerTransform nodeId:[%llu] fixRotationByUser:[%s] "
                               "surfaceNodeRotation:[%d] consumerTransform:[%d] rotateEnum:[%d]",
                               node.GetId(), std::to_string(node.GetFixRotationByUser()).c_str(),
                               surfaceNodeRotation, consumerTransform, rotateEnum);
            
    return rotateEnum;
}

void RSUniRenderUtil::LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo)
{
    auto dstRect = node.GetDstRect();
    auto srcRect = node.GetSrcRect();
    auto originSrcRect = srcRect;

    RectI dstRectI(dstRect.left_, dstRect.top_, dstRect.width_, dstRect.height_);
    int32_t screenWidth = static_cast<int32_t>(screenInfo.phyWidth);
    int32_t screenHeight = static_cast<int32_t>(screenInfo.phyHeight);
    RectI screenRectI(0, 0, screenWidth, screenHeight);
    RectI resDstRect = dstRectI.IntersectRect(screenRectI);
    if (resDstRect == dstRectI) {
        return;
    }
    if (node.IsInFixedRotation()) {
        node.SetDstRect(resDstRect);
        return;
    }
    dstRect = {resDstRect.left_, resDstRect.top_, resDstRect.width_, resDstRect.height_};
    srcRect.left_ = (resDstRect.IsEmpty() || dstRectI.IsEmpty()) ? 0 : std::ceil((resDstRect.left_ - dstRectI.left_) *
        originSrcRect.width_ / dstRectI.width_);
    srcRect.top_ = (resDstRect.IsEmpty() || dstRectI.IsEmpty()) ? 0 : std::ceil((resDstRect.top_ - dstRectI.top_) *
        originSrcRect.height_ / dstRectI.height_);
    srcRect.width_ = dstRectI.IsEmpty() ? 0 : originSrcRect.width_ * resDstRect.width_ / dstRectI.width_;
    srcRect.height_ = dstRectI.IsEmpty() ? 0 : originSrcRect.height_ * resDstRect.height_ / dstRectI.height_;
    node.SetDstRect(dstRect);
    node.SetSrcRect(srcRect);
}

bool RSUniRenderUtil::IsHwcEnabledByScalingMode(RSSurfaceRenderNode& node, const ScalingMode scalingMode)
{
    // We temporarily disabled HWC when scalingMode is freeze or no_scale_crop
    if (scalingMode == ScalingMode::SCALING_MODE_FREEZE || scalingMode == ScalingMode::SCALING_MODE_NO_SCALE_CROP) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 "disabled by scalingMode[%d]",
            node.GetName().c_str(), node.GetId(), static_cast<int>(scalingMode));
        node.SetHardwareForcedDisabledState(true);
        return false;
    }
    return true;
}

void RSUniRenderUtil::DealWithScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    if (!surfaceHandler) {
        return;
    }
    const auto buffer = surfaceHandler->GetBuffer();
    const auto consumer = surfaceHandler->GetConsumer();
    if (!consumer || !buffer) {
        return;
    }
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const float boundsWidth = node.GetRenderProperties().GetBoundsWidth();
    const float boundsHeight = node.GetRenderProperties().GetBoundsHeight();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    const Gravity frameGravity = node.GetRenderProperties().GetFrameGravity();
    const ScalingMode scalingMode = buffer->GetSurfaceBufferScalingMode();
    if (scalingMode == ScalingMode::SCALING_MODE_SCALE_TO_WINDOW || !IsHwcEnabledByScalingMode(node, scalingMode) ||
        !IsHwcEnabledByGravity(node, frameGravity)) {
        return;
    }
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Matrix gravityMatrix;
    RSPropertiesPainter::GetGravityMatrix(frameGravity,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, gravityMatrix);
    Drawing::Matrix rawScalingModeMatrix;
    RSPropertiesPainter::GetScalingModeMatrix(scalingMode,
        {0.f, 0.f, boundsWidth, boundsHeight}, bufferWidth, bufferHeight, rawScalingModeMatrix);
    Drawing::Rect rawSrcRect;
    rawScalingModeMatrix.MapRect(rawSrcRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    float xScale = ROSEN_EQ(boundsWidth, 0.f) ? 1.0f : rawSrcRect.GetWidth() / boundsWidth;
    float yScale = ROSEN_EQ(boundsHeight, 0.f) ? 1.0f : rawSrcRect.GetHeight() / boundsHeight;
    Drawing::Matrix scalingModeMatrix;
    scalingModeMatrix.PostScale(xScale, yScale);
    Drawing::Rect adjustedSrcRect;
    scalingModeMatrix.MapRect(adjustedSrcRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    float xTranslate = (bufferWidth - adjustedSrcRect.GetWidth()) / 2.0f;
    float yTranslate = (bufferHeight - adjustedSrcRect.GetHeight()) / 2.0f;
    scalingModeMatrix.PostTranslate(xTranslate, yTranslate);
    UpdateHwcNodeByScalingMode(node, totalMatrix, gravityMatrix, scalingModeMatrix);
}

void RSUniRenderUtil::UpdateHwcNodeByScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix,
    const Drawing::Matrix& gravityMatrix, const Drawing::Matrix& scalingModeMatrix)
{
    auto surfaceHandler = node.GetRSSurfaceHandler();
    const auto& property = node.GetRenderProperties();
    const auto buffer = surfaceHandler->GetBuffer();
    const auto consumer = surfaceHandler->GetConsumer();
    float bufferWidth = buffer->GetSurfaceBufferWidth();
    float bufferHeight = buffer->GetSurfaceBufferHeight();
    const GraphicTransformType consumerTransformType = node.GetFixRotationByUser() ?
        GetRotateTransformForRotationFixed(node, consumer) : GetConsumerTransform(node, buffer, consumer);
    Drawing::Matrix inverseScalingModeMatrix;
    Drawing::Matrix inverseGravityMatrix;
    Drawing::Matrix inverseTotalMatrix;
    if (!totalMatrix.Invert(inverseTotalMatrix) || !scalingModeMatrix.Invert(inverseScalingModeMatrix) ||
        !gravityMatrix.Invert(inverseGravityMatrix)) {
        return;
    }
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(bufferWidth, bufferHeight);
    }
    Drawing::Rect newDstRect;
    scalingModeMatrix.MapRect(newDstRect, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    gravityMatrix.MapRect(newDstRect, newDstRect);
    totalMatrix.MapRect(newDstRect, newDstRect);
    Drawing::Rect dstRectWithoutScaling;
    gravityMatrix.MapRect(dstRectWithoutScaling, Drawing::Rect(0.f, 0.f, bufferWidth, bufferHeight));
    totalMatrix.MapRect(dstRectWithoutScaling, dstRectWithoutScaling);
    newDstRect.Intersect(dstRectWithoutScaling);
    Drawing::Rect bounds = node.GetDstRectWithoutRenderFit();
    newDstRect.Intersect(bounds);
    node.SetDstRect({std::floor(newDstRect.GetLeft()), std::floor(newDstRect.GetTop()),
        std::ceil(newDstRect.GetWidth()), std::ceil(newDstRect.GetHeight())});
    Drawing::Rect newSrcRect;
    inverseTotalMatrix.MapRect(newSrcRect, newDstRect);
    inverseGravityMatrix.MapRect(newSrcRect, newSrcRect);
    inverseScalingModeMatrix.MapRect(newSrcRect, newSrcRect);
    if (consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_90 ||
        consumerTransformType == GraphicTransformType::GRAPHIC_ROTATE_270) {
        std::swap(newSrcRect.left_, newSrcRect.top_);
        std::swap(newSrcRect.right_, newSrcRect.bottom_);
    }
    newSrcRect = CalcSrcRectByBufferRotation(*buffer, consumerTransformType, newSrcRect);
    node.SetSrcRect({newSrcRect.GetLeft(), newSrcRect.GetTop(), newSrcRect.GetWidth(), newSrcRect.GetHeight()});
}

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
        GrBackendSemaphore backendSemaphore;
        backendSemaphore.initVulkan(semaphore);

        DestroySemaphoreInfo* destroyInfo =
            new DestroySemaphoreInfo(vkContext.vkDestroySemaphore, vkContext.GetDevice(), semaphore);

        Drawing::FlushInfo drawingFlushInfo;
        drawingFlushInfo.backendSurfaceAccess = true;
        drawingFlushInfo.numSemaphores = 1;
        drawingFlushInfo.backendSemaphore = static_cast<void*>(&backendSemaphore);
        drawingFlushInfo.finishedProc = [](void *context) {
            DestroySemaphoreInfo::DestroySemaphore(context);
        };
        drawingFlushInfo.finishedContext = destroyInfo;
        surface->Flush(&drawingFlushInfo);
        grContext->Submit();
        DestroySemaphoreInfo::DestroySemaphore(destroyInfo);
    } else {
        surface->FlushAndSubmit(true);
    }
#else
    surface->FlushAndSubmit(true);
#endif
}

void RSUniRenderUtil::AccumulateMatrixAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
    Drawing::Matrix& matrix, float& alpha)
{
    if (hwcNode == nullptr) {
        return;
    }
    const auto& property = hwcNode->GetRenderProperties();
    alpha = property.GetAlpha();
    matrix = property.GetBoundsGeometry()->GetMatrix();
    auto parent = hwcNode->GetParent().lock();
    while (parent && parent->GetType() != RSRenderNodeType::DISPLAY_NODE) {
        const auto& curProperty = parent->GetRenderProperties();
        alpha *= curProperty.GetAlpha();
        matrix.PostConcat(curProperty.GetBoundsGeometry()->GetMatrix());
        parent = parent->GetParent().lock();
    }
    if (!parent) {
        return;
    }
    const auto& parentProperty = parent->GetRenderProperties();
    alpha *= parentProperty.GetAlpha();
    matrix.PostConcat(parentProperty.GetBoundsGeometry()->GetMatrix());
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

std::optional<Drawing::Matrix> RSUniRenderUtil::GetMatrix(
    std::shared_ptr<RSRenderNode> hwcNode)
{
    if (!hwcNode) {
        return std::nullopt;
    }
    auto relativeMat = Drawing::Matrix();
    auto& property = hwcNode->GetRenderProperties();
    if (auto geo = property.GetBoundsGeometry()) {
        if (LIKELY(!property.GetSandBox().has_value())) {
            relativeMat = geo->GetMatrix();
        } else {
            auto parent = hwcNode->GetParent().lock();
            if (!parent) {
                return std::nullopt;
            }
            if (auto parentGeo = parent->GetRenderProperties().GetBoundsGeometry()) {
                auto invertAbsParentMatrix = Drawing::Matrix();
                parentGeo->GetAbsMatrix().Invert(invertAbsParentMatrix);
                relativeMat = geo->GetAbsMatrix();
                relativeMat.PostConcat(invertAbsParentMatrix);
            }
        }
    } else {
        return std::nullopt;
    }
    return relativeMat;
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

void RSUniRenderUtil::UpdateHwcNodeProperty(std::shared_ptr<RSSurfaceRenderNode> hwcNode)
{
    if (hwcNode == nullptr) {
        RS_LOGE("hwcNode is null.");
        return;
    }
    auto hwcNodeGeo = hwcNode->GetRenderProperties().GetBoundsGeometry();
    if (!hwcNodeGeo) {
        RS_LOGE("hwcNode Geometry is not prepared.");
        return;
    }
    bool hasCornerRadius = !hwcNode->GetRenderProperties().GetCornerRadius().IsZero();
    std::vector<RectI> currIntersectedRoundCornerAABBs = {};
    float alpha = hwcNode->GetRenderProperties().GetAlpha();
    Drawing::Matrix totalMatrix = hwcNodeGeo->GetMatrix();
    auto hwcNodeRect = hwcNodeGeo->GetAbsRect();
    bool isNodeRenderByDrawingCache = false;
    bool isNodeRenderBySaveLayer = false;
    hwcNode->SetAbsRotation(hwcNode->GetRenderProperties().GetRotation());
    RSUniRenderUtil::TraverseParentNodeAndReduce(
        hwcNode,
        [&isNodeRenderByDrawingCache](std::shared_ptr<RSRenderNode> parent) {
            if (isNodeRenderByDrawingCache) {
                return;
            }
            // if the parent node of hwcNode is marked freeze or nodegroup, RS closes hardware composer of hwcNode.
            isNodeRenderByDrawingCache = isNodeRenderByDrawingCache || parent->IsStaticCached() ||
                (parent->GetNodeGroupType() != RSRenderNode::NodeGroupType::NONE);
        },
        [&alpha](std::shared_ptr<RSRenderNode> parent) {
            auto& parentProperty = parent->GetRenderProperties();
            alpha *= parentProperty.GetAlpha();
        },
        [&totalMatrix](std::shared_ptr<RSRenderNode> parent) {
            if (auto opt = RSUniRenderUtil::GetMatrix(parent)) {
                totalMatrix.PostConcat(opt.value());
            } else {
                return;
            }
        },
        [&currIntersectedRoundCornerAABBs, hwcNodeRect](std::shared_ptr<RSRenderNode> parent) {
            auto& parentProperty = parent->GetRenderProperties();
            auto cornerRadius = parentProperty.GetCornerRadius();
            auto maxCornerRadius = *std::max_element(std::begin(cornerRadius.data_), std::end(cornerRadius.data_));
            auto parentGeo = parentProperty.GetBoundsGeometry();
            static const std::array offsetVecs {
                UIPoint { 0, 0 },
                UIPoint { 1, 0 },
                UIPoint { 0, 1 },
                UIPoint { 1, 1 }
            };

            // The logic here is to calculate whether the HWC Node affects
            // the round corner property of the parent node.
            // The method is calculating the rounded AABB of each HWC node
            // with respect to all parent nodes above it and storing the results.
            // When a HWC node is found below, the AABBs and the HWC node
            // are checked for intersection. If there is an intersection,
            // the node above it is disabled from taking the HWC pipeline.
            auto checkIntersectWithRoundCorner = [&currIntersectedRoundCornerAABBs, hwcNodeRect](
                const RectI& rect, float radiusX, float radiusY) {
                if (radiusX <= 0 || radiusY <= 0) {
                    return;
                }
                UIPoint offset { rect.GetWidth() - radiusX, rect.GetHeight() - radiusY };
                UIPoint anchorPoint { rect.GetLeft(), rect.GetTop() };
                std::for_each(std::begin(offsetVecs), std::end(offsetVecs),
                    [&currIntersectedRoundCornerAABBs, hwcNodeRect, offset,
                        radiusX, radiusY, anchorPoint](auto offsetVec) {
                        auto res = anchorPoint + offset * offsetVec;
                        auto roundCornerAABB = RectI(res.x_, res.y_, radiusX, radiusY);
                        if (!roundCornerAABB.IntersectRect(hwcNodeRect).IsEmpty()) {
                            currIntersectedRoundCornerAABBs.push_back(roundCornerAABB);
                        }
                    }
                );
            };
            if (parentGeo) {
                auto parentRect = parentGeo->GetAbsRect();
                checkIntersectWithRoundCorner(parentRect, maxCornerRadius, maxCornerRadius);

                if (parentProperty.GetClipToRRect()) {
                    RRect parentClipRRect = parentProperty.GetClipRRect();
                    RectI parentClipRect = parentGeo->MapAbsRect(parentClipRRect.rect_);
                    float maxClipRRectCornerRadiusX = 0;
                    float maxClipRRectCornerRadiusY = 0;
                    constexpr size_t radiusVecSize = 4;
                    for (size_t i = 0; i < radiusVecSize; ++i) {
                        maxClipRRectCornerRadiusX = std::max(maxClipRRectCornerRadiusX, parentClipRRect.radius_[i].x_);
                        maxClipRRectCornerRadiusY = std::max(maxClipRRectCornerRadiusY, parentClipRRect.radius_[i].y_);
                    }
                    checkIntersectWithRoundCorner(parentClipRect, maxClipRRectCornerRadiusX, maxClipRRectCornerRadiusY);
                }
            }
        },
        [hwcNode](std::shared_ptr<RSRenderNode> parent) {
            hwcNode->SetAbsRotation(hwcNode->GetAbsRotation() + parent->GetRenderProperties().GetRotation());
        },
        [&isNodeRenderBySaveLayer](std::shared_ptr<RSRenderNode> parent) {
            if (isNodeRenderBySaveLayer) {
                return;
            }
            const auto& parentProperty = parent->GetRenderProperties();
            isNodeRenderBySaveLayer = isNodeRenderBySaveLayer ||
                (parentProperty.IsColorBlendApplyTypeOffscreen() && !parentProperty.IsColorBlendModeNone());
        });
    if (isNodeRenderByDrawingCache || isNodeRenderBySaveLayer) {
        RS_OPTIONAL_TRACE_NAME_FMT("hwc debug: name:%s id:%" PRIu64 " disabled by drawing cache or save layer, ",
            "isNodeRenderByDrawingCache[%d] isNodeRenderBySaveLayer[%d]",
            hwcNode->GetName().c_str(), hwcNode->GetId(), isNodeRenderByDrawingCache, isNodeRenderBySaveLayer);
        hwcNode->SetHardwareForcedDisabledState(true);
    }
    hwcNode->SetTotalMatrix(totalMatrix);
    hwcNode->SetGlobalAlpha(alpha);
    hwcNode->SetIntersectedRoundCornerAABBs(std::move(currIntersectedRoundCornerAABBs));
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
} // namespace Rosen
} // namespace OHOS
