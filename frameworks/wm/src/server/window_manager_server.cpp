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
#include "window_manager_server.h"

#include "layer_controller.h"
#include "window_manager_hilog.h"

namespace OHOS {
int32_t WindowManagerServer::CreateWindow(WindowConfig& config)
{
    return LayerController::GetInstance()->CreateWindow(GetCallingPid(), config);
}

void WindowManagerServer::SwitchTop(int32_t windowID)
{
    WMLOG_I("WindowManagerServer::SwitchTop");
    LayerController::GetInstance()->ChangeWindowTop(windowID);
}

void WindowManagerServer::DestroyWindow(int32_t windowID)
{
    WMLOG_I("WindowManagerServer::DestroyWindow");
    LayerController::GetInstance()->DestroyWindow(windowID);
}
} // namespace OHOS
