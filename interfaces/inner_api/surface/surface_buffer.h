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

#ifndef INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H
#define INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H

#include <functional>

#include <memory>
#include <refbase.h>

#include "buffer_handle_utils.h"
#include "surface_type.h"
#include "egl_data.h"
#include "buffer_extra_data.h"

namespace OHOS {
class MessageParcel;
class SurfaceBuffer : public RefBase {
public:
    virtual BufferHandle *GetBufferHandle() const = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual int32_t GetStride() const = 0;
    virtual int32_t GetFormat() const = 0;
    virtual int64_t GetUsage() const = 0;
    virtual uint64_t GetPhyAddr() const = 0;
    virtual int32_t GetKey() const = 0;
    virtual void *GetVirAddr() const = 0;
    virtual int32_t GetFileDescriptor() const = 0;
    virtual uint32_t GetSize() const = 0;

    virtual const ColorGamut& GetSurfaceBufferColorGamut() const = 0;
    virtual const TransformType& GetSurfaceBufferTransform() const = 0;
    virtual void SetSurfaceBufferColorGamut(const ColorGamut& colorGamut) = 0;
    virtual void SetSurfaceBufferTransform(const TransformType& transform) = 0;

    virtual int32_t GetSurfaceBufferWidth() const = 0;
    virtual int32_t GetSurfaceBufferHeight() const = 0;
    virtual void SetSurfaceBufferWidth(int32_t width) = 0;
    virtual void SetSurfaceBufferHeight(int32_t width) = 0;

    virtual int32_t GetSeqNum() const = 0;

    // opt EglData
    virtual sptr<EglData> GetEglData() const = 0;
    virtual void SetEglData(const sptr<EglData>& data) = 0;

    virtual void SetExtraData(const sptr<BufferExtraData> &bedata) = 0;
    virtual const sptr<BufferExtraData>& GetExtraData() const = 0;
    virtual GSError WriteToMessageParcel(MessageParcel &parcel) = 0;
    virtual GSError ReadFromMessageParcel(MessageParcel &parcel) = 0;
    virtual void SetBufferHandle(BufferHandle *handle) = 0;

    // gralloc
    virtual GSError Alloc(const BufferRequestConfig &config) = 0;
    virtual GSError Map() = 0;
    virtual GSError Unmap() = 0;
    virtual GSError FlushCache() = 0;
    virtual GSError InvalidateCache() = 0;
protected:
    SurfaceBuffer(){}
    SurfaceBuffer(const SurfaceBuffer&) = delete;
    SurfaceBuffer& operator=(const SurfaceBuffer&) = delete;
    virtual ~SurfaceBuffer(){}
};

using OnReleaseFunc = std::function<GSError(sptr<SurfaceBuffer> &)>;
using OnDeleteBufferFunc = std::function<void(int32_t)>;
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H
