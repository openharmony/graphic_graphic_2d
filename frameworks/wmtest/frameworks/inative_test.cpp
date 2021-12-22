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
#include <csignal>
#include <gslogger.h>
#include <iservice_registry.h>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "inative_test_key_event_handler.h"
#include "inative_test_touch_event_handler.h"
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
}

uint32_t INativeTest::GetLastTime() const
{
    return LAST_TIME_FOREVER;
}

AutoLoadService operator |(const AutoLoadService &l, const AutoLoadService &r)
{
    return static_cast<AutoLoadService>(static_cast<int32_t>(l) | static_cast<int32_t>(r));
}

bool operator &(const AutoLoadService &l, const AutoLoadService &r)
{
    return (static_cast<int32_t>(l) & static_cast<int32_t>(r)) != 0;
}

AutoLoadService INativeTest::GetAutoLoadService() const
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
    keyHandlerMap[windowID] = new INativeTestKeyEventHandler(this);
    return MMIEventHdl.RegisterStandardizedEventHandle(token, windowID, keyHandlerMap[windowID]);
}

int32_t INativeTest::ListenWindowTouchEvent(int32_t windowID)
{
    GetToken();
    touchHandlerMap[windowID] = new INativeTestTouchEventHandler(this);
    return MMIEventHdl.RegisterStandardizedEventHandle(token, windowID, touchHandlerMap[windowID]);
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

void INativeTest::WaitSubprocessAllQuit()
{
    thiz = this;
    std::signal(SIGINT, INativeTest::Signal);
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

        std::stringstream ss2;
        std::string sss2;
        if (said > 0) {
            ss2 << "--said=" << said;
            sss2 = ss2.str();
            args.push_back(sss2.c_str());
        }

        args.insert(args.end(), extraArgs.begin(), extraArgs.end());
        args.push_back(nullptr);
        auto ret = execvp(args[0], const_cast<char *const *>(args.data()));
        GSLOG7SO(ERROR) << "execvp return: " << ret << ", " << errno;
        ExitTest();
        return ret;
    }
    return 0;
}

int32_t INativeTest::IPCServerStart()
{
    GSLOG7SO(DEBUG) << "IPCServerStarting";
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    constexpr int32_t sABegin = 4610;
    constexpr int32_t sAEnd = 4700;
    IncStrongRef(this);
    for (int32_t i = sABegin; i < sAEnd; i++) {
        GSLOG7SO(DEBUG) << "adding SystemAbility, SAID: " << i;
        if (sam->AddSystemAbility(i, AsObject()) == ERR_OK) {
            GSLOG7SO(DEBUG) << "AddSystemAbility Success";
            said = i;
            return GSERROR_OK;
        }
    }

    GSLOG7SE(ERROR) << "AddSystemAbility failed from " << sABegin << " to " << sAEnd;
    return GSERROR_API_FAILED;
}

int32_t INativeTest::IPCServerStop()
{
    GSLOG7SO(DEBUG) << "IPCServerStoping";
    if (said == 0) {
        return GSERROR_INVALID_OPERATING;
    }

    for (auto &[_, ipc] : ipcs) {
        ipc->OnMessage(GetProcessSequence(), "quit", nullptr);
    }

    ipcs.clear();
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sam->RemoveSystemAbility(said);
    said = 0;
    return GSERROR_OK;
}

GSError INativeTest::IPCClientConnectServer(int32_t said)
{
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        GSLOG7SE(ERROR) << "SystemAbilityManager is nullptr";
        return GSERROR_CONNOT_CONNECT_SAMGR;
    }

    auto robj = sam->GetSystemAbility(said);
    if (robj == nullptr) {
        GSLOG7SE(ERROR) << "Remote Object is nullptr";
        return GSERROR_CONNOT_CONNECT_SERVER;
    }

    remoteIpc = iface_cast<INativeTestIpc>(robj);
    if (remoteIpc == nullptr) {
        GSLOG7SE(ERROR) << "Cannot find proxy";
        return GSERROR_PROXY_NOT_INCLUDE;
    }

    GSLOG7SO(INFO) << "server connected";
    sptr<INativeTestIpc> ipc = this;
    remoteIpc->Register(GetProcessSequence(), ipc);
    return GSERROR_OK;
}

GSError INativeTest::IPCClientSendMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    GSLOG7SO(INFO) << "send message from " << GetProcessSequence() << " to " << sequence << ": " << message;
    return remoteIpc->SendMessage(sequence, message, robj);
}

void INativeTest::IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    GSLOG7SO(INFO) << "Received from " << sequence << ", message: " << message;
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

GSError INativeTest::SendMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    GSLOG7SO(DEBUG) << "Received from " << pidToSeq[GetCallingPid()] << " to " << sequence
        << ", message: " << message << ", robj: " << robj.GetRefPtr();
    if (sequence != -1) {
        auto it = ipcs.find(sequence);
        if (it == ipcs.end()) {
            GSLOG7SO(WARN) << "Cannot found send target! (-> " << sequence << ")";
            return GSERROR_NO_ENTRY;
        }

        GSLOG7SO(DEBUG) << "Sending to " << it->second.GetRefPtr();
        it->second->OnMessage(pidToSeq[GetCallingPid()], message, robj);
    } else {
        for (auto &[_, ipc] : ipcs) {
            ipc->OnMessage(pidToSeq[GetCallingPid()], message, robj);
        }
    }
    return GSERROR_OK;
}

GSError INativeTest::Register(int32_t sequence, sptr<INativeTestIpc> &ipc)
{
    GSLOG7SO(DEBUG) << "Register sequence: " << sequence << ", ipc: " << ipc.GetRefPtr();
    pidToSeq[GetCallingPid()] = sequence;
    ipcs[sequence] = ipc;
    return GSERROR_OK;
}

GSError INativeTest::OnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj)
{
    GSLOG7SO(INFO) << "Received from " << sequence << ", message: " << message;
    if (sequence == -1 && message == "quit") {
        ExitTest();
        return GSERROR_OK;
    }

    IPCClientOnMessage(sequence, message, robj);
    return GSERROR_OK;
}

void INativeTest::Signal(int32_t signum)
{
    (void)signum;
    thiz->PostTask(std::bind(&INativeTest::IPCServerStop, thiz));
    thiz->ExitTest();
}
} // namespace OHOS
