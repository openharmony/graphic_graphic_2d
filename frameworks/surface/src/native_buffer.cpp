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
#include "surface_type.h"
#include "buffer_log.h"
#include "native_window.h"
#include "surface_buffer_impl.h"
#include "metadata_helper.h"

using namespace OHOS;
using namespace HDI::Display::Graphic::Common::V1_0;
static std::unordered_map<OH_NativeBuffer_ColorSpace, CM_ColorSpaceType> NATIVE_COLORSPACE_TO_HDI_MAP = {
    {OH_COLORSPACE_NONE, CM_COLORSPACE_NONE},
    {OH_COLORSPACE_BT601_EBU_FULL, CM_BT601_EBU_FULL},
    {OH_COLORSPACE_BT601_SMPTE_C_FULL, CM_BT601_SMPTE_C_FULL},
    {OH_COLORSPACE_BT709_FULL, CM_BT709_FULL},
    {OH_COLORSPACE_BT2020_HLG_FULL, CM_BT2020_HLG_FULL},
    {OH_COLORSPACE_BT2020_PQ_FULL, CM_BT2020_PQ_FULL},
    {OH_COLORSPACE_BT601_EBU_LIMIT, CM_BT601_EBU_LIMIT},
    {OH_COLORSPACE_BT601_SMPTE_C_LIMIT, CM_BT601_SMPTE_C_LIMIT},
    {OH_COLORSPACE_BT709_LIMIT, CM_BT709_LIMIT},
    {OH_COLORSPACE_BT2020_HLG_LIMIT, CM_BT2020_HLG_LIMIT},
    {OH_COLORSPACE_BT2020_PQ_LIMIT, CM_BT2020_PQ_LIMIT},
    {OH_COLORSPACE_SRGB_FULL, CM_SRGB_FULL},
    {OH_COLORSPACE_P3_FULL, CM_P3_FULL},
    {OH_COLORSPACE_P3_HLG_FULL, CM_P3_HLG_FULL},
    {OH_COLORSPACE_P3_PQ_FULL, CM_P3_PQ_FULL},
    {OH_COLORSPACE_ADOBERGB_FULL, CM_ADOBERGB_FULL},
    {OH_COLORSPACE_SRGB_LIMIT, CM_SRGB_LIMIT},
    {OH_COLORSPACE_P3_LIMIT, CM_P3_LIMIT},
    {OH_COLORSPACE_P3_HLG_LIMIT, CM_P3_HLG_LIMIT},
    {OH_COLORSPACE_P3_PQ_LIMIT, CM_P3_PQ_LIMIT},
    {OH_COLORSPACE_ADOBERGB_LIMIT, CM_ADOBERGB_LIMIT},
    {OH_COLORSPACE_LINEAR_SRGB, CM_LINEAR_SRGB},
    {OH_COLORSPACE_LINEAR_BT709, CM_LINEAR_BT709},
    {OH_COLORSPACE_LINEAR_P3, CM_LINEAR_P3},
    {OH_COLORSPACE_LINEAR_BT2020, CM_LINEAR_BT2020},
    {OH_COLORSPACE_DISPLAY_SRGB, CM_DISPLAY_SRGB},
    {OH_COLORSPACE_DISPLAY_P3_SRGB, CM_DISPLAY_P3_SRGB},
    {OH_COLORSPACE_DISPLAY_P3_HLG, CM_DISPLAY_P3_HLG},
    {OH_COLORSPACE_DISPLAY_P3_PQ, CM_DISPLAY_P3_PQ},
    {OH_COLORSPACE_DISPLAY_BT2020_SRGB, CM_DISPLAY_BT2020_SRGB},
    {OH_COLORSPACE_DISPLAY_BT2020_HLG, CM_DISPLAY_BT2020_HLG},
    {OH_COLORSPACE_DISPLAY_BT2020_PQ, CM_DISPLAY_BT2020_PQ}
};

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
    config->stride = sbuffer->GetStride();
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
    config->stride = sbuffer->GetStride();
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

int32_t OH_NativeBuffer_SetColorSpace(OH_NativeBuffer *buffer, OH_NativeBuffer_ColorSpace colorSpace)
{
    if (buffer == nullptr || NATIVE_COLORSPACE_TO_HDI_MAP.find(colorSpace) == NATIVE_COLORSPACE_TO_HDI_MAP.end()) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    sptr<SurfaceBuffer> sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    GSError ret = MetadataHelper::SetColorSpaceType(sbuffer, NATIVE_COLORSPACE_TO_HDI_MAP[colorSpace]);
    if (GSErrorStr(ret) == "<500 api call failed>with low error <Not supported>") {
        return OHOS::GSERROR_NOT_SUPPORT;
    }
    return ret;
}

int32_t OH_NativeBuffer_MapPlanes(OH_NativeBuffer *buffer, void **virAddr, OH_NativeBuffer_Planes *outPlanes)
{
    if (buffer == nullptr || virAddr == nullptr || outPlanes == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    sptr<SurfaceBuffer> sbuffer = OH_NativeBufferToSurfaceBuffer(buffer);
    int32_t ret = sbuffer->Map();
    if (ret == OHOS::GSERROR_OK) {
        *virAddr = sbuffer->GetVirAddr();
    } else {
        BLOGE("Surface Buffer Map failed, %{public}d", ret);
        return ret;
    }
    OH_NativeBuffer_Planes *planes = nullptr;
    GSError retVal = sbuffer->GetPlanesInfo(reinterpret_cast<void**>(&planes));
    if (retVal != OHOS::GSERROR_OK) {
        BLOGE("Get planesInfo failed, retVal:%d", retVal);
        return retVal;
    }
    outPlanes->planeCount = planes->planeCount;
    for (uint32_t i = 0; i < planes->planeCount && i < 4; i++) { // 4: max plane count
        outPlanes->planes[i].offset = planes->planes[i].offset;
        outPlanes->planes[i].rowStride = planes->planes[i].rowStride;
        outPlanes->planes[i].columnStride = planes->planes[i].columnStride;
    }
    return OHOS::GSERROR_OK;
}

int32_t OH_NativeBuffer_FromNativeWindowBuffer(OHNativeWindowBuffer *nativeWindowBuffer, OH_NativeBuffer **buffer)
{
    if (nativeWindowBuffer == nullptr || buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    *buffer = OH_NativeBufferFromSurfaceBuffer(nativeWindowBuffer->sfbuffer);
    if (*buffer == nullptr) {
        BLOGE("get sfbuffer is nullptr");
        return OHOS::GSERROR_INVALID_OPERATING;
    }
    return OHOS::GSERROR_OK;
}