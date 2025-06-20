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

#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"

#if defined(ROSEN_OHOS)
#include <dlfcn.h>
#include <unistd.h>
#endif

#include "hpae_base/rs_hpae_log.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {
using GPInstanceGetFunc = void*(*)(PatternType_C, const char*);
using GPInstanceInitFunc = bool(*)(void*, size_t);
using GPInstanceDestroyFunc = void(*)(void*);
using GPRequstEGraphFunc = bool(*)(void*, uint64_t);
using GPReleaseEGraphFunc = bool(*)(void*, uint64_t);
using GPReleaseAllEGraphFunc = bool(*)(void*);
using GPWaitFunc = void*(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGetGPUWaitEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGetGPUNotifyEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGPTaskSubmitFunc = void(*)(void*, uint64_t, MHC_TaskInfo*);

static GPInstanceGetFunc g_getGPInstance = nullptr;
static GPInstanceInitFunc g_GPInit = nullptr;
static GPInstanceDestroyFunc g_GPDestroy = nullptr;
static GPRequestEGraphFunc g_GPRequestEGraph = nullptr;
static GPReleaseEGraphFunc g_GPReleaseEGraph = nullptr;
static GPReleaseAllEGraphFunc g_GPReleaseAll = nullptr;
static GPWaitFunc g_GPnWait = nullptr;
static GPGetGPUWaitEventFunc g_GPGetVulkanWaitEvent = nullptr;
static GPGetGPUNotifyEventFunc g_GPGetVulkanNotifyEvent = nullptr;
static GPGPTaskSubmitFunc g_GPTaskSubmit = nullptr;
static void* g_mhc_libframeworkHandle = nullptr;
}

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

RSHpaeFfrtPatternManager::~RSHpaeFfrtPatternManager()
{
    if (g_instance) {
        g_GPDestroy(g_instance);
        g_instance = nullptr;
    }

    if (g_mhcHandle) {
#if defined(ROSEN_OHOS)
        dlclose(g_mhcHandle);
#endif
        g_getGPInstance = nullptr;
        g_GPInit = nullptr;
        g_GPDestroy = nullptr;
        g_GPRequestEGraph = nullptr;
        g_GPReleaseEGraph = nullptr;
        g_GPReleaseAll = nullptr;
        g_GPWait = nullptr;
        g_GPGetVulkanWaitEvent = nullptr;
        g_GPGetVulkanNotifyEvent = nullptr;
        g_GPTaskSubmit = nullptr;
        g_mhcHandle = nullptr;
    }
}

bool RSHpaeFfrtPatternManager::MHCDlOpen()
{
#if defined(ROSEN_OHOS)
    HPAE_LOGW("mhc_so MHCDlOpen start\n");
    if (g_mhcHandle == nullptr) {
        g_mhcHandle = dlopen("/vendor/lib64/libmhc_framework.so", RTLD_LAZY | RTLD_NODELETE);
        if (!g_mhcHandle) {
            HPAE_LOGW("mhc_so dlopen libmhc_framework.so error\n");
            return false;
        }
    }
    g_getGPInstance = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_pattern_get"));
    g_GPInit = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_pattern_init"));
    g_GPDestroy = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_pattern_destroy"));
    g_GPRequestEGraph = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_request_eg"));
    g_GPReleaseEGraph = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_release_eg"));
    g_GPReleaseAll = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcHandle, "mhc_graph_relsease_all"));
    g_GPWait = reinterpret_cast<GPWaitFunc>(dlsym(g_mhcHandle, "mhc_gp_task_wait"));
    g_GPGetVulkanWaitEvent = reinterpret_cast<GPGetGPUWaitEventFunc>(dlsym(g_mhcHandle, "mhc_gp_task_get_wait_event"));
    g_GPGetVulkanNotifyEvent = reinterpret_cast<GPGetGPUNotifyEventFunc>(dlsym(g_mhcHandle, "mhc_gp_vulkan_task_get_notify_event"));
    g_GPTaskSubmit = reinterpret_cast<GPGPTaskSubmitFunc>(dlsym(g_mhcHandle, "mhc_gp_task_submit"));
    if (!g_getGPInstance || !g_GPInit || !g_GPDestroy\
        || !g_GPRequestEGraph || !g_GPReleaseEGraph ||
        || !g_GPVulkanTaskSubmit || !g_GPWait\
        || !g_GPGetVulkanWaitEvent || !g_GPGetVulkanNotifyEvent || !g_GPTaskSubmit) {
        HPAE_LOGE("mhc_so dlsym error\n");
        dlclose(g_mhcHandle);
        g_mhcHandle = nullptr;
        return false;
    }

    HPAE_LOGW("mhc_so LoadLibMHC success\n");
    return true;
