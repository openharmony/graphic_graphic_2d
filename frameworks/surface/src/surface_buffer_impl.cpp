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
#include "buffer_manager.h"

namespace OHOS {
SurfaceBufferImpl::SurfaceBufferImpl()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static int sequence_number_ = 0;
        sequenceNumber = sequence_number_++;

        mutex.unlock();
    }
    handle_ = nullptr;
    eglData_ = nullptr;
    BLOGD("ctor +[%{public}d]", sequenceNumber);
}

SurfaceBufferImpl::SurfaceBufferImpl(int seqNum)
{
    sequenceNumber = seqNum;
    handle_ = nullptr;
    BLOGD("ctor =[%{public}d]", sequenceNumber);
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGD("dtor ~[%{public}d] handle_ %{public}p", sequenceNumber, handle_);
    if (handle_) {
        if (handle_->virAddr != nullptr) {
            BLOGD("dtor ~[%{public}d] virAddr %{public}p", sequenceNumber, handle_->virAddr);
            BufferManager::GetInstance()->Unmap(handle_);
        }
        FreeBufferHandle(handle_);
    }
    eglData_ = nullptr;
}

SurfaceBufferImpl *SurfaceBufferImpl::FromBase(const sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
}

BufferHandle *SurfaceBufferImpl::GetBufferHandle() const
{
    return handle_;
}

GSError SurfaceBufferImpl::SetSurfaceBufferWidth(int32_t width)
{
    surfaceBufferWidth_ = width;
    return GSERROR_OK;
}

GSError SurfaceBufferImpl::SetSurfaceBufferHeight(int32_t height)
{
    surfaceBufferHeight_ = height;
    return GSERROR_OK;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferHeight() const
{
    return surfaceBufferHeight_;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferWidth() const
{
    return surfaceBufferWidth_;
}

GSError SurfaceBufferImpl::SetSurfaceBufferColorGamut(ColorGamut colorGamut)
{
    surfaceBufferColorGamut_ = colorGamut;
    return GSERROR_OK;
}

ColorGamut SurfaceBufferImpl::GetSurfaceBufferColorGamut() const
{
    return surfaceBufferColorGamut_;
}

GSError SurfaceBufferImpl::SetSurfaceBufferTransform(TransformType transform)
{
    transform_ = transform;
    return GSERROR_OK;
}

TransformType SurfaceBufferImpl::GetSurfaceBufferTransform() const
{
    return transform_;
}

int32_t SurfaceBufferImpl::GetWidth() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->width;
}

int32_t SurfaceBufferImpl::GetHeight() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->height;
}

int32_t SurfaceBufferImpl::GetStride() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->stride;
}

int32_t SurfaceBufferImpl::GetFormat() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->format;
}

int64_t SurfaceBufferImpl::GetUsage() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->usage;
}

uint64_t SurfaceBufferImpl::GetPhyAddr() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->phyAddr;
}

int32_t SurfaceBufferImpl::GetKey() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->key;
}

void *SurfaceBufferImpl::GetVirAddr() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return nullptr;
    }
    return handle_->virAddr;
}

int32_t SurfaceBufferImpl::GetFileDescriptor() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return -1;
    }
    return handle_->fd;
}

uint32_t SurfaceBufferImpl::GetSize() const
{
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->size;
}

GSError SurfaceBufferImpl::SetInt32(uint32_t key, int32_t val)
{
    ExtraData data = {
        .value = val,
        .type = EXTRA_DATA_TYPE_INT32,
    };
    return SetData(key, data);
}

GSError SurfaceBufferImpl::GetInt32(uint32_t key, int32_t &val)
{
    ExtraData data;
    GSError ret = GetData(key, data);
    if (ret == GSERROR_OK) {
        if (data.type == EXTRA_DATA_TYPE_INT32) {
            auto pVal = std::any_cast<int32_t>(&data.value);
            if (pVal != nullptr) {
                val = *pVal;
            } else {
                BLOGE("unexpected: INT32, any_cast failed");
            }
        } else {
            return GSERROR_TYPE_ERROR;
        }
    }
    return ret;
}

GSError SurfaceBufferImpl::SetInt64(uint32_t key, int64_t val)
{
    ExtraData data = {
        .value = val,
        .type = EXTRA_DATA_TYPE_INT64,
    };
    return SetData(key, data);
}

