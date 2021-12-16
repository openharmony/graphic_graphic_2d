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

#ifndef INTERFACES_INNERKITS_WMSERVICE_IWINDOW_MANAGER_SERVICE_H
#define INTERFACES_INNERKITS_WMSERVICE_IWINDOW_MANAGER_SERVICE_H

#include <vector>

#include <display_type.h>
#include <promise.h>
#include <refbase.h>
#include <window_manager_type.h>

#include "iwindow_change_listener_clazz.h"
#include "iwindow_manager_display_listener_clazz.h"
#include "window_manager_service_type.h"

namespace OHOS {
class IWindowManagerService : public RefBase {
public:
    virtual ~IWindowManagerService() = default;

    virtual WMError GetDisplays(std::vector<struct WMDisplayInfo> &displays) = 0;
    virtual sptr<PromisePowerStatus> GetDisplayPower(int32_t did) = 0;
    virtual sptr<PromiseWMError> SetDisplayPower(int32_t did, DispPowerStatus status) = 0;
    virtual sptr<PromiseBacklight> GetDisplayBacklight(int32_t did) = 0;
    virtual sptr<PromiseWMError> SetDisplayBacklight(int32_t did, uint32_t level) = 0;
    virtual WMError GetDisplayModes(uint32_t &displayModes) = 0;
    virtual sptr<PromiseWMError> SetDisplayMode(WMSDisplayMode modes) = 0;
    virtual WMError AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener) = 0;
    virtual sptr<PromiseWMError> OnWindowListChange(IWindowChangeListenerClazz *listener) = 0;

    virtual WMError SetDisplayDirection(WMSDisplayDirection direction) = 0;
    virtual WMError OnDisplayDirectionChange(DisplayDirectionChangeFunc func) = 0;

    virtual sptr<PromiseWMError> SetStatusBarVisibility(bool visibility) = 0;
    virtual sptr<PromiseWMError> SetNavigationBarVisibility(bool visibility) = 0;

    virtual sptr<PromiseWMSImageInfo> ShotScreen(int32_t did) = 0;
    virtual sptr<PromiseWMSImageInfo> ShotWindow(int32_t wid) = 0;

    virtual sptr<PromiseWMError> DestroyWindow(int32_t wid) = 0;
    virtual sptr<PromiseWMError> SwitchTop(int32_t wid) = 0;
    virtual sptr<PromiseWMError> Show(int32_t wid) = 0;
    virtual sptr<PromiseWMError> Hide(int32_t wid) = 0;
    virtual sptr<PromiseWMError> Move(int32_t wid, int32_t x, int32_t y) = 0;
    virtual sptr<PromiseWMError> Resize(int32_t wid, uint32_t width, uint32_t height) = 0;
    virtual sptr<PromiseWMError> ScaleTo(int32_t wid, uint32_t width, uint32_t height) = 0;
    virtual sptr<PromiseWMError> SetWindowType(int32_t wid, WindowType type) = 0;
    virtual sptr<PromiseWMError> SetWindowMode(int32_t wid, WindowMode mode) = 0;

    virtual sptr<PromiseWMError> CreateVirtualDisplay(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
    virtual sptr<PromiseWMError> DestroyVirtualDisplay(uint32_t did) = 0;

    virtual GSError StartRotationAnimation(uint32_t did, int32_t degree) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMSERVICE_IWINDOW_MANAGER_SERVICE_H
