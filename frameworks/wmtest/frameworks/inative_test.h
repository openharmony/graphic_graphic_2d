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

#ifndef FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H
#define FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include <ipc_object_stub.h>
#include <key_event_handler.h>
#include <multimodal_event_handler.h>
#include <touch_event_handler.h>
#include <vsync_helper.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

namespace OHOS {
class INativeTest;
using VisitTestFunc = std::function<bool(INativeTest *)>;

enum class AutoLoadService : int32_t {
    Null                 = 0,
    WindowManager        = 1 << 0,
    WindowManagerService = 1 << 1,
};
enum AutoLoadService operator |(const enum AutoLoadService &l, const enum AutoLoadService &r);
bool operator &(const enum AutoLoadService &l, const enum AutoLoadService &r);

class INativeTest {
public:
    enum {
        LAST_TIME_FOREVER = 999999999,
    };

    static INativeTest *VisitTests(VisitTestFunc func);
    INativeTest();
    virtual ~INativeTest() = default;

    virtual std::string GetDescription() const = 0;
    virtual std::string GetDomain() const = 0;
    virtual int32_t GetID() const = 0;
    virtual void Run(int32_t argc, const char **argv) = 0;
    virtual uint32_t GetLastTime() const;

    // auto load service
    virtual enum AutoLoadService GetAutoLoadService() const;
    sptr<WindowManager> windowManager = nullptr;
    sptr<IWindowManagerService> windowManagerService = nullptr;

    // input
    virtual bool OnKey(const KeyEvent &event);
    virtual bool OnTouch(const TouchEvent &event);
    void GetToken();
    int32_t ListenWindowKeyEvent(int32_t windowID);
    int32_t ListenWindowTouchEvent(int32_t windowID);
    void ListenWindowInputEvent(int32_t windowID); // key and touch

    // multiple process
    virtual int32_t GetProcessNumber() const;
    virtual int32_t GetProcessSequence() const;

    const char **processArgv = nullptr;
    std::vector<const char *> extraArgs;
    int32_t StartSubprocess(int32_t id);

    // thread pool
    void SetEventHandler(const std::shared_ptr<AppExecFwk::EventHandler> &handler);
    void PostTask(std::function<void()> func, uint32_t delayTime = 0) const;
    void ExitTest() const;

private:
    static inline std::vector<INativeTest *> tests;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sptr<MMI::KeyEventHandler> keyHandler = nullptr;
    sptr<MMI::TouchEventHandler> touchHandler = nullptr;
    sptr<IRemoteObject> token = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_H
