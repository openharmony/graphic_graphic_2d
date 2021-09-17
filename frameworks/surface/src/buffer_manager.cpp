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

#include <display_gralloc.h>

#include "buffer_log.h"

#define CHECK_INIT()                       \
    do {                                   \
        if (grallocFuncs_ == nullptr) {    \
            SurfaceError ret = Init();     \
            if (ret != SURFACE_ERROR_OK) { \
                return ret;                \
            }                              \
        }                                  \
    } while (0)

#define CHECK_FUNC(func)                      \
    do {                                      \
        if (func == nullptr) {                \
            return SURFACE_ERROR_NOT_SUPPORT; \
        }                                     \
    } while (0)

#define CHECK_BUFFER(buffer)              \
    do {                                  \
        if (buffer == nullptr) {          \
            return SURFACE_ERROR_NULLPTR; \
        }                                 \
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
}

BufferManager BufferManager::instance_;
SurfaceError BufferManager::Init()
{
    if (grallocFuncs_ != nullptr) {
        BLOGD("BufferManager has been initialized successfully.");
        return SURFACE_ERROR_OK;
    }

    auto dret = GrallocInitialize(&grallocFuncs_);
    if (dret != DISPLAY_SUCCESS) {
        BLOGW("GrallocInitialize failed.");
        return GenerateError(SURFACE_ERROR_INIT, dret);
    }

    if (grallocFuncs_ == nullptr) {
        BLOGW("GrallocInitialize failed.");
        return SURFACE_ERROR_INIT;
    }

    BLOGD("funcs.AllocMem            0x%{public}s", grallocFuncs_->AllocMem ? "Yes" : "No");
    BLOGD("funcs.FreeMem             0x%{public}s", grallocFuncs_->FreeMem ? "Yes" : "No");
    BLOGD("funcs.Mmap                0x%{public}s", grallocFuncs_->Mmap ? "Yes" : "No");
    BLOGD("funcs.MmapCache           0x%{public}s", grallocFuncs_->MmapCache ? "Yes" : "No");
    BLOGD("funcs.Unmap               0x%{public}s", grallocFuncs_->Unmap ? "Yes" : "No");
    BLOGD("funcs.FlushCache          0x%{public}s", grallocFuncs_->FlushCache ? "Yes" : "No");
    BLOGD("funcs.FlushMCache         0x%{public}s", grallocFuncs_->FlushMCache ? "Yes" : "No");
    BLOGD("funcs.InvalidateCache     0x%{public}s", grallocFuncs_->InvalidateCache ? "Yes" : "No");
    return SURFACE_ERROR_OK;
}

BufferManager::~BufferManager()
{
    int32_t ret = GrallocUninitialize(grallocFuncs_);
    if (ret != DISPLAY_SUCCESS) {
        BLOGE("Failure, Reason: GrallocUninitialize failed with %{public}d", ret);
    }
}

SurfaceError BufferManager::Alloc(const BufferRequestConfig &config, sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->AllocMem);
    CHECK_BUFFER(buffer);

    BufferHandle *handle = nullptr;
    AllocInfo info = {config.width, config.height, config.usage, (PixelFormat)config.format};

    auto dret = grallocFuncs_->AllocMem(&info, &handle);
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
    CHECK_FUNC(grallocFuncs_->Mmap);
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    void *virAddr = grallocFuncs_->Mmap(reinterpret_cast<BufferHandle*>(handle));
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return SURFACE_ERROR_API_FAILED;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError BufferManager::Unmap(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->Unmap);
    CHECK_BUFFER(buffer);

    if (buffer->GetVirAddr() == nullptr) {
        return SURFACE_ERROR_OK;
    }

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = grallocFuncs_->Unmap(reinterpret_cast<BufferHandle*>(handle));
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
    CHECK_FUNC(grallocFuncs_->FlushCache);
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = grallocFuncs_->FlushCache(reinterpret_cast<BufferHandle*>(handle));
    if (dret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::InvalidateCache(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->InvalidateCache);
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    auto dret = grallocFuncs_->InvalidateCache(reinterpret_cast<BufferHandle*>(handle));
    if (dret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(SURFACE_ERROR_API_FAILED, dret);
}

SurfaceError BufferManager::Free(sptr<SurfaceBufferImpl> &buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->FreeMem);
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    grallocFuncs_->FreeMem(reinterpret_cast<BufferHandle*>(handle));
    buffer->SetBufferHandle(nullptr);
    return SURFACE_ERROR_OK;
}
} // namespace OHOS