#else
    return false;
#endif
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
    if (g_getGPInstance == nullptr) {
        HPAE_LOGW("mhc_so g_getGPInstance nullptr");
        return false;
    }
    g_instance =  g_getGPInstance(PatternType_C::BLUR, "blur_graph");
    return g_GPInit(g_instance, size);
}

bool RSHpaeFfrtPatternManager::MHCRequestEGraph(uint64_t frameId)
{
    if (!g_instance) {
        HPAE_LOGW("mhc_so MHCRequestEGraph g_instance nullptr");
        return false;
    }
    if (g_GPRequestEGraph == nullptr) {
        HPAE_LOGW("mhc_so g_GPRequestEGraph nullptr");
        return false;
    }
    return g_GPRequestEGraph(g_instance, frameId);
}

bool RSHpaeFfrtPatternManager::MHCWait(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCWait", frameId)) {
        return false;
    }

    if (g_GPWait == nullptr) {
        HPAE_LOGW("mhc_so g_GPWait nullptr");
        return false;
    }

    g_GPWait(g_instance, frameId, taskName);
    return true;
}

uint16_t RSHpaeFfrtPatternManager::MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskWaitEvent", frameId)) {
        return false;
    }

    if (g_GPGetVulkanWaitEvent == nullptr) {
        HPAE_LOGW("mhc_so g_GPGetVulkanWaitEvent nullptr");
        return false;
    }

    auto eventId = g_GPGetVulkanWaitEvent(g_instance, frameId, taskName);
    HPAE_LOGW("mhc_so MHCGetVulkanTaskWaitEvent event = %{public}d, taskName=%{public}d\n", eventId, taskName);
    return eventId;
}

uint16_t RSHpaeFfrtPatternManager::MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskNotifyEvent", frameId)) {
        return false;
    }

    if (g_GPGetVulkanNotifyEvent == nullptr) {
        HPAE_LOGW("mhc_so g_GPGetVulkanNotifyEvent nullptr");
        return false;
    }

    auto eventId = g_GPGetVulkanNotifyEvent(g_instance, frameId, taskName);
    HPAE_LOGW("mhc_so MHCGetVulkanTaskNotifyEvent event = %{public}d, taskName=%{public}d\n", eventId, taskName);
    return eventId;
}

bool RSHpaeFfrtPatternManager::MHCReleaseEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCReleaseEGraph", frameId)) {
        return false;
    }

    if (g_GPReleaseEGraph == nullptr) {
        HPAE_LOGW("mhc_so g_GPReleaseEGraph nullptr");
        return false;
    }

    return g_GPReleaseEGraph(g_instance, frameId);
}

void RSHpaeFfrtPatternManager::MHCReleaseAll()
{
    HPAE_LOGW("mhc_so MHCReleaseAll");
    if (g_instance == nullptr) {
        HPAE_LOGE("mhc_so MHCReleaseAll g_instance == nullptr");
        return;
    }

    if (g_GPReleaseAll == nullptr) {
        HPAE_LOGW("mhc_so g_GPReleaseAll nullptr");
        return;
    }
    // return 1 is succ
    int ret = g_GPReleaseAll(g_instance);
    ROSEN_LOGI("mhc_so MHCReleaseAll, ret=%{public}d", ret);
}

bool RSHpaeFfrtPatternManager::MHCSubmitTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, void*** taskHandleVec, size_t numTask, std::function<void()>&&afterFunc)
{
    if (!MHCCheck("MHCSubmitTask", frameId)) {
        return false;
    }

    if (g_GPTaskSubmit == nullptr) {
        HPAE_LOGW("mhc_so g_GPTaskSubmit nullptr");
        return false;
    }

    FunctionHeader* preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    MHC_TaskInfo mhcTaskInfo = {
        .taskName = taskName,
        .c_prefunc = c_preFunc,
        .c_taskHandles = c_taskHandleVec,
        .numTasks = numTasks,
        .c_afterFunc = c_afterFunc,
    };
    g_GPTaskSubmit(g_instance, frameId, &mhcTaskInfo);
    return true;
}

bool RSHpaeFfrtPatternManager::IsThreadIdMatch()
{
#if defined(ROSEN_OHOS)
    return tid_ == gettid();
#else
    return false;
#endif
}
void RSHpaeFfrtPatternManager::SetThreadId()
{
#if defined(ROSEN_OHOS)
    tid_ = gettid();
#endif
}
bool RSHpaeFfrtPatternManager::IsUpdated()
{
    return updated_ && IsThreadIdMatch();
}

} // namespace Rosen
} // namespace OHOS