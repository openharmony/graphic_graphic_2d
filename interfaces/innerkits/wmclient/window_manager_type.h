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

#include "wm_common.h"

namespace OHOS {
enum SubwindowType {
    SUBWINDOW_TYPE_NORMAL = 0,
    SUBWINDOW_TYPE_VIDEO = 1,
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
    WINDOW_MODE_FREE = 1,
    WINDOW_MODE_TOP = 2,
    WINDOW_MODE_MAX,
};

struct WMDisplayInfo {
    int32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t vsync;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
