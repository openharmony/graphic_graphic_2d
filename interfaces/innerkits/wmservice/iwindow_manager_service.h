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
    virtual WMError GetDisplayModes(uint32_t &displayModes) = 0;
    virtual sptr<Promise<WMError>> SetDisplayMode(WMSDisplayMode modes) = 0;
    virtual WMError AddDisplayChangeListener(IWindowManagerDisplayListenerClazz *listener) = 0;
    virtual sptr<Promise<WMError>> OnWindowListChange(IWindowChangeListenerClazz *listener) = 0;

    virtual WMError SetDisplayDirection(WMSDisplayDirection direction) = 0;
    virtual WMError OnDisplayDirectionChange(DisplayDirectionChangeFunc func) = 0;

    virtual sptr<Promise<WMSImageInfo>> ShotScreen(int32_t did) = 0;
    virtual sptr<Promise<WMSImageInfo>> ShotWindow(int32_t wid) = 0;

    virtual sptr<Promise<WMError>> DestroyWindow(int32_t wid) = 0;
    virtual sptr<Promise<WMError>> SwitchTop(int32_t wid) = 0;
    virtual sptr<Promise<WMError>> Show(int32_t wid) = 0;
    virtual sptr<Promise<WMError>> Hide(int32_t wid) = 0;
    virtual sptr<Promise<WMError>> Move(int32_t wid, int32_t x, int32_t y) = 0;
    virtual sptr<Promise<WMError>> Resize(int32_t wid, uint32_t width, uint32_t height) = 0;
    virtual sptr<Promise<WMError>> ScaleTo(int32_t wid, uint32_t width, uint32_t height) = 0;
    virtual sptr<Promise<WMError>> SetWindowType(int32_t wid, WindowType type) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMSERVICE_IWINDOW_MANAGER_SERVICE_H
