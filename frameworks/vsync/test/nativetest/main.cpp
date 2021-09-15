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

#include <chrono>

#include <vsync_helper.h>

using namespace OHOS;
using namespace std::placeholders;

namespace {
int64_t GetNowTime()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void RequestSync(const SyncFunc syncFunc, uint32_t freq)
{
    struct FrameCallback cb = {
        .frequency_ = freq,
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = syncFunc,
    };

    VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret != VSYNC_ERROR_OK) {
        printf("RequestFrameCallback inner %s\n", VsyncErrorStr(ret).c_str());
    }
}

void Vsync(int64_t, void *, uint32_t freq, int32_t data, std::function<void(int32_t)> check)
{
    check(++data);
    RequestSync(std::bind(Vsync, _1, _2, freq, data, check), freq);
}

void Main()
{
    auto check = [](int32_t data, uint32_t cycle, int64_t &lastTime) {
        if (cycle != 0 && data % cycle == 0) {
            int64_t diffTime = GetNowTime() - lastTime;
            constexpr int32_t nanosecondToSecond = 1000 * 1000 * 1000;
            double hz = static_cast<double>(cycle) * nanosecondToSecond / diffTime;

            printf("%d, %u: %lfHz\n", data, cycle, hz);
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
} // namespace

int main()
{
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(Main);
    runner->Run();
    return 0;
}
