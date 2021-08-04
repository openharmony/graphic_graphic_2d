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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_H

#include <vector>

#include <refbase.h>

#include "iscreen_shot_callback.h"
#include "iwindow_shot_callback.h"
#include "subwindow.h"
#include "subwindow_option.h"
#include "window.h"
#include "window_manager_type.h"
#include "window_option.h"

namespace OHOS {
class WindowManager : public RefBase {
public:
    static sptr<WindowManager> GetInstance();

    virtual WMError Init() = 0;

    virtual WMError GetDisplays(std::vector<struct WMDisplayInfo> &displays) const = 0;

    virtual WMError CreateWindow(sptr<Window> &window, const sptr<WindowOption> &option) = 0;
    virtual WMError CreateSubwindow(sptr<Subwindow> &subwindow,
                                    const sptr<Window> &window,
                                    const sptr<SubwindowOption> &option) = 0;

    virtual WMError ListenNextScreenShot(int32_t id, IScreenShotCallback *cb) = 0;
    virtual WMError ListenNextWindowShot(const sptr<Window> &window, IWindowShotCallback *cb) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_H
