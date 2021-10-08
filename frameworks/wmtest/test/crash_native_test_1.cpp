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

#include "crash_native_test_1.h"

#include <thread>
#include <unistd.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class Object : public RefBase {
};

class CrashNativeTest1 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "raise wptr double free";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "!crash";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 1;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = 2000;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        wptr<Object> wptr = nullptr;
        sptr<Object> sptr = nullptr;

        auto threadFunc = [&wptr]() {
            while (true) {
                auto sp = wptr.promote();
                std::this_thread::yield();
            }
        };
        std::thread thread(threadFunc);

        while (true) {
            sptr = new Object();
            wptr = sptr;
            sptr = nullptr;
        }
    }
} g_autoload;
} // namespace
