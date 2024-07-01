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
#include "native_display_soloist.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t FRAME_RATE_30_HZ = 30;
    constexpr uint32_t FRAME_RATE_60_HZ = 60;
    constexpr uint32_t FRAME_RATE_90_HZ = 90;
    constexpr uint32_t FRAME_RATE_120_HZ = 120;
    constexpr uint32_t SLEEP_TIME_US = 100000;
    constexpr int32_t EXEC_SUCCESS = 0;
    constexpr int32_t SOLOIST_ERROR = -1;
}
class NativeDisplaySoloistTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline OH_DisplaySoloist* nativeDisplaySoloist = nullptr;
};

void NativeDisplaySoloistTest::SetUpTestCase()
{
}

void NativeDisplaySoloistTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_US);
}

void NativeDisplaySoloistTest::SetUp()
{
}

void NativeDisplaySoloistTest::TearDown()
{
}

static void OnVSync(long long timestamp, long long targetTimestamp, void* data)
{
}

namespace {
/*
* Function: OH_DisplaySoloist_Create
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Create by normal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Create001, Function | MediumTest | Level2)
{
    EXPECT_EQ(nullptr, nativeDisplaySoloist);
    nativeDisplaySoloist = OH_DisplaySoloist_Create(false);
    EXPECT_NE(nullptr, nativeDisplaySoloist);
}

/*
* Function: OH_DisplaySoloist_Start
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Start by abnormal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Start001, Function | MediumTest | Level2)
{
    OH_DisplaySoloist_FrameCallback callback = OnVSync;
    int32_t result = OH_DisplaySoloist_Start(nullptr, callback, nullptr);
    EXPECT_EQ(SOLOIST_ERROR, result);
}

/*
* Function: OH_DisplaySoloist_Start
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Start by normal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Start002, Function | MediumTest | Level2)
{
    OH_DisplaySoloist_FrameCallback callback = OnVSync;
    int32_t result = OH_DisplaySoloist_Start(nativeDisplaySoloist, callback, nullptr);
    EXPECT_EQ(EXEC_SUCCESS, result);
}

/*
* Function: OH_DisplaySoloist_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_SetExpectedFrameRateRange by abnormal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_SetExpectedFrameRateRange001, Function | MediumTest | Level2)
{
    DisplaySoloist_ExpectedRateRange validRange = { FRAME_RATE_30_HZ, FRAME_RATE_120_HZ, FRAME_RATE_60_HZ };
    int32_t result1 = OH_DisplaySoloist_SetExpectedFrameRateRange(nullptr, &validRange);
    EXPECT_EQ(SOLOIST_ERROR, result1);
    int32_t result2 = OH_DisplaySoloist_SetExpectedFrameRateRange(nativeDisplaySoloist, nullptr);
    EXPECT_EQ(SOLOIST_ERROR, result2);
}

/*
* Function: OH_DisplaySoloist_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_SetExpectedFrameRateRange by abnormal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_SetExpectedFrameRateRange002, Function | MediumTest | Level2)
{
    DisplaySoloist_ExpectedRateRange invalidRange = { FRAME_RATE_30_HZ, FRAME_RATE_90_HZ, FRAME_RATE_120_HZ };
    int32_t result = OH_DisplaySoloist_SetExpectedFrameRateRange(nullptr, &invalidRange);
    EXPECT_EQ(SOLOIST_ERROR, result);
}

/*
* Function: OH_DisplaySoloist_SetExpectedFrameRateRange
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_SetExpectedFrameRateRange by normal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_SetExpectedFrameRateRange003, Function | MediumTest | Level2)
{
    DisplaySoloist_ExpectedRateRange validRange = { FRAME_RATE_30_HZ, FRAME_RATE_120_HZ, FRAME_RATE_60_HZ };
    int32_t result = OH_DisplaySoloist_SetExpectedFrameRateRange(nativeDisplaySoloist, &validRange);
    EXPECT_EQ(EXEC_SUCCESS, result);
}

/*
* Function: OH_DisplaySoloist_Stop
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Stop by abnormal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Stop001, Function | MediumTest | Level2)
{
    int32_t result = OH_DisplaySoloist_Stop(nullptr);
    EXPECT_EQ(SOLOIST_ERROR, result);
}

/*
* Function: OH_DisplaySoloist_Stop
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Stop by normal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Stop002, Function | MediumTest | Level2)
{
    int32_t result = OH_DisplaySoloist_Stop(nativeDisplaySoloist);
    EXPECT_EQ(EXEC_SUCCESS, result);
}

/*
* Function: OH_DisplaySoloist_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Destroy by abnormal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Destroy001, Function | MediumTest | Level2)
{
    int32_t result = OH_DisplaySoloist_Destroy(nullptr);
    EXPECT_EQ(SOLOIST_ERROR, result);
}

/*
* Function: OH_DisplaySoloist_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_DisplaySoloist_Destroy by normal input
 */
HWTEST_F(NativeDisplaySoloistTest, OH_DisplaySoloist_Destroy002, Function | MediumTest | Level2)
{
    int32_t result = OH_DisplaySoloist_Destroy(nativeDisplaySoloist);
    EXPECT_EQ(EXEC_SUCCESS, result);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
