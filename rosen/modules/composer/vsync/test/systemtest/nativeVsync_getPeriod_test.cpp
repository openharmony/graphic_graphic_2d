/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>
#include "native_vsync.h"
#include "graphic_common_c.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
static void OnVSync(long long time, void *data)
{
}
}
class NativeVSyncGetPeriodTest : public testing::Test {
};

/*
* Function: OH_NativeVSync_GetPeriod
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: OH_NativeVSync_GetPeriod
 */
HWTEST_F(NativeVSyncGetPeriodTest, NativeVSyncGetPeriod_Test, Function | MediumTest | Level2)
{
    char name[] = "TestMultiTimes";
    OH_NativeVSync *native_vsync = OH_NativeVSync_Create(name, sizeof(name));
    OH_NativeVSync_FrameCallback callback = OnVSync;
    OH_NativeVSync_RequestFrame(native_vsync, callback, nullptr);
    usleep(200000); // 200000us
    long long period = 0;
    ASSERT_EQ(OH_NativeVSync_GetPeriod(native_vsync, &period), VSYNC_ERROR_OK);
    ASSERT_NE(period, 0);
}
}