/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "native_vsync.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class NativeVsyncTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline OH_NativeVSync* native_vsync = nullptr;
};

void NativeVsyncTest::SetUpTestCase()
{
}

void NativeVsyncTest::TearDownTestCase()
{
}

static void OnVSync(long long timestamp, void* data)
{
    if (data != nullptr) {
        delete (std::string *)data;
        data = nullptr;
    }
}

namespace {
/*
* Function: OH_NativeVSync_Create
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Create by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Create001, Function | MediumTest | Level2)
{
    ASSERT_EQ(OH_NativeVSync_Create(nullptr, 0), nullptr);
}

/*
* Function: OH_NativeVSync_Create
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Create
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Create002, Function | MediumTest | Level2)
{
    char name[] = "test";
    native_vsync = OH_NativeVSync_Create(name, sizeof(name));
    ASSERT_NE(native_vsync, nullptr);
}

/*
* Function: OH_NativeVSync_Create_ForAssociatedWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Create_ForAssociatedWindow by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Create_ForAssociatedWindow001, Function | MediumTest | Level2)
{
    ASSERT_EQ(OH_NativeVSync_Create_ForAssociatedWindow(0, nullptr, 0), nullptr);
}

/*
* Function: OH_NativeVSync_Create_ForAssociatedWindow
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Create_ForAssociatedWindow
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Create_ForAssociatedWindow002, Function | MediumTest | Level2)
{
    char name[] = "test";
    ASSERT_NE(OH_NativeVSync_Create_ForAssociatedWindow(0, name, sizeof(name)), nullptr);
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrame by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrame001, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_RequestFrame(nullptr, nullptr, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrame by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrame002, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_RequestFrame(native_vsync, nullptr, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrame
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrame
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrame003, Function | MediumTest | Level2)
{
    OH_NativeVSync_FrameCallback callback = OnVSync;
    ASSERT_EQ(OH_NativeVSync_RequestFrame(native_vsync, callback, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback001, Function | MediumTest | Level2)
{
    std::string *userData = new std::string("userData");
    OH_NativeVSync_FrameCallback callback = OnVSync;
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(nullptr, callback, userData), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback002, Function | MediumTest | Level2)
{
    std::string *userData = new std::string("userData");
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(native_vsync, nullptr, userData), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback003, Function | MediumTest | Level2)
{
    OH_NativeVSync_FrameCallback callback = OnVSync;
    ASSERT_EQ(OH_NativeVSync_RequestFrameWithMultiCallback(native_vsync, callback, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback004, Function | MediumTest | Level2)
{
    std::string *userData = new std::string("userData");
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(nullptr, nullptr, userData), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback005, Function | MediumTest | Level2)
{
    OH_NativeVSync_FrameCallback callback = OnVSync;
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(nullptr, callback, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback006, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(native_vsync, nullptr, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback007, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_RequestFrameWithMultiCallback(nullptr, nullptr, nullptr), 0);
}

/*
* Function: OH_NativeVSync_RequestFrameWithMultiCallback
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_RequestFrameWithMultiCallback with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_RequestFrameWithMultiCallback008, Function | MediumTest | Level2)
{
    std::string *userData = new std::string("userData");
    OH_NativeVSync_FrameCallback callback = OnVSync;
    ASSERT_EQ(OH_NativeVSync_RequestFrameWithMultiCallback(native_vsync, callback, userData), 0);
}

/*
* Function: OH_NativeVSync_DVSyncSwitch
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_DVSyncSwitch with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_DVSyncSwitch001, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_DVSyncSwitch(nullptr, true), 0);
    ASSERT_NE(OH_NativeVSync_DVSyncSwitch(nullptr, false), 0);
}

/*
* Function: OH_NativeVSync_DVSyncSwitch
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_DVSyncSwitch with abnormal parameters
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_DVSyncSwitch002, Function | MediumTest | Level2)
{
    ASSERT_EQ(OH_NativeVSync_DVSyncSwitch(native_vsync, true), 0);
    ASSERT_EQ(OH_NativeVSync_DVSyncSwitch(native_vsync, false), 0);
}

/*
* Function: OH_NativeVSync_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_SetExpectedFrameRateRange by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_SetExpectedFrameRateRange001, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_SetExpectedFrameRateRange(nullptr, nullptr), 0);
}
 
/*
* Function: OH_NativeVSync_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_SetExpectedFrameRateRange by abnormal input
*                  2. check ret
 */
 HWTEST_F(NativeVsyncTest, OH_NativeVSync_SetExpectedFrameRateRange002, Function | MediumTest | Level2)
{
    OH_NativeVSync_ExpectedRateRange range = {0, 160, 0};
    ASSERT_NE(OH_NativeVSync_SetExpectedFrameRateRange(native_vsync, &range), 0);
    range = {-60, 120, 120};
    ASSERT_NE(OH_NativeVSync_SetExpectedFrameRateRange(native_vsync, &range), 0);
    range = {60, 30, 30};
    ASSERT_NE(OH_NativeVSync_SetExpectedFrameRateRange(native_vsync, &range), 0);
    range = {0, 60, 120};
    ASSERT_NE(OH_NativeVSync_SetExpectedFrameRateRange(native_vsync, &range), 0);
}
 
/*
* Function: OH_NativeVSync_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_SetExpectedFrameRateRange
*                  2. check ret
 */
 HWTEST_F(NativeVsyncTest, OH_NativeVSync_SetExpectedFrameRateRange003, Function | MediumTest | Level2)
{
    OH_NativeVSync_ExpectedRateRange range = {60, 120, 120};
    ASSERT_EQ(OH_NativeVSync_SetExpectedFrameRateRange(native_vsync, &range), 0);
}

/*
* Function: OH_NativeVSync_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Destroy with abnormal input
*                  2. call OH_NativeVSync_Destroy with normal input
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Destroy001, Function | MediumTest | Level2)
{
    OH_NativeVSync_Destroy(nullptr);

    ASSERT_NE(native_vsync, nullptr);
    OH_NativeVSync_Destroy(native_vsync);
    native_vsync = nullptr;
}

/*
* Function: OH_NativeVSync_GetPeriod
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_GetPeriod by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_GetPeriod001, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_GetPeriod(nullptr, nullptr), 0);
}

/*
* Function: OH_NativeVSync_GetPeriod
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_GetPeriod 
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_GetPeriod002, Function | MediumTest | Level2)
{
    ASSERT_NE(OH_NativeVSync_GetPeriod(native_vsync, nullptr), 0);
}

} // namespace
} // namespace Rosen
} // namespace OHOS