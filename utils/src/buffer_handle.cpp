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

#include "buffer_handle.h"

#include <cstdlib>
#include <securec.h>

#include <hilog/log.h>
#include <message_parcel.h>
#include <unistd.h>

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "graphicutils" };
#define UTILS_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, __VA_ARGS__)
#define UTILS_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, __VA_ARGS__)
#define UTILS_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, __VA_ARGS__)
#define UTILS_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, __VA_ARGS__)
#define UTILS_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, __VA_ARGS__)

BufferHandle *AllocateBufferHandle(uint32_t reserveFds, uint32_t reserveInts)
{
    if (reserveFds > BUFFER_HANDLE_MAX_RESERVE_FDS || reserveFds < 0 ||
        reserveInts > BUFFER_HANDLE_MAX_RESERVE_INTS || reserveInts < 0) {
        UTILS_LOGE("InitBufferHandle with illegal params reserveInts:%d, reserveFds:%d", reserveFds, reserveFds);
        return nullptr;
    }
    size_t handleSize = sizeof(BufferHandle) + (sizeof(int32_t) * (reserveFds + reserveInts));
    BufferHandle *handle = static_cast<BufferHandle *>(malloc(handleSize));
    if (handle != nullptr) {
        handle->fd = -1;
        for (uint32_t i = 0; i < reserveFds; i++) {
            handle->reserve[i] = -1;
        }
        handle->reserveFds = reserveFds;
        handle->reserveInts = reserveInts;
    } else {
        UTILS_LOGE("InitBufferHandle malloc %zu failed", handleSize);
    }
    return handle;
}

int32_t FreeBufferHandle(BufferHandle *handle)
{
    if (handle == nullptr) {
        UTILS_LOGW("FreeBufferHandle with nullptr handle");
        return 0;
    }
    if (handle->fd >= 0) {
        close(handle->fd);
        handle->fd = -1;
    }
    const uint32_t reserveFds = handle->reserveFds;
    for (uint32_t i = 0; i < reserveFds; i++) {
        if (handle->reserve[i] >= 0) {
            close(handle->reserve[i]);
            handle->reserve[i] = -1;
        }
    }
    free(handle);
    return 0;
}

