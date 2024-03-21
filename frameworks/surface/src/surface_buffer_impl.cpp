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

#include "surface_buffer_impl.h"

#include <mutex>

#include <message_parcel.h>
#include <securec.h>
#include <sys/mman.h>
#include "buffer_log.h"
#include "buffer_extra_data_impl.h"
#include "native_buffer.h"
#include "v1_1/buffer_handle_meta_key_type.h"
#include "v1_1/include/idisplay_buffer.h"

namespace OHOS {
namespace {
GSError GenerateError(GSError err, GraphicDispErrCode code)
{
    switch (code) {
        case GRAPHIC_DISPLAY_SUCCESS: return static_cast<GSError>(err + 0);
        case GRAPHIC_DISPLAY_FAILURE: return static_cast<GSError>(err + LOWERROR_FAILURE);
        case GRAPHIC_DISPLAY_FD_ERR: return static_cast<GSError>(err + EBADF);
        case GRAPHIC_DISPLAY_PARAM_ERR: return static_cast<GSError>(err + EINVAL);
        case GRAPHIC_DISPLAY_NULL_PTR: return static_cast<GSError>(err + EINVAL);
        case GRAPHIC_DISPLAY_NOT_SUPPORT: return static_cast<GSError>(err + EOPNOTSUPP);
        case GRAPHIC_DISPLAY_NOMEM: return static_cast<GSError>(err + ENOMEM);
        case GRAPHIC_DISPLAY_SYS_BUSY: return static_cast<GSError>(err + EBUSY);
        case GRAPHIC_DISPLAY_NOT_PERM: return static_cast<GSError>(err + EPERM);
        default: break;
    }
    return static_cast<GSError>(err + LOWERROR_INVALID);
}

inline GSError GenerateError(GSError err, int32_t code)
{
    return GenerateError(err, static_cast<GraphicDispErrCode>(code));
}

using namespace OHOS::HDI::Display::Buffer::V1_1;
using IDisplayBufferSptr = std::shared_ptr<IDisplayBuffer>;
static IDisplayBufferSptr g_displayBuffer;
static std::mutex g_DisplayBufferMutex;
class DisplayBufferDiedRecipient : public OHOS::IRemoteObject::DeathRecipient {
public:
    DisplayBufferDiedRecipient() = default;
    virtual ~DisplayBufferDiedRecipient() = default;
    void OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject>& remote) override
    {
        std::lock_guard<std::mutex> lock(g_DisplayBufferMutex);
        g_displayBuffer = nullptr;
        BLOGD("IDisplayBuffer died and g_displayBuffer is nullptr");
    };
};
IDisplayBufferSptr GetDisplayBuffer()
{
    std::lock_guard<std::mutex> lock(g_DisplayBufferMutex);
    if (g_displayBuffer != nullptr) {
        return g_displayBuffer;
    }

    g_displayBuffer.reset(IDisplayBuffer::Get());
    if (g_displayBuffer == nullptr) {
        BLOGE("IDisplayBuffer::Get return nullptr.");
        return nullptr;
    }
    sptr<IRemoteObject::DeathRecipient> recipient = new DisplayBufferDiedRecipient();
    g_displayBuffer->AddDeathRecipient(recipient);
    return g_displayBuffer;
}

}

sptr<SurfaceBuffer> SurfaceBuffer::Create()
{
    sptr<SurfaceBuffer> surfaceBufferImpl = new SurfaceBufferImpl();
    return surfaceBufferImpl;
}

void SurfaceBufferImpl::DisplayBufferDeathCallback(void* data)
{
    std::lock_guard<std::mutex> lock(g_DisplayBufferMutex);
    g_displayBuffer = nullptr;
    BLOGD("gralloc_host died and g_displayBuffer is nullptr.");
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
    BLOGD("dtor ~[%{public}u]", sequenceNumber_);
    FreeBufferHandleLocked();
}

SurfaceBufferImpl *SurfaceBufferImpl::FromBase(const sptr<SurfaceBuffer>& buffer)
{
    return static_cast<SurfaceBufferImpl*>(buffer.GetRefPtr());
}

