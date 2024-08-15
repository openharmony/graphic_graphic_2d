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

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
int g_counter = 0;
int g_lastData = 0;
static void OnVSync(long long time, void *data)
{
    if (!data) {
        return;
    }
    g_lastData = *(int *)data;
}
}
class NativeVSyncRequestFrameTest : public testing::Test {
public:
    void TestMultiTimes(int times);
};

void NativeVSyncRequestFrameTest::TestMultiTimes(int times)
{
    char name[] = "TestMultiTimes";
    OH_NativeVSync *native_vsync = OH_NativeVSync_Create(name, sizeof(name));
    int lastUserData = 0;
    for (int i = 0; i < times; i++) {
        int *userData = new int(g_counter++);
        OH_NativeVSync_FrameCallback callback = OnVSync;
        OH_NativeVSync_RequestFrame(native_vsync, callback, userData);
        lastUserData = *userData;
    }
    usleep(200000); // 200000us
    ASSERT_EQ(g_lastData, lastUserData);
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: OH_NativeVSync_RequestFrame
 */
HWTEST_F(NativeVSyncRequestFrameTest, OH_NativeVSync_RequestFrame_1_time, Function | MediumTest | Level2)
{
    TestMultiTimes(1); // test 1 time
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: OH_NativeVSync_RequestFrame
 */
HWTEST_F(NativeVSyncRequestFrameTest, OH_NativeVSync_RequestFrame_3_times, Function | MediumTest | Level2)
{
    TestMultiTimes(3); // test 3 times
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: OH_NativeVSync_RequestFrame
 */
HWTEST_F(NativeVSyncRequestFrameTest, OH_NativeVSync_RequestFrame_10_times, Function | MediumTest | Level2)
{
    TestMultiTimes(10); // test 10 times
}
}
