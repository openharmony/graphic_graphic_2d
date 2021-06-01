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
#include "window_manager_proxy.h"
#include "window_manager_hilog.h"
namespace OHOS {
WindowManagerProxy::WindowManagerProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IWindowManagerService>(impl)
{
}

WindowManagerProxy::~WindowManagerProxy()
{
}

void WindowManagerProxy::SwitchTop(int windowID)
{
    WMLOG_I("WindowManagerProxy::SwitchTop start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        WMLOG_E("write interface token failed");
    }
    in.WriteInt32(windowID);
    int error = Remote()->SendRequest(WM_SWITCH_TOP, in, out, option);
    if (error != ERR_NONE) {
        return;
    }
    return;
}

int32_t WindowManagerProxy::CreateWindow(WindowConfig& config)
{
    WMLOG_I("WindowManagerProxy::CreateWindow start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        WMLOG_E("write interface token failed");
    }
    in.WriteRawData(&config, sizeof(WindowConfig));
    int error = Remote()->SendRequest(WM_CREATE_WINDOW, in, out, option);
    WMLOG_I("WindowManagerProxy::CreateWindow SendRequest ret=%{public}d", error);
    if (error == ERR_NONE) {
        int ret = out.ReadInt32();
        return ret;
    }
    return 0;
}

void WindowManagerProxy::DestroyWindow(int windowID)
{
    WMLOG_I("WindowManagerProxy::DestroyWindow start");
    MessageOption option;
    MessageParcel in, out;
    if (!in.WriteInterfaceToken(GetDescriptor())) {
        WMLOG_E("write interface token failed");
    }
    in.WriteInt32(windowID);
    int error = Remote()->SendRequest(WM_DESTROY_WINDOW, in, out, option);
    if (error != ERR_NONE) {
        return;
    }
    return;
}
} // namespace OHOS
