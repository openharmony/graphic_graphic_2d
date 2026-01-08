/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_PIPELINE_RS_LPP_VIDEO_H
#define ROSEN_RENDER_SERVICE_PIPELINE_RS_LPP_VIDEO_H
#include <string>
#include <vector>
#include <atomic>

#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
namespace OHOS::Rosen {
constexpr size_t LPP_SURFACE_NODE_MAX_SIZE = 10;
enum LppState { UNKOWN = 0, LPP_LAYER = 1, UNI_RENDER = 2 };
class LppVideoHandler {
private:
    std::mutex mutex_;
    mutable std::atomic<bool> hasVirtualMirrorDisplay_ = false;
    LppState lppShbState_ = LppState::UNKOWN;
    LppState lppRsState_ = LppState::UNKOWN;
    bool isRequestedVsync_ = false;
    std::atomic<bool> hasLppVideo_ = { false };
    // <vsyncId, <RSSurfaceRenderNode>>
    std::map<uint64_t, std::vector<wptr<IConsumerSurface>>> lppConsumerMap_;

public:
    static LppVideoHandler& Instance();
    // << call from main thread
    void SetHasVirtualMirrorDisplay(bool hasVirtualMirrorDisplay) const;
    void JudgeRequestVsyncForLpp(uint64_t vsyncId);
    void ConsumeAndUpdateLppBuffer(uint64_t vsyncId, const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    // >>
    // << call from hareware thread
    void JudgeLppLayer(uint64_t vsyncId, std::set<uint64_t> lppLayerIds);
    // >>
    bool HasLppVideo();
};
} // namespace OHOS::Rosen

#endif