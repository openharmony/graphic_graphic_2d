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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H

#include <map>

#include <surface_buffer.h>
#include <buffer_handle_parcel.h>

namespace OHOS {
enum ExtraDataType {
    EXTRA_DATA_TYPE_MIN,
    EXTRA_DATA_TYPE_INT32,
    EXTRA_DATA_TYPE_INT64,
    EXTRA_DATA_TYPE_MAX,
};

typedef struct {
    BufferHandle* handle_;
    int32_t sequenceNumber;
} SurfaceBufferData;

typedef struct {
    void* value;
    uint32_t size;
    ExtraDataType type;
} ExtraData;

class MessageParcel;
class SurfaceBufferImpl : public SurfaceBuffer {
public:
    SurfaceBufferImpl();
    SurfaceBufferImpl(int seqNum);
    virtual ~SurfaceBufferImpl();

    static SurfaceBufferImpl* FromBase(sptr<SurfaceBuffer>& buffer);

    BufferHandle* GetBufferHandle() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    int32_t GetFormat() const override;
    int64_t GetUsage() const override;
    uint64_t GetPhyAddr() const override;
    int32_t GetKey() const override;
    void* GetVirAddr() const override;
    int32_t GetFileDescriptor() const override;
    uint32_t GetSize() const override;

    int32_t GetSeqNum();
    SurfaceError SetInt32(uint32_t key, int32_t val) override;
    SurfaceError GetInt32(uint32_t key, int32_t& val) override;
    SurfaceError SetInt64(uint32_t key, int64_t val) override;
    SurfaceError GetInt64(uint32_t key, int64_t& val) override;

    void SetBufferHandle(BufferHandle* handle);
    BufferHandle* GetBufferHandle();

    void WriteToMessageParcel(MessageParcel& parcel);

    SurfaceBufferData bufferData_;

private:
    SurfaceError SetData(uint32_t key, ExtraData data);
    SurfaceError GetData(uint32_t key, ExtraData& data);
    std::map<uint32_t, ExtraData> extraDatas_;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
