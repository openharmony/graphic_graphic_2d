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
    static void MergeDirtyRectAfterMergeHistory(
        std::shared_ptr<RSDirtyRegionManager> dirtyManager, Occlusion::Region& dirtyRegion);
    static std::vector<RectI> MergeDirtyHistory(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        int32_t bufferAge, ScreenInfo& screenInfo, RSDirtyRectsDfx& rsDirtyRectsDfx, RSDisplayRenderParams& params);
    static std::vector<RectI> MergeDirtyHistoryInVirtual(
        DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, int32_t bufferAge, ScreenInfo& screenInfo);
    // merge history dirty region of current display node and its child surfacenode(app windows)
    // for mirror display, call this function twice will introduce additional dirtyhistory in dirtymanager
    static void MergeDirtyHistoryForDrawable(DrawableV2::RSDisplayRenderNodeDrawable& drawable, int32_t bufferAge,
        RSDisplayRenderParams& params, bool useAlignedDirtyRegion = false);
    static void SetAllSurfaceDrawableGlobalDirtyRegion(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceDrawables,
        const Occlusion::Region& globalDirtyRegion);
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
    static Occlusion::Region MergeVisibleDirtyRegionInVirtual(
        std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr>& allSurfaceNodeDrawables);
    static std::vector<RectI> GetCurrentFrameVisibleDirty(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable,
        ScreenInfo& screenInfo, RSDisplayRenderParams& params);
    static std::vector<RectI> ScreenIntersectDirtyRects(const Occlusion::Region &region, ScreenInfo& screenInfo);
    static std::vector<RectI> GetFilpDirtyRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static std::vector<RectI> FilpRects(const std::vector<RectI>& srcRects, const ScreenInfo& screenInfo);
    static GraphicIRect IntersectRect(const GraphicIRect& first, const GraphicIRect& second);
    static void UpdateVirtualExpandDisplayAccumulatedParams(
        RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    static bool CheckVirtualExpandDisplaySkip(
        RSDisplayRenderParams& params, DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
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
    static float GetFloatRotationDegreeFromMatrix(Drawing::Matrix matrix);
    static bool HasNonZRotationTransform(Drawing::Matrix matrix);

    static void ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface = true);
    static void ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface>&& cacheSurface,
        std::shared_ptr<Drawing::Surface>&& cacheCompletedSurface,
        uint32_t cacheSurfaceThreadIndex, uint32_t completedSurfaceThreadIndex);
    static void CacheSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& oldSubThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
#ifdef RS_ENABLE_VK
    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static void SetVkImageInfo(std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo,
        const VkImageCreateInfo& imageInfo);
    static Drawing::BackendTexture MakeBackendTexture(uint32_t width, uint32_t height,
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
#endif
    static void UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect);
    static void DealWithNodeGravity(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static void DealWithScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix);
    static void CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node);
    static void LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static GraphicTransformType GetLayerTransform(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void OptimizedFlushAndSubmit(std::shared_ptr<Drawing::Surface>& surface,
        Drawing::GPUContext* const grContext, bool optFenceWait = true);
    static void AccumulateMatrixAndAlpha(std::shared_ptr<RSSurfaceRenderNode>& hwcNode,
        Drawing::Matrix& matrix, float& alpha);
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
    template<typename... Callbacks>
    static void TraverseParentNodeAndReduce(std::shared_ptr<RSSurfaceRenderNode> hwcNode, Callbacks&&... callbacks)
    {
        static_assert((std::is_invocable<Callbacks, std::shared_ptr<RSRenderNode>>::value && ...),
                    "uninvocable callback");
        if (!hwcNode) {
            return;
        }
        auto parent = std::static_pointer_cast<RSRenderNode>(hwcNode);
        while (static_cast<bool>(parent = parent->GetParent().lock())) {
            (std::invoke(callbacks, parent), ...);
            if (parent->GetType() == RSRenderNodeType::DISPLAY_NODE) {
                break;
            }
        }
    }
    static std::optional<Drawing::Matrix> GetMatrix(std::shared_ptr<RSRenderNode> hwcNode);
    // RTthread needs to draw one more frame when screen is turned off. For other threads, use extraframe default value.
    static bool CheckRenderSkipIfScreenOff(bool extraFrame = false, std::optional<ScreenId> screenId = std::nullopt);
    static void UpdateHwcNodeProperty(std::shared_ptr<RSSurfaceRenderNode> hwcNode);
    static void MultiLayersPerf(size_t layerNum);
    static GraphicTransformType GetConsumerTransform(const RSSurfaceRenderNode& node,
        const sptr<SurfaceBuffer> buffer, const sptr<IConsumerSurface> consumer);
    static Drawing::Rect CalcSrcRectByBufferRotation(const SurfaceBuffer& buffer,
        const GraphicTransformType consumerTransformType, Drawing::Rect newSrcRect);
    static bool IsHwcEnabledByGravity(RSSurfaceRenderNode& node, const Gravity frameGravity);
    static void DealWithNodeGravityOldVersion(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static Drawing::Rect GetImageRegions(float screenWidth, float screenHeight,
        float realImageWidth, float realImageHeight);
    static RectI SrcRectRotateTransform(const SurfaceBuffer& buffer,
        const GraphicTransformType bufferRotateTransformType, const RectI& newSrcRect);
    static void CalcSrcRectByBufferFlip(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static bool IsHwcEnabledByScalingMode(RSSurfaceRenderNode& node, const ScalingMode scalingMode);
    static void UpdateHwcNodeByScalingMode(RSSurfaceRenderNode& node, const Drawing::Matrix& totalMatrix,
        const Drawing::Matrix& gravityMatrix, const Drawing::Matrix& scalingModeMatrix);

private:
    static void SetSrcRect(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer);
    static void SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    static void HandleHardwareNode(const std::shared_ptr<RSSurfaceRenderNode>& node);
    static void PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex);
    static GraphicTransformType GetRotateTransformForRotationFixed(RSSurfaceRenderNode& node,
        sptr<IConsumerSurface> consumer);
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
