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

#include "hpae_base/rs_hpae_hianimation.h"

#if defined(ROSEN_OHOS)
#include <dlfcn.h>
#include "ffrt_inner.h"
#endif

#include "common/rs_optional_trace.h"
#include "hpae_base/rs_hpae_log.h"

namespace OHOS::Rosen {
using namespace std::chrono_literals;

constexpr int HPAE_BLUR_DELAY = 2;

HianimationManager& HianimationManager::GetInstance()
{
    static HianimationManager mgr;
    return mgr;
}

HianimationManager::HianimationManager()
{
}

HianimationManager::~HianimationManager()
{
    CloseDevice();
#if defined(ROSEN_OHOS)
    if (libHandle_ != nullptr) {
        dlclose(libHandle_);
    }
#endif
    libHandle_ = nullptr;
    hianimationDevice_.device = nullptr;
}

void HianimationManager::OpenDeviceAsync()
{
#if defined(ROSEN_OHOS)
    ffrt::submit_h([this]() {
        this->OpenDevice();
        }, {}, {});
#else
    OpenDevice();
#endif
}

void HianimationManager::AlgoInitAsync(uint32_t imgWidth, uint32_t imgHeight, float maxSigma, uint32_t format)
{
#if defined(ROSEN_OHOS)
    {
        std::unique_lock<MutexType> lock(algoInitMutex_);
        algoInitDone_ = false;
    }

    ffrt::submit_h([=]() {
        this->HianimationAlgoInit(imgWidth, imgHeight, maxSigma, format);
        std::unique_lock<MutexType> lock(algoInitMutex_);
        this->algoInitDone_ = true;
        this->algoInitCv_.notify_all();
        }, {}, {}, ffrt::task_attr().qos(5));
#else
    HianimationAlgoInit(imgWidth, imgHeight, maxSigma, format);
#endif
}

void HianimationManager::WaitAlgoInit()
{
    using namespace std::chrono_literals;
    std::unique_lock<MutexType> lock(algoInitMutex_);
    if (!algoInitDone_) {
        RS_TRACE_NAME("HpaeWaitAlgoInit");
        HPAE_LOGI("HpaeWaitAlgoInit");
        algoInitCv_.wait_for(lock, 30ms, [this]() {
            return this->algoInitDone_;
        });
    }
}

void HianimationManager::AlgoDeInitAsync()
{
#if defined(ROSEN_OHOS)
    ffrt::submit_h([this]() {
        this->HianimationAlgoDeInit();
        }, {}, {});
#else
    HianimationAlgoDeInit();
#endif
}

bool HianimationManager::HianimationInvalid()
{
    return hianimationDevice_.device == nullptr;
}

void HianimationManager::OpenDevice()
{
#if defined(ROSEN_OHOS)
    std::unique_lock<MutexType> lock(mutex_);
    if (hianimationDevice_.device != nullptr) {
        // do nothing
        return;
    }

    RS_OPTIONAL_TRACE_NAME("Hianimation: OpenDevice");

    if (libHandle_ != nullptr) {
        HPAE_LOGW("[%{public}s_%{public}d]:exception and close library", __func__, __LINE__);
        dlclose(libHandle_);
    }

    libHandle_ = dlopen("libhianimation_client.z.so", RTLD_LAZY);
    if (libHandle_ == nullptr) {
        HPAE_LOGW("[%{public}s_%{public}d]:load library failed, reason: %{public}s", __func__, __LINE__, dlerror());
        return;
    }

    hianimationDevice_.openDevice = (HianimationOpenDeviceFunc)dlsym(libHandle_, "HianimationOpenDevice");
    hianimationDevice_.closeDevice = (HianimationCloseDeviceFunc)dlsym(libHandle_, "HianimationCloseDevice");
    hianimationDevice_.hianimationInputCheck = (HianimationInputCheckFunc)dlsym(libHandle_, "HianimationInputCheck");
    hianimationDevice_.hianimationAlgoInit = (HianimationAlgoInitFunc)dlsym(libHandle_, "HianimationAlgoInit");
    hianimationDevice_.hianimationAlgoDeInit = (HianimationAlgoDeInitFunc)dlsym(libHandle_, "HianimationAlgoDeInit");
    hianimationDevice_.hianimationBuildTask = (HianimationBuildTaskFunc)dlsym(libHandle_, "HianimationBuildTask");
    hianimationDevice_.hianimationDestroyTask =
        (HianimationDestroyTaskFunc)dlsym(libHandle_, "HianimationDestroyTask");
    hianimationDevice_.hianimationDumpDebugInfo =
        (HianimationDumpDebugInfoFunc)dlsym(libHandle_, "HianimationDumpDebugInfo");

    bool symbolReady = hianimationDevice_.openDevice && hianimationDevice_.closeDevice &&
        hianimationDevice_.hianimationInputCheck && hianimationDevice_.hianimationAlgoInit &&
        hianimationDevice_.hianimationAlgoDeInit && hianimationDevice_.hianimationBuildTask &&
        hianimationDevice_.hianimationDestroyTask && hianimationDevice_.hianimationDumpDebugInfo;
    if (symbolReady) {
        hianimationDevice_.device = hianimationDevice_.openDevice();
    }
    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGW("hianimation device is null");
        hianimationDevice_ = {};
        dlclose(libHandle_);
        libHandle_ = nullptr;
        return;
    }

    HPAE_LOGI("[%{public}s_%{public}d]:load success", __func__, __LINE__);
#endif
}

