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

#ifndef RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H
#define RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include "hdi_output.h"
#include "irs_render_to_composer_connection.h"
#include "rs_composer_context.h"
#include "screen_manager/rs_screen_info.h"

namespace OHOS {
namespace Rosen {
class RSComposerClient {
public:
    ~RSComposerClient() = default;

    static std::shared_ptr<RSComposerClient> Create(
        const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
        const sptr<IRSComposerToRenderConnection>& composerToRenderConn);
    std::shared_ptr<RSLayer> GetRSLayer(RSLayerId rsLayerId);
    void CommitLayers(ComposerInfo& composerInfo);
    void ReleaseLayerBuffers(uint64_t screenId,
        std::vector<std::tuple<RSLayerId, bool, GraphicPresentTimestamp>>& timestampVec,
        std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>>& releaseBufferFenceVec);
    void RegisterOnReleaseLayerBuffersCB(OnReleaseLayerBuffersCB cb);
    std::shared_ptr<RSComposerContext> GetComposerContext();
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void ClearFrameBuffers();
    uint32_t GetUnExecuteTaskNum() const;
    void UpdatePipelineParam(const PipelineParam& pipelineParam);
    PipelineParam GetPipelineParam();
    int GetAccumulatedBufferCount() const;
    void DumpLayersInfo(std::string& dumpString);
    void DumpCurrentFrameLayers();
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds);
    void SetScreenBacklight(uint32_t level);
    static void ConvertScreenInfo(const ScreenInfo& screenInfo, ComposerScreenInfo& composerScreenInfo);
    void PreAllocProtectedFrameBuffers(const sptr<SurfaceBuffer>& buffer);
    std::shared_ptr<HdiOutput> GetOutput() const;
    void SetOutput(const std::shared_ptr<HdiOutput>& output);

private:
    explicit RSComposerClient(const sptr<IRSRenderToComposerConnection>& renderToComposerConn);

    bool WaitComposerThreadTaskExecute(std::unique_lock<std::mutex>& lock);
    void NotifyComposerThreadCanExecuteTask();
    void IncUnExecuteTaskNum();
    void SubUnExecuteTaskNum();
    std::mutex clientMutex_; /* Locking is only necessary if not running on uni render thread */
    std::shared_ptr<RSComposerContext> rsComposerContext_;
    std::condition_variable composerThreadTaskCond_;
    std::atomic<uint32_t> unExecuteTaskNum_ = 0;
    std::atomic<int> acquiredBufferCount_ = 0;
    bool isPreAllocProtectedFrameBuffer_ = false;
    PipelineParam pipelineParam_;
    std::shared_ptr<HdiOutput> output_ = nullptr;

    friend std::shared_ptr<RSComposerClient> Create(
        const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
        const sptr<IRSComposerToRenderConnection>& composerToRenderConn);
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_COMPOSER_CLIENT_PIPELINE_RS_RENDER_COMPOSER_CLIENT_H