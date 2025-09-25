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

#ifndef RS_HDR_PATTERN_MANAGER_H
#define RS_HDR_PATTERN_MANAGER_H

#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

typedef enum {
    PATTERN_HDR,  // PatternType::HDR
    PATTERN_BLUR  // PatternType::BLUR
} PatternType;

typedef enum {
    MHC_PATTERN_TASK_HDR_HPAE = 0,
    MHC_PATTERN_TASK_HDR_GPU,
    MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_GPU0 = MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_HPAE,
    MHC_PATTERN_TASK_BLUR_GPU1,
    MHC_PATTERN_TASK_BLUR_MAX
} MHCPatternTaskName;

namespace OHOS {
namespace Rosen {
using GPInstanceGetFunc = void* (*)(PatternType, const char*);
using GPInstanceInitFunc = bool (*)(void*, size_t);
using GPInstanceDestroyFunc = void (*)(void*);
using GPRequestEGraphFunc = bool (*)(void*, uint64_t);
using GPReleaseEGraphFunc = bool (*)(void*, uint64_t);
using GPReleaseAllFunc = bool (*)(void*);
using GPHpaeTaskSubmitFunc = void (*)(void*, uint64_t, MHCPatternTaskName, void*, void**, void*);
using GPHpaeRaskExecutionQuery = int32_t (*)(void*, uint64_t, MHCPatternTaskName);
using GPWaitFunc = void (*)(void*, uint64_t, MHCPatternTaskName);
using GPGPUTaskSubmitFunc = void (*)(void*, uint64_t, MHCPatternTaskName, void*, void*);
using GPGetGPUWaitEventFunc = uint16_t (*)(void*, uint64_t, MHCPatternTaskName);
using GPGetGPUNotifyEventFunc = uint16_t (*)(void*, uint64_t, MHCPatternTaskName);
 
struct MHCDevice {
    GPInstanceGetFunc getGraphPatternInstance = nullptr;
    GPInstanceInitFunc graphPatternInit = nullptr;
    GPInstanceDestroyFunc graphPatternDestroy = nullptr;
    GPRequestEGraphFunc graphPatternRequestEGraph = nullptr;
    GPReleaseEGraphFunc graphPatternReleaseEGraph = nullptr;
    GPReleaseAllFunc graphPatternReleaseAll = nullptr;
    GPHpaeTaskSubmitFunc graphPatternAnimationTaskSubmit = nullptr;
    GPHpaeRaskExecutionQuery graphPatternHpaeTaskExecutionQuery = nullptr;
    GPGPUTaskSubmitFunc graphPatternVulkanTaskSubmit = nullptr;
    GPWaitFunc graphPatternWait = nullptr;
    GPGetGPUWaitEventFunc graphPatternGetVulkanWaitEvent = nullptr;
    GPGetGPUNotifyEventFunc graphPatternGetVulkanNotifyEvent = nullptr;
};

class RSB_EXPORT RSHDRPatternManager {
public:
    static RSHDRPatternManager& Instance();
    RSHDRPatternManager(const RSHDRPatternManager&) = delete;
    RSHDRPatternManager(RSHDRPatternManager&&) = delete;
    RSHDRPatternManager& operator=(const RSHDRPatternManager&) = delete;
    RSHDRPatternManager& operator=(RSHDRPatternManager&&) = delete;

    void SetThreadId(RSPaintFilterCanvas& canvas);

    void SetBufferFlushed()
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        flushedBuffer_ = true;
    }

    bool TryConsumeBuffer(std::function<void()>&& consumingFunc)
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        if (flushedBuffer_ && lastFrameConsumed_) {
            return false;
        }
        if (flushedBuffer_) {
            consumingFunc();
            flushedBuffer_ = false;
            lastFrameConsumed_ = true;
        }
        return true;
    }

    void MHCDlClose();

    bool MHCDlOpen();

    bool MHCGraphPatternInit(size_t size);

    bool MHCRequestEGraph(uint64_t frameId);

    bool MHCSubmitHDRTask(uint64_t frameId, MHCPatternTaskName taskName, std::function<void()>&& preFunc,
        void** taskHandle, std::function<void()>&& afterFunc);

