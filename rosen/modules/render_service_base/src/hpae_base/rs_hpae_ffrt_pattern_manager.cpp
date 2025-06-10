#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_log.h"
#include "platform/common/rs_log.h"
#include <dlfcn.h>

namespace OHOS {
namespace Rosen {

using GPInstanceGetFunc = void*(*)(PatternType_C, const char*);
using GPInstanceInitFunc = bool(*)(void*, size_t);
using GPInstanceDstroyFunc = void(*)(void*);
using GPRequestEGraphFunc = bool(*)(void*, uint64_t);
using GPReleaseEGraphFunc = bool(*)(void*, uint64_t);
using GPReleaseAllEGraphFunc = bool(*)(void*);
using GPAAETaskSubmitFunc = void*(*)(void*, uint64_t, MHC_PatternTaskName, void*, void**, void*);
using GPWaitFunc = void*(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGPUTaskSubmitFunc = void*(*)(void*, uint64_t, MHC_PatternTaskName, void*, void*);
using GPGetGPUWaitEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGetGPUNotifyEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGPTaskSubmitFunc = void(*)(void*, uint64_t, MHC_PatternTaskName);

static GPInstanceGetFunc g_getGraphPatternInstance = nullptr;
static GPInstanceInitFunc g_graphPatternInit = nullptr;
static GPInstanceDstroyFunc g_graphPatternDestroy = nullptr;
static GPRequestEGraphFunc g_graphPatternRequestEGraph = nullptr;
static GPReleaseEGraphFunc g_graphPatternReleaseEGraph = nullptr;
static GPReleaseAllEGraphFunc g_graphPatternReleaseAll = nullptr;
static GPAAETaskSubmitFunc g_graphPatternAnimationTaskSubmit = nullptr;
static GPGPUTaskSubmitFunc g_graphPatternVulkanTaskSubmit = nullptr;
static GPWaitFunc g_graphPatternWait = nullptr;
static GPGetGPUWaitEventFunc g_graphPatternGetVulkanWaitEvent = nullptr;
static GPGetGPUNotifyEventFunc g_graphPatternGetVulkanNotifyEvent = nullptr;
static GPGPTaskSubmitFunc g_graphPatternTaskSubmit = nullptr;
static void* g_mhc_libframeworkHandle = nullptr;

RSHpaeFfrtPatternManager& RSHpaeFfrtPatternManager::Instance()
{
    static RSHpaeFfrtPatternManager instance;
    return instance;
}

RSHpaeFfrtPatternManager::RSHpaeFfrtPatternManager()
{
    if (!MHCDlOpen()) {
        HPAE_LOGE("MHCDlOpen() failed!");
    }
    if (!MHCGraphPatternInit(GRAPH_NUM)) {
        HPAE_LOGE("MHCGraphPatternInit() failed!");
    }
    HPAE_LOGW("mhc_so MHCGraphPatternInit success!");
}

RSHpaeFfrtPatternManager::~RSHpaeFfrPatternManager()
{
    if (g_instance) {
        g_graphPatternDestroy(g_instance);
        g_instance = nullptr;
    }
    if (g_mhc_libframeworkHandle) {
        dlclose(g_mhc_libframeworkHandle);
        g_getGraphPatternInstance = nullptr;
        g_graphPatternInit = nullptr;
        g_graphPatternDestroy = nullptr;
        g_graphPatternRequestEGraph = nullptr;
        g_graphPatternReleaseEGraph = nullptr;
        g_graphPatternReleaseAll = nullptr;
        g_graphPatternAnimationTaskSubmit = nullptr;
        g_graphPatternVulkanTaskSubmit = nullptr;
        g_graphPatternWait = nullptr;
        g_graphPatternGetVulkanWaitEvent = nullptr;
        g_graphPatternGetVulkanNotifyEvent = nullptr;
        g_graphPatternTaskSubmit = nullptr;
        g_mhc_libframeworkHandle = nullptr;
    }
}

bool RSHpaeFfrtPatternManager::MHCDlOpen()
{
    HPAE_LOGW("mhc_so MHCDlOpen start\n");
    if (g_mhc_libframeworkHandle == nullptr) {
        g_mhc_libframeworkHandle = dlopen("/vendor/lib64/libmhc_framework.so", RTLD_LAZY | RTLD_NODELETE);
        if (!g_mhc_libframeworkHandle) {
            HPAE_LOGW("mhc_so diopen libmhc_framework.so error\n");
            return false;
        }
    }

    g_getGraphPatternInstance = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_getGraphPatternInstance, "mhc_graph_pattern_get"));
    g_graphPatternInit = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternInit, "mhc_graph_pattern_init"));
    g_graphPatternDestroy = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternDestroy, "mhc_graph_pattern_destroy"));
    g_graphPatternRequestEGraph = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternRequestEGraph, "mhc_graph_request_eg"));
    g_graphPatternReleaseEGraph = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternReleaseEGraph, "mhc_graph_release_eg"));
    g_graphPatternReleaseAll = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternReleaseAll, "mhc_graph_relsease_all"));
    g_graphPatternAnimationTaskSubmit = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternAnimationTaskSubmit, "mhc_gp_animation_task_submit"));
    g_graphPatternVulkanTaskSubmit = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_getGraphPatternInstance, "mhc_gp_vulkan_task_submit"));
    g_graphPatternWait = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternWait, "mhc_gp_task_wait"));
    g_graphPatternGetVulkanWaitEvent = reinterpret_cast<GPInstanceGetFunc>(gdlsym(_graphPatternGetVulkanWaitEvent, "mhc_gp_vulkan_task_get_wait_event"));
    g_graphPatternGetVulkanNotifyEvent = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternGetVulkanNotifyEvent, "mhc_gp_vulkan_task_get_notify_event"));
    g_graphPatternTaskSubmit = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_graphPatternTaskSubmit, "mhc_gp_task_submit"));

    if (!g_getGraphPatternInstance || !g_graphPatternInit || !g_graphPatternDestroy\
        || !g_graphPatternRequestEGraph || !g_graphPatternReleaseEGraph || !g_graphPatternReleaseAll\
        || !g_graphPatternAnimationTaskSubmit || !g_graphPatternVulkanTaskSubmit || !g_graphPatternWait\
        || !g_graphPatternGetVulkanWaitEvent || !g_graphPatternGetVulkanNotifyEvent || !g_graphPatternTaskSubmit) {
        HPAE_LOGE("mhc_so dlsym error\n");
        dlclose(g_mhc_libframeworkHandle);
        g_mhc_libframeworkHandle = nullptr;
        return false;
    }

    HPAE_LOGW("mhc_so LoadLibMHC success\n");
    return true;
}

