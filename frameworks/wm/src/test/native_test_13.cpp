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

#include "native_test_13.h"

#include <cstdio>

#include <window_manager.h>

#include "native_test_6.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest13 : public NativeTest6 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "subwindow destory";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 13;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 6000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        NativeTest6::Run(argc, argv);
        constexpr uint32_t nextTime = 3000;
        PostTask(std::bind(&NativeTest13::AfterRun1, this), nextTime);
    }

    void AfterRun1() const
    {
        subwindow->Destroy();
        constexpr uint32_t nextTime = 2000;
        PostTask(std::bind(&NativeTest13::AfterRun2, this), nextTime);
    }

    void AfterRun2() const
    {
        window->Destroy();
    }
} g_autoload;
} // namespace
