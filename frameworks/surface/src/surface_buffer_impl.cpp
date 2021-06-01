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

#include "surface_buffer_impl.h"

#include <mutex>

#include <message_parcel.h>
#include <securec.h>

#include "buffer_log.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "SurfaceBufferImpl" };
}

SurfaceBufferImpl::SurfaceBufferImpl()
{
    BLOGFD("");
    {
        static std::mutex mutex;
        mutex.lock();

        static int sequence_number_ = 0;
        this->bufferData_.sequenceNumber = sequence_number_++;

        mutex.unlock();
    }
    this->bufferData_.handle_ = nullptr;
}

SurfaceBufferImpl::SurfaceBufferImpl(int seqNum)
{
    BLOGFD("");
    bufferData_.sequenceNumber = seqNum;
    bufferData_.handle_ = nullptr;
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGFD("");
    for (auto it = this->extraDatas_.begin(); it != this->extraDatas_.end(); it++) {
        if (it->second.value) {
            delete[] reinterpret_cast<uint8_t*>(it->second.value);
        }
    }

    if (this->bufferData_.handle_) {
        delete[] reinterpret_cast<uint8_t*>(this->bufferData_.handle_);
    }
}

SurfaceBufferImpl* SurfaceBufferImpl::FromBase(sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
}

BufferHandle* SurfaceBufferImpl::GetBufferHandle() const
{
    return bufferData_.handle_;
}

int32_t SurfaceBufferImpl::GetWidth() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->width;
}

int32_t SurfaceBufferImpl::GetHeight() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->height;
}

int32_t SurfaceBufferImpl::GetFormat() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->format;
}

int64_t SurfaceBufferImpl::GetUsage() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->usage;
}

uint64_t SurfaceBufferImpl::GetPhyAddr() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return 0;
    }
    return this->bufferData_.handle_->phyAddr;
}

int32_t SurfaceBufferImpl::GetKey() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->key;
}

void* SurfaceBufferImpl::GetVirAddr() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return nullptr;
    }
    return this->bufferData_.handle_->virAddr;
}

int32_t SurfaceBufferImpl::GetFileDescriptor() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return -1;
    }
    return this->bufferData_.handle_->fd;
}

uint32_t SurfaceBufferImpl::GetSize() const
{
    if (this->bufferData_.handle_ == nullptr) {
        BLOGFW("handle is nullptr");
        return 0;
    }
    return this->bufferData_.handle_->size;
}

SurfaceError SurfaceBufferImpl::SetInt32(uint32_t key, int32_t val)
{
    ExtraData int32 = {
        .value = &val,
        .size = sizeof(int32_t),
        .type = EXTRA_DATA_TYPE_INT32,
    };
    return SetData(key, int32);
}

SurfaceError SurfaceBufferImpl::GetInt32(uint32_t key, int32_t& val)
{
    ExtraData int32;
    SurfaceError ret = GetData(key, int32);
    if (ret == SURFACE_ERROR_OK) {
        if (int32.type == EXTRA_DATA_TYPE_INT32) {
            val = *reinterpret_cast<int32_t*>(int32.value);
        } else {
            return SURFACE_ERROR_TYPE_ERROR;
        }
    }
    return ret;
}

SurfaceError SurfaceBufferImpl::SetInt64(uint32_t key, int64_t val)
{
    ExtraData int64 = {
        .value = &val,
        .size = sizeof(int64_t),
        .type = EXTRA_DATA_TYPE_INT64,
    };
    return SetData(key, int64);
}

SurfaceError SurfaceBufferImpl::GetInt64(uint32_t key, int64_t& val)
{
    ExtraData int64;
    SurfaceError ret = GetData(key, int64);
    if (ret == SURFACE_ERROR_OK) {
        if (int64.type == EXTRA_DATA_TYPE_INT64) {
            val = *reinterpret_cast<int64_t*>(int64.value);
        } else {
            return SURFACE_ERROR_TYPE_ERROR;
        }
    }
    return ret;
}

SurfaceError SurfaceBufferImpl::SetData(uint32_t key, ExtraData data)
{
    if (data.type <= EXTRA_DATA_TYPE_MIN || data.type >= EXTRA_DATA_TYPE_MAX) {
        BLOG_INVALID("data.type is out of range");
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (data.size <= 0 || data.size > sizeof(int64_t)) {
        BLOG_INVALID("data.size is out of range");
        return SURFACE_ERROR_INVALID_PARAM;
    }

    if (this->extraDatas_.size() > SURFACE_MAX_USER_DATA_COUNT) {
        BLOGFW("SurfaceBuffer has too many extra data, cannot save one more!!!");
        return SURFACE_ERROR_OUT_OF_RANGE;
    }

    ExtraData mapData;
    SurfaceError ret = GetData(key, mapData);
    if (ret == SURFACE_ERROR_OK) {
        delete[] reinterpret_cast<uint8_t*>(mapData.value);
    }

    mapData = data;
    mapData.value = new uint8_t[mapData.size];
    errno_t reterr = memcpy_s(mapData.value, mapData.size, data.value, data.size);
    if (reterr) {
        BLOGFD("memcpy_s failed with %{public}d", reterr);
    }

    this->extraDatas_[key] = mapData;
    return SURFACE_ERROR_OK;
}

SurfaceError SurfaceBufferImpl::GetData(uint32_t key, ExtraData& data)
{
    auto it = this->extraDatas_.find(key);
    if (it == this->extraDatas_.end()) {
        return SURFACE_ERROR_NO_ENTRY;
    }

    data = it->second;
    return SURFACE_ERROR_OK;
}

void SurfaceBufferImpl::SetBufferHandle(BufferHandle* handle)
{
    bufferData_.handle_ = handle;
}

BufferHandle* SurfaceBufferImpl::GetBufferHandle()
{
    return bufferData_.handle_;
}

void SurfaceBufferImpl::WriteToMessageParcel(MessageParcel& parcel)
{
    if (bufferData_.handle_ == nullptr) {
        BLOG_FAILURE("bufferData_.handle_ is nullptr");
        return;
    }

    bool ret = WriteBufferHandle(parcel, *bufferData_.handle_);
    if (ret == false) {
        BLOG_FAILURE("WriteBufferHandle return false");
    }
}

int32_t SurfaceBufferImpl::GetSeqNum()
{
    return this->bufferData_.sequenceNumber;
}
} // namespace OHOS