bool RSHpaeFfrtPatternManager::MHCCheck(const std::string logTag, uint64_t frameId)
{
    if (!g_instance) {
        HPAE_LOGE("mhc_so MHCCheck %{public}s g_instance == nullptr", logTag.c_str());
        return false;
    }
    HPAE_LOGW("mhc_so %{public}s MHCCheck frameId:%{public}" PRIu64 " ", logTag.c_str(), frameId);
    return true;
}

bool RSHpaeFfrtPatternManager::MHCGraphPatternInit(size_t size)
{
    HPAE_LOGW("mhc_so MHCGraphPatternInit");
    if (g_instance) {
        return true;
    }
    if (g_getGraphPatternInstance == nullptr) {
        HPAE_LOGW("mhc_so g_getGraphPatternInstance nullptr");
        return false;
    }
    g_instance =  g_getGraphPatternInstance(PATTERN_BLUR, "test_blur_graph");
    return g_graphPatternInit(g_instance, size);
}

bool RSHpaeFfrtPatternManager::MHCRequestEGraph(uint64_t frameId)
{
    if (!g_instance) {
        HPAE_LOGW("mhc_so MHCRequestEGraph g_instance nullptr");
        return false;
    }
    if (g_graphPatternRequestEGraph == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternRequestEGraph nullptr");
        return false;
    }
    return g_graphPatternRequestEGraph(g_instance, frameId);
}

