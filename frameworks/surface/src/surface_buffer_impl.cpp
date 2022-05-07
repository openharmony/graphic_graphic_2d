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
#include <sys/mman.h>

#include "buffer_log.h"
#include "buffer_manager.h"
#include "buffer_extra_data_impl.h"

namespace OHOS {
namespace {
GSError GenerateError(GSError err, DispErrCode code)
{
    switch (code) {
        case DISPLAY_SUCCESS: return static_cast<GSError>(err + 0);
        case DISPLAY_FAILURE: return static_cast<GSError>(err + LOWERROR_FAILURE);
        case DISPLAY_FD_ERR: return static_cast<GSError>(err + EBADF);
        case DISPLAY_PARAM_ERR: return static_cast<GSError>(err + EINVAL);
        case DISPLAY_NULL_PTR: return static_cast<GSError>(err + EINVAL);
        case DISPLAY_NOT_SUPPORT: return static_cast<GSError>(err + EOPNOTSUPP);
        case DISPLAY_NOMEM: return static_cast<GSError>(err + ENOMEM);
        case DISPLAY_SYS_BUSY: return static_cast<GSError>(err + EBUSY);
        case DISPLAY_NOT_PERM: return static_cast<GSError>(err + EPERM);
        default: break;
    }
    return static_cast<GSError>(err + LOWERROR_INVALID);
}

inline GSError GenerateError(GSError err, int32_t code)
{
    return GenerateError(err, static_cast<DispErrCode>(code));
}
}

SurfaceBufferImpl::IDisplayGrallocSptr SurfaceBufferImpl::displayGralloc_ = nullptr;
SurfaceBufferImpl::IDisplayGrallocSptr SurfaceBufferImpl::GetDisplayGralloc()
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (displayGralloc_ != nullptr) {
        BLOGD("IDisplayGralloc has been initialized successfully.");
        return displayGralloc_;
    }

    displayGralloc_.reset(::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get());
    if (displayGralloc_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return nullptr;
    }
    return displayGralloc_;
}

SurfaceBufferImpl::SurfaceBufferImpl()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static int sequence_number_ = 0;
        sequenceNumber_ = sequence_number_++;

        mutex.unlock();
    }
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor +[%{public}d]", sequenceNumber_);
}

SurfaceBufferImpl::SurfaceBufferImpl(int seqNum)
{
    sequenceNumber_ = seqNum;
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor =[%{public}d]", sequenceNumber_);
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGD("dtor ~[%{public}d] handle_ %{public}p", sequenceNumber_, handle_);
    FreeBufferHandleLocked();
}

SurfaceBufferImpl *SurfaceBufferImpl::FromBase(const sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
}

