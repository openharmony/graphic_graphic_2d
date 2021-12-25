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

#include "wmclient_native_test_14.h"

#include <gslogger.h>

#include "inative_test.h"
#include "native_test_class.h"

using namespace OHOS;

namespace {
class WMClientNativeTest14 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "ipc draw";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 14;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    int32_t GetProcessNumber() const override
    {
        return 2;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto ret = IPCServerStart();
        if (ret) {
            GSLOG7SE(ERROR) << ret;
            ExitTest();
        }

        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(1);
        sleep(1);
        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(0);
        WaitSubprocessAllQuit();
    }
} g_autoload;

class WMClientNativeTest14Sub0 : public WMClientNativeTest14 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "window consumer";
        return desc;
    }

    int32_t GetProcessSequence() const override
    {
        return 0;
    }

    void Run(int32_t argc, const char **argv) override
    {
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            GSLOG7SO(ERROR) << "window == nullptr";
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto surf = window->GetSurface();
        if (surf == nullptr) {
            GSLOG7SO(ERROR) << "surf == nullptr";
            ExitTest();
            return;
        }

        IPCClientSendMessage(1, "producer", surf->GetProducer()->AsObject());
    }

    void IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "quit") {
            ExitTest();
        }
    }

private:
    sptr<Window> window;
} g_autoload0;

class WMClientNativeTest14Sub1 : public WMClientNativeTest14 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "surface producer";
        return desc;
    }

    int32_t GetProcessSequence() const override
    {
        return 1;
    }

    void Run(int32_t argc, const char **argv) override
    {
    }

    void Run2()
    {
        auto bp = iface_cast<IBufferProducer>(remoteObject);
        if (bp == nullptr) {
            GSLOG7SO(ERROR) << "bp == nullptr";
            ExitTest();
            return;
        }

        auto ipcSurface = Surface::CreateSurfaceAsProducer(bp);
        if (ipcSurface == nullptr) {
            GSLOG7SO(ERROR) << "ipcSurface == nullptr";
            ExitTest();
            return;
        }

        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, ipcSurface);
    }

    void IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "producer") {
            remoteObject = robj;
            if (remoteObject == nullptr) {
                PostTask(std::bind(&INativeTest::IPCClientSendMessage, this, 0, "quit", nullptr));
                ExitTest();
            } else {
                PostTask(std::bind(&WMClientNativeTest14Sub1::Run2, this));
            }
            return;
        }
    }

private:
    sptr<NativeTestSync> windowSync = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
} g_autoload1;
} // namespace
