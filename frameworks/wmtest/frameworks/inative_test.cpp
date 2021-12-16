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

#include "inative_test.h"

#include <algorithm>
#include <codecvt>
#include <gslogger.h>
#include <sstream>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <vector>
#include <unistd.h>

#include "inative_test_key_handler.h"
#include "inative_test_touch_handler.h"
#include "util.h"

namespace OHOS {
INativeTest *INativeTest::VisitTests(VisitTestFunc func)
{
    auto sortFunc = [](const auto &it, const auto &jt) {
        if (it->GetDomain() != jt->GetDomain()) {
            return it->GetDomain() < jt->GetDomain();
        }
        if (it->GetID() != jt->GetID()) {
            return it->GetID() < jt->GetID();
        }
        return it->GetProcessSequence() < jt->GetProcessSequence();
    };
    std::sort(tests.begin(), tests.end(), sortFunc);

    for (auto &test : tests) {
        if (func(test)) {
            return test;
        }
    }
    return nullptr;
}

INativeTest::INativeTest()
{
    tests.push_back(this);
    keyHandler = new INativeTestKeyEventHandler(this);
    touchHandler = new INativeTestTouchEventHandler(this);
}

uint32_t INativeTest::GetLastTime() const
{
    return LAST_TIME_FOREVER;
}

enum AutoLoadService operator |(const enum AutoLoadService &l, const enum AutoLoadService &r)
{
    return static_cast<enum AutoLoadService>(static_cast<int32_t>(l) | static_cast<int32_t>(r));
}

bool operator &(const enum AutoLoadService &l, const enum AutoLoadService &r)
{
    return (static_cast<int32_t>(l) & static_cast<int32_t>(r)) != 0;
}

enum AutoLoadService INativeTest::GetAutoLoadService() const
{
    return AutoLoadService::Null;
}

bool INativeTest::OnKey(const KeyEvent &event)
{
    return false;
}

bool INativeTest::OnTouch(const TouchEvent &event)
{
    return false;
}

void INativeTest::GetToken()
{
    if (token == nullptr) {
        std::stringstream ss;
        ss << GetDomain() << GetID() << GetProcessSequence();
        auto sss = ss.str();
        auto u16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t >().from_bytes(sss);
        token = new IPCObjectStub(u16);
    }
}

int32_t INativeTest::ListenWindowKeyEvent(int32_t windowID)
{
    GetToken();
    return MMIEventHdl.RegisterStandardizedEventHandle(token, windowID, keyHandler);
}

int32_t INativeTest::ListenWindowTouchEvent(int32_t windowID)
{
    GetToken();
    return MMIEventHdl.RegisterStandardizedEventHandle(token, windowID, touchHandler);
}

void INativeTest::ListenWindowInputEvent(int32_t windowID)
{
    ListenWindowKeyEvent(windowID);
    ListenWindowTouchEvent(windowID);
}

int32_t INativeTest::GetProcessNumber() const
{
    return 1;
}

int32_t INativeTest::GetProcessSequence() const
{
    return -1;
}

int32_t INativeTest::StartSubprocess(int32_t id)
{
    GSLOG7SO(INFO) << "StartSubprocess " << id;
    auto pid = fork();
    if (pid < 0) {
        return pid;
    }

    if (pid == 0) {
        std::vector<const char *> args;
        for (const char **p = processArgv; *p != nullptr; p++) {
            args.push_back(*p);
        }

        std::stringstream ss;
        ss << "--process=" << id;
        auto sss = ss.str();
        args.push_back(sss.c_str());
        args.insert(args.end(), extraArgs.begin(), extraArgs.end());
        args.push_back(nullptr);
        auto ret = execvp(args[0], const_cast<char *const *>(args.data()));
        GSLOG7SO(ERROR) << "execvp return: " << ret << ", " << errno;
        ExitTest();
        return ret;
    }
    return 0;
}

void INativeTest::SetEventHandler(const std::shared_ptr<AppExecFwk::EventHandler> &handler)
{
    this->handler = handler;
}

void INativeTest::PostTask(std::function<void()> func, uint32_t delayTime) const
{
    handler->PostTask(func, delayTime);
}

void INativeTest::ExitTest() const
{
    GSLOG7SO(INFO) << "exiting, call PostTask(&AppExecFwk::EventRunner::Stop)";
    PostTask(std::bind(&AppExecFwk::EventRunner::Stop, handler->GetEventRunner()));
}
} // namespace OHOS
