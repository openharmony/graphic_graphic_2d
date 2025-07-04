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
#include "hgm_test_base.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t ACE_ANIMATOR_OFFSET = 16;
const std::string bufferName = "frameBuffer";
const std::string aceAnimator = "AceAnimato";
const std::string rosenWeb = "RosenWeb";
const std::string flutterBuffer = "oh_flutter";
const std::string otherSurface = "Other_SF";
constexpr uint64_t  fromtest = 3;
constexpr uint64_t  currTime = 100000000;
constexpr uint64_t  lastTime = 200000000;
constexpr uint32_t  fps30HZ = 30;
constexpr uint32_t  fps60HZ = 60;
constexpr uint32_t  fps90HZ = 90;
constexpr uint32_t  fps120HZ = 120;
constexpr pid_t Pid = 0;
const NodeId id = 0;
}

class HgmIdleDetectorTest : public HgmTestBase {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmIdleDetectorTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}
void HgmIdleDetectorTest::TearDownTestCase() {}
void HgmIdleDetectorTest::SetUp() {}
void HgmIdleDetectorTest::TearDown() {}

/**
 * @tc.name: SetAndGetAppSupportState
 * @tc.desc: Verify the result of SetAndGetAppSupportState function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetAppSupportState, Function | SmallTest | Level0)
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
HWTEST_F(HgmIdleDetectorTest, SetAndGetAceAnimatorIdleState, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    // fail case: when AppSupportedState false && bufferFpsMap_ not contains
    ASSERT_NE(idleDetector, nullptr);
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_TRUE(idleDetector->GetAceAnimatorIdleState());

    // fail case: when AppSupportedState false
    idleDetector->SetAppSupportedState(false);
    idleDetector->bufferFpsMap_[aceAnimator] = fps120HZ;
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_TRUE(idleDetector->GetAceAnimatorIdleState());

    // fail case: when bufferFpsMap_ not contains
    idleDetector->SetAppSupportedState(true);
    idleDetector->bufferFpsMap_.clear();
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_TRUE(idleDetector->GetAceAnimatorIdleState());

    // success case: fits all
    idleDetector->SetAppSupportedState(true);
    idleDetector->bufferFpsMap_[aceAnimator] = fps120HZ;
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_FALSE(idleDetector->GetAceAnimatorIdleState());
}

/**
 * @tc.name: SetAndGetSurfaceTimeState
 * @tc.desc: Verify the result of SetAndGetSurfaceTimeState function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, SetAndGetSurfaceTimeState, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    // fail case: when AppSupportedState false && bufferFpsMap_ not contains
    ASSERT_NE(idleDetector, nullptr);
    idleDetector->surfaceTimeMap_.clear();
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());

    // fail case: when AppSupportedState false
    idleDetector->SetAppSupportedState(false);
    idleDetector->bufferFpsMap_[bufferName] = fps120HZ;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());
    idleDetector->surfaceTimeMap_.clear();

    // fail case: when bufferFpsMap_ not contains
    idleDetector->SetAppSupportedState(true);
    idleDetector->bufferFpsMap_.clear();
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());
    idleDetector->bufferFpsMap_[otherSurface] = 0;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());
    idleDetector->bufferFpsMap_[otherSurface] = fps120HZ;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_FALSE(idleDetector->GetSurfaceIdleState());
    idleDetector->surfaceTimeMap_.clear();

    // success case: fits all && match bufferName
    idleDetector->bufferFpsMap_[bufferName] = fps120HZ;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_FALSE(idleDetector->GetSurfaceIdleState());
    idleDetector->surfaceTimeMap_.clear();

    // fail case: black list
    idleDetector->bufferFpsMap_.clear();
    idleDetector->bufferFpsMap_[bufferName] = 0;
    idleDetector->bufferFpsMap_[otherSurface] = fps120HZ;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_SURFACE);
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());
    idleDetector->surfaceTimeMap_.clear();

    // fail case: not match otherSurface
    idleDetector->bufferFpsMap_.clear();
    idleDetector->bufferFpsMap_[otherSurface] = fps120HZ;
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, UIFWKType::FROM_UNKNOWN);
    ASSERT_TRUE(idleDetector->GetSurfaceIdleState());
    idleDetector->UpdateSurfaceTime("", currTime, Pid, UIFWKType::FROM_SURFACE);
    OHOS::Rosen::UIFWKType pidType = static_cast<OHOS::Rosen::UIFWKType>(fromtest);
    idleDetector->UpdateSurfaceTime(bufferName, currTime, Pid, pidType);
    std::string surfaceName = "surfaceName";
    idleDetector->surfaceTimeMap_.try_emplace(surfaceName, currTime);
    idleDetector->UpdateSurfaceState(currTime);
}

/**
 * @tc.name: GetTouchUpExpectedFPS001
 * @tc.desc: Verify the result of ThirdFrameNeedHighRefresh function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, GetTouchUpExpectedFPS001, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    // bufferFpsMap_ not contains
    idleDetector->SetAppSupportedState(true);
    idleDetector->bufferFpsMap_.clear();
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), 0);

    // bufferFpsMap_ contains && check AceAnimatorIdleState
    idleDetector->bufferFpsMap_[aceAnimator] = fps90HZ;
    idleDetector->SetAceAnimatorIdleState(true);
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), 0);
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps90HZ);

    // bufferFpsMap_ contains && check bufferName
    idleDetector->bufferFpsMap_[bufferName] = fps60HZ;
    idleDetector->surfaceTimeMap_[bufferName] = currTime;
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps90HZ);
    idleDetector->bufferFpsMap_[bufferName] = fps120HZ;
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps120HZ);

    // decide max
    idleDetector->bufferFpsMap_[flutterBuffer] = fps90HZ;
    idleDetector->surfaceTimeMap_[flutterBuffer] = currTime;
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps120HZ);

    // decide max
    idleDetector->surfaceTimeMap_[rosenWeb] = currTime;
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps120HZ);
}

/**
 * @tc.name: GetTouchUpExpectedFPS002
 * @tc.desc: Verify the result of GetTouchUpExpectedFPS function
 * @tc.type: FUNC
 * @tc.require: IAFG2V
 */
