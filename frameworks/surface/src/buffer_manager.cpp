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
}

BufferManager BufferManager::instance_;
SurfaceError BufferManager::Init()
{
    if (grallocFuncs_ != nullptr) {
        BLOGFD("BufferManager has been initialized successfully.");
        return SURFACE_ERROR_OK;
    }

    if (GrallocInitialize(&grallocFuncs_) != DISPLAY_SUCCESS) {
        BLOGFW("GrallocInitialize failed.");
        return SURFACE_ERROR_INIT;
    }

    if (grallocFuncs_ == nullptr) {
        BLOGFW("GrallocInitialize failed.");
        return SURFACE_ERROR_INIT;
    }

    BLOGFD("funcs.AllocMem            0x%{public}s", grallocFuncs_->AllocMem ? "Yes" : "No");
    BLOGFD("funcs.FreeMem             0x%{public}s", grallocFuncs_->FreeMem ? "Yes" : "No");
    BLOGFD("funcs.Mmap                0x%{public}s", grallocFuncs_->Mmap ? "Yes" : "No");
    BLOGFD("funcs.MmapCache           0x%{public}s", grallocFuncs_->MmapCache ? "Yes" : "No");
    BLOGFD("funcs.Unmap               0x%{public}s", grallocFuncs_->Unmap ? "Yes" : "No");
    BLOGFD("funcs.FlushCache          0x%{public}s", grallocFuncs_->FlushCache ? "Yes" : "No");
    BLOGFD("funcs.FlushMCache         0x%{public}s", grallocFuncs_->FlushMCache ? "Yes" : "No");
    BLOGFD("funcs.InvalidateCache     0x%{public}s", grallocFuncs_->InvalidateCache ? "Yes" : "No");
    return SURFACE_ERROR_OK;
}

BufferManager::~BufferManager()
{
    int32_t ret = GrallocUninitialize(grallocFuncs_);
    if (ret != 0) {
        BLOG_FAILURE("GrallocUninitialize failed with %{public}d", ret);
    }
}

SurfaceError BufferManager::Alloc(BufferRequestConfig& config, sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->AllocMem);
    CHECK_BUFFER(buffer);

    BufferHandle* handle = nullptr;
    AllocInfo info = {config.width, config.height, config.usage, (PixelFormat)config.format, DMA_MEM};

    int ret = grallocFuncs_->AllocMem(&info, &handle);
    if (ret == DISPLAY_SUCCESS) {
        buffer->SetBufferHandle(handle);
        return SURFACE_ERROR_OK;
    }
    BLOGFW("Failed with %{public}d", ret);

    if (ret == DISPLAY_NOMEM) {
        return SURFACE_ERROR_NOMEM;
    }

    return SURFACE_ERROR_API_FAILED;
}

SurfaceError BufferManager::Map(sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->Mmap);
    CHECK_BUFFER(buffer);

    BufferHandle* handle = buffer->GetBufferHandle();
    void* virAddr = grallocFuncs_->Mmap(reinterpret_cast<BufferHandle*>(handle));
    if (virAddr == nullptr) {
        return SURFACE_ERROR_API_FAILED;
    }
    return SURFACE_ERROR_OK;
}

SurfaceError BufferManager::Unmap(sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->Unmap);
    CHECK_BUFFER(buffer);

    if (buffer->GetVirAddr() == nullptr) {
        return SURFACE_ERROR_OK;
    }

    BufferHandle* handle = buffer->GetBufferHandle();
    int32_t ret = grallocFuncs_->Unmap(reinterpret_cast<BufferHandle*>(handle));
    if (ret == DISPLAY_SUCCESS) {
        handle->virAddr = nullptr;
        return SURFACE_ERROR_OK;
    }
    BLOGFW("Failed with %{public}d", ret);
    return SURFACE_ERROR_API_FAILED;
}

SurfaceError BufferManager::FlushCache(sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->FlushCache);
    CHECK_BUFFER(buffer);

    BufferHandle* handle = buffer->GetBufferHandle();
    int32_t ret = grallocFuncs_->FlushCache(reinterpret_cast<BufferHandle*>(handle));
    if (ret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGFW("Failed with %{public}d", ret);
    return SURFACE_ERROR_API_FAILED;
}

SurfaceError BufferManager::InvalidateCache(sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->InvalidateCache);
    CHECK_BUFFER(buffer);

    BufferHandle* handle = buffer->GetBufferHandle();
    int32_t ret = grallocFuncs_->InvalidateCache(reinterpret_cast<BufferHandle*>(handle));
    if (ret == DISPLAY_SUCCESS) {
        return SURFACE_ERROR_OK;
    }
    BLOGFW("Failed with %{public}d", ret);
    return SURFACE_ERROR_API_FAILED;
}

SurfaceError BufferManager::Free(sptr<SurfaceBufferImpl>& buffer)
{
    CHECK_INIT();
    CHECK_FUNC(grallocFuncs_->FreeMem);
    CHECK_BUFFER(buffer);

    BufferHandle* handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return SURFACE_ERROR_NULLPTR;
    }

    grallocFuncs_->FreeMem(reinterpret_cast<BufferHandle*>(handle));
    buffer->SetBufferHandle(nullptr);
    return SURFACE_ERROR_OK;
}
} // namespace OHOS
