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

#ifndef RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H
#define RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H

#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS::Rosen {

enum class PatternType_C {
    HDR,  // 对应 PatternType::HDR
    BLUR // 对应 PatternType::BLUR
};

enum class MHC_PatternTaskName {
    HDR_HPAE = 0,
    HDR_GPU,
    HDR_MAX,
    BLUR_GPU0 = HDR_MAX,
    BLUR_HPAE,
    BLUR_GPU1,
    BLUR_MAX
};
// end #include "rs_mhc.h"

struct MHC_TaskInfo {
    MHC_PatternTaskName taskName;
    void* c_prefunc = nullptr;
    void*** c_taskHandles = nullptr;
    size_t numTasks = 0;
    void* c_afterFunc = nullptr;
};

struct FunctionHeader {
    void (*execute)(void*);    // 执行函数指针
    void (*destroy)(void*);    // 销毁函数指针
    void* data;                // 用户数据（存储lambda）
};

template<typename F>
FunctionHeader* create_function_wrapper(F&& func)
{
    if (func == nullptr) {
        return nullptr;
    }
    // 1. 复制/移动 lambda 到堆内存
    auto* func_copy = new std::decay_t<F>(std::forward<F>(func));

    // 2. 创建 C 接口需要的头结构
    auto* header = new FunctionHeader;

    // 3. 设置执行函数：调用 lambda
    header->execute = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        (*f)(); // 调用 operator()
    };

    //4. 设置销毁函数：释放内存
    header->destroy = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        delete f;
    };

    // 5. 保存 lambda 指针
    header->data = func_copy;
    return header;
}

class RSB_EXPORT RSHpaeFfrtPatternManager final {
public:
    static RSHpaeFfrtPatternManager& Instance();
    RSHpaeFfrtPatternManager(const RSHpaeFfrtPatternManager&) = delete;
    RSHpaeFfrtPatternManager(RSHpaeFfrtPatternManager&&) = delete;
    RSHpaeFfrtPatternManager& operator=(const RSHpaeFfrtPatternManager&) = delete;
    RSHpaeFfrtPatternManager& operator=(RSHpaeFfrtPatternManager&&) = delete;

    bool IsUpdated();

    void SetUpdatedFlag()
    {
        updated_ = true;
    }

    void ResetUpdatedFlag()
    {
        updated_ = false;
    }

    void SetThreadId();

    bool MHCDlOpen();

    bool MHCGraphPatternInit(size_t size);

    bool MHCRequestEGraph(uint64_t frameId);

    bool MHCWait(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName);

    bool MHCReleaseEGraph(uint64_t frameId);

    void MHCReleaseAll();

    bool MHCSubmitTask(uint64_t frameId, MHC_PatternTaskName taskName, std::function<void()>&& preFunc,
        void*** taskHandleVec, size_t numTask, std::function<void()>&& afterFunc);

    bool MHCCheck(const std::string logTag, uint64_t frameId);

    void MHCSetVsyncId(uint64_t vsyncId)
    {
        vsyncId_ = vsyncId;
    }

    uint64_t MHCGetVsyncId()
    {
        return vsyncId_;
    }

    void MHCSetCurFrameId(uint64_t frameId)
    {
        lastFrameId_ = curFrameId_;
        curFrameId_ = frameId;
    }

    uint64_t MHCGetCurFrameId()
    {
        return curFrameId_;
    }

    void MHCSetLastFrameId(uint64_t frameId)
    {
        lastFrameId_ = frameId;
    }

    uint64_t MHCGetLastFrameId()
    {
        return lastFrameId_;
    }

    bool UseGraphicPattern() // 开关，控制是否使能ffrt GP
    {
        return true;
    }

protected:
    RSHpaeFfrtPatternManager();
    ~RSHpaeFfrtPatternManager();

    bool IsThreadIdMatch();

    void* g_instance = nullptr;
    bool updated_ = false;
#if defined(ROSEN_OHOS)
    pid_t tid_ = -1;
#else
    int32_t tid_ = -1;
#endif
    uint64_t vsyncId_ = 0;
    uint64_t curFrameId_ = 0;
    uint64_t lastFrameId_ = 0;
    static const int GRAPH_NUM = 5;
};

}
#endif // RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H