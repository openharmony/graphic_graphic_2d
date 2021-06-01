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
#include "buffer_log.h"

#define VIDEO_WINDOW_DEBUG

#ifndef VIDEO_WINDOW_DEBUG
#define VIDEO_WINDOW_ENTER() ((void)0)
#define VIDEO_WINDOW_EXIT() ((void)0)
#else
#define VIDEO_WINDOW_ENTER() do { \
    BLOGFD("enter..."); \
} while (0)

#define VIDEO_WINDOW_EXIT() do { \
    BLOGFD("exit..."); \
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
        display = new (std::nothrow) VideoDisplayManager();
        if (display == nullptr) {
            BLOGFE("new video display manager fail");
        }
        Init();
        VIDEO_WINDOW_EXIT();
    }

    VideoWindow::~VideoWindow()
    {
        VIDEO_WINDOW_ENTER();
        if (display != nullptr) {
            display->DetachLayer(layerId_);
        }
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
        int32_t ret = VideoDisplayManager::CreateLayer(layerInfo, layerId, surface);
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    void VideoWindow::DestroyLayer(uint32_t layerId)
    {
        VIDEO_WINDOW_ENTER();
        VideoDisplayManager::DestroyLayer(layerId);
        VIDEO_WINDOW_EXIT();
    }

    int32_t VideoWindow::Init()
    {
        VIDEO_WINDOW_ENTER();
        int32_t ret = DISPLAY_SUCCESS;
        if (display == nullptr) {
            BLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
        producer = display->AttachLayer(surface_, layerId_);
        if (producer == nullptr) {
            BLOGFE("attach layer fail");
            ret = DISPLAY_FAILURE;
        }
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

    int32_t VideoWindow::DisplayRectChange(uint32_t layerId, IRect rect)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            BLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
        int32_t ret = display->SetRect(layerId, rect);
        if (ret != DISPLAY_SUCCESS) {
            BLOGFW("set rect fail, ret:%d", ret);
        }
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    int32_t VideoWindow::ZorderChange(uint32_t layerId, uint32_t zorder)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            BLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
        int32_t ret = display->SetZorder(layerId, zorder);
        if (ret != DISPLAY_SUCCESS) {
            BLOGFW("set zorder fail, ret:%d", ret);
        }
        VIDEO_WINDOW_EXIT();
        return ret;
    }

    int32_t VideoWindow::TransformChange(uint32_t layerId, TransformType type)
    {
        VIDEO_WINDOW_ENTER();
        if (display == nullptr) {
            BLOGFE("display layer is not create");
            return DISPLAY_FAILURE;
        }
        int32_t ret = display->SetTransformMode(layerId, type);
        if (ret != DISPLAY_SUCCESS) {
            BLOGFW("set transform mode fail, ret:%d", ret);
        }
        VIDEO_WINDOW_EXIT();
        return ret;
    }
} // namespace OHOS
