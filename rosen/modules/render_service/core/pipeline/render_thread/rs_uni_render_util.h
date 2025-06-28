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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H

#include <list>
#include <mutex>
#include <set>
#include <unordered_map>

#include "surface.h"
#include "surface_type.h"
#include "sync_fence.h"

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_display_render_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"
#include "rs_base_render_engine.h"
#include "rs_base_render_util.h"
#include "transaction/rs_uiextension_data.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
class RSDirtyRectsDfx;
class RSUniRenderUtil {
public:
    // planning: dirty region related functions should be moved to feature_utils
    static void ExpandDamageRegionToSingleRect(Occlusion::Region& damageRegion);
    static std::vector<RectI> MergeDirtyHistory(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx, RSDisplayRenderParams& params);
    // isSecScreen means that special layers can be displayed on the screen, for example wired mirror screen.
    static std::vector<RectI> MergeDirtyHistoryInVirtual(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        int32_t bufferAge, ScreenInfo& screenInfo, bool isSecScreen = false);
    // merge history dirty region of current display node and its child surfacenode(app windows)
    // for mirror display, call this function twice will introduce additional dirtyhistory in dirtymanager
    static void MergeDirtyHistoryForDrawable(DrawableV2::RSDisplayRenderNodeDrawable& drawable, int32_t bufferAge,
        RSDisplayRenderParams& params, bool useAlignedDirtyRegion = false);
    static void SetDrawRegionForQuickReject(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceDrawables,
        const Occlusion::Region mergedDirtyRects);
    /* we want to set visible dirty region of each surfacenode into DamageRegionKHR interface, hence
     * occlusion is calculated.
     * make sure this function is called after merge dirty history
     */
    static Occlusion::Region MergeVisibleDirtyRegion(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
        std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec, bool useAlignedDirtyRegion = false);
    static Occlusion::Region MergeVisibleAdvancedDirtyRegion(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
        std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec);
    static Occlusion::Region MergeDirtyRects(Occlusion::Region dirtyRegion);
    static void MergeDirtyHistoryInVirtual(
        DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, int32_t bufferAge, bool renderParallel = false);
    // isSecScreen means that special layers can be displayed on the screen, for example wired mirror screen.
    static Occlusion::Region MergeVisibleDirtyRegionInVirtual(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables,
        RSDisplayRenderParams& displayParams, bool isSecScreen = false);
    // This is used for calculate matrix from buffer coordinate to window's relative coordinate
    static Drawing::Matrix GetMatrixOfBufferToRelRect(const RSSurfaceRenderNode& node);
    static void SrcRectScaleDown(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
        const sptr<IConsumerSurface>& surface, RectF& localBounds);
    static void SrcRectScaleFit(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
        const sptr<IConsumerSurface>& surface, RectF& localBounds);
    static BufferDrawParam CreateBufferDrawParam(const RSSurfaceRenderNode& node,
        bool forceCPU, uint32_t threadIndex = UNI_RENDER_THREAD_INDEX, bool useRenderParams = false);
    static BufferDrawParam CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU);
    static BufferDrawParam CreateBufferDrawParam(const RSSurfaceHandler& surfaceHandler, bool forceCPU);
    static BufferDrawParam CreateBufferDrawParam(
        const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, bool forceCPU, uint32_t threadIndex);
    static BufferDrawParam CreateBufferDrawParamForRotationFixed(
        const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, RSSurfaceRenderParams& renderParams);
    static BufferDrawParam CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU);
    static void DealWithRotationAndGravityForRotationFixed(GraphicTransformType transform, Gravity gravity,
        RectF& localBounds, BufferDrawParam& params);
    static bool IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info);
    static void DrawRectForDfx(RSPaintFilterCanvas& canvas, const RectI& rect, Drawing::Color color,
        float alpha, const std::string& extraInfo = "");
    static Occlusion::Region AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits = 32);
    static int TransferToAntiClockwiseDegrees(int angle);
    static int GetRotationFromMatrix(Drawing::Matrix matrix);
    static int GetRotationDegreeFromMatrix(Drawing::Matrix matrix);

    static void ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface = true);
    static void ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface>&& cacheSurface,
        std::shared_ptr<Drawing::Surface>&& cacheCompletedSurface,
        uint32_t cacheSurfaceThreadIndex, uint32_t completedSurfaceThreadIndex);
    static void OptimizedFlushAndSubmit(std::shared_ptr<Drawing::Surface>& surface,
        Drawing::GPUContext* const grContext, bool optFenceWait = true);
    static std::vector<GrBackendSemaphore> PrepareHdrSemaphoreVector(GrBackendSemaphore& backendSemaphore,
        std::shared_ptr<Drawing::Surface>& surface);
    static SecRectInfo GenerateSecRectInfoFromNode(RSRenderNode& node, RectI rect);
    static SecSurfaceInfo GenerateSecSurfaceInfoFromNode(
        NodeId uiExtensionId, NodeId hostId, SecRectInfo uiExtensionRectInfo);
    static void UIExtensionFindAndTraverseAncestor(
        const RSRenderNodeMap& nodeMap, UIExtensionCallbackData& callbackData, bool isUnobscured = false);
    static void TraverseAndCollectUIExtensionInfo(std::shared_ptr<RSRenderNode> node,
        Drawing::Matrix parentMatrix, NodeId hostId, UIExtensionCallbackData& callbackData, bool isUnobscured = false);
    static void ProcessCacheImage(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed);
    static void ProcessCacheImageRect(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed,
        const Drawing::Rect& src, const Drawing::Rect& dst);
    static void ProcessCacheImageForMultiScreenView(RSPaintFilterCanvas& canvas, Drawing::Image& cacheImageProcessed,
        const RectF& rect);
    static void FlushDmaSurfaceBuffer(Media::PixelMap* pixelMap);
    // RTthread needs to draw one more frame when screen is turned off. For other threads, use extraframe default value.
    static bool CheckRenderSkipIfScreenOff(bool extraFrame = false, std::optional<ScreenId> screenId = std::nullopt);
    static void MultiLayersPerf(size_t layerNum);
    static Drawing::Rect GetImageRegions(float screenWidth, float screenHeight,
        float realImageWidth, float realImageHeight);

private:
    static void SetSrcRect(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer);
    static void PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex);
    static bool FrameAwareTraceBoost(size_t layerNum);
    static void RequestPerf(uint32_t layerLevel, bool onOffTag);
    static void GetSampledDamageAndDrawnRegion(const ScreenInfo& screenInfo, const Occlusion::Region& srcDamageRegion,
        bool isDirtyAlignEnabled, Occlusion::Region& sampledDamageRegion, Occlusion::Region& sampledDrawnRegion);

    static inline int currentUIExtensionIndex_ = -1;
    static inline const std::string RELEASE_SURFACE_TASK = "releaseSurface";
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
