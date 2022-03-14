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

#include "buffer_manager.h"

#include <cerrno>
#include <mutex>
#include <sys/mman.h>

#include "buffer_log.h"

#define CHECK_INIT() \
    do { \
        if (displayGralloc_ == nullptr) { \
            GSError ret = Init(); \
            if (ret != GSERROR_OK) { \
                return ret; \
            } \
        } \
    } while (0)

#define CHECK_BUFFER(buffer) \
    do { \
        if ((buffer) == nullptr) { \
            return GSERROR_INVALID_ARGUMENTS; \
        } \
    } while (0)

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
} // namespace

sptr<BufferManager> BufferManager::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new BufferManager();
        }
    }
    return instance;
}

GSError BufferManager::Init()
{
    if (displayGralloc_ != nullptr) {
        BLOGD("BufferManager has been initialized successfully.");
        return GSERROR_OK;
    }

    displayGralloc_.reset(::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get());
    if (displayGralloc_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return GSERROR_INTERNEL;
    }
    return GSERROR_OK;
}

GSError BufferManager::Alloc(const BufferRequestConfig &config, sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = nullptr;
    int32_t allocWidth = config.width;
    int32_t allocHeight = config.height;
    if (config.transform == TransformType::ROTATE_90 || config.transform == TransformType::ROTATE_270) {
        std::swap(allocWidth, allocHeight);
    }
    AllocInfo info = {allocWidth, allocHeight, config.usage, (PixelFormat)config.format};
    auto dret = displayGralloc_->AllocMem(info, handle);
    if (dret == DISPLAY_SUCCESS) {
        buffer->SetBufferHandle(handle);
        buffer->SetSurfaceBufferWidth(allocWidth);
        buffer->SetSurfaceBufferHeight(allocHeight);
        buffer->SetSurfaceBufferColorGamut(config.colorGamut);
        buffer->SetSurfaceBufferTransform(config.transform);
        BLOGI("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle,
            allocWidth, allocHeight, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);

    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::Map(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    void *virAddr = displayGralloc_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError BufferManager::Unmap(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    if (buffer->GetVirAddr() == nullptr) {
        return GSERROR_OK;
    }

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto dret = displayGralloc_->Unmap(*handle);
    if (dret == DISPLAY_SUCCESS) {
        handle->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::Unmap(BufferHandle *bufferHandle)
{
    CHECK_INIT();
    if (bufferHandle == nullptr) {
        return GSERROR_OK;
    }
    if (bufferHandle->virAddr == nullptr) {
        return GSERROR_OK;
    }
    auto dret = displayGralloc_->Unmap(*bufferHandle);
    if (dret == DISPLAY_SUCCESS) {
        bufferHandle->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::FlushCache(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto dret = displayGralloc_->FlushCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::InvalidateCache(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto dret = displayGralloc_->InvalidateCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::Free(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    buffer->SetBufferHandle(nullptr);
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    displayGralloc_->FreeMem(*handle);
    return GSERROR_OK;
}
} // namespace OHOS
