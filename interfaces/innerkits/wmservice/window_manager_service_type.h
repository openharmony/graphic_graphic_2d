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

#ifndef INTERFACES_INNERKITS_WMSERVICE_WINDOW_MANAGER_SERVICE_TYPE_H
#define INTERFACES_INNERKITS_WMSERVICE_WINDOW_MANAGER_SERVICE_TYPE_H

#include <functional>

#include <window_manager_type.h>

namespace OHOS {
// bitmask
enum WMSDisplayMode {
    WM_DISPLAY_MODE_SINGLE = 1,
    WM_DISPLAY_MODE_CLONE = 2,
    WM_DISPLAY_MODE_EXTEND = 4,
    WM_DISPLAY_MODE_EXPAND = 8,
};

enum WMSDisplayDirection {
    WMS_DISPLAY_DIRECTION_DOWN = 0,
    WMS_DISPLAY_DIRECTION_LEFT = 1,
    WMS_DISPLAY_DIRECTION_UP = 2,
    WMS_DISPLAY_DIRECTION_RIGHT = 3,
};

using DisplayDirectionChangeFunc = std::function<void(WMSDisplayDirection newDirection)>;

struct WMSImageInfo {
    WMError wret;
    int32_t fd;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint64_t timestamp;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMSERVICE_WINDOW_MANAGER_SERVICE_TYPE_H
