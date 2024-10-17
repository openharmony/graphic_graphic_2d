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
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const std::string bufferName = "frameBuffer";
    const std::string aceAnimator = "AceAnimato";
    const std::string rosenWeb = "RosenWeb";
    const std::string flutterBuffer = "oh_flutter";
    const std::string otherSurface = "Other_SF";
    constexpr uint64_t  currTime = 100000000;
    constexpr uint64_t  lastTime = 200000000;
    constexpr uint32_t  fps30HZ = 30;
    constexpr uint32_t  fps60HZ = 60;
    constexpr uint32_t  fps90HZ = 90;
    constexpr uint32_t  fps120HZ = 120;
    constexpr pid_t Pid = 0;
    const NodeId id = 0;
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
 * @tc.name: SetAndGetAppSupportState
 * @tc.desc: Verify the result of SetAndGetAppSupportState function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetAppSupportState, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetAppSupportedState(false);
        }
        STEP("3. get app support status") {
            bool ret = idleDetector->GetAppSupportedState();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: SetAndGetAceAnimatorIdleState
 * @tc.desc: Verify the result of SetAndGetAceAnimatorIdleState function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetAceAnimatorIdleState, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set aceAnimator idle status") {
            idleDetector->SetAceAnimatorIdleState(false);
        }
        STEP("3. get aceAnimator idle status") {
            bool ret = idleDetector->GetAceAnimatorIdleState();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: SetAndGetSurfaceTimeState
 * @tc.desc: Verify the result of SetAndGetSurfaceTimeState function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetSurfaceTimeState, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetAppSupportedState(true);
            idleDetector->supportAppBufferList_.insert(idleDetector->supportAppBufferList_.begin(), otherSurface);
        }
        STEP("3. set buffer renew time") {
            idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
            bool ret = idleDetector->GetSurfaceIdleState(lastTime);
            STEP_ASSERT_EQ(ret, false);
        }
        STEP("4. get buffer idle state") {
            idleDetector->SetAppSupportedState(false);
            idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
            bool ret = idleDetector->GetSurfaceIdleState(lastTime);
            STEP_ASSERT_EQ(ret, true);

            idleDetector->supportAppBufferList_.clear();
            idleDetector->SetAppSupportedState(true);
            idleDetector->supportAppBufferList_.insert(idleDetector->supportAppBufferList_.begin(), rosenWeb);
            idleDetector->UpdateSurfaceTime(rosenWeb, currTime, Pid, UIFWKType::FROM_SURFACE);
            ret = idleDetector->GetSurfaceIdleState(lastTime);
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: ThirdFrameNeedHighRefresh
 * @tc.desc: Verify the result of ThirdFrameNeedHighRefresh function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, ThirdFrameNeedHighRefresh, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set surface date") {
            idleDetector->SetAppSupportedState(true);
            idleDetector->ClearAppBufferBlackList();
            bool ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, true);

            idleDetector->SetAceAnimatorIdleState(false);
            ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, true);

            idleDetector->appBufferBlackList_.push_back(aceAnimator);
            ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, false);

            idleDetector->frameTimeMap_[bufferName] = currTime;
            ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, true);

            idleDetector->appBufferBlackList_.push_back(bufferName);
            ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, false);

            idleDetector->appBufferBlackList_.push_back(otherSurface);
            ret = idleDetector->ThirdFrameNeedHighRefresh();
            STEP_ASSERT_EQ(ret, false);
        }
    }
}

