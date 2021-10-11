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

#include "video_window.h"

#include "window_manager_hilog.h"
#include "wl_surface_factory.h"

#define VIDEO_WINDOW_DEBUG

#ifndef VIDEO_WINDOW_DEBUG
#define VIDEO_WINDOW_ENTER() ((void)0)
#define VIDEO_WINDOW_EXIT() ((void)0)
#else
#define VIDEO_WINDOW_ENTER() do { \
    WMLOGFD("enter..."); \
} while (0)

#define VIDEO_WINDOW_EXIT() do { \
    WMLOGFD("exit..."); \
} while (0)
#endif

namespace OHOS {
namespace {
    static constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VideoWindow" };
}

    VideoWindow::VideoWindow(InnerWindowInfo &winInfo) : SubWindow(winInfo.windowid, winInfo.surface),
        layerId_(winInfo.voLayerId), surface_(winInfo.surface), producer(nullptr)
    {
        VIDEO_WINDOW_ENTER();
#ifdef TARGET_CPU_ARM
        display = new (std::nothrow) VideoDisplayManager();
#else
        display = nullptr;
#endif
        if (display == nullptr) {
            WMLOGFE("new video display manager fail");
        }
        Init();
        Move(winInfo.pos_x, winInfo.pos_y);
        VIDEO_WINDOW_EXIT();
    }

    VideoWindow::~VideoWindow()
    {
        VIDEO_WINDOW_ENTER();
#ifdef TARGET_CPU_ARM
        if (display != nullptr) {
            display->DetachLayer(layerId_);
        }
#endif
        DestroyLayer(layerId_);
        VIDEO_WINDOW_EXIT();
    }

    int32_t VideoWindow::CreateLayer(InnerWindowInfo &winInfo, uint32_t &layerId, sptr<Surface> &surface)
    {
        VIDEO_WINDOW_ENTER();
        LayerInfo layerInfo = {winInfo.width, winInfo.height, LAYER_TYPE_SDIEBAND, 8, PIXEL_FMT_YCRCB_420_SP};
        layerInfo.pixFormat = (PixelFormat)winInfo.windowconfig.format;
        if (winInfo.windowconfig.type == -1) {
            layerInfo.type = LAYER_TYPE_OVERLAY;
        }
#ifdef TARGET_CPU_ARM
        int32_t ret = VideoDisplayManager::CreateLayer(layerInfo, layerId, surface);
#else
        int32_t ret = DISPLAY_FAILURE;
#endif
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    void VideoWindow::DestroyLayer(uint32_t layerId)
    {
        VIDEO_WINDOW_ENTER();
#ifdef TARGET_CPU_ARM
        VideoDisplayManager::DestroyLayer(layerId);
#endif
        VIDEO_WINDOW_EXIT();
    }

    int32_t VideoWindow::Init()
    {
        VIDEO_WINDOW_ENTER();
        int32_t ret = DISPLAY_SUCCESS;
        if (display == nullptr) {
            WMLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
#ifdef TARGET_CPU_ARM
        producer = display->AttachLayer(surface_, layerId_);
        if (producer == nullptr) {
            WMLOGFE("attach layer fail");
            ret = DISPLAY_FAILURE;
        }
#endif
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    sptr<Surface> VideoWindow::GetSurface()
    {
        VIDEO_WINDOW_ENTER();
        sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
        VIDEO_WINDOW_EXIT();
        return surface;
    }

    void VideoWindow::Move(int32_t x, int32_t y)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            WMLOGFE("display layer is not create");
            return;
        }

#ifdef TARGET_CPU_ARM
        IRect rect = {};
        int32_t ret = display->GetRect(layerId_, rect);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("get rect fail, ret:%{public}d", ret);
            return;
        }

        WMLOGFI("get layer: x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d", rect.x, rect.y, rect.w, rect.h);
        constexpr float BAR_WIDTH_PERCENT = 0.07;
        int maxHeight = LayerControllerClient::GetInstance()->GetMaxHeight();
        rect.x = x;
        rect.y = y + maxHeight * BAR_WIDTH_PERCENT; // status bar
        WMLOGFI("set layer: x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d", rect.x, rect.y, rect.w, rect.h);
        ret = display->SetRect(layerId_, rect);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("set rect fail, ret:%{public}d", ret);
            return;
        }
        SubWindow::Move(x, y);
#endif
        VIDEO_WINDOW_EXIT();
    }

    void VideoWindow::SetSubWindowSize(int32_t width, int32_t height)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            WMLOGFE("display layer is not create");
            return;
        }

#ifdef TARGET_CPU_ARM
        IRect rect = {};
        int32_t ret = display->GetRect(layerId_, rect);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("get rect fail, ret:%d", ret);
            return;
        }
        WMLOGFI("get layer: x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d", rect.x, rect.y, rect.w, rect.h);
        rect.w = width;
        rect.h = height;
        WMLOGFI("set layer: x=%{public}d, y=%{public}d, w=%{public}d, h=%{public}d", rect.x, rect.y, rect.w, rect.h);
        ret = display->SetRect(layerId_, rect);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("set rect fail, ret:%{public}d", ret);
            return;
        }
        SubWindow::SetSubWindowSize(width, height);
#endif
        VIDEO_WINDOW_EXIT();
    }

    int32_t VideoWindow::ZorderChange(uint32_t layerId, uint32_t zorder)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            WMLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
#ifdef TARGET_CPU_ARM
        int32_t ret = display->SetZorder(layerId, zorder);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("set zorder fail, ret:%{public}d", ret);
        }
#else
        int32_t ret = DISPLAY_FAILURE;
#endif
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    int32_t VideoWindow::TransformChange(uint32_t layerId, TransformType type)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            WMLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
#ifdef TARGET_CPU_ARM
        int32_t ret = display->SetTransformMode(layerId, type);
        if (ret != DISPLAY_SUCCESS) {
            WMLOGFW("set transform mode fail, ret:%{public}d", ret);
        }
#else
        int32_t ret = DISPLAY_FAILURE;
#endif
        VIDEO_WINDOW_EXIT();
        return ret;
    }
} // namespace OHOS
