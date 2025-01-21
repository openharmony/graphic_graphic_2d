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
#include <shared_mutex>
#include <condition_variable>
#include "render_context/render_context.h"
#include "event_handler.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "screen_manager/screen_types.h"
#include "feature/round_corner_display/rs_round_corner_config.h"

namespace OHOS {
namespace Rosen {
enum class ScreenRotation : uint32_t;
constexpr char TOPIC_RCD_DISPLAY_SIZE[] = "RCD_UPDATE_DISPLAY_SIZE";
constexpr char TOPIC_RCD_DISPLAY_ROTATION[] = "RCD_UPDATE_DISPLAY_ROTATION";
constexpr char TOPIC_RCD_DISPLAY_NOTCH[] = "RCD_UPDATE_DISPLAY_NOTCH";
constexpr char TOPIC_RCD_RESOURCE_CHANGE[] = "TOPIC_RCD_RESOURCE_CHANGE";
constexpr char TOPIC_RCD_DISPLAY_HWRESOURCE[] = "RCD_UPDATE_DISPLAY_HWRESOURCE";

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

enum class RoundCornerDirtyType : uint8_t {
    // Use bit operation to judge dirty type
    RCD_DIRTY_NONE = 0x00,
    RCD_DIRTY_TOP = 0x01,
    RCD_DIRTY_BOTTOM = 0x02,
    RCD_DIRTY_ALL = 0x03
};

using RectU = RectT<uint32_t>;

class RoundCornerDisplay {
public:
    RoundCornerDisplay() {};
    explicit RoundCornerDisplay(NodeId id);
    virtual ~RoundCornerDisplay();

    // update Render Rect
    void UpdateDisplayParameter(uint32_t left, uint32_t top, uint32_t width, uint32_t height);

    // update notchStatus_
    void UpdateNotchStatus(int status);

    // update curOrientation_ and lastOrientation_
    void UpdateOrientationStatus(ScreenRotation orientation);

    // update hardInfo_.resourceChanged after hw resource applied
    void UpdateHardwareResourcePrepared(bool prepared);

    void DrawTopRoundCorner(RSPaintFilterCanvas* canvas);

    void DrawBottomRoundCorner(RSPaintFilterCanvas* canvas);

    bool HandleTopRcdDirty(RectI& dirtyRect);

    bool HandleBottomRcdDirty(RectI& dirtyRect);

    bool IsSupportHardware() const
    {
        return supportHardware_;
    }

    void RunHardwareTask(const std::function<void()>& task)
    {
        if (!supportHardware_) {
            return;
        }
        UpdateParameter(updateFlag_);
        task(); // do task
    }

    rs_rcd::RoundCornerHardware GetHardwareInfo()
    {
        std::shared_lock<std::shared_mutex> lock(resourceMut_);
        return hardInfo_;
    }

    rs_rcd::RoundCornerHardware GetHardwareInfoPreparing()
    {
        std::unique_lock<std::shared_mutex> lock(resourceMut_);
        if (hardInfo_.resourceChanged) {
            hardInfo_.resourcePreparing = true;
        }
        return hardInfo_;
    }

    bool IsNotchNeedUpdate(bool notchStatus)
    {
        std::shared_lock<std::shared_mutex> lock(resourceMut_);
        bool result = notchStatus != lastNotchStatus_;
        lastNotchStatus_ = notchStatus;
        return result;
    }

    void InitOnce();

private:
    NodeId renderTargetId_ = 0;
    bool isInit = false;
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
    // current display resolution
    RectU displayRect_;

    // last time rcv display resolution and notchState
    RectU lastRcvDisplayRect_;

    // status of the notch
    int notchStatus_ = WINDOW_NOTCH_DEFAULT;

    // type of rcd dirty region
    RoundCornerDirtyType rcdDirtyType_ = RoundCornerDirtyType::RCD_DIRTY_NONE;

    int showResourceType_ = (notchStatus_ == WINDOW_NOTCH_DEFAULT) ? TOP_PORTRAIT : TOP_HIDDEN;
    bool lastNotchStatus_ = false;

    // status of the rotation
    ScreenRotation curOrientation_ = ScreenRotation::ROTATION_0;
    ScreenRotation lastOrientation_ = ScreenRotation::ROTATION_0;

    bool supportTopSurface_ = false;
    bool supportBottomSurface_ = false;
    bool supportHardware_ = false;
    bool resourceChanged = false;

    // the resource to be drawn
    std::shared_ptr<Drawing::Image> curTop_ = nullptr;
    std::shared_ptr<Drawing::Image> curBottom_ = nullptr;

    std::shared_mutex resourceMut_;

    rs_rcd::RoundCornerHardware hardInfo_;

    bool Init();

    static bool LoadConfigFile();

    // choose LCD mode
    bool SeletedLcdModel(const char* lcdModelName);

    // load single image as Drawingimage
    static bool LoadImg(const char* path, std::shared_ptr<Drawing::Image>& img);

    static bool DecodeBitmap(std::shared_ptr<Drawing::Image> image, Drawing::Bitmap &bitmap);
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

    void PrintRCDInfo();

    // Check if the input resolution changes
    bool CheckResolutionChanged(uint32_t width, uint32_t height);
};
} // namespace Rosen
} // namespace OHOS
#endif