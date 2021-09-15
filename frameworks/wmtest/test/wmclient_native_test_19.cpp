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

#include "wmclient_native_test_19.h"

#include <cstdio>

#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest19 : public INativeTest, public IWindowChangeListenerClazz {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "listen global window event";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 19;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto wmsc = WindowManagerServiceClient::GetInstance();
        wmsc->Init();
        auto wms = wmsc->GetService();
        auto wretPromise = wms->OnWindowListChange(this);
        auto wret = wretPromise->Await();
        if (wret != WM_OK) {
            printf("Register Window Change Listener failed\n");
            ExitTest();
            return;
        }
    }

    void OnWindowCreate(int32_t pid, int32_t wid) override
    {
        printf("process%d %d window created\n", pid, wid);
    }

    void OnWindowDestroy(int32_t pid, int32_t wid) override
    {
        printf("process%d %d window destroyed\n", pid, wid);
    }

private:
} g_autoload;
} // namespace