BufferHandle *CloneBufferHandle(const BufferHandle *handle)
{
    if (handle == nullptr) {
        UTILS_LOGW("%{public}s handle is nullptr", __func__);
        return nullptr;
    }

    BufferHandle *newHandle = AllocateBufferHandle(handle->reserveFds, handle->reserveInts);
    if (newHandle == nullptr) {
        UTILS_LOGW("%{public}s AllocateBufferHandle failed, newHandle is nullptr", __func__);
        return nullptr;
    }

    if (handle->fd == -1) {
        newHandle->fd = handle->fd;
    } else {
        newHandle->fd = dup(handle->fd);
        if (newHandle->fd == -1) {
            UTILS_LOGE("CloneBufferHandle dup failed");
            FreeBufferHandle(newHandle);
            return nullptr;
        }
    }
    newHandle->width = handle->width;
    newHandle->stride = handle->stride;
    newHandle->height = handle->height;
    newHandle->size = handle->size;
    newHandle->format = handle->format;
    newHandle->usage = handle->usage;
    newHandle->phyAddr = handle->phyAddr;
    newHandle->key = handle->key;

    for (uint32_t i = 0; i < newHandle->reserveFds; i++) {
        newHandle->reserve[i] = dup(handle->reserve[i]);
        if (newHandle->reserve[i] == -1) {
            UTILS_LOGE("CloneBufferHandle dup reserveFds failed");
            FreeBufferHandle(newHandle);
            return nullptr;
        }
    }
    if (memcpy_s(&newHandle->reserve[newHandle->reserveFds], sizeof(int32_t) * newHandle->reserveInts,
        &handle->reserve[handle->reserveFds], sizeof(int32_t) * handle->reserveInts) != EOK) {
        UTILS_LOGE("CloneBufferHandle memcpy_s failed");
        FreeBufferHandle(newHandle);
        return nullptr;
    }
    return newHandle;
}
namespace OHOS {
bool WriteBufferHandle(MessageParcel &parcel, const BufferHandle &handle)
{
    if (!parcel.WriteUint32(handle.reserveFds) || !parcel.WriteUint32(handle.reserveInts) ||
        !parcel.WriteInt32(handle.width) || !parcel.WriteInt32(handle.stride) || !parcel.WriteInt32(handle.height) ||
        !parcel.WriteInt32(handle.size) || !parcel.WriteInt32(handle.format) || !parcel.WriteInt64(handle.usage) ||
        !parcel.WriteUint64(handle.phyAddr) || !parcel.WriteInt32(handle.key)) {
        UTILS_LOGE("%{public}s a lot failed", __func__);
        return false;
    }
    bool validFd = (handle.fd >= 0);
    if (!parcel.WriteBool(validFd)) {
        UTILS_LOGE("%{public}s parcel.WriteBool failed", __func__);
        return false;
    }
    if (validFd && !parcel.WriteFileDescriptor(handle.fd)) {
        UTILS_LOGE("%{public}s parcel.WriteFileDescriptor fd failed", __func__);
        return false;
    }

    for (uint32_t i = 0; i < handle.reserveFds; i++) {
        if (!parcel.WriteFileDescriptor(handle.reserve[i])) {
            UTILS_LOGE("%{public}s parcel.WriteFileDescriptor reserveFds failed", __func__);
            return false;
        }
    }
    for (uint32_t j = 0; j < handle.reserveInts; j++) {
        if (!parcel.WriteInt32(handle.reserve[handle.reserveFds + j])) {
            UTILS_LOGE("%{public}s parcel.WriteInt32 reserve failed", __func__);
            return false;
        }
    }
    return true;
}

BufferHandle *ReadBufferHandle(MessageParcel &parcel)
{
    uint32_t reserveFds = 0;
    uint32_t reserveInts = 0;
    if (!parcel.ReadUint32(reserveFds) || !parcel.ReadUint32(reserveInts)) {
        UTILS_LOGE("%{public}s parcel.ReadUint32 reserveFds failed", __func__);
        return nullptr;
    }

    BufferHandle *handle = AllocateBufferHandle(reserveFds, reserveInts);
    if (handle == nullptr) {
        UTILS_LOGE("%{public}s AllocateBufferHandle failed", __func__);
        return nullptr;
    }

    if (!parcel.ReadInt32(handle->width) || !parcel.ReadInt32(handle->stride) || !parcel.ReadInt32(handle->height) ||
        !parcel.ReadInt32(handle->size) || !parcel.ReadInt32(handle->format) || !parcel.ReadInt64(handle->usage) ||
        !parcel.ReadUint64(handle->phyAddr) || !parcel.ReadInt32(handle->key)) {
        UTILS_LOGE("%{public}s a lot failed", __func__);
        FreeBufferHandle(handle);
        return nullptr;
    }

    bool validFd = false;
    if (!parcel.ReadBool(validFd)) {
        UTILS_LOGE("%{public}s ReadBool validFd failed", __func__);
        FreeBufferHandle(handle);
        return nullptr;
    }
    if (validFd) {
        handle->fd = parcel.ReadFileDescriptor();
        if (handle->fd == -1) {
            UTILS_LOGE("%{public}s ReadFileDescriptor fd failed", __func__);
            FreeBufferHandle(handle);
            return nullptr;
        }
    }

    for (uint32_t i = 0; i < handle->reserveFds; i++) {
        handle->reserve[i] = parcel.ReadFileDescriptor();
        if (handle->reserve[i] == -1) {
            UTILS_LOGE("%{public}s ReadFileDescriptor reserve failed", __func__);
            FreeBufferHandle(handle);
            return nullptr;
        }
    }
    for (uint32_t j = 0; j < handle->reserveInts; j++) {
        if (!parcel.ReadInt32(handle->reserve[reserveFds + j])) {
            UTILS_LOGE("%{public}s ReadInt32 reserve failed", __func__);
            FreeBufferHandle(handle);
            return nullptr;
        }
    }
    return handle;
}
} // namespace OHOS
