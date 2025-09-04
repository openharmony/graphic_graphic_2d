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
namespace OHOS {
namespace Rosen {

struct FunctionHeader {
    void (*execute)(void*);  // Execute function pointer
    void (*destroy)(void*);  // Destroy function pointer
    void* data;              // User data (storing lambda)
};

template <typename F>
FunctionHeader* CreateFunctionWrapper(F&& func)
{
    if (func == nullptr) {
        return nullptr;
    }
    // 1. Allocate callable on heap with perfect forwarding
    auto* funcCopy = new std::decay_t<F>(std::forward<F>(func));

    // 2. Create function header structure
    auto* header = new FunctionHeader;

    // 3. Set up execution function that invokes the callable
    header->execute = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        (*f)();  // operator()
    };

    // 4. Set up cleanup function to release resources
    header->destroy = [](void* data) {
        auto* f = static_cast<std::decay_t<F>*>(data);
        delete f;
    };

    // 5. Store callable pointer in header
    header->data = funcCopy;
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
    if (graphPatternInstance_) {
        MHCDevice_->graphPatternDestroy(graphPatternInstance_);
        graphPatternInstance_ = nullptr;
    }
    MHCDlClose();
}

void RSHDRPatternManager::MHCDlClose()
{
#ifdef ROSEN_OHOS
    if (MHCLibFrameworkHandle_) {
        dlclose(MHCLibFrameworkHandle_);
        MHCDevice_->getGraphPatternInstance = nullptr;
        MHCDevice_->graphPatternInit = nullptr;
        MHCDevice_->graphPatternDestroy = nullptr;
        MHCDevice_->graphPatternRequestEGraph = nullptr;
        MHCDevice_->graphPatternReleaseEGraph = nullptr;
        MHCDevice_->graphPatternAnimationTaskSubmit = nullptr;
        MHCDevice_->graphPatternVulkanTaskSubmit = nullptr;
        MHCDevice_->graphPatternWait = nullptr;
        MHCDevice_->graphPatternGetVulkanWaitEvent = nullptr;
        MHCDevice_->graphPatternGetVulkanNotifyEvent = nullptr;
        MHCLibFrameworkHandle_ = nullptr;
    }
#endif
}

bool RSHDRPatternManager::MHCDlOpen()
{
#ifdef ROSEN_OHOS
    RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCDlOpen start");
    if (isFinishDLOpen_) {
        RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCDlOpen the MHC is already opened");
        return true;
    }
    if (MHCLibFrameworkHandle_ == nullptr) {
        MHCLibFrameworkHandle_ = dlopen("/vendor/lib64/libmhc_framework.so", RTLD_LAZY | RTLD_NODELETE);
        if (!MHCLibFrameworkHandle_) {
            RS_LOGW("[hdrHetero]:RSHDRPatternManager MHCDlOpen dlopen MHC failed");
            return false;
        }
    }

    MHCDevice_->getGraphPatternInstance = reinterpret_cast<GPInstanceGetFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_get"));
    MHCDevice_->graphPatternInit = reinterpret_cast<GPInstanceInitFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_init"));
    MHCDevice_->graphPatternDestroy = reinterpret_cast<GPInstanceDestroyFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_destroy"));
    MHCDevice_->graphPatternRequestEGraph = reinterpret_cast<GPRequestEGraphFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_request_eg"));
    MHCDevice_->graphPatternReleaseEGraph = reinterpret_cast<GPReleaseEGraphFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_release_eg"));
    MHCDevice_->graphPatternReleaseAll = reinterpret_cast<GPReleaseAllFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_graph_pattern_release_all"));
    MHCDevice_->graphPatternAnimationTaskSubmit = reinterpret_cast<GPHpaeTaskSubmitFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_gp_animation_task_submit"));
    MHCDevice_->graphPatternVulkanTaskSubmit = reinterpret_cast<GPGPUTaskSubmitFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_gp_vulkan_task_submit"));
    MHCDevice_->graphPatternWait = reinterpret_cast<GPWaitFunc>(dlsym(MHCLibFrameworkHandle_, "mhc_gp_task_wait"));
    MHCDevice_->graphPatternGetVulkanWaitEvent = reinterpret_cast<GPGetGPUWaitEventFunc>(dlsym(MHCLibFrameworkHandle_,
        "mhc_gp_vulkan_task_get_wait_event"));
    MHCDevice_->graphPatternGetVulkanNotifyEvent = reinterpret_cast<GPGetGPUNotifyEventFunc>(
        dlsym(MHCLibFrameworkHandle_, "mhc_gp_vulkan_task_get_notify_event"));

    if (MHCDlsymInvalid()) {
        RS_LOGE("[hdrHetero]:RSHDRPatternManager MHCDlOpen dlsym error");
        dlclose(MHCLibFrameworkHandle_);
        MHCLibFrameworkHandle_ = nullptr;
        return false;
    }
    isFinishDLOpen_ = true;
    RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCDlOpen success");
    return true;
#else
    return false;
#endif
}

bool RSHDRPatternManager::MHCDlsymInvalid()
{
    return !MHCDevice_->getGraphPatternInstance || !MHCDevice_->graphPatternInit || !MHCDevice_->graphPatternDestroy ||
           !MHCDevice_->graphPatternRequestEGraph || !MHCDevice_->graphPatternReleaseEGraph ||
           !MHCDevice_->graphPatternAnimationTaskSubmit || !MHCDevice_->graphPatternVulkanTaskSubmit ||
           !MHCDevice_->graphPatternWait || !MHCDevice_->graphPatternGetVulkanWaitEvent ||
           !MHCDevice_->graphPatternGetVulkanNotifyEvent || !MHCDevice_->graphPatternReleaseAll;
}

bool RSHDRPatternManager::MHCCheck(const std::string logTag)
{
    if (!graphPatternInstance_) {
        RS_LOGE("[hdrHetero]:RSHDRPatternManager MHCCheck %{public}s graphPatternInstance_ == nullptr", logTag.c_str());
        return false;
    }
    return true;
}

bool RSHDRPatternManager::MHCGraphPatternInit(size_t size)
{
    if (graphPatternInstance_) {
        return true;
    }
    if (!isFinishDLOpen_) {
        RS_LOGE("[hdrHetero]:RSHDRPatternManager MHCGraphPatternInit MHC dlsym error");
        return false;
    }
    graphPatternInstance_ = MHCDevice_->getGraphPatternInstance(PATTERN_HDR, "hdr_graph");
    return MHCDevice_->graphPatternInit(graphPatternInstance_, size);
}

bool RSHDRPatternManager::MHCRequestEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCRequestEGraph")) {
        return false;
    }
    return MHCDevice_->graphPatternRequestEGraph(graphPatternInstance_, frameId);
}

