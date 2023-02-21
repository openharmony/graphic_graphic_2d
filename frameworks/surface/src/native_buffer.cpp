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

#include "native_buffer_inner.h"

#include <cinttypes>
#include <surface_type.h>
#include "buffer_log.h"
#include "external_window.h"
#include "surface_buffer_impl.h"

using namespace OHOS;

static OH_NativeBuffer* OH_NativeBufferFromSurfaceBuffer(SurfaceBuffer* buffer)
{
    return buffer->SurfaceBufferToNativeBuffer();
}

static SurfaceBuffer* OH_NativeBufferToSurfaceBuffer(OH_NativeBuffer *buffer)
{
    return SurfaceBuffer::NativeBufferToSurfaceBuffer(buffer);
}

static const SurfaceBuffer* OH_NativeBufferToSurfaceBuffer(const OH_NativeBuffer *buffer)
{
    return SurfaceBuffer::NativeBufferToSurfaceBuffer(buffer);
}

OH_NativeBuffer* OH_NativeBuffer_Alloc(const OH_NativeBuffer_Config* config)
{
    if (config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    BufferRequestConfig bfConfig = {};
    bfConfig.width = config->width;
    bfConfig.height = config->height;
    bfConfig.strideAlignment = 0x8; // set 0x8 as default value to alloc SurfaceBufferImpl
    bfConfig.format = config->format; // PixelFormat
    bfConfig.usage = config->usage;
    bfConfig.timeout = 0;
    bfConfig.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    bfConfig.transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl();
    GSError ret = bufferImpl->Alloc(bfConfig);
    if (ret != GSERROR_OK) {
        BLOGE("Surface Buffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }

    OH_NativeBuffer* buffer = OH_NativeBufferFromSurfaceBuffer(bufferImpl);
    int32_t err = OH_NativeBuffer_Reference(buffer);
    if (err != OHOS::GSERROR_OK) {
        BLOGE("NativeBufferReference failed");
        return nullptr;
    }
    return buffer;
}

int32_t OH_NativeBuffer_Reference(OH_NativeBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->IncStrongRef(ref);
    return OHOS::GSERROR_OK;
}

int32_t OH_NativeBuffer_Unreference(OH_NativeBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->DecStrongRef(ref);
    return OHOS::GSERROR_OK;
}

void OH_NativeBuffer_GetConfig(OH_NativeBuffer *buffer, OH_NativeBuffer_Config* config)
{
    if (buffer == nullptr || config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return;
    }
    const SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    config->width = sbuffer->GetWidth();
    config->height = sbuffer->GetHeight();
    config->format = sbuffer->GetFormat();
    config->usage = sbuffer->GetUsage();
}

int32_t OH_NativeBuffer_Map(OH_NativeBuffer *buffer, void **virAddr)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    int32_t ret = sbuffer->Map();
    if (ret == OHOS::GSERROR_OK) {
        *virAddr = sbuffer->GetVirAddr();
    }
    return ret;
}

int32_t OH_NativeBuffer_Unmap(OH_NativeBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    return sbuffer->Unmap();
}

uint32_t OH_NativeBuffer_GetSeqNum(OH_NativeBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    const SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    return sbuffer->GetSeqNum();
}

const BufferHandle* OH_NativeBuffer_GetBufferHandle(const OH_NativeBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    const SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    return sbuffer->GetBufferHandle();
}

void OH_NativeBuffer_GetNativeBufferConfig(const OH_NativeBuffer *buffer, OH_NativeBuffer_Config* config)
{
    if (buffer == nullptr || config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return;
    }
    const SurfaceBuffer* sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    config->width = sbuffer->GetWidth();
    config->height = sbuffer->GetHeight();
    config->format = sbuffer->GetFormat();
    config->usage = sbuffer->GetUsage();
}

OH_NativeBuffer* OH_NativeBufferFromNativeWindowBuffer(OHNativeWindowBuffer* nativeWindowBuffer)
{
    if (nativeWindowBuffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    OH_NativeBuffer* buffer = OH_NativeBufferFromSurfaceBuffer(nativeWindowBuffer->sfbuffer);
    return buffer;
}
