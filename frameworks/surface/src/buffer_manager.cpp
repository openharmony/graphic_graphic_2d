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
#include <sys/mman.h>

#include "buffer_log.h"

#define CHECK_INIT() \
    do { \
        if (displayGralloc_ == nullptr) { \
            SurfaceError ret = Init(); \
            if (ret != SURFACE_ERROR_OK) { \
                return ret; \
            } \
        } \
    } while (0)

#define CHECK_BUFFER(buffer) \
    do { \
        if ((buffer) == nullptr) { \
            return SURFACE_ERROR_NULLPTR; \
        } \
    } while (0)

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "BufferManager" };

SurfaceError GenerateError(SurfaceError err, DispErrCode code)
{
    switch (code) {
        case DISPLAY_SUCCESS: return static_cast<SurfaceError>(err + 0);
        case DISPLAY_FAILURE: return static_cast<SurfaceError>(err + LOWERROR_FAILURE);
        case DISPLAY_FD_ERR: return static_cast<SurfaceError>(err + EBADF);
        case DISPLAY_PARAM_ERR: return static_cast<SurfaceError>(err + EINVAL);
        case DISPLAY_NULL_PTR: return static_cast<SurfaceError>(err + EINVAL);
        case DISPLAY_NOT_SUPPORT: return static_cast<SurfaceError>(err + EOPNOTSUPP);
        case DISPLAY_NOMEM: return static_cast<SurfaceError>(err + ENOMEM);
        case DISPLAY_SYS_BUSY: return static_cast<SurfaceError>(err + EBUSY);
        case DISPLAY_NOT_PERM: return static_cast<SurfaceError>(err + EPERM);
        default: break;
    }
    return static_cast<SurfaceError>(err + LOWERROR_INVALID);
}

inline SurfaceError GenerateError(SurfaceError err, int32_t code)
{
    return GenerateError(err, static_cast<DispErrCode>(code));
}
} // namespace

BufferManager *BufferManager::GetInstance()
{
    static BufferManager instance {};
    return &instance;
}

SurfaceError BufferManager::Init()
{
    if (displayGralloc_ != nullptr) {
        BLOGD("BufferManager has been initialized successfully.");
        return SURFACE_ERROR_OK;
    }

    displayGralloc_.reset(::OHOS::HDI::Display::V1_0::IDisplayGralloc::Get());
    if (displayGralloc_ == nullptr) {
        BLOGE("IDisplayGralloc::Get return nullptr.");
        return SURFACE_ERROR_INIT;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError BufferManager::Alloc(const BufferRequestConfig &config, sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = nullptr;
    int32_t width = (config.width + 15) / 16 * 16;
    AllocInfo info = {width, config.height, config.usage, (PixelFormat)config.format};
    auto dret = displayGralloc_->AllocMem(info, handle);
    if (dret == DISPLAY_SUCCESS) {
        buffer->SetBufferHandle(handle);
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);

    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::Map(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    void *virAddr = displayGralloc_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return SURFACE_ERROR_API_FAILED;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError BufferManager::Unmap(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    if (buffer->GetVirAddr() == nullptr) {
        return SURFACE_ERROR_OK;
    }

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = displayGralloc_->Unmap(*handle);
    if (dret == DISPLAY_SUCCESS) {
        handle->virAddr = nullptr;
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::FlushCache(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = displayGralloc_->FlushCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::InvalidateCache(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = displayGralloc_->InvalidateCache(*handle);
    if (dret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::Free(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    displayGralloc_->FreeMem(*handle);
    buffer->SetBufferHandle(nullptr);
    return SURFACE_ERROR_OK;
}
} // namespace OHOS
