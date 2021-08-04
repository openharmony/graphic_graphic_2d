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

#include <map>

#include <display_type.h>
#include <drm_fourcc.h>
#include <promise.h>

#include "window_manager_hilog.h"
#include "wl_display.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWlDMABufferFactory" };
std::map<struct zwp_linux_buffer_params_v1 *, sptr<Promise<struct wl_buffer *>>> g_bufferPromises;
constexpr int32_t STRIDE_NUM = 4;
constexpr int32_t MODIFY_OFFSET = 32;
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

struct zwp_linux_buffer_params_v1 *WlDMABufferFactory::CreateParam(int32_t fd, uint32_t w, uint32_t h)
{
    auto display = delegator.Dep<WlDisplay>();
    if (dmabuf == nullptr) {
        WMLOGFE("dmabuf is nullptr");
        return nullptr;
    }

    auto param = zwp_linux_dmabuf_v1_create_params(dmabuf);
    if (param == nullptr) {
        WMLOGFE("zwp_linux_dmabuf_v1_create_params failed with %{public}d", display->GetError());
        return nullptr;
    }

    uint64_t modifier = 0;
    zwp_linux_buffer_params_v1_add(param, fd, 0, 0,
        w * STRIDE_NUM, modifier >> MODIFY_OFFSET, modifier & 0xffffffff);
    if (display->GetError() != 0) {
        WMLOGFE("zwp_linux_buffer_params_v1_add failed with %{public}d", display->GetError());
        WMLOGFE("args: fd(%{public}d), w(%{public}d)", fd, w);
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

sptr<WlBuffer> WlDMABufferFactory::Create(int32_t fd, uint32_t w, uint32_t h, int32_t format)
{
    auto display = delegator.Dep<WlDisplay>();
    uint32_t drmFormat;
    if (PixelFormatToDrmFormat(format, drmFormat) == false) {
        WMLOGFE("PixelFormatToDrmFormat failed");
        return nullptr;
    }

    auto param = CreateParam(fd, w, h);
    if (param == nullptr) {
        return nullptr;
    }

    // add death listener before send, and remove before erase(promise destroy)
    g_bufferPromises[param] = new Promise<struct wl_buffer *>();
    auto dl = display->AddDispatchDeathListener([&]() { g_bufferPromises[param]->Resolve(nullptr); });

    constexpr uint32_t flags = ZWP_LINUX_BUFFER_PARAMS_V1_FLAGS_Y_INVERT;
    zwp_linux_buffer_params_v1_create(param, w, h, drmFormat, flags);
    display->Flush(); // send request

    auto buffer = g_bufferPromises[param]->Await();
    display->RemoveDispatchDeathListener(dl);
    g_bufferPromises.erase(param);
    if (display->GetError() != 0) {
        WMLOGFE("zwp_linux_buffer_params_v1_create failed with %{public}d", display->GetError());
        WMLOGFE("args: (%{public}dx%{public}d), format(0x%{public}x), flags(0x%{public}x)", w, h, drmFormat, flags);
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