HWTEST_F(HgmIdleDetectorTest, GetTouchUpExpectedFPS002, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();
    ASSERT_NE(idleDetector, nullptr);

    idleDetector->SetAppSupportedState(true);
    idleDetector->bufferFpsMap_.clear();
    idleDetector->bufferFpsMap_[aceAnimator] = fps120HZ;
    idleDetector->bufferFpsMap_[rosenWeb] = fps90HZ;
    idleDetector->bufferFpsMap_[bufferName] = fps60HZ;
    idleDetector->surfaceTimeMap_.clear();

    // max of rosenWeb & bufferName
    idleDetector->surfaceTimeMap_[rosenWeb] = currTime;
    idleDetector->surfaceTimeMap_[bufferName] = currTime;
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps90HZ);

    // check effective: aceAnimator
    idleDetector->SetAceAnimatorIdleState(false);
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps120HZ);

    // check aceAnimator ineffective
    idleDetector->SetAceAnimatorIdleState(true);
    ASSERT_EQ(idleDetector->GetTouchUpExpectedFPS(), fps90HZ);
}

/**
 * @tc.name: UpdateAndGetAceAnimatorExpectedFrameRate001
 * @tc.desc: Verify the result of UpdateAndGetAceAnimatorExpectedFrameRate001 function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, UpdateAndGetAceAnimatorExpectedFrameRate001, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update ace animator expected frame rate") {
            // verify out of range conditions
            idleDetector->UpdateAceAnimatorExpectedFrameRate(-2);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, HgmIdleDetector::ANIMATOR_NOT_RUNNING);

            idleDetector->UpdateAceAnimatorExpectedFrameRate(0);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 0);
            idleDetector->UpdateAceAnimatorExpectedFrameRate(60 << ACE_ANIMATOR_OFFSET);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 0);

            idleDetector->ResetAceAnimatorExpectedFrameRate();
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, HgmIdleDetector::ANIMATOR_NOT_RUNNING);
            idleDetector->UpdateAceAnimatorExpectedFrameRate(90 << ACE_ANIMATOR_OFFSET);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 90);
            idleDetector->UpdateAceAnimatorExpectedFrameRate(60 << ACE_ANIMATOR_OFFSET);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 90);
            idleDetector->UpdateAceAnimatorExpectedFrameRate(0);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 0);

            idleDetector->UpdateAceAnimatorExpectedFrameRate((90 << ACE_ANIMATOR_OFFSET) + 1);
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 0);
        }
    }
}

/**
 * @tc.name: UpdateAndGetAceAnimatorExpectedFrameRate002
 * @tc.desc: Verify the result of UpdateAndGetAceAnimatorExpectedFrameRate002 function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, UpdateAndGetAceAnimatorExpectedFrameRate002, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update ace animator expected frame rate") {
            idleDetector->UpdateAceAnimatorExpectedFrameRate(60 << ACE_ANIMATOR_OFFSET);
        }
        STEP("3. get ace animator expected frame rate") {
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, 60);
        }
    }
}

/**
 * @tc.name: ResetAceAnimatorExpectedFrameRate
 * @tc.desc: Verify the result of ResetAceAnimatorExpectedFrameRate function
 * @tc.type: FUNC
 * @tc.require: IAW09K
 */
HWTEST_F(HgmIdleDetectorTest, ResetAceAnimatorExpectedFrameRate, Function | SmallTest | Level0)
{
    std::unique_ptr<HgmIdleDetector> idleDetector = std::make_unique<HgmIdleDetector>();

    PART("CaseDescription") {
        STEP("1. get an idleDetector") {
            STEP_ASSERT_NE(idleDetector, nullptr);
        }
        STEP("2. update ace animator expected frame rate") {
            idleDetector->UpdateAceAnimatorExpectedFrameRate(120);
        }
        STEP("3. reset ace animator expected frame rate") {
            idleDetector->ResetAceAnimatorExpectedFrameRate();
        }
        STEP("4. get ace animator expected frame rate") {
            STEP_ASSERT_EQ(idleDetector->aceAnimatorExpectedFrameRate_, HgmIdleDetector::ANIMATOR_NOT_RUNNING);
        }
    }
}
} // namespace Rosen
} // namespace OHOS