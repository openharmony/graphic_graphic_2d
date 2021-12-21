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

#include "wmclient_native_test_30.h"

#include <cstdio>
#include <securec.h>
#include <thread>
#include <gslogger.h>

#include <display_type.h>
#include <iservice_registry.h>
#include <window_manager.h>
#include <surface.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest30 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "attach/detach test";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 30;
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

class WMClientNativeTest30Sub0 : public WMClientNativeTest30, public IBufferConsumerListenerClazz {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "bq1 bq2";
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
        bq2 = window->GetSurface();
        auto func = [this](sptr<SurfaceBuffer> &buffer) {
            OnReleaseBuffer(buffer);
            return GSERROR_OK;
        };
        bq2->RegisterReleaseListener(func);
        if (bq2 == nullptr) {
            GSLOG7SO(ERROR) << "bq2 == nullptr";
            ExitTest();
            return;
        }

        bq1 = Surface::CreateSurfaceAsConsumer("bq1");
        bq1->SetDefaultWidthAndHeight(window->GetWidth(), window->GetHeight());
        bq1->SetDefaultUsage(bq2->GetDefaultUsage());
        bq1->RegisterConsumerListener(this);
        IPCClientSendMessage(1, "producer", bq1->GetProducer()->AsObject());
    }

    void IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "quit") {
            ExitTest();
        }
    }

    GSError OnReleaseBuffer(sptr<SurfaceBuffer> rbuffer)
    {
        auto sret = bq2->DetachBuffer(rbuffer);
        if (sret != GSERROR_OK) {
            printf("OnReleaseBuffer, Detach failed!\n");
            return GSERROR_NO_ENTRY;
        }

        do {
            sret = bq1->AttachBuffer(rbuffer);
        } while (sret != GSERROR_OK);

        auto ret = bq1->ReleaseBuffer(rbuffer, -1);
        if (ret != GSERROR_OK) {
            printf("release buffer failed!\n");
        }
        return GSERROR_OK;
    }

    virtual void OnBufferAvailable() override
    {
        int32_t flushFence;
        int64_t timestamp;
        Rect damage;
        auto sret = bq1->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
        if (sret != GSERROR_OK) {
            printf("AcquireBuffer failed!\n");
            return;
        }

        sret = bq1->DetachBuffer(sbuffer);
        if (sret != GSERROR_OK) {
            printf("Detach buffer failed!\n");
            return;
        }

        if (sbuffer == nullptr) {
            printf("sbuffer is null\n");
        }

        do {
            sret = bq2->AttachBuffer(sbuffer);
        } while (sret != GSERROR_OK);

        if (sret != GSERROR_OK) {
            if (sret == GSERROR_NO_BUFFER) {
                printf("No Buffer!, %d\n", __LINE__);
                bq1->AttachBuffer(sbuffer);
                bq1->ReleaseBuffer(sbuffer, -1);
            } else {
                printf("Attach buffer failed!\n");
            }
            return;
        }

        BufferFlushConfig fconfig = {
            .damage = {
                .w = sbuffer->GetWidth(),
                .h = sbuffer->GetHeight(),
            }
        };

        sret = bq2->FlushBuffer(sbuffer, -1, fconfig);
        if (sret != GSERROR_OK) {
            printf("flush buffer failed\n");
        }
    }

private:
    sptr<Window> window;
    sptr<Surface> bq1 = nullptr;
    sptr<Surface> bq2 = nullptr;
    sptr<SurfaceBuffer> sbuffer = nullptr;
} g_autoload0;

class WMClientNativeTest30Sub1 : public WMClientNativeTest30 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "window";
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

        ipcSurface = Surface::CreateSurfaceAsProducer(bp);
        if (ipcSurface == nullptr) {
            GSLOG7SO(ERROR) << "ipcSurface == nullptr";
            ExitTest();
            return;
        }
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, ipcSurface);
    }

    void IPCClientOnMessage (int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "producer") {
            remoteObject = robj;
            if (remoteObject == nullptr) {
                PostTask(std::bind(&INativeTest::IPCClientSendMessage, this, 0, "quit", nullptr));
                ExitTest();
            } else {
                PostTask(std::bind(&WMClientNativeTest30Sub1::Run2, this));
            }
            return;
        }
    }

private:
    sptr<NativeTestSync> windowSync = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    sptr<Surface> ipcSurface = nullptr;
} g_autoload1;
} // namespace
