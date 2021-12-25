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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVER_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVER_H

#include <atomic>
#include <functional>
#include <unistd.h>

#include <promise.h>
#include <raw_parser.h>
#include <touch_event_handler.h>
#include <vsync_helper.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "animation_service_stub.h"
#include "animation_module.h"
#include "cursor_module.h"

namespace OHOS {
class AnimationServer : public AnimationServiceStub {
public:
    GSError Init();

    GSError StartRotationAnimation(int32_t did, int32_t degree) override;
    GSError SplitModeCreateBackground() override;
    GSError SplitModeCreateMiddleLine() override;
    GSError CreateLaunchPage(const std::string &filename) override;
    GSError CancelLaunchPage() override;

    void OnSplitStatusChange(SplitStatus status);
    bool OnTouch(const TouchEvent &event);

private:
    void SplitWindowUpdate();
    void SplitWindowDraw(uint32_t *vaddr, uint32_t width, uint32_t height, uint32_t count);

    void LaunchPageWindowUpdate();

    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    AnimationModule animationModule;

    sptr<Window> splitWindow = nullptr;
    bool haveMiddleLine = false;
    bool midlineDown = false;
    int32_t midlineYBackup = -100;
    int32_t midlineY = -100;
    int32_t downX = 0;
    int32_t downY = 0;
    sptr<Window> launchPageWindow = nullptr;
    RawParser resource;

    class TouchEventHandler : public MMI::TouchEventHandler {
    public:
        explicit TouchEventHandler(AnimationServer *server) : server_(server)
        {
        }

        virtual bool OnTouch(const TouchEvent &event) override
        {
            return server_->OnTouch(event);
        }

    private:
        AnimationServer *server_ = nullptr;
    };
    sptr<TouchEventHandler> thandler = nullptr;
    sptr<IRemoteObject> token = new IPCObjectStub(u"animation_server");
    CursorModule cursorModule;
};
} // namespace OHOS

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_ANIMATION_SERVER_H
