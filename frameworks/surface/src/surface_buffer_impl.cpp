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
#include "native_buffer.h"

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

static const std::map<uint64_t, uint64_t> bufferUsageConvertMap = {
    {BUFFER_USAGE_CPU_READ,                    HBM_USE_CPU_READ},
    {BUFFER_USAGE_CPU_WRITE,                   HBM_USE_CPU_WRITE},
    {BUFFER_USAGE_MEM_MMZ,                     HBM_USE_MEM_MMZ},
    {BUFFER_USAGE_MEM_DMA,                     HBM_USE_MEM_DMA},
    {BUFFER_USAGE_MEM_SHARE,                   HBM_USE_MEM_SHARE},
    {BUFFER_USAGE_MEM_MMZ_CACHE,               HBM_USE_MEM_MMZ_CACHE},
    {BUFFER_USAGE_MEM_FB,                      HBM_USE_MEM_FB},
    {BUFFER_USAGE_ASSIGN_SIZE,                 HBM_USE_ASSIGN_SIZE},
    {BUFFER_USAGE_HW_RENDER,                   HBM_USE_HW_RENDER},
    {BUFFER_USAGE_HW_TEXTURE,                  HBM_USE_HW_TEXTURE},
    {BUFFER_USAGE_HW_COMPOSER,                 HBM_USE_HW_COMPOSER},
    {BUFFER_USAGE_PROTECTED,                   HBM_USE_PROTECTED},
    {BUFFER_USAGE_CAMERA_READ,                 HBM_USE_CAMERA_READ},
    {BUFFER_USAGE_CAMERA_WRITE,                HBM_USE_CAMERA_WRITE},
    {BUFFER_USAGE_VIDEO_ENCODER,               HBM_USE_VIDEO_ENCODER},
    {BUFFER_USAGE_VIDEO_DECODER,               HBM_USE_VIDEO_DECODER},
    {BUFFER_USAGE_VENDOR_PRI0,                 HBM_USE_VENDOR_PRI0},
    {BUFFER_USAGE_VENDOR_PRI1,                 HBM_USE_VENDOR_PRI1},
    {BUFFER_USAGE_VENDOR_PRI2,                 HBM_USE_VENDOR_PRI2},
    {BUFFER_USAGE_VENDOR_PRI3,                 HBM_USE_VENDOR_PRI3},
    {BUFFER_USAGE_VENDOR_PRI4,                 HBM_USE_VENDOR_PRI4},
    {BUFFER_USAGE_VENDOR_PRI5,                 HBM_USE_VENDOR_PRI5},
    {BUFFER_USAGE_VENDOR_PRI6,                 HBM_USE_VENDOR_PRI6},
    {BUFFER_USAGE_VENDOR_PRI7,                 HBM_USE_VENDOR_PRI7},
    {BUFFER_USAGE_VENDOR_PRI8,                 HBM_USE_VENDOR_PRI8},
    {BUFFER_USAGE_VENDOR_PRI9,                 HBM_USE_VENDOR_PRI9},
    {BUFFER_USAGE_VENDOR_PRI10,                HBM_USE_VENDOR_PRI10},
    {BUFFER_USAGE_VENDOR_PRI11,                HBM_USE_VENDOR_PRI11},
    {BUFFER_USAGE_VENDOR_PRI12,                HBM_USE_VENDOR_PRI12},
    {BUFFER_USAGE_VENDOR_PRI13,                HBM_USE_VENDOR_PRI13},
    {BUFFER_USAGE_VENDOR_PRI14,                HBM_USE_VENDOR_PRI14},
    {BUFFER_USAGE_VENDOR_PRI15,                HBM_USE_VENDOR_PRI15},
    {BUFFER_USAGE_VENDOR_PRI16,                HBM_USE_VENDOR_PRI16},
    {BUFFER_USAGE_VENDOR_PRI17,                HBM_USE_VENDOR_PRI17},
    {BUFFER_USAGE_VENDOR_PRI18,                HBM_USE_VENDOR_PRI18},
    {BUFFER_USAGE_VENDOR_PRI19,                HBM_USE_VENDOR_PRI19},
};

SurfaceBufferImpl::IDisplayGrallocSptr SurfaceBufferImpl::displayGralloc_ = nullptr;
std::mutex SurfaceBufferImpl::displayGrallocMutex_;

void SurfaceBufferImpl::DisplayGrallocDeathCallback(void* data)
{
    std::lock_guard<std::mutex> lock(displayGrallocMutex_);
    displayGralloc_ = nullptr;
    BLOGD("gralloc_host died and displayGralloc_ is nullptr.");
}

SurfaceBufferImpl::IDisplayGrallocSptr SurfaceBufferImpl::GetDisplayGralloc()
{
    std::lock_guard<std::mutex> lock(displayGrallocMutex_);
    if (displayGralloc_ != nullptr) {
        return displayGralloc_;
    }

    displayGralloc_.reset(::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get());
    if (displayGralloc_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return nullptr;
    }
    displayGralloc_->RegAllocatorDeathCallback(&SurfaceBufferImpl::DisplayGrallocDeathCallback, nullptr);
    return displayGralloc_;
}

