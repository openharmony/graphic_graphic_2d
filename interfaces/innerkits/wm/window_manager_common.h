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

#ifndef INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H
#define INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H

#include <functional>
namespace OHOS {
#define WM_VOLIB_NOT_SUPPORT

typedef enum {
    WINDOW_LAYER_DEFINE_NORMAL_ID = 5000,
    WINDOW_LAYER_DEFINE_STATUSBAR_ID = 6000,
    WINDOW_LAYER_DEFINE_NAVIBAR_ID = 7000,
    WINDOW_LAYER_DEFINE_ALARM_ID = 8000,
} LayerID;

constexpr int32_t LAYER_ID_APP_TYPE_BASE = 5000;
constexpr int32_t LAYER_ID_TYPE_OFSSET = 1000;

typedef enum {
    WINDOW_TYPE_NORMAL = 0,
    WINDOW_TYPE_STATUS_BAR = 1,
    WINDOW_TYPE_NAVI_BAR = 2,
    WINDOW_TYPE_ALARM_SCREEN = 3,
    WINDOW_TYPE_SYSTEM_UI = 4,
    WINDOW_TYPE_LAUNCHER = 5,
    WINDOW_TYPE_VIDEO = 6,
} WindowType;

struct FormatTbl {
    int32_t pix_fmt;
    uint32_t drm_fmt;
};

typedef enum {
    WM_WINDOW_TYPE_SUB_NORMAL = 0,
    WM_WINDOW_TYPE_SUB_VIDEO = 1,
} SubWindowType;

typedef enum {
    SHOT_WINDOW = 0,
    SHOT_SCREEN,
    SHOT_INVALID
} ShotType;

class TouchEvent;
class KeyEvent;

enum rotateType {
    /**
    * no transform
    */
    WM_ROTATE_TYPE_NORMAL = 0,
    /**
    * 90 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_90 = 1,
    /**
    * 180 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_180 = 2,
    /**
    * 270 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_270 = 3,
    /**
    * 180 degree flip around a vertical axis
    */
    WM_ROTATE_TYPE_FLIPPED = 4,
    /**
    * flip and rotate 90 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_FLIPPED_90 = 5,
    /**
    * flip and rotate 180 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_FLIPPED_180 = 6,
    /**
    * flip and rotate 270 degrees counter-clockwise
    */
    WM_ROTATE_TYPE_FLIPPED_270 = 7,
};

struct ImageHeader {
    /** Color format, which is used to match image type. This variable is important. */
    uint32_t colorMode;
    uint32_t reserved;
    /** Image width */
    uint16_t width;
    /** Image height */
    uint16_t height;
};

/**
* @ Defines image information.
*/
struct ImageInfo {
    /** Image head node information. For details, see {@link ImageHeader}. */
    ImageHeader header;
    /** Size of the image data (in bytes) */
    uint32_t dataSize;
    /** Pixel color data of pixelmap images */
    const uint8_t* data;
    /** User-defined data */
    void* userData;
    /** Size of the User-defined data (in bytes) */
    uint32_t userDataSize;
};

struct WindowInfo {
    int32_t width;
    int32_t height;
    int32_t pos_x;
    int32_t pos_y;
};

using funcWindowInfoChange = void (*)(WindowInfo &info);

using FuncShotDone = void (*)(ImageInfo& imageInfo);
using FuncSync = void (*)(uint64_t timestamp);

using funcPointerEnter = void (*)(int32_t x, int32_t y, int32_t serila);
using funcPointerLeave = void (*)(int32_t serial);
using funcPointerMotion = void (*)(int32_t x, int32_t y, int32_t time);
using funcPointerFrame = void (*)();
using funcPointerAxisSource = void (*)(int32_t axisSource);
using funcPointerAxis = void (*)(int32_t time, int32_t axis, int32_t value);
using funcPointerAxisStop = void (*)(int32_t time, int32_t axis);
using funcPointerAxisDiscrete = void (*)(int32_t axis, int32_t discrete);
using funcPointerButton = void (*)(int32_t serial, int32_t button, int32_t state, int32_t time);

using funcTouchFrame = void (*)();
using funcTouchCancel = void (*)();
using funcTouchShape = void (*)(int32_t major, int32_t minor);
using funcTouchOrientation = void (*)(int32_t id, int32_t Orientation);
using funcTouchDown = void (*)(int32_t x, int32_t y, int32_t serial, int32_t time, int32_t id);
using funcTouchUp = void (*)(int32_t serial, int32_t time, int32_t id);
using funcTouchEmotion = void (*)(int32_t x, int32_t y, int32_t time, int32_t id);
using funcOnKey = std::function<bool(KeyEvent)>;
using funcOnTouch = std::function<bool(TouchEvent)>;

struct WindowConfig {
    int32_t width;
    int32_t height;
    int32_t pos_x;
    int32_t pos_y;
    int32_t format;
    int32_t stride;
    int32_t type;
    int32_t parentid;
    bool subwindow;
    FuncSync sync;
};

struct MoveReq {
    int32_t id;
    int32_t pos_x;
    int32_t pos_y;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WM_WINDOW_MANAGER_COMMON_H
