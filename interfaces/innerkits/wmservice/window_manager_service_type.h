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

#include <display_type.h>
#include <promise.h>
#include <window_manager_type.h>

namespace OHOS {
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

struct PowerStatus {
    WMError wret;
    DispPowerStatus status;
};

struct Backlight {
    WMError wret;
    uint32_t level;
};

using PromiseWMError = Promise<WMError>;
using PromiseWMSImageInfo = Promise<WMSImageInfo>;
using PromisePowerStatus = Promise<PowerStatus>;
using PromiseBacklight = Promise<Backlight>;
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMSERVICE_WINDOW_MANAGER_SERVICE_TYPE_H