GSError SurfaceBufferImpl::Alloc(const BufferRequestConfig &config)
{
    if (GetDisplayGralloc() == nullptr) {
        BLOGE("GetDisplayGralloc failed!");
        return GSERROR_INTERNEL;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ != nullptr) {
            FreeBufferHandleLocked();
        }
    }

    BufferHandle *handle = nullptr;
    AllocInfo info = {config.width, config.height, config.usage, (PixelFormat)config.format};
    auto dret = displayGralloc_->AllocMem(info, handle);
    if (dret == DISPLAY_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceBufferColorGamut_ = config.colorGamut;
        transform_ = config.transform;
        surfaceBufferWidth_ = config.width;
        surfaceBufferHeight_ = config.height;
        handle_ = handle;
        BLOGI("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle_,
            handle_->width, handle_->height, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::Map()
{
    if (GetDisplayGralloc() == nullptr) {
        BLOGE("GetDisplayGralloc failed!");
        return GSERROR_INTERNEL;
    }

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }
    void *virAddr = displayGralloc_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}
GSError SurfaceBufferImpl::Unmap()
{
    if (GetDisplayGralloc() == nullptr) {
        BLOGE("GetDisplayGralloc failed!");
        return GSERROR_INTERNEL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }

    auto dret = displayGralloc_->Unmap(*handle);
    if (dret == DISPLAY_SUCCESS) {
        handle_->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::FlushCache()
{
    if (GetDisplayGralloc() == nullptr) {
        BLOGE("GetDisplayGralloc failed!");
        return GSERROR_INTERNEL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }

    if (handle->virAddr == nullptr) {
        return GSERROR_API_FAILED;
    }

    auto dret = displayGralloc_->FlushCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::InvalidateCache()
{
    if (GetDisplayGralloc() == nullptr) {
        BLOGE("GetDisplayGralloc failed!");
        return GSERROR_INTERNEL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        }
        handle = handle_;
    }

    auto dret = displayGralloc_->InvalidateCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

void SurfaceBufferImpl::FreeBufferHandleLocked()
{
    if (handle_) {
        if (handle_->virAddr != nullptr) {
            displayGralloc_->Unmap(*handle_);
        }
        FreeBufferHandle(handle_);
    }
    handle_ = nullptr;
}

BufferHandle *SurfaceBufferImpl::GetBufferHandle() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return handle_;
}

void SurfaceBufferImpl::SetSurfaceBufferColorGamut(const ColorGamut& colorGamut)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferColorGamut_ = colorGamut;
}

const ColorGamut& SurfaceBufferImpl::GetSurfaceBufferColorGamut() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferColorGamut_;
}

void SurfaceBufferImpl::SetSurfaceBufferTransform(const TransformType& transform)
{
    std::lock_guard<std::mutex> lock(mutex_);
    transform_ = transform;
}

const TransformType& SurfaceBufferImpl::GetSurfaceBufferTransform() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return transform_;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferWidth_;
}

int32_t SurfaceBufferImpl::GetSurfaceBufferHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferHeight_;
}

void SurfaceBufferImpl::SetSurfaceBufferWidth(int32_t width)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferWidth_ = width;
}

void SurfaceBufferImpl::SetSurfaceBufferHeight(int32_t height)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferHeight_ = height;
}

int32_t SurfaceBufferImpl::GetWidth() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->width;
}

int32_t SurfaceBufferImpl::GetHeight() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->height;
}

int32_t SurfaceBufferImpl::GetStride() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->stride;
}

int32_t SurfaceBufferImpl::GetFormat() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->format;
}

int64_t SurfaceBufferImpl::GetUsage() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->usage;
}

uint64_t SurfaceBufferImpl::GetPhyAddr() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return 0;
    }
    return handle_->phyAddr;
}

int32_t SurfaceBufferImpl::GetKey() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->key;
}

void *SurfaceBufferImpl::GetVirAddr() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return nullptr;
    }
    return handle_->virAddr;
}

int32_t SurfaceBufferImpl::GetFileDescriptor() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("handle is nullptr");
        return -1;
    }
    return handle_->fd;
}

uint32_t SurfaceBufferImpl::GetSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGW("handle is nullptr");
        return 0;
    }
    return handle_->size;
}

void SurfaceBufferImpl::SetExtraData(const sptr<BufferExtraData> &bedata)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bedata_ = bedata;
}

const sptr<BufferExtraData>& SurfaceBufferImpl::GetExtraData() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return bedata_;
}

void SurfaceBufferImpl::SetBufferHandle(BufferHandle *handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    handle_ = handle;
}

GSError SurfaceBufferImpl::WriteToMessageParcel(MessageParcel &parcel)
{
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("Failure, Reason: handle_ is nullptr");
            return GSERROR_NOT_INIT;
        }
        handle = handle_;
    }

    bool ret = WriteBufferHandle(parcel, *handle);
    if (ret == false) {
        BLOGE("Failure, Reason: WriteBufferHandle return false");
        return GSERROR_API_FAILED;
    }

    return GSERROR_OK;
}

GSError SurfaceBufferImpl::ReadFromMessageParcel(MessageParcel &parcel)
{
    std::lock_guard<std::mutex> lock(mutex_);
    FreeBufferHandleLocked();
    handle_ = ReadBufferHandle(parcel);
    if (handle_ == nullptr) {
        return GSERROR_API_FAILED;
    }

    return GSERROR_OK;
}

int32_t SurfaceBufferImpl::GetSeqNum() const
{
    return sequenceNumber_;
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
