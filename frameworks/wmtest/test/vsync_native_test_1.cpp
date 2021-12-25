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

#include "vsync_native_test_1.h"

#include <vsync_helper.h>

#include "inative_test.h"
#include "util.h"

using namespace OHOS;
using namespace std::placeholders;

namespace {
class VsyncNativeTest1 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "vsync 20, 30, 60 test";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "vsync";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 1;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto check = [](int32_t data, uint32_t cycle, int64_t& lastTime) {
            if (cycle != 0 && data % cycle == 0) {
                int64_t diffTime = GetNowTime() - lastTime;
                constexpr int32_t nanosecondToSecond = 1000 * 1000 * 1000;
                double hz = static_cast<double>(cycle) * nanosecondToSecond / diffTime;

                printf("[%u]: frames=%d, avg=%lfHz\n", cycle, data, hz);
                lastTime = GetNowTime();
            }
        };

        static struct {
            uint32_t cycle;
            int64_t lastTime;
        } test[] = { {60}, {30}, {20}, };

        for (uint32_t i = 0; i < sizeof(test) / sizeof(*test); i++) {
            Vsync(0, nullptr, test[i].cycle, 0, std::bind(check, _1, test[i].cycle, test[i].lastTime));
        }
    }

    void Vsync(int64_t, void *, uint32_t freq, int32_t data, std::function<void(int32_t)> check)
    {
        check(++data);
        RequestSync(std::bind(&VsyncNativeTest1::Vsync, this, _1, _2, freq, data, check), freq);
    }

    void RequestSync(const SyncFunc syncFunc, uint32_t freq)
    {
        struct FrameCallback cb = {
            .frequency_ = freq,
            .timestamp_ = 0,
            .userdata_ = nullptr,
            .callback_ = syncFunc,
        };

        GSError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
        if (ret != GSERROR_OK) {
            printf("RequestFrameCallback error %s\n", GSErrorStr(ret).c_str());
        }
    }
} g_autoload;
} // namespace
