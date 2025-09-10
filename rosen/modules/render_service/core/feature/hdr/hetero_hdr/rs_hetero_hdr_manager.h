/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_HETERO_HDR_MANAGER_H
#define RS_HETERO_HDR_MANAGER_H

#include <condition_variable>
#include <set>
#include <unordered_map>
#include <vector>

#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_processor.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
class RSHeteroHDRManager {
public:
    static RSHeteroHDRManager& Instance();
    void UpdateHDRNodes(RSSurfaceRenderNode& node, bool isCurrentFrameBufferConsumed);
    void PostHDRSubTasks();
    bool UpdateHDRHeteroParams(RSPaintFilterCanvas& canvas,
        const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& drawableParams);
    void GenerateHDRHeteroShader(BufferDrawParam& param, std::shared_ptr<Drawing::ShaderEffect>& imageShader);
    
    bool ReleaseBuffer();

    std::shared_ptr<RSSurfaceHandler> GetHDRSurfaceHandler()
    {
        return rsHeteroHDRBufferLayer_.GetMutableRSSurfaceHandler();
    }
    HdrStatus GetCurHandleStatus() const
    {
        return curHandleStatus_;
    }
    virtual ~RSHeteroHDRManager() = default;
private:
    RSHeteroHDRManager();
    RSHeteroHDRManager(const RSHeteroHDRManager&) = delete;
    RSHeteroHDRManager(const RSHeteroHDRManager&&) = delete;
    RSHeteroHDRManager& operator=(const RSHeteroHDRManager&) = delete;
    RSHeteroHDRManager& operator=(const RSHeteroHDRManager&&) = delete;
    RectI RectRound(RectI src);

    bool GetCurFrameHeteroHandleCanBeUsed(NodeId nodeId) const
    {
        return curFrameHeteroHandleCanBeUsed_ && curNodeId_ == nodeId;
    }

    void ProcessParamsUpdate(RSPaintFilterCanvas& canvas,
        const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable, BufferDrawParam& drawableParams);

    void GetFixedDstRectStatus(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& drawable,
        bool isUiFirstMode, RSSurfaceRenderParams* surfaceParams);
    int32_t BuildHDRTask(RSSurfaceRenderParams* surfaceParams, MDCRectT srcRect, uint32_t* taskId,
        void** taskPtr, HdrStatus curHandleStatus);
    bool IsHDRSurfaceNodeSkipped(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable);
    bool PrepareHpaeTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable,
        RSSurfaceRenderParams* surfaceParams, uint64_t curFrameId);
    void FindParentLeashWindowNode();
    ScreenId GetScreenIDByDrawable(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& drawable);

    bool ProcessPendingNode(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, uint64_t curFrameId);
    bool CheckWindowOwnership(NodeId nodeId);
    bool PrepareAndSubmitHDRTask(
        std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& nodeDrawable,
        RSSurfaceRenderParams* surfaceParams, NodeId nodeId, uint64_t curFrameId);
    void ClearBufferCache();

    std::shared_ptr<Drawing::ShaderEffect> MakeHDRHeadroomShader(float hdrRatio,
        std::shared_ptr<Drawing::ShaderEffect>& imageShader);
 
    std::shared_ptr<Drawing::ShaderEffect> MakeAIHDRGainmapHeadroomShader(float hdrRatio,
        std::shared_ptr<Drawing::ShaderEffect>& imageShader);

    virtual bool MHCRequestEGraph(uint64_t frameId)
    {
        return RSHDRPatternManager::Instance().MHCRequestEGraph(frameId);
    }
    virtual bool MHCSetCurFrameId(uint64_t frameId)
    {
        return RSHDRPatternManager::Instance().MHCSetCurFrameId(frameId);
    }
    virtual bool TryConsumeBuffer(std::function<void()>&& consumingFunc)
    {
        return RSHDRPatternManager::Instance().TryConsumeBuffer(std::move(consumingFunc));
    }

    RSHeteroHDRBufferLayer rsHeteroHDRBufferLayer_{"HDRHeterogeneousLayer", INVALID_NODEID};
    std::unordered_map<NodeId, NodeId> ownedLeashWindowIdMap_;
    std::unordered_map<NodeId, NodeId> ownedAppWindowIdMap_;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> pendingPostNodes_;
    std::unordered_map<NodeId, RectI> nodeSrcRectMap_;
    std::unordered_map<NodeId, bool> isCurrentFrameBufferConsumedMap_;
    void* taskPtr_ = nullptr;
    std::atomic<uint32_t> taskId_ = 0;
    int32_t buildHdrTaskStatus_ = 0;
    std::mutex shaderMutex_;
    std::atomic<bool> destroyedFlag_{ true };
    bool isFixedDstBuffer_ = false;
    RectI dst_ = { 0, 0, 0, 0 };
    sptr<SurfaceBuffer> dstBuffer_ = nullptr;
    bool curFrameHeteroHandleCanBeUsed_ = false;
    bool isHeteroComputingHdrOn_ = false;
    int framesNoApplyCnt_ = 0;
    NodeId curNodeId_ = 0;
    bool preFrameHeteroHandleCanBeUsed_ = false;
    MDCRectT hpaeDstRect_;
    HdrStatus curHandleStatus_ = HdrStatus::NO_HDR;
    bool isHdrOn_ = false; // whether hdr is turn on in current screen
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_HETERO_HDR_MANAGER_H