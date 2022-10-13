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
#include "v1_0/include/idisplay_buffer.h"

#define CHECK_INIT() \
    do { \
        if (displayBuffer_ == nullptr) { \
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

using namespace OHOS::HDI::Display::Buffer::V1_0;
using IDisplayBufferSptr = std::shared_ptr<IDisplayBuffer>;
static std::unique_ptr<IDisplayBuffer> displayBuffer_ = nullptr;

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
    if (displayBuffer_ != nullptr) {
        BLOGD("BufferManager has been initialized successfully.");
        return GSERROR_OK;
    }

    displayBuffer_.reset(IDisplayBuffer::Get());
    if (displayBuffer_ == nullptr) {
        BLOGE("IDisplayBuffer::Get return nullptr.");
        return GSERROR_INTERNAL;
    }
    return GSERROR_OK;
}

GSError BufferManager::Alloc(const BufferRequestConfig &config, sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = nullptr;
    int32_t allocWidth = config.width;
    int32_t allocHeight = config.height;
    AllocInfo info = {allocWidth, allocHeight, config.usage, config.format};
    auto dret = displayBuffer_->AllocMem(info, handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        buffer->SetBufferHandle(handle);
        buffer->SetSurfaceBufferWidth(allocWidth);
        buffer->SetSurfaceBufferHeight(allocHeight);
        buffer->SetSurfaceBufferColorGamut(static_cast<GraphicColorGamut>(config.colorGamut));
        buffer->SetSurfaceBufferTransform(static_cast<GraphicTransformType>(config.transform));
        BLOGI("buffer handle %{public}p w: %{public}d h: %{public}d t: %{public}d", handle,
            allocWidth, allocHeight, config.transform);
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);

    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::Map(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    void *virAddr = displayBuffer_->Mmap(*handle);
    if (virAddr == nullptr || virAddr == MAP_FAILED) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError BufferManager::Unmap(sptr<SurfaceBuffer> &buffer)
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

    auto dret = displayBuffer_->Unmap(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
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
    auto dret = displayBuffer_->Unmap(*bufferHandle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        bufferHandle->virAddr = nullptr;
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::FlushCache(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto dret = displayBuffer_->FlushCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::InvalidateCache(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    auto dret = displayBuffer_->InvalidateCache(*handle);
    if (dret == GRAPHIC_DISPLAY_SUCCESS) {
        return GSERROR_OK;
    }
    BLOGW("Failed with %{public}d", dret);
    return GenerateError(GSERROR_API_FAILED, dret);
}

GSError BufferManager::Free(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    buffer->SetBufferHandle(nullptr);
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    displayBuffer_->FreeMem(*handle);
    return GSERROR_OK;
}

GSError BufferManager::IsSupportedAlloc(const std::vector<BufferVerifyAllocInfo> &infos,
                                        std::vector<bool> &supporteds)
{
    CHECK_INIT();
    // mock data
    supporteds.clear();
    for (uint32_t index = 0; index < infos.size(); index++) {
        if (infos[index].format == GRAPHIC_PIXEL_FMT_RGBA_8888 ||
            infos[index].format == GRAPHIC_PIXEL_FMT_YCRCB_420_SP) {
            supporteds.push_back(true);
        } else {
            supporteds.push_back(false);
        }
    }
    return GSERROR_OK;
}
} // namespace OHOS