SurfaceBufferImpl::SurfaceBufferImpl()
{
    {
        static std::mutex mutex;
        mutex.lock();

        static uint32_t sequence_number_ = 0;
        sequenceNumber_ = sequence_number_++;

        mutex.unlock();
    }
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor +[%{public}u]", sequenceNumber_);
}

SurfaceBufferImpl::SurfaceBufferImpl(uint32_t seqNum)
{
    sequenceNumber_ = seqNum;
    bedata_ = new BufferExtraDataImpl;
    BLOGD("ctor =[%{public}u]", sequenceNumber_);
}

SurfaceBufferImpl::~SurfaceBufferImpl()
{
    BLOGD("dtor ~[%{public}u] handle_ %{public}p", sequenceNumber_, handle_);
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
        return GSERROR_INTERNAL;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ != nullptr) {
            FreeBufferHandleLocked();
        }
    }
    
    GSError ret = CheckBufferConfig(config.width, config.height, config.format, config.usage);
    if (ret != GSERROR_OK) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    BufferHandle *handle = nullptr;
    uint64_t usage = BufferUsageToGrallocUsage(config.usage);
    AllocInfo info = {config.width, config.height, usage, (PixelFormat)config.format};
    auto dret = displayGralloc_->AllocMem(info, handle);
    if (dret == DISPLAY_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceBufferColorGamut_ = config.colorGamut;
        transform_ = config.transform;
        surfaceBufferWidth_ = config.width;
        surfaceBufferHeight_ = config.height;
        handle_ = handle;
        BLOGD("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle_,
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
        return GSERROR_INTERNAL;
    }

    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        } else if (handle_->virAddr != nullptr) {
            BLOGD("handle_->virAddr has been maped");
            return GSERROR_OK;
        }
        handle = handle_;
    }
#ifdef RS_ENABLE_AFBC
    handle->usage |= (BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA);
#endif
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
        return GSERROR_INTERNAL;
    }
    BufferHandle *handle = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (handle_ == nullptr) {
            BLOGE("handle is nullptr");
            return GSERROR_INVALID_OPERATING;
        } else if (handle_->virAddr == nullptr) {
            BLOGW("handle has been unmaped");
            return GSERROR_OK;
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
        return GSERROR_INTERNAL;
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
        return GSERROR_INTERNAL;
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
        if (handle_->virAddr != nullptr && displayGralloc_ != nullptr) {
            displayGralloc_->Unmap(*handle_);
            handle_->virAddr = nullptr;
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

void SurfaceBufferImpl::SetSurfaceBufferColorGamut(const GraphicColorGamut& colorGamut)
{
    std::lock_guard<std::mutex> lock(mutex_);
    surfaceBufferColorGamut_ = colorGamut;
}

const GraphicColorGamut& SurfaceBufferImpl::GetSurfaceBufferColorGamut() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferColorGamut_;
}

void SurfaceBufferImpl::SetSurfaceBufferTransform(const GraphicTransformType& transform)
{
    std::lock_guard<std::mutex> lock(mutex_);
    transform_ = transform;
}

const GraphicTransformType& SurfaceBufferImpl::GetSurfaceBufferTransform() const
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

uint64_t SurfaceBufferImpl::GetUsage() const
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

void *SurfaceBufferImpl::GetVirAddr()
{
    GSError ret = this->Map();
    if (ret != GSERROR_OK) {
        BLOGW("Map failed");
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

OH_NativeBuffer* SurfaceBufferImpl::SurfaceBufferToNativeBuffer()
{
    return reinterpret_cast<OH_NativeBuffer *>(this);
}

uint32_t SurfaceBufferImpl::GetSeqNum() const
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

GSError SurfaceBufferImpl::CheckBufferConfig(int32_t width, int32_t height,
                                             int32_t format, uint64_t usage)
{
    if (width <= 0 || height <= 0) {
        BLOGE("width or height is greater than 0, now is w %{public}d h %{public}d", width, height);
        return GSERROR_INVALID_ARGUMENTS;
    }

    if (format < 0 || format > GRAPHIC_PIXEL_FMT_BUTT) {
        BLOGE("format [0, %{public}d], now is %{public}d", GRAPHIC_PIXEL_FMT_BUTT, format);
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

uint64_t SurfaceBufferImpl::BufferUsageToGrallocUsage(uint64_t bufferUsage)
{
    uint64_t grallocUsage = 0;
    for (auto iter = bufferUsageConvertMap.begin(); iter != bufferUsageConvertMap.end(); iter++) {
        if (bufferUsage & iter->first) {
            grallocUsage |= iter->second;
        }
    }
    return grallocUsage;
}

BufferWrapper SurfaceBufferImpl::GetBufferWrapper()
{
    BufferWrapper wrapper;
    return wrapper;
}

void SurfaceBufferImpl::SetBufferWrapper(BufferWrapper wrapper) {}
} // namespace OHOS
