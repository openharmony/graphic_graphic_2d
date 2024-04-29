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

#ifdef __cplusplus
#include <functional>
#include <string>
#include <map>
#endif

#include "common/graphic_common.h"

#ifdef __cplusplus
namespace OHOS {
#endif

#define WINDOW_TYPE_LIST(macro) \
    macro(WINDOW_TYPE_NORMAL), \
    macro(WINDOW_TYPE_STATUS_BAR), \
    macro(WINDOW_TYPE_NAVI_BAR), \
    macro(WINDOW_TYPE_ALARM_SCREEN), \
    macro(WINDOW_TYPE_SYSTEM_UI), \
    macro(WINDOW_TYPE_LAUNCHER), \
    macro(WINDOW_TYPE_VIDEO), \
    macro(WINDOW_TYPE_INPUT_METHOD), \
    macro(WINDOW_TYPE_INPUT_METHOD_SELECTOR), \
    macro(WINDOW_TYPE_VOLUME_OVERLAY), \
    macro(WINDOW_TYPE_NOTIFICATION_SHADE), \
    macro(WINDOW_TYPE_FLOAT), \
    macro(WINDOW_TYPE_CALLING), \
    macro(WINDOW_TYPE_FULL_SCREEN), \
    macro(WINDOW_TYPE_POPUP), \
    macro(WINDOW_TYPE_SAFE_OVERLAY), \
    macro(WINDOW_TYPE_SCREENSAVER_CALLING), \
    macro(WINDOW_TYPE_SCREENSAVER_DIALOG), \
    macro(WINDOW_TYPE_SCREENSAVER), \
    macro(WINDOW_TYPE_SEARCHER), \
    macro(WINDOW_TYPE_STATUS_SCREEN), \
    macro(WINDOW_TYPE_SYSTEM_ERROR), \
    macro(WINDOW_TYPE_TOAST), \
    macro(WINDOW_TYPE_WALLPAPER), \
    macro(WINDOW_TYPE_ANIMATION), \
    macro(WINDOW_TYPE_SPLIT_LINE), \
    macro(WINDOW_TYPE_LAUNCH_PAGE), \
    macro(WINDOW_TYPE_CURSOR), \
    macro(WINDOW_TYPE_APPLICATION_BASE), \
    macro(WINDOW_TYPE_APPLICATION_FIRST), \
    macro(WINDOW_TYPE_APPLICATION_NORMAL), \
    macro(WINDOW_TYPE_TOP), \
    macro(WINDOW_TYPE_MAX),

#define DEFINE_ENUM_WINDOW_TYPE(id) id
#define DEFINE_STRMAP_WINDOW_TYPE(id) { #id, id, }

enum WindowType { WINDOW_TYPE_LIST(DEFINE_ENUM_WINDOW_TYPE) };

#ifdef __cplusplus
static const std::map<std::string, enum WindowType> windowTypeStrs = {
    WINDOW_TYPE_LIST(DEFINE_STRMAP_WINDOW_TYPE)
};
#endif

enum SubWindowType {
    WM_WINDOW_TYPE_SUB_NORMAL = 0,
    WM_WINDOW_TYPE_SUB_VIDEO = 1,
};

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
    enum GSError wret;
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

#ifdef __cplusplus

using funcWindowInfoChange = std::function<void(WindowInfo &info)>;
using WindowPositionChangeFunc   = std::function<void(int32_t x, int32_t y)>;
using WindowSizeChangeFunc       = std::function<void(uint32_t width, uint32_t height)>;
using WindowVisibilityChangeFunc = std::function<void(bool visibility)>;
using WindowTypeChangeFunc       = std::function<void(WindowType type)>;
using WindowPIPModeChangeFunc    = std::function<void(bool inPIPMode)>;
#endif

typedef void (*FuncSync)(uint64_t timestamp);
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
#ifdef __cplusplus
} // namespace OHOS
#endif // __cplusplus

#endif // INTERFACES_INNERKITS_WMCLIENT_WM_COMMON_H
