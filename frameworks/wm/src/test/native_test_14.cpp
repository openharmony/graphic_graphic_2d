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

#include "native_test_14.h"

#include <cstdio>
#include <memory>
#include <thread>

#include <window_manager.h>

#include "native_test_7.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class NativeTest14 : public NativeTest7 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "video subwindow destory";
        return desc;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 14;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 6000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        NativeTest7::Run(argc, argv);
        constexpr uint32_t nextTime = 3000;
        PostTask(std::bind(&NativeTest14::AfterRun1, this), nextTime);
    }

    void AfterRun1()
    {
        destroyThread = std::make_unique<std::thread>(std::bind(&Subwindow::Destroy, subwindow));
        constexpr uint32_t nextTime = 2000;
        PostTask(std::bind(&NativeTest14::AfterRun2, this), nextTime);
    }

    void AfterRun2() const
    {
        window->Destroy();
    }

private:
    std::unique_ptr<std::thread> destroyThread = nullptr;
} g_autoload;
} // namespace
