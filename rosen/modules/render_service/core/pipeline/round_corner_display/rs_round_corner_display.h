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

    void DrawRoundCorner(std::shared_ptr<RSPaintFilterCanvas> canvas);

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
    sk_sp<SkImage> imgTopPortrait_ = nullptr;
    sk_sp<SkImage> imgTopLadsOrit_ = nullptr;
    sk_sp<SkImage> imgTopHidden_ = nullptr;
    sk_sp<SkImage> imgBottomPortrait_ = nullptr;

    // display resolution
    uint32_t displayWidth_ = 0;
    uint32_t displayHeight_ = 0;

    // setting of the notch
    int notchSetting_ = WINDOW_NOTCH_DEFAULT;
    // status of the notch
    int notchStatus_ = notchSetting_;

    int showResourceType_ = TOP_PORTRAIT;

    // status of the rotation
    ScreenRotation curOrientation_ = ScreenRotation::ROTATION_0;
    ScreenRotation lastOrientation_ = ScreenRotation::ROTATION_0;

    bool supportTopSurface_ = false;
    bool supportBottomSurface_ = false;

    // the resource to be drawn
    sk_sp<SkImage> curTop_ = nullptr;
    sk_sp<SkImage> curBottom_ = nullptr;

    std::mutex resourceMut_;

    bool Init();

    bool LoadConfigFile();

    // choose LCD mode
    bool SeletedLcdModel(const char* lcdModelName);

    // load single image as skimage
    bool LoadImg(const char* path, sk_sp<SkImage>& img);

    // load all images according to the resolution
    bool LoadImgsbyResolution(uint32_t width, uint32_t height);

    bool GetTopSurfaceSource();

    bool GetBottomSurfaceSource();

    // update resource
    void UpdateParameter(std::map<std::string, bool> &updateFlag);

    // choose top rcd resource type
    void RcdChooseTopResourceType();

    void RcdChooseRSResource();
};
} // namespace Rosen
} // namespace OHOS
#endif