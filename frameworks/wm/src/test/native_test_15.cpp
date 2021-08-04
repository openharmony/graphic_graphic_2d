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

#include "native_test_15.h"

#include <cstdio>
#include <securec.h>
#include <thread>

#include <display_type.h>
#include <iservice_registry.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest15 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "ipc draw(raise samgr/ipc crash)";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 15;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 1 << 30;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        pipe(pipeFd);
        pid_t pid = fork();
        if (pid < 0) {
            printf("%s fork failed", __func__);
            ExitTest();
            return;
        }

        if (pid == 0) {
            ChildProcess();
        } else {
            MainProcess();
        }
    }

private:
    void MainProcess()
    {
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            printf("%s window == nullptr\n", __func__);
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto surface = window->GetSurface();
        if (surface == nullptr) {
            printf("%s surface == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto producer = surface->GetProducer();
        if (producer == nullptr) {
            printf("%s producer == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto producerObject = producer->AsObject();
        if (producerObject == nullptr) {
            printf("%s producerObject == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("%s sam == nullptr\n", __func__);
            ExitTest();
            return;
        }

        sam->AddSystemAbility(SAID, producerObject);

        uint32_t msg = 0;
        write(pipeFd[1], &msg, sizeof(msg));

        sleep(0);

        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));
        sam->RemoveSystemAbility(SAID);
        ExitTest();
    }

    void ChildProcess()
    {
        char buf[10];
        read(pipeFd[0], &buf, sizeof(buf));

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("%s main_process sam == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto robj = sam->GetSystemAbility(SAID);
        if (robj == nullptr) {
            printf("%s main_process robj == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto bp = iface_cast<IBufferProducer>(robj);
        if (bp == nullptr) {
            printf("%s main_process bp == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto ipcSurface = Surface::CreateSurfaceAsProducer(bp);
        if (ipcSurface == nullptr) {
            printf("%s main_process ipcSurface == nullptr\n", __func__);
            ExitTest();
            return;
        }

        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, ipcSurface);

        constexpr uint32_t delayTime = 3000;
        PostTask(std::bind(&NativeTest15::ChildProcessAfter, this), delayTime);
    }

    void ChildProcessAfter()
    {
        char buf[10] = "end";
        write(pipeFd[1], buf, sizeof(buf));
        ExitTest();
        return;
    }

private:
    int32_t pipeFd[2];
    static inline constexpr uint32_t SAID = 4699;
    sptr<Window> window;
    sptr<NativeTestSync> windowSync;
} g_autoload;
} // namespace
