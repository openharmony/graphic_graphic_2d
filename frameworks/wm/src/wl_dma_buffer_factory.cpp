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

#include "wl_dma_buffer_factory.h"

#include <cinttypes>
#include <map>

#include <display_type.h>
#include <drm_fourcc.h>
#include <promise.h>

#include "wayland-util.h"
#include "window_manager_hilog.h"
#include "wl_display.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWlDMABufferFactory" };
std::map<struct zwp_linux_buffer_params_v1 *, sptr<Promise<struct wl_buffer *>>> g_bufferPromises;
constexpr int32_t STRIDE_NUM = 4;
constexpr int32_t MODIFY_OFFSET = 32;
constexpr int32_t BITS_OFFSET_32 = 32;
}

sptr<WlDMABufferFactory> WlDMABufferFactory::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlDMABufferFactory();
        }
    }
    return instance;
}

void WlDMABufferFactory::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WlDMABufferFactory::OnAppear);
}

void WlDMABufferFactory::Deinit()
{
    if (dmabuf != nullptr) {
        zwp_linux_dmabuf_v1_destroy(dmabuf);
        dmabuf = nullptr;
    }
}

void WlDMABufferFactory::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    if (iname == "zwp_linux_dmabuf_v1") {
        constexpr uint32_t dmabufVersion = 3;
        auto ret = get(&zwp_linux_dmabuf_v1_interface, dmabufVersion);
        dmabuf = static_cast<struct zwp_linux_dmabuf_v1 *>(ret);
    }
}

namespace {
void Success(void *, struct zwp_linux_buffer_params_v1 *param, struct wl_buffer *buffer)
{
    WMLOGFI("Success");
    g_bufferPromises[param]->Resolve(buffer);
}

void Failure(void *, struct zwp_linux_buffer_params_v1 *param)
{
    WMLOGFI("Failure");
    g_bufferPromises[param]->Resolve(nullptr);
}
} // namespace

struct zwp_linux_buffer_params_v1 *WlDMABufferFactory::CreateParam(BufferHandle *handle)
{
    auto display = delegator.Dep<WlDisplay>();
    if (dmabuf == nullptr) {
        WMLOGFE("dmabuf is nullptr");
        return nullptr;
    }

    auto param = zwp_linux_dmabuf_v1_create_params(dmabuf);
    if (param == nullptr) {
        auto err = display->GetError();
        WMLOGFE("zwp_linux_dmabuf_v1_create_params failed with %{public}s", strerror(err));
        return nullptr;
    }

    uint64_t modifier = 0;
    zwp_linux_buffer_params_v1_add(param, handle->fd, 0, 0,
        handle->stride, modifier >> MODIFY_OFFSET, modifier & 0xffffffff);

    SendBufferHandle(param, handle);

    if (display->GetError() != 0) {
        auto err = display->GetError();
        WMLOGFE("zwp_linux_buffer_params_v1_add failed with %{public}s", strerror(err));
        WMLOGFE("args: fd(%{public}d), w(%{public}d)", handle->fd, handle->width);
        zwp_linux_buffer_params_v1_destroy(param);
        return nullptr;
    }

    const struct zwp_linux_buffer_params_v1_listener listener = { Success, Failure };
    if (zwp_linux_buffer_params_v1_add_listener(param, &listener, nullptr) == -1) {
        WMLOGFE("zwp_linux_buffer_params_v1_add_listener failed");
        zwp_linux_buffer_params_v1_destroy(param);
        return nullptr;
    }
    return param;
}

namespace {
bool PixelFormatToDrmFormat(int32_t pixelFormat, uint32_t &drmFormat)
{
    constexpr struct {
        int32_t pixelFormat;
        uint32_t drmFormat;
    } formatTable[] = {
        {PIXEL_FMT_RGB_565, DRM_FORMAT_RGB565},
        {PIXEL_FMT_RGBX_4444, DRM_FORMAT_RGBX4444},
        {PIXEL_FMT_RGBA_4444, DRM_FORMAT_RGBA4444},
        {PIXEL_FMT_RGBX_5551, DRM_FORMAT_RGBX5551},
        {PIXEL_FMT_RGBA_5551, DRM_FORMAT_RGBA5551},
        {PIXEL_FMT_RGBX_8888, DRM_FORMAT_RGBX8888},
        {PIXEL_FMT_RGBA_8888, DRM_FORMAT_RGBA8888},
        {PIXEL_FMT_RGB_888, DRM_FORMAT_RGB888},
        {PIXEL_FMT_BGR_565, DRM_FORMAT_BGR565},
        {PIXEL_FMT_BGRX_4444, DRM_FORMAT_BGRX4444},
        {PIXEL_FMT_BGRA_4444, DRM_FORMAT_BGRA4444},
        {PIXEL_FMT_BGRX_5551, DRM_FORMAT_BGRX5551},
        {PIXEL_FMT_BGRA_5551, DRM_FORMAT_BGRA5551},
        {PIXEL_FMT_BGRX_8888, DRM_FORMAT_BGRX8888},
        {PIXEL_FMT_BGRA_8888, DRM_FORMAT_BGRA8888},
        {PIXEL_FMT_YUV_422_I, DRM_FORMAT_YUV422},
        {PIXEL_FMT_YUYV_422_PKG, DRM_FORMAT_YUYV},
        {PIXEL_FMT_UYVY_422_PKG, DRM_FORMAT_UYVY},
        {PIXEL_FMT_YVYU_422_PKG, DRM_FORMAT_YVYU},
        {PIXEL_FMT_VYUY_422_PKG, DRM_FORMAT_VYUY},
        {PIXEL_FMT_YCBCR_420_SP, DRM_FORMAT_NV12},
        {PIXEL_FMT_YCRCB_420_SP, DRM_FORMAT_NV21},
        {PIXEL_FMT_YCBCR_420_P, DRM_FORMAT_YUV420},
    };

    for (const auto &fmt : formatTable) {
        if (fmt.pixelFormat == pixelFormat) {
            drmFormat = fmt.drmFormat;
            return true;
        }
    }
    return false;
}
} // namespace

