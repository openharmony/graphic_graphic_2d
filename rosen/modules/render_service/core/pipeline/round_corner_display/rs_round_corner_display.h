/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_DISPLAY_H
#define RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_RCD_DISPLAY_H

#pragma once
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "render_context/render_context.h"
#include "event_handler.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "screen_manager/screen_types.h"
#include "pipeline/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {
enum class ScreenRotation : uint32_t;

// On the devices that LCD/AMOLED contain notch, at settings-->display-->notch
// we can set default or hide notch.
enum WindowNotchStatus {
    // Notch default setting show the notch on the screen, and the
    // single/time/battery status show at the same horizontal.
    WINDOW_NOTCH_DEFAULT = 0,

    // Notch hidden setting fill the status bar with black, so
    // single/time/battery status show on the backgound of black.
    WINDOW_NOTCH_HIDDEN
};

enum ShowTopResourceType {
    // choose type and then choose resource for harden or RS
    TOP_PORTRAIT = 0,
    TOP_LADS_ORIT,
    TOP_HIDDEN
};

enum RoundCornerSurfaceType {
    // choose type and then choose resource for harden or RS
    TOP_SURFACE = 0,
    BOTTOM_SURFACE
};

class RoundCornerDisplay {
public:
    RoundCornerDisplay();
    virtual ~RoundCornerDisplay();

    // update displayWidth_ and displayHeight_
    void UpdateDisplayParameter(uint32_t width, uint32_t height);

    // update notchStatus_
    void UpdateNotchStatus(int status);

    // update curOrientation_ and lastOrientation_
    void UpdateOrientationStatus(ScreenRotation orientation);

    void DrawRoundCorner(RSPaintFilterCanvas* canvas);

    void DrawTopRoundCorner(RSPaintFilterCanvas* canvas);

    void DrawBottomRoundCorner(RSPaintFilterCanvas* canvas);

    bool IsSupportHardware() const
    {
        return supportHardware_;
    }

    void RunHardwareTask(const std::function<void()>& task)
    {
        std::lock_guard<std::mutex> lock(resourceMut_);
        if (!supportHardware_) {
            return;
        }
        UpdateParameter(updateFlag_);
        task(); // do task
    }
    
    rs_rcd::RoundCornerHardware GetHardwareInfo() const
    {
        return hardInfo_;
    }

    bool GetRcdEnable() const
    {
        return isRcdEnable_;
    }

private:
    // load config
    rs_rcd::LCDModel* lcdModel_ = nullptr;
    rs_rcd::ROGSetting* rog_ = nullptr;

    std::map<std::string, bool> updateFlag_ = {
        // true of change
        {"display", false},
        {"notch", false},
        {"orientation", false}
    };

    // notch resources
    std::shared_ptr<Drawing::Image> imgTopPortrait_ = nullptr;
    std::shared_ptr<Drawing::Image> imgTopLadsOrit_ = nullptr;
    std::shared_ptr<Drawing::Image> imgTopHidden_ = nullptr;
    std::shared_ptr<Drawing::Image> imgBottomPortrait_ = nullptr;

    // notch resources for harden
    Drawing::Bitmap bitmapTopPortrait_;
    Drawing::Bitmap bitmapTopLadsOrit_;
    Drawing::Bitmap bitmapTopHidden_;
    Drawing::Bitmap bitmapBottomPortrait_;
    // display resolution
    uint32_t displayWidth_ = 0;
    uint32_t displayHeight_ = 0;

    // status of the notch
    int notchStatus_ = WINDOW_NOTCH_DEFAULT;

    int showResourceType_ = (notchStatus_ == WINDOW_NOTCH_DEFAULT) ? TOP_PORTRAIT : TOP_HIDDEN;

    // status of the rotation
    ScreenRotation curOrientation_ = ScreenRotation::ROTATION_0;
    ScreenRotation lastOrientation_ = ScreenRotation::ROTATION_0;

    bool supportTopSurface_ = false;
    bool supportBottomSurface_ = false;
    bool supportHardware_ = false;
    bool resourceChanged = false;

    bool isRcdEnable_ = false;

    // the resource to be drawn
    std::shared_ptr<Drawing::Image> curTop_ = nullptr;
    std::shared_ptr<Drawing::Image> curBottom_ = nullptr;

    std::mutex resourceMut_;

    rs_rcd::RoundCornerHardware hardInfo_;

    bool Init();

    static bool LoadConfigFile();

    // choose LCD mode
    bool SeletedLcdModel(const char* lcdModelName);

    // load single image as Drawingimage
    static bool LoadImg(const char* path, std::shared_ptr<Drawing::Image>& img);

    static bool DecodeBitmap(std::shared_ptr<Drawing::Image> drImage, Drawing::Bitmap &bitmap);
    bool SetHardwareLayerSize();

    // load all images according to the resolution
    bool LoadImgsbyResolution(uint32_t width, uint32_t height);

    bool GetTopSurfaceSource();

    bool GetBottomSurfaceSource();

    void DrawOneRoundCorner(RSPaintFilterCanvas* canvas, int surfaceType);

    // update resource
    void UpdateParameter(std::map<std::string, bool> &updateFlag);

    // choose top rcd resource type
    void RcdChooseTopResourceType();

    void RcdChooseRSResource();

    void RcdChooseHardwareResource();
};
} // namespace Rosen
} // namespace OHOS
#endif