/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cinttypes>
#include <surface_type.h>
#include "buffer_log.h"
#include "external_window.h"
#include "surface_buffer_impl.h"

#ifndef weak_alias
    #define weak_alias(old, new) \
        extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))
#endif

using namespace OHOS;

OHHardwareBuffer* HardwareBufferFromSurfaceBuffer(SurfaceBuffer* buffer)
{
    return buffer->SurfaceBufferToHardwareBuffer();
}

const SurfaceBuffer* OHHardwareBufferToSurfaceBuffer(const OHHardwareBuffer *buffer)
{
    return SurfaceBuffer::HardwareBufferToSurfaceBuffer(buffer);
}

SurfaceBuffer* OHHardwareBufferToSurfaceBuffer(OHHardwareBuffer *buffer)
{
    return SurfaceBuffer::HardwareBufferToSurfaceBuffer(buffer);
}

OHHardwareBuffer* HardwareBufferAlloc(const OHHardwareBufferConfig* config)
{
    if (config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    BufferRequestConfig bfConfig = {};
    bfConfig.width = config->width;
    bfConfig.height = config->height;
    bfConfig.strideAlignment = config->strideAlignment;
    bfConfig.format = config-> format; // PixelFormat
    bfConfig.usage = config-> usage;
    bfConfig.timeout = 0;
    bfConfig.colorGamut = ColorGamut::COLOR_GAMUT_SRGB;
    bfConfig.transform = TransformType::ROTATE_NONE;
    sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl();
    GSError ret = bufferImpl->Alloc(bfConfig);
    if (ret != GSERROR_OK) {
        BLOGE("Surface Buffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }
    OHHardwareBuffer* buffer = HardwareBufferFromSurfaceBuffer(bufferImpl);
    HardwareBufferReference(buffer);
    return buffer;
}

int32_t HardwareBufferReference(OHHardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->IncStrongRef(ref);
    return OHOS::GSERROR_OK;
}

int32_t HardwareBufferUnreference(OHHardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->DecStrongRef(ref);
    return OHOS::GSERROR_OK;
}

void GetHardwareBufferConfig(OHHardwareBuffer *buffer, OHHardwareBufferConfig* config)
{
    if (buffer == nullptr || config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return;
    }
    const SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    config->width = sbuffer->GetWidth();
    config->height = sbuffer->GetHeight();
    config->format = sbuffer->GetFormat();
    config->strideAlignment = sbuffer->GetStride();
    config->usage = sbuffer->GetUsage();
}

int32_t HardwareBufferMap(OHHardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    return sbuffer->Map();
}

int32_t HardwareBufferUnmap(OHHardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    return sbuffer->Unmap();
}

uint32_t HardwareBufferGetSeqNum(OHHardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    const SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    return sbuffer->GetSeqNum();
}

weak_alias(HardwareBufferAlloc, OH_HardwareBuffer_HardwareBufferAlloc);
weak_alias(HardwareBufferReference, OH_HardwareBuffer_HardwareBufferReference);
weak_alias(HardwareBufferUnreference, OH_HardwareBuffer_HardwareBufferUnreference);
weak_alias(GetHardwareBufferConfig, OH_HardwareBuffer_GetHardwareBufferConfig);
weak_alias(HardwareBufferMap, OH_HardwareBuffer_HardwareBufferMap);
weak_alias(HardwareBufferUnmap, OH_HardwareBuffer_HardwareBufferUnmap);
weak_alias(HardwareBufferGetSeqNum, OH_HardwareBuffer_HardwareBufferGetSeqNum);