void WlDMABufferFactory::SendBufferHandle(zwp_linux_buffer_params_v1 *param, BufferHandle *handle)
{
    auto display = delegator.Dep<WlDisplay>();
    wl_array reservefds;
    wl_array reserveints;

    WMLOGFI("BufferHandle->fd = %{public}d", handle->fd);
    WMLOGFI("BufferHandle->width = %{public}d", handle->width);
    WMLOGFI("BufferHandle->stride = %{public}d", handle->stride);
    WMLOGFI("BufferHandle->height = %{public}d", handle->height);
    WMLOGFI("BufferHandle->size = %{public}d", handle->size);
    WMLOGFI("BufferHandle->format = %{public}d", handle->format);
    WMLOGFI("BufferHandle->usage = %{public}" PRIu64 ", %{public}u, %{public}u",
        handle->usage, (uint32_t)(handle->usage >> BITS_OFFSET_32), (uint32_t)(handle->usage & 0xFFFFFFFF));
    WMLOGFI("BufferHandle->phyAddr = %{public}" PRIu64 ", %{public}u, %{public}u",
        handle->phyAddr, (uint32_t)(handle->phyAddr >> BITS_OFFSET_32), (uint32_t)(handle->phyAddr & 0xFFFFFFFF));
    WMLOGFI("BufferHandle->key = %{public}d", handle->key);

    WMLOGFI("BufferHandle->reserveFds = %{public}u", handle->reserveFds);
    WMLOGFI("BufferHandle->reserveInts = %{public}u", handle->reserveInts);

    wl_array_init(&reservefds);
    for (int i = 0; i < handle->reserveFds; i++) {
        uint32_t *p = (uint32_t *)wl_array_add(&reservefds, sizeof(uint32_t));
        if (p != nullptr) {
            *p = handle->reserve[i];
        }
    }

    wl_array_init(&reserveints);
    for (int i = 0; i < handle->reserveInts; i++) {
        uint32_t *p = (uint32_t *)wl_array_add(&reserveints, sizeof(uint32_t));
        if (p != nullptr) {
            *p = handle->reserve[i + handle->reserveFds];
        }
    }

    zwp_linux_buffer_params_v1_add_buffer_handle(param,
        handle->fd, handle->width, handle->stride, handle->height,
        handle->size, handle->format, handle->usage >> BITS_OFFSET_32, handle->usage & 0xFFFFFFFF,
        handle->phyAddr >> BITS_OFFSET_32, handle->phyAddr & 0xFFFFFFFF, handle->key,
        &reservefds, &reserveints);
}

sptr<WlBuffer> WlDMABufferFactory::Create(BufferHandle *handle)
{
    auto display = delegator.Dep<WlDisplay>();
    uint32_t drmFormat;

    if (handle == nullptr) {
        WMLOGFE("handle == nullptr");
        return nullptr;
    }

    if (PixelFormatToDrmFormat(handle->format, drmFormat) == false) {
        WMLOGFE("PixelFormatToDrmFormat failed");
        return nullptr;
    }

    auto param = CreateParam(handle);
    if (param == nullptr) {
        return nullptr;
    }

    // add death listener before send, and remove before erase(promise destroy)
    g_bufferPromises[param] = new Promise<struct wl_buffer *>();
    auto dl = display->AddDispatchDeathListener([&]() { g_bufferPromises[param]->Resolve(nullptr); });

    constexpr uint32_t flags = 0;
    zwp_linux_buffer_params_v1_create(param, handle->width, handle->height, drmFormat, flags);
    display->Flush(); // send request

    auto buffer = g_bufferPromises[param]->Await();
    display->RemoveDispatchDeathListener(dl);
    g_bufferPromises.erase(param);
    if (display->GetError() != 0) {
        WMLOGFE("zwp_linux_buffer_params_v1_create failed with %{public}d", display->GetError());
        WMLOGFE("args: (%{public}dx%{public}d), format(0x%{public}x), flags(0x%{public}x)",
            handle->width, handle->height, drmFormat, flags);
        zwp_linux_buffer_params_v1_destroy(param);
        return nullptr;
    }

    zwp_linux_buffer_params_v1_destroy(param);
    display->Sync();
    if (display->GetError() != 0) {
        WMLOGFW("zwp_linux_buffer_params_v1_destroy failed with %{public}d", display->GetError());
    }

    if (buffer == nullptr) {
        WMLOGFE("buffer is nullptr");
        return nullptr;
    }

    sptr<WlBuffer> ret = new WlBuffer(buffer);
    if (ret == nullptr) {
        WMLOGFE("new WlBuffer failed");
        return nullptr;
    }
    return ret;
}
} // namespace OHOS
