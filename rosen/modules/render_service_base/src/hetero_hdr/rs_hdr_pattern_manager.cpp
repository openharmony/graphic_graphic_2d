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

#include "hetero_hdr/rs_hdr_pattern_manager.h"

#ifdef ROSEN_OHOS
#include <dlfcn.h>
#endif

using GPInstanceGetFunc = void*(*)(PatternType_C, const char*);
using GPInstanceInitFunc = bool(*)(void*, size_t);
using GPInstanceDestroyFunc = void(*)(void*);
using GPRequstEGraphFunc = bool(*)(void*, uint64_t);
using GPQueryTaskError = int32_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPReleaseEGraphFunc = bool(*)(void*, uint64_t);
using GPReleaseAllFunc = bool(*)(void*);
using GPAAETaskSubmitFunc = void(*)(void*, uint64_t, MHC_PatternTaskName, void*, void**, void*);
using GPWaitFunc = void(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGPUTaskSubmitFunc = void(*)(void*, uint64_t, MHC_PatternTaskName, void*, void*);
using GPGetGPUWaitEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);
using GPGetGPUNotifyEventFunc = uint16_t(*)(void*, uint64_t, MHC_PatternTaskName);

namespace {
static GPInstanceGetFunc g_getGraphPatternInstance = nullptr;
static GPInstanceInitFunc g_graphPatternInit = nullptr;
static GPInstanceDestroyFunc g_graphPatternDestroy = nullptr;
static GPRequstEGraphFunc g_graphPatternRequestEGraph = nullptr;
static GPReleaseEGraphFunc g_graphPatternReleaseEGraph = nullptr;
static GPReleaseAllFunc g_graphPatternReleaseAll = nullptr;
static GPAAETaskSubmitFunc g_graphPatternAnimationTaskSubmit = nullptr;
static GPGPUTaskSubmitFunc g_graphPatternVulkanTaskSubmit = nullptr;
static GPWaitFunc g_graphPatternWait = nullptr;
static GPGetGPUWaitEventFunc g_graphPatternGetVulkanWaitEvent = nullptr;
static GPGetGPUNotifyEventFunc g_graphPatternGetVulkanNotifyEvent = nullptr;

#ifdef ROSEN_OHOS
static void* g_mhcLibframeworkHandle = nullptr;
#endif
}

namespace OHOS {
namespace Rosen {

template <typename F>
std::shared_ptr<FunctionHeader> create_function_wrapper(F &&func)
{
    if (func == nullptr) {
        return nullptr;
    }
    // 1. Allocate callable on heap with perfect forwarding
    auto *func_copy = new std::decay_t<F>(std::forward<F>(func));

    // 2. Create function header structure
    auto header = std::make_shared<FunctionHeader>();

    // 3. Set up execution function that invokes the callable
    header->execute = [](void *data) {
        auto *f = static_cast<std::decay_t<F> *>(data);
        (*f)();  // operator()
    };

    // 4. Set up cleanup function to release resources
    header->destroy = [](void *data) {
        auto *f = static_cast<std::decay_t<F> *>(data);
        delete f;
    };

    // 5. Store callable pointer in header
    header->data = func_copy;
    return header;
}

RSHDRPatternManager& RSHDRPatternManager::Instance()
{
    static RSHDRPatternManager instance;
    return instance;
}

RSHDRPatternManager::RSHDRPatternManager()
{
}

RSHDRPatternManager::~RSHDRPatternManager()
{
    if (g_instance) {
        g_graphPatternDestroy(g_instance);
        g_instance = nullptr;
    }
#ifdef ROSEN_OHOS
    if (g_mhcLibframeworkHandle) {
        dlclose(g_mhcLibframeworkHandle);
        g_getGraphPatternInstance = nullptr;
        g_graphPatternInit = nullptr;
        g_graphPatternDestroy = nullptr;
        g_graphPatternRequestEGraph = nullptr;
        g_graphPatternReleaseEGraph = nullptr;
        g_graphPatternAnimationTaskSubmit = nullptr;
        g_graphPatternVulkanTaskSubmit = nullptr;
        g_graphPatternWait = nullptr;
        g_graphPatternGetVulkanWaitEvent = nullptr;
        g_graphPatternGetVulkanNotifyEvent = nullptr;
    }
#endif
}

bool RSHDRPatternManager::MHCDlOpen()
{
#ifdef ROSEN_OHOS
    RS_LOGW("mhc_so MHCDlOpen start\n");
    if (isFinishDLOpen_) {
        RS_LOGE("mhc_so MHCDlOpen isFinishDLOpen_ true\n");
        return true;
    }
    if (g_mhcLibframeworkHandle == nullptr) {
        g_mhcLibframeworkHandle = dlopen("/vendor/lib64/libmhc_framework.so", RTLD_LAZY | RTLD_NODELETE);
        if (!g_mhcLibframeworkHandle) {
            RS_LOGW("mhc_so dlopen libmhc_framework.so error\n");
            return false;
        }
    }

    g_getGraphPatternInstance = reinterpret_cast<GPInstanceGetFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_get"));
    g_graphPatternInit = reinterpret_cast<GPInstanceInitFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_init"));
    g_graphPatternDestroy = reinterpret_cast<GPInstanceDestroyFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_destroy"));
    g_graphPatternRequestEGraph = reinterpret_cast<GPRequstEGraphFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_request_eg"));
    g_graphPatternReleaseEGraph = reinterpret_cast<GPReleaseEGraphFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_release_eg"));
    g_graphPatternReleaseAll = reinterpret_cast<GPReleaseAllFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_graph_pattern_release_all"));
    g_graphPatternAnimationTaskSubmit = reinterpret_cast<GPAAETaskSubmitFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_gp_animation_task_submit"));
    g_graphPatternVulkanTaskSubmit = reinterpret_cast<GPGPUTaskSubmitFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_gp_vulkan_task_submit"));
    g_graphPatternWait = reinterpret_cast<GPWaitFunc>(dlsym(g_mhcLibframeworkHandle, "mhc_gp_task_wait"));
    g_graphPatternGetVulkanWaitEvent = reinterpret_cast<GPGetGPUWaitEventFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_gp_vulkan_task_get_wait_event"));
    g_graphPatternGetVulkanNotifyEvent = reinterpret_cast<GPGetGPUNotifyEventFunc>(dlsym(g_mhcLibframeworkHandle,
        "mhc_gp_vulkan_task_get_notify_event"));

    if (!g_getGraphPatternInstance || !g_graphPatternInit || !g_graphPatternDestroy || !g_graphPatternRequestEGraph\
        || !g_graphPatternReleaseEGraph || !g_graphPatternAnimationTaskSubmit || !g_graphPatternVulkanTaskSubmit\
        || !g_graphPatternWait || !g_graphPatternGetVulkanWaitEvent || !g_graphPatternGetVulkanNotifyEvent\
        || !g_graphPatternReleaseAll) {
        RS_LOGE("mhc_so dlsym error\n");
        dlclose(g_mhcLibframeworkHandle);
        return false;
    }
    isFinishDLOpen_ = true;
    RS_LOGW("mhc_so LoadLibMHC success\n");
    return true;
#else
    return false;
#endif
}

bool RSHDRPatternManager::MHCCheck(const std::string logTag, uint64_t frameId)
{
    if (!g_instance) {
        RS_LOGE("mhc_so MHCCheck %{public}s g_instance == nullptr", logTag.c_str());
        return false;
    }
    RS_LOGW("mhc_so %{public}s MHCCheck frameId:%{public}" PRIu64 " ", logTag.c_str(), frameId);
    return true;
}

bool RSHDRPatternManager::MHCGraphPatternInit(size_t size)
{
    RS_LOGW("mhc_so MHCGraphPatternInit");
    if (g_instance) {
        return true;
    }
    if (!isFinishDLOpen_) {
        RS_LOGE("mhc_so dlsym error");
        return false;
    }
    g_instance = g_getGraphPatternInstance(PATTERN_HDR, "test_graph");
    return g_graphPatternInit(g_instance, size);
}

bool RSHDRPatternManager::MHCRequestEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCRequestEGraph", frameId)) {
        return false;
    }
    return g_graphPatternRequestEGraph(g_instance, frameId);
}

