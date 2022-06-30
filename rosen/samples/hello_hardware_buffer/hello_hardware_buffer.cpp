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

#include "hardware_buffer.h"

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
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloHardwareBuffer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define LOGE(fmt, ...) ::OHOS::HiviewDFX::HiLog::Error(           \
    ::OHOS::HiviewDFX::HiLogLabel {LOG_CORE, 0, "HelloHardwareBuffer"}, \
    "%{public}s: " fmt, __func__, ##__VA_ARGS__)

constexpr uint32_t HARDWARE_BUFFER_REFERENCE_TWICE = 2;

OH_HardwareBuffer_Config config {
    .width = 0x100,
    .height = 0x100,
    .format = PIXEL_FMT_RGBA_8888,
    .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA
};
}

void CompareOHHardwareBufferConfig(OH_HardwareBuffer_Config &config, OH_HardwareBuffer_Config &checkConfig)
{
    if (config.width != checkConfig.width) {
        LOGE("OHHardwareBufferConfig width different");
    }
    if (config.height != checkConfig.height) {
        LOGE("OHHardwareBufferConfig height different");
    }
    if (config.format != checkConfig.format) {
        LOGE("OHHardwareBufferConfig format different");
    }
    if (config.usage != checkConfig.usage) {
        LOGE("OHHardwareBufferConfig usage different");
    }
}

int32_t main(int32_t argc, const char *argv[])
{
    LOGI("sample start");
    OHHardwareBuffer* buffer = OH_HardwareBuffer_HardwareBufferAlloc(&config);
    if (buffer == nullptr) {
        LOGE("HardwareBufferAlloc failed");
    }

    int32_t ret = OH_HardwareBuffer_HardwareBufferReference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("HardwareBufferReference failed");
    }

    OH_HardwareBuffer_Config checkConfig = {};
    OH_HardwareBuffer_GetHardwareBufferConfig(buffer, &checkConfig);
    CompareOHHardwareBufferConfig(config, checkConfig);

    uint32_t hwBufferID = OH_HardwareBuffer_HardwareBufferGetSeqNum(buffer);
    OHOS::SurfaceBuffer *sfBuffer = SurfaceBuffer::HardwareBufferToSurfaceBuffer(buffer);
    uint32_t sfBufferID = sfBuffer->GetSeqNum();
    if (hwBufferID != sfBufferID) {
        LOGE("HardwareBufferGetSeqNum occured error");
    }

    if (sfBuffer->GetSptrRefCount() != HARDWARE_BUFFER_REFERENCE_TWICE) {
        LOGE("HardwareBufferReference occured error");
    }

    void *virAddr = nullptr;
    ret = OH_HardwareBuffer_HardwareBufferMap(buffer, &virAddr);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("HardwareBufferMap failed");
    }

    void *sfVirAddr = sfBuffer->GetVirAddr();
    if (sfVirAddr != virAddr) {
        LOGE("HardwareBufferMap occured error");
    }

    ret = OH_HardwareBuffer_HardwareBufferUnreference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("HardwareBufferUnreference failed");
    }

    if (sfBuffer->GetSptrRefCount() != 1) {
        LOGE("HardwareBufferUnreference occured error");
    }

    ret = OH_HardwareBuffer_HardwareBufferUnmap(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("HardwareBufferUnmap failed");
    }

    ret = OH_HardwareBuffer_HardwareBufferUnreference(buffer);
    if (ret != OHOS::GSERROR_OK) {
        LOGE("HardwareBufferUnreference failed");
    }
    LOGI("sample end");
    return 0;
}