bool RSHDRPatternManager::MHCSubmitHDRTask(uint64_t frameId, MHCPatternTaskName taskName,
    std::function<void()>&& preFunc, void** taskHandle, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitHDRTask")) {
        return false;
    }
    FunctionHeader* preFuncHeader = CreateFunctionWrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = CreateFunctionWrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    MHCDevice_->graphPatternAnimationTaskSubmit(graphPatternInstance_, frameId, taskName,
        c_preFunc, taskHandle, c_afterFunc);
    return true;
}

bool RSHDRPatternManager::MHCSubmitVulkanTask(uint64_t frameId, MHCPatternTaskName taskName,
    std::function<void()>&& preFunc, std::function<void()>&& afterFunc)
{
    if (!MHCCheck("MHCSubmitVulkanTask")) {
        return false;
    }
    FunctionHeader* preFuncHeader = CreateFunctionWrapper(std::move(preFunc));
    void* c_preFunc = static_cast<void*>(preFuncHeader);
    FunctionHeader* afterFuncHeader = CreateFunctionWrapper(std::move(afterFunc));
    void* c_afterFunc = static_cast<void*>(afterFuncHeader);

    MHCDevice_->graphPatternVulkanTaskSubmit(graphPatternInstance_, frameId, taskName, c_preFunc, c_afterFunc);
    return true;
}

bool RSHDRPatternManager::MHCWait(uint64_t frameId, MHCPatternTaskName taskName)
{
    if (!MHCCheck("MHCWait")) {
        return false;
    }

    MHCDevice_->graphPatternWait(graphPatternInstance_, frameId, taskName);
    return true;
}

uint16_t RSHDRPatternManager::MHCGetVulkanTaskWaitEvent(uint64_t frameId, MHCPatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskWaitEvent")) {
        return false;
    }
    auto eventId = MHCDevice_->graphPatternGetVulkanWaitEvent(graphPatternInstance_, frameId, taskName);
    RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCGetVulkanTaskWaitEvent event = %{public}d", eventId);
    return eventId;
}

uint16_t RSHDRPatternManager::MHCGetVulkanTaskNotifyEvent(uint64_t frameId, MHCPatternTaskName taskName)
{
    if (!MHCCheck("MHCGetVulkanTaskNotifyEvent")) {
        return false;
    }
    auto eventId = MHCDevice_->graphPatternGetVulkanNotifyEvent(graphPatternInstance_, frameId, taskName);
    RS_LOGD("[hdrHetero]:RSHDRPatternManager MHCGetVulkanTaskNotifyEvent event = %{public}d", eventId);
    return eventId;
}

bool RSHDRPatternManager::MHCReleaseEGraph(uint64_t frameId)
{
    if (!MHCCheck("MHCReleaseEGraph")) {
        return false;
    }
    return MHCDevice_->graphPatternReleaseEGraph(graphPatternInstance_, frameId);
}

bool RSHDRPatternManager::MHCReleaseAll()
{
    if (!graphPatternInstance_) {
        return false;
    }
    MHCDevice_->graphPatternReleaseAll(graphPatternInstance_);
    return true;
}

std::vector<uint64_t> RSHDRPatternManager::MHCGetFrameIdForGPUTask()
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