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
#include <test_header.h>

#include "hgm_idle_detector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string bufferName = "frameBuffer";
    const std::string aceAnimator = "AceAnimato";
    const std::string rosenWeb = "RosenWeb";
    const std::string FlutterBuffer = "oh_flutter";
    const std::string otherSurface = "Other_SF";
    constexpr uint64_t  currTime = 100000000;
    constexpr uint64_t  lastTime = 200000000;
    constexpr uint32_t  fps30HZ = 30;
    constexpr uint32_t  fps60HZ = 60;
    constexpr uint32_t  fps90HZ = 90;
    constexpr uint32_t  fps120HZ = 120;
}
class HgmIdleDetectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmIdleDetectorTest::SetUpTestCase() {}
void HgmIdleDetectorTest::TearDownTestCase() {}
void HgmIdleDetectorTest::SetUp() {}
void HgmIdleDetectorTest::TearDown() {}

/**
 * @tc.name: SetAndGetAppSupportStatus
 * @tc.desc: Verify the result of SetAndGetAppSupportStatus function
 * @tc.type: FUNC
 * @tc.require: 19N2FS
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetAppSupportStatus, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetAppSupportStatus(false);
        }
        STEP("3. get app support status") {
            bool ret = idleDetector->GetAppSupportStatus();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: SetAndGetAceAnimatorIdleStatus
 * @tc.desc: Verify the result of SetAndGetAceAnimatorIdleStatus function
 * @tc.type: FUNC
 * @tc.require: 19N2FS
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetAceAnimatorIdleStatus, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set aceAnimator idle status") {
            idleDetector->SetAceAnimatorIdleStatus(false);
        }
        STEP("3. get aceAnimator idle status") {
            bool ret = idleDetector->GetAceAnimatorIdleStatus();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: SetAndGetTouchUpTime
 * @tc.desc: Verify the result of SetAndReadAppSupportStatus function
 * @tc.type: FUNC
 * @tc.require: 19N2FS
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetTouchUpTime, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetTouchUpTime(currTime);
        }
        STEP("3. get app support status") {
            uint64_t time = idleDetector->GetTouchUpTime();
            STEP_ASSERT_GE(time, 0);
        }
    }
}

/**
 * @tc.name: SetAndGetSurfaceTimeStatus
 * @tc.desc: Verify the result of SetAndGetSurfaceTimeStatus function
 * @tc.type: FUNC
 * @tc.require: 19N2FS
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetSurfaceTimeStatus, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetAppSupportStatus(true);
            idleDetector->supportAppBufferList_.insert(idleDetector->supportAppBufferList_.begin(), otherSurface);
        }
        STEP("3. set buffer renew time") {
            idleDetector->UpdateSurfaceTime(bufferName, currTime);
        }
        STEP("4. get buffer idle state") {
            bool ret = idleDetector->GetSurfaceIdleState(lastTime);
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: GetSupportSurface
 * @tc.desc: Verify the result of GetSupportSurface function
 * @tc.type: FUNC
 * @tc.require: IAA0JX
 */
HWTEST_F(HgmIdleDetectorTest, GetSupportSurface, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set surface date") {
            idleDetector->SetAppSupportStatus(true);
            idleDetector->ClearAppBufferBlackList();
            bool ret = idleDetector->GetSupportSurface();
            STEP_ASSERT_EQ(ret, true);

            idleDetector->appBufferBlackList_.push_back(aceAnimator);
            ret = idleDetector->GetSupportSurface();
            STEP_ASSERT_EQ(ret, false);

            idleDetector->frameTimeMap_[bufferName] = currTime;
            ret = idleDetector->GetSupportSurface();
            STEP_ASSERT_EQ(ret, true);

            idleDetector->appBufferBlackList_.push_back(bufferName);
            ret = idleDetector->GetSupportSurface();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: GetSurfaceUpExpectFps001
 * @tc.desc: Verify the result of GetSurfaceUpExpectFps001 function
 * @tc.type: FUNC
 * @tc.require: IAA0JX
 */
HWTEST_F(HgmIdleDetectorTest, GetSurfaceUpExpectFps001, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. get surface up expect fps") {
            idleDetector->SetAppSupportStatus(true);
            idleDetector->ClearAppBufferList();
            uint32_t ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->SetAceAnimatorIdleStatus(false);
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps90HZ));
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps90HZ);

            idleDetector->frameTimeMap_[bufferName] = currTime;
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->frameTimeMap_[FlutterBuffer] = currTime;
            idleDetector->appBufferList_.push_back(std::make_pair(FlutterBuffer, fps90HZ));
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->frameTimeMap_[rosenWeb] = currTime;
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);
        }
    }
}

/**
 * @tc.name: GetSurfaceUpExpectFps002
 * @tc.desc: Verify the result of GetSurfaceUpExpectFps002 function
 * @tc.type: FUNC
 * @tc.require: IAA0JX
 */
HWTEST_F(HgmIdleDetectorTest, GetSurfaceUpExpectFps002, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. get surface up expect fps") {
            idleDetector->SetAppSupportStatus(true);

            idleDetector->frameTimeMap_[rosenWeb] = currTime;
            idleDetector->frameTimeMap_[FlutterBuffer] = currTime;
            idleDetector->frameTimeMap_[bufferName] = currTime;

            idleDetector->ClearAppBufferList();
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(FlutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps60HZ));
            uint32_t ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->ClearAppBufferList();
            idleDetector->SetAceAnimatorIdleStatus(true);
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(FlutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps60HZ));
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps90HZ);

            idleDetector->ClearAppBufferList();
            idleDetector->SetAceAnimatorIdleStatus(false);
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(FlutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps60HZ));
            ret = idleDetector->GetSurfaceUpExpectFps();
            STEP_ASSERT_EQ(ret, fps120HZ);
        }
    }
}

} // namespace Rosen
} // namespace OHOS