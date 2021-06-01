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

#ifndef FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_BASE_H
#define FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_BASE_H

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "window_manager_common.h"

namespace OHOS {
class IWindowManagerService : public IRemoteBroker {
public:
    enum {
        WM_INIT = 1,
        WM_CREATE_WINDOW,
        WM_RESIZE_WINDOW,
        WM_SWITCH_TOP,
        WM_DESTROY_WINDOW,
        WM_HIDE,
        WM_SHOW,
        WM_MOVE
    };
    virtual int32_t CreateWindow(WindowConfig& config) = 0;
    virtual void SwitchTop(int32_t windowID) = 0;
    virtual void DestroyWindow(int32_t windowID) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IWindowManagerService");
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WINDOW_MANAGER_BASE_H