GSError SurfaceBufferImpl::GetInt64(uint32_t key, int64_t &val)
{
    ExtraData data;
    GSError ret = GetData(key, data);
    if (ret == GSERROR_OK) {
        if (data.type == EXTRA_DATA_TYPE_INT64) {
            auto pVal = std::any_cast<int64_t>(&data.value);
            if (pVal != nullptr) {
                val = *pVal;
            } else {
                BLOGE("unexpected: INT64, any_cast failed");
            }
        } else {
            return GSERROR_TYPE_ERROR;
        }
    }
    return ret;
}

GSError SurfaceBufferImpl::SetData(uint32_t key, ExtraData data)
{
    if (data.type <= EXTRA_DATA_TYPE_MIN || data.type >= EXTRA_DATA_TYPE_MAX) {
        BLOGW("Invalid, data.type is out of range");
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (extraDatas_.size() > SURFACE_MAX_USER_DATA_COUNT) {
        BLOGW("SurfaceBuffer has too many extra data, cannot save one more!!!");
        return GSERROR_OUT_OF_RANGE;
    }

    extraDatas_[key] = data;
    return GSERROR_OK;
}

GSError SurfaceBufferImpl::GetData(uint32_t key, ExtraData &data)
{
    auto it = extraDatas_.find(key);
    if (it == extraDatas_.end()) {
        return GSERROR_NO_ENTRY;
    }

    data = it->second;
    return GSERROR_OK;
}

void SurfaceBufferImpl::SetExtraData(const BufferExtraData &bedata)
{
    auto bedatai = static_cast<const BufferExtraDataImpl*>(&bedata);
    bedataimpl = *bedatai;
}

void SurfaceBufferImpl::GetExtraData(BufferExtraData &bedata) const
{
    auto bedatai = static_cast<BufferExtraDataImpl*>(&bedata);
    *bedatai = bedataimpl;
}

GSError SurfaceBufferImpl::ExtraGet(std::string key, int32_t &value) const
{
    return bedataimpl.ExtraGet(key, value);
}

GSError SurfaceBufferImpl::ExtraGet(std::string key, int64_t &value) const
{
    return bedataimpl.ExtraGet(key, value);
}

GSError SurfaceBufferImpl::ExtraGet(std::string key, double &value) const
{
    return bedataimpl.ExtraGet(key, value);
}

GSError SurfaceBufferImpl::ExtraGet(std::string key, std::string &value) const
{
    return bedataimpl.ExtraGet(key, value);
}

GSError SurfaceBufferImpl::ExtraSet(std::string key, int32_t value)
{
    return bedataimpl.ExtraSet(key, value);
}

GSError SurfaceBufferImpl::ExtraSet(std::string key, int64_t value)
{
    return bedataimpl.ExtraSet(key, value);
}

GSError SurfaceBufferImpl::ExtraSet(std::string key, double value)
{
    return bedataimpl.ExtraSet(key, value);
}

GSError SurfaceBufferImpl::ExtraSet(std::string key, std::string value)
{
    return bedataimpl.ExtraSet(key, value);
}

void SurfaceBufferImpl::SetBufferHandle(BufferHandle *handle)
{
    handle_ = handle;
}

void SurfaceBufferImpl::WriteToMessageParcel(MessageParcel &parcel)
{
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return;
    }

    bool ret = WriteBufferHandle(parcel, *handle_);
    if (ret == false) {
        BLOGE("Failure, Reason: WriteBufferHandle return false");
    }

    parcel.WriteInt32(extraDatas_.size());
    for (const auto &[k, v] : extraDatas_) {
        parcel.WriteUint32(k);
        parcel.WriteInt32(v.type);
        if (v.type == EXTRA_DATA_TYPE_INT32) {
            auto pVal = std::any_cast<int32_t>(&v.value);
            if (pVal != nullptr) {
                parcel.WriteInt32(*pVal);
            } else {
                BLOGE("unexpected: INT32, any_cast failed");
            }
        }
        if (v.type == EXTRA_DATA_TYPE_INT64) {
            auto pVal = std::any_cast<int64_t>(&v.value);
            if (pVal != nullptr) {
                parcel.WriteInt64(*pVal);
            } else {
                BLOGE("unexpected: INT64, any_cast failed");
            }
        }
    }
}

int32_t SurfaceBufferImpl::GetSeqNum()
{
    return sequenceNumber;
}

sptr<EglData> SurfaceBufferImpl::GetEglData() const
{
    return eglData_;
}

void SurfaceBufferImpl::SetEglData(const sptr<EglData>& data)
{
    eglData_ = data;
}
} // namespace OHOS
