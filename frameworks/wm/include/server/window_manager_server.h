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

#ifndef FRAMEWORKS_WM_INCLUDE_SERVER_WINDOW_MANAGER_SERVER_H
#define FRAMEWORKS_WM_INCLUDE_SERVER_WINDOW_MANAGER_SERVER_H

#include "window_manager_stub.h"

namespace OHOS {
class WindowManagerServer : public WindowManagerStub {
public:
    virtual int32_t CreateWindow(WindowConfig& config) override;
    virtual void SwitchTop(int windowID) override;
    virtual void DestroyWindow(int windowID) override;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_SERVER_WINDOW_MANAGER_SERVER_H
