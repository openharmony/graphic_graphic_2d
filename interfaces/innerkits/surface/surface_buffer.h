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
    virtual int32_t GetSurfaceBufferWidth() const = 0;
    virtual int32_t GetSurfaceBufferHeight() const = 0;
    virtual ColorGamut GetSurfaceBufferColorGamut() const = 0;
    virtual TransformType GetSurfaceBufferTransform() const = 0;
    virtual GSError SetSurfaceBufferWidth(int32_t width) = 0;
    virtual GSError SetSurfaceBufferHeight(int32_t height) = 0;
    virtual GSError SetSurfaceBufferColorGamut(ColorGamut colorGamut) = 0;
    virtual GSError SetSurfaceBufferTransform(TransformType transform) = 0;
    virtual int32_t GetFormat() const = 0;
    virtual int64_t GetUsage() const = 0;
    virtual uint64_t GetPhyAddr() const = 0;
    virtual int32_t GetKey() const = 0;
    virtual void *GetVirAddr() const = 0;
    virtual int GetFileDescriptor() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual GSError SetInt32(uint32_t key, int32_t value) = 0;
    virtual GSError GetInt32(uint32_t key, int32_t &value) = 0;
    virtual GSError SetInt64(uint32_t key, int64_t value) = 0;
    virtual GSError GetInt64(uint32_t key, int64_t &value) = 0;
    virtual sptr<EglData> GetEglData() const = 0;
    virtual void SetEglData(const sptr<EglData>& data) = 0;

    virtual void SetExtraData(const sptr<BufferExtraData> &bedata) = 0;
    virtual void GetExtraData(sptr<BufferExtraData> &bedata) const = 0;
    virtual void WriteToMessageParcel(MessageParcel &parcel) = 0;
    virtual void SetBufferHandle(BufferHandle *handle) = 0;

    // support ipc data
    virtual GSError ExtraGet(std::string key, int32_t &value) const = 0;
    virtual GSError ExtraGet(std::string key, int64_t &value) const = 0;
    virtual GSError ExtraGet(std::string key, double &value) const = 0;
    virtual GSError ExtraGet(std::string key, std::string &value) const = 0;
    virtual GSError ExtraSet(std::string key, int32_t value) = 0;
    virtual GSError ExtraSet(std::string key, int64_t value) = 0;
    virtual GSError ExtraSet(std::string key, double value) = 0;
    virtual GSError ExtraSet(std::string key, std::string value) = 0;

    virtual int32_t GetSeqNum() const = 0;

protected:
    SurfaceBuffer(){}
    virtual ~SurfaceBuffer(){}
};

using OnReleaseFunc = std::function<GSError(sptr<SurfaceBuffer> &)>;
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_SURFACE_BUFFER_H