void HianimationManager::CloseDevice()
{
#if defined(ROSEN_OHOS)
    std::unique_lock<MutexType> lock(mutex_);
    RS_OPTIONAL_TRACE_NAME("Hianimation: CloseDevice");
    if (hianimationDevice_.closeDevice) {
        hianimationDevice_.closeDevice();
    }
    if (libHandle_ != nullptr) {
        dlclose(libHandle_);
    }
    hianimationDevice_ = {};
#endif
}

bool HianimationManager::HianimationInputCheck(const struct BlurImgParam *imgInfo,
    const struct HaeNoiseValue *noisePara)
{
    WaitAlgoInit();

    std::unique_lock<MutexType> lock(mutex_);
    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return false;
    }

    if (!algoInited_) {
        HPAE_LOGW("hianimation not inited");
        return false;
    }

    HPAE_LOGD("HianimationInputCheck: w:%{public}d, h:%{public}d, sigma:%{public}f",
        imgInfo->width, imgInfo->height, imgInfo->sigmaNum);
    return (hianimationDevice_.hianimationInputCheck(imgInfo, noisePara) == 0); // 0 for success
}

int32_t HianimationManager::HianimationAlgoInit(uint32_t panelWidth, uint32_t panelHeight, float maxSigma,
    uint32_t format)
{
    WaitAllTaskDone(); // do outside mutex_

    std::unique_lock<MutexType> lock(mutex_);
    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    if (algoInited_) {
        HPAE_LOGI("hianimation already inited");
        return 0;
    }

    RS_OPTIONAL_TRACE_NAME("Hianimation: HianimationAlgoInit");
    HPAE_LOGI("HianimationAlgoInit");

    for (auto taskId : taskIdMap_) {
        HPAE_LOGW("force destroy task: %d", taskId);
        hianimationDevice_.hianimationDestroyTask(taskId);
    }
    taskIdMap_.clear();

    auto result = hianimationDevice_.hianimationAlgoInit(panelWidth, panelHeight, maxSigma, format);

    algoInited_ = (result == 0);

    return result;
}

int32_t HianimationManager::HianimationAlgoDeInit()
{
    WaitAlgoInit();
    WaitAllTaskDone(); // do outside mutex_

    std::unique_lock<MutexType> lock(mutex_);
    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    RS_OPTIONAL_TRACE_NAME("Hianimation: HianimationAlgoDeInit");
    HPAE_LOGI("HianimationAlgoDeInit");

    for (auto taskId : taskIdMap_) {
        HPAE_LOGW("force destroy task: %d", taskId);
        hianimationDevice_.hianimationDestroyTask(taskId);
    }
    taskIdMap_.clear();

    int32_t result = 0;
    if (algoInited_) {
        result = hianimationDevice_.hianimationAlgoDeInit();
    }

    algoInited_ = false;

    return result;
}

bool HianimationManager::WaitAllTaskDone()
{
    std::unique_lock<MutexType> lock(mutex_);
    bool result = true;
    if (!taskIdMap_.empty()) {
        RS_TRACE_NAME("HpaeWaitAllTaskDone");
        HPAE_LOGI("HpaeWaitAllTaskDone");
        result = taskAvailableCv_.wait_for(lock, 30ms, [this]() {
            return this->taskIdMap_.empty();
        });
    }

    if (!result) {
        HPAE_LOGE("hpae task lost!");
    }

    return result;
}

bool HianimationManager::WaitPreviousTask()
{
    std::unique_lock<MutexType> lock(mutex_);
    bool result = true;
    if (taskIdMap_.size() >= HPAE_BLUR_DELAY) {
        RS_TRACE_NAME("HpaeWaitPreviousTask");
        HPAE_LOGI("HpaeWaitPreviousTask");
        result = taskAvailableCv_.wait_for(lock, 5ms, [this]() {
            return this->taskIdMap_.size() < HPAE_BLUR_DELAY;
        });
    }

    return result;
}

int32_t HianimationManager::HianimationBuildTask(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo, uint32_t *outTaskId, void **outTaskPtr)
{
    std::unique_lock<MutexType> lock(mutex_);
    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    RS_OPTIONAL_TRACE_NAME("Hianimation: HianimationBuildTask");
    auto ret = hianimationDevice_.hianimationBuildTask(basicInfo, effectInfo, outTaskId, outTaskPtr);
    if (ret == 0 && outTaskId) {
        taskIdMap_.insert(*outTaskId);
    }
    return ret;
}

void HianimationManager::HianimationDumpDebugInfo(uint32_t taskId)
{
    std::unique_lock<MutexType> lock(mutex_);

    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("Hianimation: HianimationDumpDebugInfo: %d", taskId);
    hianimationDevice_.hianimationDumpDebugInfo(taskId);
}

int32_t HianimationManager::HianimationDestroyTask(uint32_t taskId)
{
    std::unique_lock<MutexType> lock(mutex_);

    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("Hianimation: HianimationDestroyTask: %d", taskId);
    taskIdMap_.erase(taskId);
    return hianimationDevice_.hianimationDestroyTask(taskId);
}

int32_t HianimationManager::HianimationDestroyTaskAndNotify(uint32_t taskId)
{
    std::unique_lock<MutexType> lock(mutex_);

    if (hianimationDevice_.device == nullptr) {
        HPAE_LOGE("device is nullptr");
        return -1;
    }

    RS_OPTIONAL_TRACE_NAME_FMT("Hianimation: HianimationDestroyTaskAndNotify: %d", taskId);
    int32_t ret = hianimationDevice_.hianimationDestroyTask(taskId);
    taskIdMap_.erase(taskId);
    taskAvailableCv_.notify_all();

    return ret;
}

} // OHOS::Rosen