bool RSHpaeFfrtPatternManager::MHCSubmitBlurTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, void** taskHandle, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitBlurTask", frameId)) {
        return false;
    }

    if (g_graphPatternAnimationTaskSubmit == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternAnimationTaskSubmit nullptr");
        return false;
    }

    FunctionHeader* preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    g_graphPatternAnimationTaskSubmit(g_instance, frameId, taskName, c_preFunc, taskHandle, c_afterFunc);
    return true;
}

bool RSHpaeFfrtPatternManager::MHCSubmitVulkanTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitVulkanTask", frameId)) {
        return false;
    }

    if (g_graphPatternVulkanTaskSubmit == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternVulkanTaskSubmit nullptr");
        return false;
    }

    FunctionHeader* preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    g_graphPatternVulkanTaskSubmit(g_instance, frameId, taskName, c_preFunc, c_afterFunc);
    return true;
}

bool RSHpaeFfrtPatternManager::MHCWait(uint64_t framId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCWait", frameId)) {
        return false;
    }

    if (g_graphPatternWait == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternVulkanTaskSubmit nullptr");
        return false;
    }

    g_graphPatternWait(g_instance, frameId, taskName);
    return true;
}

uint16_t RSHpaeFfrtPatternManager::MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskWaitEvent", frameId)) {
        return false;
    }

    if (g_graphPatternGetVulkanWaitEvent == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternGetVulkanWaitEvent nullptr");
        return false;
    }

    auto eventId = g_graphPatternGetVulkanWaitEvent(g_instance, frameId, taskName);
    HPAE_LOGW("mhc_so RSHpaeFfrtPatternManager::MHCGetVulkanTaskWaitEvent event = %{public}d, taskName=%{public}d\n", eventId, taskName);
    return eventId;
}

uint64_t RSHpaeFfrtPatternManager::MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskNotifyEvent", frameId)) {
        return false;
    }

    if (g_graphPatternGetVulkanNotifyEvent == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternGetVulkanNotifyEvent nullptr");
        return false;
    }

    auto eventId = g_graphPatternGetVulkanNotifyEvent(g_instance, frameId, taskName);
    HPAE_LOGW("mhc_so RSHpaeFfrtPatternManager::MHCGetVulkanTaskNotifyEvent event = %{public}d, taskName=%{public}d\n", eventId, taskName);
    return eventId;
}

bool RSHpaeFfrtPatternManager::MHCReleaseEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCReleaseEGraph", frameId)) {
        return false;
    }

    if (g_graphPatternReleaseEGraph == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternReleaseEGraph nullptr");
        return false;
    }

    return g_graphPatternReleaseEGraph(g_instance, frameId);
}

void RSHpaeFfrtPatternManager::MHCReleaseAll()
{
    HPAE_LOGW("mhc_so MHCReleaseAll");
    if (g_instance == nullptr) {
        HPAE_LOGE("mhc_so MHCReleaseAll g_instance == nullptr");
        return;
    }
    // return 1 is succ
    int ret = g_graphPatternReleaseAll(g_instance);
    HPAE_LOGW("mhc_so MHCReleaseAll, ret=%{public}d", ret);
}

bool RSHpaeFfrtPatternManager::MHCSubmitTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, void*** taskHandleVec, size_t numTask, std::function<void()&&afterFunc>)
{
    if (!MHCCheck("MHCSubmitTask", frameId)) {
        return false;
    }

    if (g_graphPatternTaskSubmit == nullptr) {
        HPAE_LOGW("mhc_so g_graphPatternTaskSubmit nullptr");
        return false;
    }

    FunctionHeader* preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    MHC_TaskInfo mhcTaskInfo = {
        .taskName = taskName;
        .c_prefunc = c_prefunc;
        .c_taskHandles = c_taskHandles;
        .numTasks = numTasks;
        .c_afterFunc = c_afterFunc;
    };
    g_graphPatternTaskSubmit(g_instance, frameId, &mhcTaskInfo);
    return true;
}

} // namespace Rosen
} // namespace OHOS