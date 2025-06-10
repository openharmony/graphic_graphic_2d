#ifndef RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H
#define RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H
#include <vector>
#include <stdexcept>
#include <memory>
#include <utility>
#include "rs_trace.h"
#include "ffrt_inner.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

//#include "rs_mhc.h"
typedef enum {
    PATTERN_HDR, // 对应 PatternType::HDR
    PATTERN_BULR, // 对应 PatternType::BLUR
} PatternType_C;

typedef enum {
    MHC_PATTERN_TASK_HDR_AAE = 0, // 需移除aae
    MHC_PATTERN_TASK_HDR_GPU,
    MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_GPU0 = MHC_PATTERN_TASK_HDR_MAX,
    MHC_PATTERN_TASK_BLUR_AAE,
    MHC_PATTERN_TASK_BLUR_GPU1,
    MHC_PATTERN_TASK_BLUR_MAX
} MHC_PatternTaskName;
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
FunctionHeader* create_function_wrapper(F&& func) {
    if (func == nullptr) {
        return nullptr;
    }
    // 1.复制/移动 lambda到堆内存
    auto* func_copy = new std::decay_t<F>(std::forward<F>(func));

    // 2.创建 C 接口需要的头结构
    auto* header = new FunctionHeader;

    // 3.设置执行函数：调用 lambda
    header->execute = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        (*f)(); // 调用 operator()
    };

    //4.设置销毁函数：释放内存
    header->destroy = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        delete f;
    };

    // 5.保存 lambda 指针
    header->data = func_copy;
    return header;
}

class RSB_EXPORT RSHpaeFfrtPatternManager final {
public:
    static RSHpaeFfrtPatternManager& Instance();
    RSHpaeFfrtPatternManager(const RSHpaeFfrtPatternManager&) = delete;
    RSHpaeFfrtPatternManager(RSHpaeFfrtPatternManager&&) = delete;
    RSHpaeFfrtPatternManager& operator=(const RSHpaeFfrtPatternManager) = delete;
    RSHpaeFfrtPatternManager& operator=(RSHpaeFfrtPatternManager&&) = delete;

    bool IsUpdated()
    {
        return updated_ && IsThreadIdMatch();
    }

    void SetUpdatedFlag()
    {
        updated_ = true;
    }

    void ResetUpdatedFlag()
    {
        updated_ = false;
    }

    void SetThreadId()
    {
        tid_ = gettid();
    }

    bool MHCDlOpen();

    bool MHCGraphPatternInit(size_t size);

    bool MHCRequestEGraph(uint64_t frameId);

    bool MHCSubmitBlurTask(uint64_t frameId, MHC_PatternTaskName taskName, std::function<void()>&& preFunc, void** taskHandle, std::function<void()>&& afterFunc);

    bool MHCSubmitVulkanTask(uint64_t frameId, MHC_PatternTaskName taskName, std::function<void()>&& preFunc, std::function<void()>&& afterFunc);

    bool MHCWait(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVlukanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName);

    uint16_t MHCGetVlukanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName);

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

    bool IsThreadIdMatch()
    {
        return tid_ == gettid();
    }

    void* g_instance = nullptr;
    bool updated_ = false;
    pid_t tid_ = -1;
    uint64_t vsyncId_ = 0;
    uint64_t curFrameId_ = 0;
    uint64_t lastFrameId_ = 0;
    static const int GRAPH_NUM = 5;
};

}
#endif // RENDER_SERVICE_BASE_RS_HPAE_FFRT_PATTERN_MANAGER_H