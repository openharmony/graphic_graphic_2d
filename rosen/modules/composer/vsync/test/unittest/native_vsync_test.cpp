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
* Function: OH_NativeVSync_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Destroy by abnormal input
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Destroy001, Function | MediumTest | Level2)
{
    OH_NativeVSync_Destroy(nullptr);
}

/*
* Function: OH_NativeVSync_Destroy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_NativeVSync_Destroy
*                  2. check ret
 */
HWTEST_F(NativeVsyncTest, OH_NativeVSync_Destroy002, Function | MediumTest | Level2)
{
    OH_NativeVSync_Destroy(native_vsync);
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