/**
 * @tc.name: GetTouchUpExpectedFPS001
 * @tc.desc: Verify the result of GetTouchUpExpectedFPS001 function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, GetTouchUpExpectedFPS001, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. get surface up expect fps") {
            idleDetector->SetAppSupportedState(true);
            idleDetector->ClearAppBufferList();
            int32_t ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->SetAceAnimatorIdleState(false);
            idleDetector->frameTimeMap_[bufferName] = currTime;
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->frameTimeMap_.clear();
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps90HZ));
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps90HZ);

            idleDetector->frameTimeMap_[bufferName] = currTime;
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->frameTimeMap_[flutterBuffer] = currTime;
            idleDetector->appBufferList_.push_back(std::make_pair(flutterBuffer, fps90HZ));
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->frameTimeMap_[rosenWeb] = currTime;
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);
        }
    }
}

/**
 * @tc.name: GetTouchUpExpectFPS002
 * @tc.desc: Verify the result of GetTouchUpExpectFPS002 function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, GetTouchUpExpectFPS002, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. get surface up expect fps") {
            idleDetector->SetAppSupportedState(true);

            idleDetector->frameTimeMap_[rosenWeb] = currTime;
            idleDetector->frameTimeMap_[flutterBuffer] = currTime;
            idleDetector->frameTimeMap_[bufferName] = currTime;

            idleDetector->ClearAppBufferList();
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(flutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps60HZ));
            int32_t ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);

            idleDetector->ClearAppBufferList();
            idleDetector->SetAceAnimatorIdleState(true);
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(flutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps60HZ));
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps90HZ);

            idleDetector->ClearAppBufferList();
            idleDetector->SetAceAnimatorIdleState(false);
            idleDetector->appBufferList_.push_back(std::make_pair(aceAnimator, fps120HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(rosenWeb, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(flutterBuffer, fps90HZ));
            idleDetector->appBufferList_.push_back(std::make_pair(bufferName, fps60HZ));
            ret = idleDetector->GetTouchUpExpectedFPS();
            STEP_ASSERT_EQ(ret, fps120HZ);
        }
    }
}

/**
 * @tc.name: GetUiFrameworkTypeTable
 * @tc.desc: Verify the result of GetUiFrameworkTypeTable function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, GetUiFrameworkTypeTable, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. set app support status") {
            idleDetector->SetAppSupportedState(true);
            idleDetector->supportAppBufferList_.push_back(otherSurface);
            auto uiFrameworkTypeList = idleDetector->GetUiFrameworkTypeTable();
            auto ret = std::count(uiFrameworkTypeList.begin(), uiFrameworkTypeList.end(), otherSurface);
            STEP_ASSERT_GT(ret, 0);
            ret = std::count(uiFrameworkTypeList.begin(), uiFrameworkTypeList.end(), flutterBuffer);
            STEP_ASSERT_EQ(ret, 0);
        }
    }
}

/**
 * @tc.name: UpdateAndGetAnimatorExpectedFrameRate001
 * @tc.desc: Verify the result of UpdateAndGetAnimatorExpectedFrameRate001 function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, UpdateAndGetAnimatorExpectedFrameRate001, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update animator expected frame rate") {
            idleDetector->UpdateAnimatorExpectedFrameRate(-2);
        }
        STEP("3. get animator expected frame rate") {
            int32_t ret = idleDetector->GetAnimatorExpectedFrameRate();
            STEP_ASSERT_EQ(ret, ANIMATOR_NOT_RUNNING);
        }
    }
}

/**
 * @tc.name: UpdateAndGetAnimatorExpectedFrameRate002
 * @tc.desc: Verify the result of UpdateAndGetAnimatorExpectedFrameRate002 function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, UpdateAndGetAnimatorExpectedFrameRate002, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update animator expected frame rate") {
            idleDetector->UpdateAnimatorExpectedFrameRate(60);
        }
        STEP("3. get animator expected frame rate") {
            int32_t ret = idleDetector->GetAnimatorExpectedFrameRate();
            STEP_ASSERT_EQ(ret, 60);
        }
    }
}

/**
 * @tc.name: ResetAnimatorExpectedFrameRate
 * @tc.desc: Verify the result of ResetAnimatorExpectedFrameRate function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, ResetAnimatorExpectedFrameRate, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update animator expected frame rate") {
            idleDetector->UpdateAnimatorExpectedFrameRate(120);
        }
        STEP("3. reset animator expected frame rate") {
            idleDetector->ResetAnimatorExpectedFrameRate();
        }
        STEP("4. get animator expected frame rate") {
            int32_t ret = idleDetector->GetAnimatorExpectedFrameRate();
            STEP_ASSERT_EQ(ret, ANIMATOR_NOT_RUNNING);
        }
    }
}
} // namespace Rosen
} // namespace OHOS