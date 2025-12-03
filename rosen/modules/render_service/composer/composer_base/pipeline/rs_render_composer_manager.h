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

#ifndef RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H
#define RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H

#include <mutex>
#include "rs_render_composer.h"
#include "rs_render_composer_client.h"
#include "rs_render_to_composer_connection.h"

namespace OHOS::Rosen {
class RSRenderComposerManager {
public:
    static RSRenderComposerManager& GetInstance();
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output);
    void OnScreenDisconnected(ScreenId screenId);
    sptr<RSRenderToComposerConnection> GetRSComposerConnection(ScreenId screenId);
    std::shared_ptr<RSRenderComposerClient> CreateRSRenderComposerClient(ScreenId screenId);

    void PreAllocateProtectedBuffer(ScreenId screenId, sptr<SurfaceBuffer> buffer);
    uint32_t GetUnExecuteTaskNum(ScreenId screenId);
    int32_t GetAccumulatedBufferCount(ScreenId screenId);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    Return PostSyncTask(ScreenId screenId, Task&& task)
    {
        std::shared_ptr<RSRenderComposer> composer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto iter = rsRenderComposerMap_.find(screenId);
            if (iter == rsRenderComposerMap_.end()) {
                RS_LOGE("ScheduleTask not find screenId:%{public}" PRIu64, screenId);
                return task();
            }
            composer = iter->second;
        }
        auto [scheduledTask, taskFuture] = Composer::ScheduledTask<Task>::Create(std::move(task));
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(composer);
#ifdef RS_ENABLE_GPU
        renderComposerAgent->PostTask([t(std::move(scheduledTask))]() { t->Run(); });
#endif
        return taskFuture.get();
    }
    void PostTask(ScreenId screenId, const std::function<void()>& task);
    void PostTaskToAllScreens(const std::function<void()>& task);
    void PostTaskWithInnerDelay(ScreenId screenId, const std::function<void()>& task);
    void ForEachScreen(const std::function<void(ScreenId, std::shared_ptr<RSRenderComposer>)>& func);
    GSError ClearFrameBuffers(ScreenId screenId, bool isNeedResetContext = true);
    void OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp);
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    sptr<SyncFence> GetReleaseFence(ScreenId screenId);
    bool WaitComposerTaskExecute(ScreenId screenId);
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void FpsDump(std::string& dumpString, std::string& layerName);
    void ClearFpsDump(std::string& dumpString, std::string& layerName);
    void DumpCurrentFrameLayers();
    void HitchsDump(std::string& dumpString, std::string& layerArg);
    void DumpVkImageInfo(std::string& dumpString);
    void ClearRedrawGPUCompositionCache(const std::set<uint64_t>& bufferIds);
    void SetScreenPowerOnChanged(ScreenId screenId, bool flag);

private:
    RSRenderComposerManager() = default;
    ~RSRenderComposerManager() = default;
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposer>> rsRenderComposerMap_;
    std::unordered_map<ScreenId, sptr<RSRenderToComposerConnection>> rsComposerConnectionMap_;
    std::unordered_map<ScreenId, std::shared_ptr<RSRenderComposerClient>> rsComposerClientMap_;
    std::mutex mutex_;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_BASE_PIPELINE_RS_RENDER_COMPOSER_MANAGER_H