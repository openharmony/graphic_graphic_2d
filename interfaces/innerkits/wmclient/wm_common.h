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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WM_COMMON_H
#define INTERFACES_INNERKITS_WMCLIENT_WM_COMMON_H

#include <functional>
#include <string>

#include "../common/graphic_common.h"

namespace OHOS {
enum WindowType {
    WINDOW_TYPE_NORMAL = 0,
    WINDOW_TYPE_STATUS_BAR = 1,
    WINDOW_TYPE_NAVI_BAR = 2,
    WINDOW_TYPE_ALARM_SCREEN = 3,
    WINDOW_TYPE_SYSTEM_UI = 4,
    WINDOW_TYPE_LAUNCHER = 5,
    WINDOW_TYPE_VIDEO = 6,
    WINDOW_TYPE_MAX,
};

enum SubWindowType {
    WM_WINDOW_TYPE_SUB_NORMAL = 0,
    WM_WINDOW_TYPE_SUB_VIDEO = 1,
};

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

struct WMImageInfo {
    WMError wret;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t size;
    const void *data;
};

struct WindowInfo {
    int32_t width;
    int32_t height;
    int32_t pos_x;
    int32_t pos_y;
};

using funcWindowInfoChange = std::function<void(WindowInfo &info)>;

using FuncSync = void (*)(uint64_t timestamp);

using funcOnKey = std::function<bool(KeyEvent)>;
using funcOnTouch = std::function<bool(TouchEvent)>;

using WindowInfoChangeFunc = std::function<void(WindowInfo &info)>;
using OnKeyFunc = std::function<bool(KeyEvent)>;
using OnTouchFunc = std::function<bool(TouchEvent)>;

using WindowPositionChangeFunc   = std::function<void(int32_t x, int32_t y)>;
using WindowSizeChangeFunc       = std::function<void(uint32_t width, uint32_t height)>;
using WindowVisibilityChangeFunc = std::function<void(bool visibility)>;
using WindowTypeChangeFunc       = std::function<void(WindowType type)>;

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
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WM_COMMON_H
