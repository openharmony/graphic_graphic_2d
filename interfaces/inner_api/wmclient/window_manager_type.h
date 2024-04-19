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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H

#ifdef __cplusplus
#include <functional>
#include <string>
#endif

#include "wm_common.h"

#include "common/graphic_common.h"

#ifdef __cplusplus
namespace OHOS {
#endif

enum SubwindowType {
    SUBWINDOW_TYPE_NORMAL = 0,
    SUBWINDOW_TYPE_VIDEO = 1,
    SUBWINDOW_TYPE_OFFSCREEN = 2,
    SUBWINDOW_TYPE_MAX,
};

enum WindowRotateType {
    WINDOW_ROTATE_TYPE_NORMAL = 0,
    WINDOW_ROTATE_TYPE_90 = 1,
    WINDOW_ROTATE_TYPE_180 = 2,
    WINDOW_ROTATE_TYPE_270 = 3,
    WINDOW_ROTATE_TYPE_FLIPPED = 4,
    WINDOW_ROTATE_TYPE_FLIPPED_90 = 5,
    WINDOW_ROTATE_TYPE_FLIPPED_180 = 6,
    WINDOW_ROTATE_TYPE_FLIPPED_270 = 7,
    WINDOW_ROTATE_TYPE_MAX,
};

enum WindowMode {
    WINDOW_MODE_UNSET = 0,
    WINDOW_MODE_FULL = 1,
    WINDOW_MODE_FREE = 2,
    WINDOW_MODE_MAX,
};

// bitmask
enum WMSDisplayMode {
    WM_DISPLAY_MODE_SINGLE = 1,
    WM_DISPLAY_MODE_CLONE = 2,
    WM_DISPLAY_MODE_EXTEND = 4,
    WM_DISPLAY_MODE_EXPAND = 8,
};

enum DisplayType {
    DISPLAY_TYPE_PHYSICAL = 0,
    DISPLAY_TYPE_VIRTUAL,
    DISPLAY_TYPE_MAX,
};

enum SplitStatus {
    SPLIT_STATUS_CLEAR = 0,
    SPLIT_STATUS_VAGUE = 1,
    SPLIT_STATUS_RETAIN = 2,
    SPLIT_STATUS_DESTROY = 3,
    SPLIT_STATUS_MAX,
};

enum WindowSplitMode {
    WINDOW_SPLIT_MODE_UNSET = 0,
    WINDOW_SPLIT_MODE_SET = 1,
    WINDOW_SPLIT_MODE_MAX,
};

enum SplitMode {
    SPLIT_MODE_NULL = 0,
    SPLIT_MODE_UNENABLE = 1,
    SPLIT_MODE_SINGLE = 2,
    SPLIT_MODE_SELECT = 3,
    SPLIT_MODE_CONFIRM = 4,
    SPLIT_MODE_DIVIDER_TOUCH_DOWN = 5,
    SPLIT_MODE_DIVIDER_TOUCH_MOVE = 6,
    SPLIT_MODE_DIVIDER_TOUCH_UP = 7,
    SPLIT_MODE_MAX,
};

struct WMDisplayInfo {
    int32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t vsync;
    enum DisplayType type;
};

#ifdef __cplusplus
using WindowModeChangeFunc = std::function<void(WindowMode mode)>;
using BeforeFrameSubmitFunc = std::function<void()>;
using SplitStatusChangeFunc = std::function<void(SplitStatus status)>;
} // namespace OHOS
#endif

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
