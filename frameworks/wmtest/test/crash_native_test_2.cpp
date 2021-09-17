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

#include "crash_native_test_2.h"

#include <cstdio>
#include <unistd.h>

#include <iservice_registry.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class CrashNativeTest2 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "raise samgr/ipc crash";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "!crash";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 2;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 2000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        constexpr int32_t said = 123123;
        auto robj = sam->GetSystemAbility(said);

        pid_t pid = fork();
        if (pid < 0) {
            printf("%s fork failed", __func__);
            ExitTest();
            return;
        }

        if (pid == 0) {
            auto sam2 = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            auto robj2 = sam2->GetSystemAbility(said);
        } else {
            ExitTest();
        }
    }
} g_autoload;
} // namespace
