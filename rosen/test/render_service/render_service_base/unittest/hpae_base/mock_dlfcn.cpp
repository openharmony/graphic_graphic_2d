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
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "hpae_base/rs_hpae_ffrt_pattern_manager.h"
#include "hpae_base/rs_hpae_hianimation.h"
#include "hpae_base/rs_hpae_base_types.h"

namespace OHOS {
namespace Rosen {

void* MockgetGPInstance(PatternType_C patternType, const char* name)
{
    return reinterpret_cast<void*>(0x1234);
}
bool MockGPInit(void* instance, size_t size)
{
    return true;
}
void MockGPDestroy(void* instance)
{
    return;
}
bool MockGPRequestEGraph(void* instance, uint64_t frameId)
{
    return true;
}
bool MockGPReleaseEGraph(void* instance, uint64_t frameId)
{
    return true;
}
bool MockGPReleaseAll(void* instance)
{
    return true;
}
void* MockGPWait(void* instance, uint64_t frameId, MHC_PatternTaskName taskName)
{
    return reinterpret_cast<void*>(0x1234);
}
uint16_t MockGPGetVulkanWaitEvent(void* instance, uint64_t frameId, MHC_PatternTaskName taskName)
{
    return 1;
}
uint16_t MockGPGetVulkanNotifyEvent(void* instance, uint64_t frameId, MHC_PatternTaskName taskName)
{
    return 2;
}
void MockGPTaskSubmit(void* instance, uint64_t frameId, MHC_TaskInfo* taskInfo)
{
    return;
}

static constexpr int32_t HIANIMATION_SUCC_ = 0;
static constexpr int32_t HIANIMATION_FAIL_ = -1;

bool MockHianimationInputCheck_(const struct BlurImgParam *imgInfo, const struct HaeNoiseValue *noisePara)
{
    return true;
}
int32_t MockHianimationAlgoInit_(uint32_t imgWeight, uint32_t imgHeight, float maxSigma, uint32_t format)
{
    return HIANIMATION_SUCC_;
}
int32_t MockHianimationAlgoDeInit_()
{
    return HIANIMATION_SUCC_;
}
int32_t MockHianimationBuildTask_(const struct HaeBlurBasicAttr *basicInfo, const struct HaeBlurEffectAttr *effectInfo,
    uint32_t *outTaskId, void **outTaskPtr)
{
    *outTaskPtr = reinterpret_cast<void*>(0x1234);
    return HIANIMATION_SUCC_;
}
int32_t MockHianimationBuildTaskFail_(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo,
    uint32_t *outTaskId, void **outTaskPtr)
{
    return HIANIMATION_FAIL_;
}
int32_t MockHianimationSyncProcess_(const struct HaeBlurBasicAttr *basicInfo,
    const struct HaeBlurEffectAttr *effectInfo)
{
    return HIANIMATION_SUCC_;
}
int32_t MockHianimationDestroyTask_(uint32_t taskId)
{
    return HIANIMATION_SUCC_;
}

static hianimation_algo_device_t mockHianimationDevice;

hianimation_algo_device_t* MockGetHianimationDeviceFunc()
{
    mockHianimationDevice.hianimationInputCheck = MockHianimationInputCheck_;
    mockHianimationDevice.hianimationAlgoInit = MockHianimationAlgoInit_;
    mockHianimationDevice.hianimationAlgoDeInit = MockHianimationAlgoDeInit_;
    mockHianimationDevice.hianimationBuildTask = MockHianimationBuildTask_;
    mockHianimationDevice.hianimationSyncProcess = MockHianimationSyncProcess_;
    mockHianimationDevice.hianimationDestroyTask = MockHianimationDestroyTask_;
    return &mockHianimationDevice;
}

extern "C" void* dlsym(void* handle, const char* symbol) {
    if (strcmp(symbol, "mhc_graph_pattern_get") == 0) {
        return reinterpret_cast<void *>(MockgetGPInstance);
    }
    if (strcmp(symbol, "mhc_graph_pattern_init") == 0) {
        return reinterpret_cast<void *>(MockGPInit);
    }
    if (strcmp(symbol, "mhc_graph_pattern_destroy") == 0) {
        return reinterpret_cast<void *>(MockGPDestroy);
    }
    if (strcmp(symbol, "mhc_graph_pattern_request_eg") == 0) {
        return reinterpret_cast<void *>(MockGPRequestEGraph);
    }
    if (strcmp(symbol, "mhc_graph_pattern_release_eg") == 0) {
        return reinterpret_cast<void *>(MockGPReleaseEGraph);
    }
    if (strcmp(symbol, "mhc_graph_pattern_release_all") == 0) {
        return reinterpret_cast<void *>(MockGPReleaseAll);
    }
    if (strcmp(symbol, "mhc_gp_task_wait") == 0) {
        return reinterpret_cast<void *>(MockGPWait);
    }
    if (strcmp(symbol, "mhc_gp_vulkan_task_get_wait_event") == 0) {
        return reinterpret_cast<void *>(MockGPGetVulkanWaitEvent);
    }
    if (strcmp(symbol, "mhc_gp_vulkan_task_get_notify_event") == 0) {
        return reinterpret_cast<void *>(MockGPGetVulkanNotifyEvent);
    }
    if (strcmp(symbol, "mhc_gp_task_submit") == 0) {
        return reinterpret_cast<void *>(MockGPTaskSubmit);
    }
    if (strcmp(symbol, "GetHianimationDevice") == 0) {
        return reinterpret_cast<void *>(MockGetHianimationDeviceFunc);
    }
    return nullptr;
}

extern "C" void* dlopen(const char *filename, int flags)
{
    if (strcmp(filename, "/vendor/lib64/libmhc_framework.so") == 0) {
        return reinterpret_cast<void *>(0x1234);
    }
    if (strcmp(filename, "libanimation.z.so") == 0) {
        return reinterpret_cast<void *>(0x1234);
    }
    return nullptr;
}

extern "C" int dlclose(void *handle)
{
    if (handle == reinterpret_cast<void *>(0x1234)) {
        return 0;
    }
    return -1;
}

} // namespace Rosen
} // namespace OHOS