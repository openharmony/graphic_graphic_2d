/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surface_tunnel_handle.h"
#include <securec.h>
#include "buffer_log.h"

namespace OHOS {
GraphicExtDataHandle *AllocExtDataHandle(uint32_t reserveInts)
{
    if ((size_t)reserveInts > (SIZE_MAX - sizeof(GraphicExtDataHandle)) / sizeof(int32_t)) {
        BLOGE("AllocExtDataHandle failed, reserveInts: %u is too large", reserveInts);
        return nullptr;
    }
    size_t handleSize = sizeof(GraphicExtDataHandle) + (sizeof(int32_t) * reserveInts);
    GraphicExtDataHandle *handle = static_cast<GraphicExtDataHandle *>(malloc(handleSize));
    if (handle == nullptr) {
        BLOGE("AllocExtDataHandle malloc %zu failed", handleSize);
        return nullptr;
    }
    auto ret = memset_s(handle, handleSize, 0, handleSize);
    if (ret != EOK) {
        free(handle);
        BLOGE("AllocExtDataHandle memset_s failed");
        return nullptr;
    }
    handle->fd = -1;
    handle->reserveInts = reserveInts;
    for (uint32_t i = 0; i < reserveInts; i++) {
        handle->reserve[i] = -1;
    }
    return handle;
}

void FreeExtDataHandle(GraphicExtDataHandle *handle)
{
    if (handle == nullptr) {
        BLOGW("FreeExtDataHandle with nullptr handle");
        return ;
    }
    if (handle->fd >= 0) {
        close(handle->fd);
        handle->fd = -1;
    }
    free(handle);
}

SurfaceTunnelHandle::SurfaceTunnelHandle()
{
    BLOGD("ctor");
}

SurfaceTunnelHandle::~SurfaceTunnelHandle()
{
    std::lock_guard<std::mutex> lock(mutex_);
    BLOGD("dtor tunnelHandle_ %{public}p", tunnelHandle_);
    FreeExtDataHandle(tunnelHandle_);
}

GSError SurfaceTunnelHandle::SetHandle(const GraphicExtDataHandle *handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (tunnelHandle_ != nullptr) {
        FreeExtDataHandle(tunnelHandle_);
        tunnelHandle_ = nullptr;
    }
    if (handle == nullptr) {  // handle is nullptr, which is valid and tunnelHandle_ is nullptr now
        BLOGW("SetHandle with nullptr");
        return GSERROR_OK;
    }
    tunnelHandle_ = AllocExtDataHandle(handle->reserveInts);
    if (tunnelHandle_ == nullptr) {
        BLOGE("SetHandle failed because of AllocExtDataHandle failed");
        return GSERROR_INVALID_OPERATING;
    }
    tunnelHandle_->fd = handle->fd;
    for (uint32_t index = 0; index < handle->reserveInts; index++) {
        tunnelHandle_->reserve[index] = handle->reserve[index];
    }
    return GSERROR_OK;
}

GraphicExtDataHandle *SurfaceTunnelHandle::GetHandle()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return tunnelHandle_;
}

bool SurfaceTunnelHandle::Different(const sptr<SurfaceTunnelHandle> &handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (tunnelHandle_ == nullptr) {
        return false;
    }
    if (handle == nullptr || handle->GetHandle() == nullptr) {
        return true;
    }

    bool diffHandle = tunnelHandle_->fd != handle->GetHandle()->fd ||
                      tunnelHandle_->reserveInts != handle->GetHandle()->reserveInts;
    for (uint32_t index = 0; index < handle->GetHandle()->reserveInts; index++) {
        diffHandle = diffHandle || tunnelHandle_->reserve[index] != handle->GetHandle()->reserve[index];
    }
    return diffHandle;
}
} // namespace OHOS