GSError SurfaceBufferImpl::Alloc(const BufferRequestConfig &config)
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
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
    OHOS::HDI::Display::Buffer::V1_0::AllocInfo info = {config.width, config.height, config.usage, config.format};
    auto dret = g_displayBuffer->AllocMem(info, handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaceBufferColorGamut_ = static_cast<GraphicColorGamut>(config.colorGamut);
        transform_ = static_cast<GraphicTransformType>(config.transform);
        surfaceBufferWidth_ = config.width;
        surfaceBufferHeight_ = config.height;
        bufferRequestConfig_ = config;
        handle_ = handle;
        BLOGD("buffer handle w: %{public}d h: %{public}d t: %{public}d",
            handle_->width, handle_->height, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::Map()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
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

    if (handle->usage & BUFFER_USAGE_PROTECTED) {
        BLOGD("handle usage is BUFFER_USAGE_PROTECTED, do not Map");
        return GSERROR_OK;
    }

    void *virAddr = g_displayBuffer->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}
GSError SurfaceBufferImpl::Unmap()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
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

    auto dret = g_displayBuffer->Unmap(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        handle_->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}
GSError SurfaceBufferImpl::FlushCache()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
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

    auto dret = g_displayBuffer->FlushCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::InvalidateCache()
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
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

    auto dret = g_displayBuffer->InvalidateCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

void SurfaceBufferImpl::FreeBufferHandleLocked()
{
    if (handle_) {
        if (handle_->virAddr != nullptr && g_displayBuffer != nullptr) {
            g_displayBuffer->Unmap(*handle_);
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
    if (surfaceBufferColorGamut_ != colorGamut) {
        surfaceBufferColorGamut_ = colorGamut;
    }
}

const GraphicColorGamut& SurfaceBufferImpl::GetSurfaceBufferColorGamut() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return surfaceBufferColorGamut_;
}

void SurfaceBufferImpl::SetSurfaceBufferTransform(const GraphicTransformType& transform)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (transform_ != transform) {
        transform_ = transform;
    }
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
    if (handle_ != nullptr) {
        FreeBufferHandleLocked();
    }
    handle_ = handle;
}

GSError SurfaceBufferImpl::WriteBufferRequestConfig(MessageParcel &parcel)
{
    if (!parcel.WriteInt32(bufferRequestConfig_.width) || !parcel.WriteInt32(bufferRequestConfig_.height) ||
        !parcel.WriteInt32(bufferRequestConfig_.strideAlignment) || !parcel.WriteInt32(bufferRequestConfig_.format) ||
        !parcel.WriteUint64(bufferRequestConfig_.usage) || !parcel.WriteInt32(bufferRequestConfig_.timeout) ||
        !parcel.WriteUint32(static_cast<uint32_t>(bufferRequestConfig_.colorGamut)) ||
        !parcel.WriteUint32(static_cast<uint32_t>(bufferRequestConfig_.transform))) {
        BLOGE("%{public}s a lot failed", __func__);
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
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

GSError SurfaceBufferImpl::ReadBufferRequestConfig(MessageParcel &parcel)
{
    uint32_t colorGamut = 0;
    uint32_t transform = 0;
    if (!parcel.ReadInt32(bufferRequestConfig_.width) || !parcel.ReadInt32(bufferRequestConfig_.height) ||
        !parcel.ReadInt32(bufferRequestConfig_.strideAlignment) || !parcel.ReadInt32(bufferRequestConfig_.format) ||
        !parcel.ReadUint64(bufferRequestConfig_.usage) || !parcel.ReadInt32(bufferRequestConfig_.timeout) ||
        !parcel.ReadUint32(colorGamut) || !parcel.ReadUint32(transform)) {
        BLOGE("%{public}s a lot failed", __func__);
        return GSERROR_API_FAILED;
    }
    bufferRequestConfig_.colorGamut = static_cast<GraphicColorGamut>(colorGamut);
    bufferRequestConfig_.transform = static_cast<GraphicTransformType>(transform);
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

BufferWrapper SurfaceBufferImpl::GetBufferWrapper()
{
    BufferWrapper wrapper;
    return wrapper;
}

void SurfaceBufferImpl::SetBufferWrapper(BufferWrapper wrapper) {}

GSError SurfaceBufferImpl::SetMetadata(uint32_t key, const std::vector<uint8_t>& value)
{
    if (key == 0 || key >= HDI::Display::Graphic::Common::V1_1::ATTRKEY_END) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return GSERROR_NOT_INIT;
    }
    auto dret = g_displayBuffer->SetMetadata(*handle_, key, value);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::GetMetadata(uint32_t key, std::vector<uint8_t>& value)
{
    if (key == 0 || key >= HDI::Display::Graphic::Common::V1_1::ATTRKEY_END) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return GSERROR_NOT_INIT;
    }
    auto dret = g_displayBuffer->GetMetadata(*handle_, key, value);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::ListMetadataKeys(std::vector<uint32_t>& keys)
{
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    keys.clear();
    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return GSERROR_NOT_INIT;
    }
    auto dret = g_displayBuffer->ListMetadataKeys(*handle_, keys);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError SurfaceBufferImpl::EraseMetadataKey(uint32_t key)
{
    if (key == 0 || key >= HDI::Display::Graphic::Common::V1_1::ATTRKEY_END) {
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (GetDisplayBuffer() == nullptr) {
        BLOGE("GetDisplayBuffer failed!");
        return GSERROR_INTERNAL;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (handle_ == nullptr) {
        BLOGE("Failure, Reason: handle_ is nullptr");
        return GSERROR_NOT_INIT;
    }
    auto dret = g_displayBuffer->EraseMetadataKey(*handle_, key);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    return GenerateError(GSERROR_API_FAILED, dret);
}

const BufferRequestConfig* SurfaceBufferImpl::GetBufferRequestConfig() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return &bufferRequestConfig_;
}

void SurfaceBufferImpl::SetBufferRequestConfig(const BufferRequestConfig &config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bufferRequestConfig_ = config;
}

void SurfaceBufferImpl::SetConsumerAttachBufferFlag(bool value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    isConsumerAttachBufferFlag_ = value;
}

bool SurfaceBufferImpl::GetConsumerAttachBufferFlag()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return isConsumerAttachBufferFlag_;
}
} // namespace OHOS
