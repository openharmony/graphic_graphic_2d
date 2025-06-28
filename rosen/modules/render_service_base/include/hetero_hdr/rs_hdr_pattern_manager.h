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
#include <vector>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <utility>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

typedef enum {
    PATTERN_HDR,  // PatternType::HDR
    PATTERN_BLUR  // PatternType::BLUR
} PatternType_C;

typedef enum {
    MHC_PATTERN_TASK_HDR_HPAE = 0,
    MHC_PATTERN_TASK_HDR_GPU,
    MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_GPU0 = MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_HPAE,
    MHC_PATTERN_TASK_BLUR_GPU1,
    MHC_PATTERN_TASK_BLUR_MAX
} MHC_PatternTaskName;

namespace OHOS::Rosen {
struct FunctionHeader {
    void (*execute)(void *);  // Execute function pointer
    void (*destroy)(void *);  // Destroy function pointer
    void *data;               // User data (storing lambda)
};

class RSB_EXPORT RSHDRPatternManager final {
public:
    static RSHDRPatternManager &Instance();
    RSHDRPatternManager(const RSHDRPatternManager &) = delete;
    RSHDRPatternManager(RSHDRPatternManager &&) = delete;
    RSHDRPatternManager &operator=(const RSHDRPatternManager &) = delete;
    RSHDRPatternManager &operator=(RSHDRPatternManager &&) = delete;

    void SetThreadId()
    {
#ifdef ROSEN_OHOS
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        if (processConsumed_) {
            return;
        }
        if (flushedBuffer) {  // one HDR buffer can be consumed only once
            flushedBuffer = false;
            processConsumed_ = true;
            tid_ = gettid();
        }
#endif
    }

    void SetBufferFlushed()
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        flushedBuffer = true;
    }

    bool TryConsumeBuffer(std::function<void()> &&consumingFunc)
    {
        std::unique_lock<std::mutex> lock(frameIdMutex_);
        if (flushedBuffer && lastFrameConsumed_) {
            return false;
        }
        if (flushedBuffer) {
            consumingFunc();
            flushedBuffer = false;
            lastFrameConsumed_ = true;
        }
        return true;
    }

    bool MHCDlOpen();

    bool MHCGraphPatternInit(size_t size);

    bool MHCRequestEGraph(uint64_t frameId);

    bool MHCSubmitHDRTask(uint64_t frameId, MHC_PatternTaskName taskName, std::function<void()> &&preFunc,
        void **taskHandle, std::function<void()> &&afterFunc);

    bool MHCSubmitVulkanTask(uint64_t frameId, MHC_PatternTaskName taskName, std::function<void()> &&preFunc,
        std::function<void()> &&afterFunc);

    bool MHCWait(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName);

    bool MHCReleaseEGraph(uint64_t frameId);

    bool MHCReleaseAll();

    bool MHCCheck(const std::string logTag, uint64_t frameId);

    std::vector<uint64_t> MHCGetFrameIdForGpuTask();

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

    void MHCReSetCurFrameId()
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
protected:
    RSHDRPatternManager();
    ~RSHDRPatternManager();

    bool IsThreadIdMatch()
    {
#ifdef ROSEN_OHOS
        return processConsumed_ && tid_ == gettid();
#else
        return false;
#endif
    }

    bool MHCCheckIsFirstFrame()
    {
        return (curFrameIdUsed_ == false) && (lastFrameIdUsed_ == true);
    }

    bool MHCCheckIsNextFrame()
    {
        return lastFrameIdUsed_ == false;
    }

    void *g_instance = nullptr;
    bool processConsumed_ = false;
    bool lastFrameConsumed_ = false;
    bool flushedBuffer = false;
    pid_t tid_ = -1;
    std::mutex frameIdMutex_;
    uint64_t curFrameId_ = 0;
    uint64_t lastFrameId_ = 0;
    bool curFrameIdUsed_ = true;
    bool lastFrameIdUsed_ = true;
    uint64_t vsyncId_ = 0;
    bool isFinishDLOpen_ = false;
};

}  // namespace OHOS::Rosen
#endif  // RS_HDR_PATTERN_MANAGER_H