bool RSHDRPatternManager::MHCSubmitHDRTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, void** taskHandle, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitHDRTask", frameId)) {
        return false;
    }
    std::shared_ptr<FunctionHeader> preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader.get());
    std::shared_ptr<FunctionHeader> afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader.get());

    g_graphPatternAnimationTaskSubmit(g_instance, frameId, taskName, c_preFunc, taskHandle, c_afterFunc);
    return true;
}

bool RSHDRPatternManager::MHCSubmitVulkanTask(uint64_t frameId, MHC_PatternTaskName taskName, \
    std::function<void()>&& preFunc, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitVulkanTask", frameId)) {
        return false;
    }
    std::shared_ptr<FunctionHeader> preFuncHeader = create_function_wrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader.get());
    std::shared_ptr<FunctionHeader> afterFuncHeader = create_function_wrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader.get());

    g_graphPatternVulkanTaskSubmit(g_instance, frameId, taskName, c_preFunc, c_afterFunc);
    return true;
}

bool RSHDRPatternManager::MHCWait(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCWait", frameId)) {
        return false;
    }

    g_graphPatternWait(g_instance, frameId, taskName);
    return true;
}

uint16_t RSHDRPatternManager::MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskWaitEvent", frameId)) {
        return false;
    }
    auto eventId = g_graphPatternGetVulkanWaitEvent(g_instance, frameId, taskName);
    RS_LOGW("mhc_so RSHDRPatternManager::MHCGetVulkanTaskWaitEvent event = %{public}d\n", eventId);
    return eventId;
}

uint16_t RSHDRPatternManager::MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHC_PatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskNotifyEvent", frameId)) {
        return false;
    }
    auto eventId = g_graphPatternGetVulkanNotifyEvent(g_instance, frameId, taskName);
    RS_LOGW("mhc_so RSHDRPatternManager::MHCGetVulkanTaskNotifyEvent event = %{public}d\n", eventId);
    return eventId;
}

bool RSHDRPatternManager::MHCReleaseEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCReleaseEGraph", frameId)) {
        return false;
    }
    return g_graphPatternReleaseEGraph(g_instance, frameId);
}

bool RSHDRPatternManager::MHCReleaseAll()
{
    if (!g_instance) {
        return false;
    }
    g_graphPatternReleaseAll(g_instance);
    return true;
}

std::vector<uint64_t> RSHDRPatternManager::MHCGetFrameIdForGpuTask()
{
    std::vector<uint64_t> frameIdVec{};
#ifdef ROSEN_OHOS
    std::unique_lock<std::mutex> lock(frameIdMutex_);
    if (lastFrameIdUsed_ && curFrameIdUsed_) {
        return frameIdVec;
    }

    if (lastFrameIdUsed_ == false && lastFrameConsumed_) {
        lastFrameIdUsed_ = true;
        lastFrameConsumed_ = false;
        frameIdVec.emplace_back(lastFrameId_);
    }
    if (curFrameIdUsed_ == false && IsThreadIdMatch()) {
        curFrameIdUsed_ = true;
        processConsumed_ = false;
        frameIdVec.emplace_back(curFrameId_);
    }
#endif
    return frameIdVec;
}

} // namespace Rosen
} // namespace OHOS