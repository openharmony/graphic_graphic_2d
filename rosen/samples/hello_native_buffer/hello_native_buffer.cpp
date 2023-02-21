/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "native_buffer.h"

#include <cstddef>
#include <iostream>
#include <ostream>
#include <thread>
#include <unistd.h>

#include "surface_buffer_impl.h"
#include "buffer_log.h"

using namespace OHOS;

namespace {
#define LOGI(fmt, ...) ::OHOS::HiviewDFX::HiLog::Info(            \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloNativeBuffer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define LOGE(fmt, ...) ::OHOS::HiviewDFX::HiLog::Error(           \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloNativeBuffer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)

constexpr uint32_t HARDWARE_BUFFER_REFERENCE_TWICE = 2;

OH_NativeBuffer_Config config {
    .width = 0x100,
    .height = 0x100,
    .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
    .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA
};

void CompareOH_NativeBufferConfig(const OH_NativeBuffer_Config &config, const OH_NativeBuffer_Config &checkConfig)
{
    if (config.width != checkConfig.width) {
        LOGE("OH_NativeBufferConfig width different");
    }
    if (config.height != checkConfig.height) {
        LOGE("OH_NativeBufferConfig height different");
    }
    if (config.format != checkConfig.format) {
        LOGE("OH_NativeBufferConfig format different");
    }
    if (config.usage != checkConfig.usage) {
        LOGE("OH_NativeBufferConfig usage different");
    }
}

}

int32_t main(int32_t argc, const char *argv[])
{
    LOGI("sample start");
    OH_NativeBuffer* buffer = OH_NativeBuffer_Alloc(&config);
    if (buffer == nullptr) {
        LOGE("NativeBufferAlloc failed");
    }

    int32_t ret = OH_NativeBuffer_Reference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("NativeBufferReference failed");
    }

    OH_NativeBuffer_Config checkConfig = {};
    OH_NativeBuffer_GetConfig(buffer, &checkConfig);
    CompareOH_NativeBufferConfig(config, checkConfig);

    uint32_t hwBufferID = OH_NativeBuffer_GetSeqNum(buffer);
    OHOS::SurfaceBuffer *sfBuffer = SurfaceBuffer::NativeBufferToSurfaceBuffer(buffer);
    uint32_t sfBufferID = sfBuffer->GetSeqNum();
    if (hwBufferID != sfBufferID) {
        LOGE("NativeBufferGetSeqNum occured error");
    }

    if (sfBuffer->GetSptrRefCount() != HARDWARE_BUFFER_REFERENCE_TWICE) {
        LOGE("NativeBufferReference occured error");
    }

    void *virAddr = nullptr;
    ret = OH_NativeBuffer_Map(buffer, &virAddr);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("NativeBufferMap failed");
    }

    void *sfVirAddr = sfBuffer->GetVirAddr();
    if (sfVirAddr != virAddr) {
        LOGE("NativeBufferMap occured error");
    }

    ret = OH_NativeBuffer_Unreference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("NativeBufferUnreference failed");
    }

    if (sfBuffer->GetSptrRefCount() != 1) {
        LOGE("NativeBufferUnreference occured error");
    }

    ret = OH_NativeBuffer_Unmap(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("NativeBufferUnmap failed");
    }

    ret = OH_NativeBuffer_Unreference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("NativeBufferUnreference failed");
    }
    LOGI("sample end");
    return 0;
}