    void MHCGraphQueryTaskError(uint64_t frameId, MHCPatternTaskName taskName);

    bool MHCSubmitVulkanTask(uint64_t frameId, MHCPatternTaskName taskName, std::function<void()>&& preFunc,
        std::function<void()>&& afterFunc);

    bool MHCWait(uint64_t frameId, MHCPatternTaskName taskName);

    uint16_t MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHCPatternTaskName taskName);

    uint16_t MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHCPatternTaskName taskName);

    bool MHCReleaseEGraph(uint64_t frameId);

    bool MHCReleaseAll();

    bool MHCCheck(const std::string logTag);

    bool MHCSetCurFrameId(uint64_t frameId)
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        if (!lastFrameIdUsed_) {
            return false;
        }
        lastFrameId_ = curFrameId_;
        curFrameId_ = frameId;
        lastFrameIdUsed_ = curFrameIdUsed_;
        curFrameIdUsed_ = false;
        return true;
    }

    void MHCResetCurFrameId()
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        curFrameId_ = lastFrameId_;
        curFrameIdUsed_ = lastFrameIdUsed_;
        lastFrameIdUsed_ = true;
        lastFrameId_ = 0;
    }

    void MHCSetVsyncId(uint64_t frameId)
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        vsyncId_ = frameId;
    }

    bool MHCCheckWaitSemaphoreSet(uint64_t frameId)
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        auto it = waitSemaphoreSet_.find(frameId);
        if (it != waitSemaphoreSet_.end()) {
            waitSemaphoreSet_.erase(it);
            return true;
        }
        return false;
    }

    std::vector<uint64_t> MHCGetFrameIdForGPUTask();

    void MHCRegisterSubmitGPUFunc(void* key, std::function<void()> func)
    {
        std::unique_lock<std::mutex> lock(funcMutex_);
        submitFuncs_[key] = func;
    }

    void MHCSubmitGPUTask(std::vector<void *> keys)
    {
        std::unique_lock<std::mutex> lock(funcMutex_);
        for (auto key : keys) {
            auto it = submitFuncs_.find(key);
            if (it != submitFuncs_.end()) {
                auto func = it->second;
                func();
                submitFuncs_.erase(it);
            }
        }
    }

    void MHCClearGPUTaskFunc(std::vector<uint64_t> frameIdVec)
    {
        if (frameIdVec.size() == 0) {
            RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCClearGPUTaskFunc no need clear");
            return;
        }
        std::unique_lock<std::mutex> lock(funcMutex_);
        if (!submitFuncs_.empty()) {
            for (auto it = submitFuncs_.begin(); it != submitFuncs_.end(); it++) {
                auto func = it->second;
                func();
            }
            RS_LOGW("[hdrHetero]:RSHDRPatternManager MHCClearGPUTaskFunc submitFuncs_ is not empty");
            submitFuncs_.clear();
        }
    }

private:
    RSHDRPatternManager();
    ~RSHDRPatternManager();

    bool MHCDlsymInvalid();

    bool IsThreadIdMatch()
    {
#ifdef ROSEN_OHOS
        return processConsumed_ && tid_ == gettid();
#else
        return false;
#endif
    }

#ifdef ROSEN_OHOS
    void* MHCLibFrameworkHandle_ = nullptr;
    bool processConsumed_ = false;
    pid_t tid_ = -1;
#endif
    std::shared_ptr<MHCDevice> MHCDevice_ = std::make_shared<MHCDevice>();
    void* graphPatternInstance_ = nullptr;
    bool lastFrameConsumed_ = false;
    bool flushedBuffer_ = false;
    std::mutex frameIdMutex_;
    uint64_t curFrameId_ = 0;
    uint64_t lastFrameId_ = 0;
    bool curFrameIdUsed_ = true;
    bool lastFrameIdUsed_ = true;
    uint64_t vsyncId_ = 0;
    bool isFinishDLOpen_ = false;
    std::set<uint64_t> waitSemaphoreSet_{};
    std::mutex funcMutex_;
    std::unordered_map<void*, std::function<void()>> submitFuncs_{};
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_HDR_PATTERN_MANAGER_H