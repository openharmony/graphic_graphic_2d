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

#include "util.h"

#include <chrono>
#include <thread>
#include <unistd.h>

#include <gslogger.h>

using namespace std::chrono_literals;

namespace OHOS {
namespace {
int32_t g_vsyncRate = 0;
} // namespace

void SetVsyncRate(int32_t rate)
{
    g_vsyncRate = rate;
}

uint32_t RequestSync(const SyncFunc syncFunc, void *data)
{
    struct FrameCallback cb = {
        .frequency_ = g_vsyncRate,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = syncFunc,
    };
    return VsyncHelper::Current()->RequestFrameCallback(cb);
}

int64_t GetNowTime()
{
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    return (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}
} // namespace OHOS
