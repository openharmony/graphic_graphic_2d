/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
#define RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>

#include "system/rs_system_parameters.h"

#include "common/rs_exception_check.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_render_node.h"
#include "platform/ohos/rs_jank_stats_helper.h"
#include "rs_uni_render_thread.h"

namespace OHOS {
namespace Rosen {
class RSDrawFrame {
public:
    RSDrawFrame();
    ~RSDrawFrame() noexcept;

    RSDrawFrame(const RSDrawFrame&) = delete;
    RSDrawFrame(const RSDrawFrame&&) = delete;
    RSDrawFrame& operator=(const RSDrawFrame&) = delete;
    RSDrawFrame& operator=(const RSDrawFrame&&) = delete;

    void SetRenderThreadParams(std::unique_ptr<RSRenderThreadParams>& stagingRenderThreadParams);
    void PostAndWait();
    void PostDirectCompositionJankStats(const JankDurationParams& rsParams, bool optimizeLoad);

private:
    void RenderFrame();
    void SetEarlyZFlag(Drawing::GPUContext* gpuContext);
    void UnblockMainThread();
    void Sync();
    void Render();
    void ReleaseSelfDrawingNodeBuffer();
    void NotifyClearGpuCache();
    bool CheckCanvasSkipSync(std::shared_ptr<RSRenderNode>);
    void StartCheck();
    void EndCheck();

    RSUniRenderThread& unirenderInstance_;

    std::mutex frameMutex_;
    std::condition_variable frameCV_;
    bool canUnblockMainThread = false;
    std::unique_ptr<RSRenderThreadParams> stagingRenderThreadParams_ = nullptr;
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> stagingSyncCanvasDrawingNodes_;
    RsParallelType rsParallelType_;
    static bool debugTraceEnabled_;
    std::shared_ptr<RSTimer> timer_ = nullptr;
    int longFrameCount_ = 0;
    ExceptionCheck exceptionCheck_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
