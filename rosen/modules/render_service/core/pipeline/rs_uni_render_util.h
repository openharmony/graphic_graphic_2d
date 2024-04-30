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
#include "sync_fence.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_map.h"
#include "common/rs_obj_abs_geometry.h"

#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderUtil {
public:
    // merge history dirty region of current display node and its child surfacenode(app windows)
    // for mirror display, call this function twice will introduce additional dirtyhistory in dirtymanager
    static void MergeDirtyHistory(std::shared_ptr<RSDisplayRenderNode>& node, int32_t bufferAge,
        bool useAlignedDirtyRegion = false, bool renderParallel = false);

    static void SetAllSurfaceGlobalDityRegion(
        std::vector<RSBaseRenderNode::SharedPtr>& allSurfaces, const RectI& globalDirtyRegion);

    /* we want to set visible dirty region of each surfacenode into DamageRegionKHR interface, hence
     * occlusion is calculated.
     * make sure this function is called after merge dirty history
     */
    static Occlusion::Region MergeVisibleDirtyRegion(std::vector<RSRenderNode::SharedPtr>& allSurfaceNodes,
        std::vector<NodeId>& hasVisibleDirtyRegionSurfaceVec, bool useAlignedDirtyRegion = false,
        bool renderParallel = false);
    static std::vector<RectI> ScreenIntersectDirtyRects(const Occlusion::Region &region, ScreenInfo& screenInfo);
    static bool HandleSubThreadNode(RSSurfaceRenderNode& node, RSPaintFilterCanvas& canvas);
    static bool HandleCaptureNode(RSRenderNode& node, RSPaintFilterCanvas& canvas);
    // This is used for calculate matrix from buffer coordinate to window's relative coordinate
    static Drawing::Matrix GetMatrixOfBufferToRelRect(const RSSurfaceRenderNode& node);
    static void SrcRectScaleDown(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
        const sptr<IConsumerSurface>& surface, RectF& localBounds);
    static void SrcRectScaleFit(BufferDrawParam& params, const sptr<SurfaceBuffer>& buffer,
        const sptr<IConsumerSurface>& surface, RectF& localBounds);
    static BufferDrawParam CreateBufferDrawParam(const RSSurfaceRenderNode& node,
        bool forceCPU, pid_t threadIndex = UNI_RENDER_THREAD_INDEX, bool isRenderThread = false);
    static BufferDrawParam CreateBufferDrawParam(const RSDisplayRenderNode& node, bool forceCPU);
    static BufferDrawParam CreateLayerBufferDrawParam(const LayerInfoPtr& layer, bool forceCPU);
    static bool IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info);
    static void DrawRectForDfx(RSPaintFilterCanvas& canvas, const RectI& rect, Drawing::Color color,
        float alpha, const std::string& extraInfo = "");
    static Occlusion::Region AlignedDirtyRegion(const Occlusion::Region& dirtyRegion, int32_t alignedBits = 32);
    static int GetRotationFromMatrix(Drawing::Matrix matrix);
    static int GetRotationDegreeFromMatrix(Drawing::Matrix matrix);
    static bool Is3DRotation(Drawing::Matrix matrix);

    static void ReleaseColorPickerFilter(std::shared_ptr<RSFilter> RSFilter);
    static void ReleaseColorPickerResource(std::shared_ptr<RSRenderNode>& node);
    static void AssignWindowNodes(const std::shared_ptr<RSDisplayRenderNode>& displayNode,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    static void ClearSurfaceIfNeed(const RSRenderNodeMap& map, const std::shared_ptr<RSDisplayRenderNode>& displayNode,
        std::set<std::shared_ptr<RSBaseRenderNode>>& oldChildren, DeviceType deviceType = DeviceType::PHONE);
    static void ClearCacheSurface(RSRenderNode& node, uint32_t threadIndex, bool isClearCompletedCacheSurface = true);
    static void ClearNodeCacheSurface(std::shared_ptr<Drawing::Surface>&& cacheSurface,
        std::shared_ptr<Drawing::Surface>&& cacheCompletedSurface,
        uint32_t cacheSurfaceThreadIndex, uint32_t completedSurfaceThreadIndex);
    static void CacheSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& oldSubThreadNodes,
        std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    // use floor value of translateX and translateY in matrix of canvas to avoid jittering
    static void CeilTransXYInCanvasMatrix(RSPaintFilterCanvas& canvas);
    static bool IsNodeAssignSubThread(std::shared_ptr<RSSurfaceRenderNode> node, bool isDisplayRotation);
#ifdef RS_ENABLE_VK
    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static void SetVkImageInfo(std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo,
        const VkImageCreateInfo& imageInfo);
    static Drawing::BackendTexture MakeBackendTexture(uint32_t width, uint32_t height,
        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
#endif
    static void UpdateRealSrcRect(RSSurfaceRenderNode& node, const RectI& absRect);
    static void DealWithNodeGravity(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void CheckForceHardwareAndUpdateDstRect(RSSurfaceRenderNode& node);
    static void LayerRotate(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void LayerCrop(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
    static void LayerScaleDown(RSSurfaceRenderNode& Node);
    static GraphicTransformType GetLayerTransform(RSSurfaceRenderNode& node, const ScreenInfo& screenInfo);
private:
    static RectI SrcRectRotateTransform(RSSurfaceRenderNode& node);
    static void AssignMainThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& mainThreadNodes,
        const std::shared_ptr<RSSurfaceRenderNode>& node);
    static void AssignSubThreadNode(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes,
        const std::shared_ptr<RSSurfaceRenderNode>& node);
    static void SortSubThreadNodes(std::list<std::shared_ptr<RSSurfaceRenderNode>>& subThreadNodes);
    static void HandleHardwareNode(const std::shared_ptr<RSSurfaceRenderNode>& node);
    static void PostReleaseSurfaceTask(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex);
};
}
}
#endif // RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_UTIL_H
