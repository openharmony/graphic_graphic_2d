/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H

#include <buffer_extra_data.h>
#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <surface_buffer.h>
#include "egl_data.h"
#include "stdint.h"

struct BufferWrapper {};

namespace OHOS {
class SurfaceBufferImpl : public SurfaceBuffer {
public:
    static void DisplayBufferDeathCallback(void* data);

    SurfaceBufferImpl();
    SurfaceBufferImpl(uint32_t seqNum);
    virtual ~SurfaceBufferImpl();

    static SurfaceBufferImpl *FromBase(const sptr<SurfaceBuffer>& buffer);

    GSError Alloc(const BufferRequestConfig &config) override;
    GSError Map() override;
    GSError Unmap() override;
    GSError FlushCache() override;
    GSError InvalidateCache() override;

    BufferHandle *GetBufferHandle() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    int32_t GetStride() const override;
    int32_t GetFormat() const override;
    uint64_t GetUsage() const override;
    uint64_t GetPhyAddr() const override;
    void *GetVirAddr() override;
    int32_t GetFileDescriptor() const override;
    uint32_t GetSize() const override;

    const GraphicColorGamut& GetSurfaceBufferColorGamut() const override;
    const GraphicTransformType& GetSurfaceBufferTransform() const override;
    void SetSurfaceBufferColorGamut(const GraphicColorGamut& colorGamut) override;
    void SetSurfaceBufferTransform(const GraphicTransformType& transform) override;

    int32_t GetSurfaceBufferWidth() const override;
    int32_t GetSurfaceBufferHeight() const override;
    void SetSurfaceBufferWidth(int32_t width) override;
    void SetSurfaceBufferHeight(int32_t width) override;

    uint32_t GetSeqNum() const override;

    sptr<EglData> GetEglData() const override;
    void SetEglData(const sptr<EglData>& data) override;

    void SetExtraData(const sptr<BufferExtraData> &bedata) override;
    const sptr<BufferExtraData>& GetExtraData() const override;

    void SetBufferHandle(BufferHandle *handle) override;
    GSError WriteToMessageParcel(MessageParcel &parcel) override;
    GSError ReadFromMessageParcel(MessageParcel &parcel) override;

    OH_NativeBuffer* SurfaceBufferToNativeBuffer() override;

    static GSError CheckBufferConfig(int32_t width, int32_t height,
                                     int32_t format, uint64_t usage);

    BufferWrapper GetBufferWrapper() override;
    void SetBufferWrapper(BufferWrapper wrapper) override;

    // metadata
    GSError SetMetadata(uint32_t key, const std::vector<uint8_t>& value) override;
    GSError GetMetadata(uint32_t key, std::vector<uint8_t>& value) override;
    GSError ListMetadataKeys(std::vector<uint32_t>& keys) override;
    GSError EraseMetadataKey(uint32_t key) override;

    GSError WriteBufferRequestConfig(MessageParcel &parcel) override;
    GSError ReadBufferRequestConfig(MessageParcel &parcel) override;
    const BufferRequestConfig* GetBufferRequestConfig() const override;
    void SetBufferRequestConfig(const BufferRequestConfig &config) override;

private:
    void FreeBufferHandleLocked();

    BufferHandle *handle_ = nullptr;
    uint32_t sequenceNumber_ = UINT32_MAX;
    sptr<BufferExtraData> bedata_ = nullptr;
    sptr<EglData> eglData_ = nullptr;
    GraphicColorGamut surfaceBufferColorGamut_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    int32_t surfaceBufferWidth_ = 0;
    int32_t surfaceBufferHeight_ = 0;
    mutable std::mutex mutex_;
    BufferRequestConfig bufferRequestConfig_;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
