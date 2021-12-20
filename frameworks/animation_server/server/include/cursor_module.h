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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_CURSOR_MODULE_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_CURSOR_MODULE_H

#include <vector>

#include <event_handler.h>
#include <graphic_common.h>
#include <ipc_object_stub.h>
#include <raw_parser.h>
#include <touch_event_handler.h>
#include <window_manager.h>

namespace OHOS {
class CursorModule : MMI::TouchEventHandler {
public:
    GSError Init();

private:
    void Update();
    bool OnTouch(const TouchEvent &event) override;

    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    std::vector<struct WMDisplayInfo> displays;
    RawParser resource;
    sptr<Window> window = nullptr;
    static constexpr const char *cursorFilepath = "/etc/cursor.raw";
    sptr<IRemoteObject> token_ = new IPCObjectStub(u"cursor_module");
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_CURSOR_MODULE_H
