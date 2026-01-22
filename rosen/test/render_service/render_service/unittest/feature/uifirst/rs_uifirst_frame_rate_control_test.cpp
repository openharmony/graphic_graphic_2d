/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "gtest/gtest.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
namespace OHOS::Rosen {
class RSUifirstFrameRateControlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUifirstFrameRateControlTest::SetUpTestCase()
{
    // Initialize eventInfo_ with default values
    EventInfo eventInfo_;
    eventInfo_.description = "";
    eventInfo_.eventStatus = 0;
}
void RSUifirstFrameRateControlTest::TearDownTestCase() {}
void RSUifirstFrameRateControlTest::SetUp() {}
void RSUifirstFrameRateControlTest::TearDown() {}

/**
 * @tc.name: InstanceTest001
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, InstanceTest001, TestSize.Level1)
{
    OHOS::Rosen::RSUifirstFrameRateControl& instance = OHOS::Rosen::RSUifirstFrameRateControl::Instance();
    EXPECT_NE(&instance, nullptr); // Check if the instance is not null
}

/**
 * @tc.name: InstanceTest002
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, InstanceTest002, TestSize.Level1)
{
    OHOS::Rosen::RSUifirstFrameRateControl& instance1 = OHOS::Rosen::RSUifirstFrameRateControl::Instance();
    OHOS::Rosen::RSUifirstFrameRateControl& instance2 = OHOS::Rosen::RSUifirstFrameRateControl::Instance();
    EXPECT_EQ(&instance1, &instance2); // Check if both instances are the same
}

/**
 * @tc.name: GetSceneIdTest001
 * @tc.desc: Test GetSceneId
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest001, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "LAUNCHER_APP_LAUNCH_FROM_ICON";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_LAUNCH_FROM_ICON);
}

/**
* @tc.name: GetSceneIdTest002
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest002, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "LAUNCHER_APP_SWIPE_TO_HOME";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_SWIPE_TO_HOME);
}

/**
* @tc.name: GetSceneIdTest003
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest003, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "GESTURE_TO_RECENTS";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::GESTURE_TO_RECENTS);
}

/**
* @tc.name: GetSceneIdTest004
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest004, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "LAUNCHER_APP_LAUNCH_FROM_RECENT";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::LAUNCHER_APP_LAUNCH_FROM_RECENT);
}

/**
* @tc.name: GetSceneIdTest005
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest005, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "EXIT_RECENT_2_HOME_ANI";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::EXIT_RECENT_2_HOME_ANI);
}

/**
* @tc.name: GetSceneIdTest006
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest006, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "CLEAR_1_RECENT_ANI";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::CLEAR_1_RECENT_ANI);
}

/**
* @tc.name: GetSceneIdTest007
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest007, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "CLEAR_All_RECENT_ANI";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::CLEAR_ALL_RECENT_ANI);
}

/**
* @tc.name: GetSceneIdTest008
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest008, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "AOD_TO_LAUNCHER";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::AOD_TO_LAUNCHER);
}

/**
* @tc.name: GetSceneIdTest009
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest009, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "LOCKSCREEN_TO_LAUNCHER";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::LOCKSCREEN_TO_LAUNCHER);
}

/**
* @tc.name: GetSceneIdTest0010
* @tc.desc: Test GetSceneId
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, GetSceneIdTest010, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    std::string sceneId = "UNKNOWN_SCENE_ID";
    auto result = control.GetSceneId(sceneId);
    EXPECT_EQ(result, RSUifirstFrameRateControl::SceneId::UNKNOWN);
}

/**
* @tc.name: SetAnimationStartInfo001
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo001, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_ICON";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo002
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo002, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_SWIPE_TO_HOME";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo003
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo003, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "GESTURE_TO_RECENTS";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo004
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo004, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_RECENT";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo005
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo005, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "AOD_TO_LAUNCHER";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo006
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo006, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LOCKSCREEN_TO_LAUNCHER";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo007
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo007, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LOCKSCREEN_TO_LAUNCHER";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo008
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo008, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "UNKNOWN";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationStartInfo009
* @tc.desc: Test SetAnimationStartInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationStartInfo009, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_DOCK";
    control.SetAnimationStartInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo001
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo001, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_ICON";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo002
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo002, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_SWIPE_TO_HOME";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo003
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo003, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_RECENT";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo004
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo004, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "EXIT_RECENT_2_HOME_ANI";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo005
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo005, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "CLEAR_1_RECENT_ANI";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo006
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo006, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "CLEAR_ALL_RECENT_ANI";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo007
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo007, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "UNKNOWN";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
* @tc.name: SetAnimationEndInfo001
* @tc.desc: Test SetAnimationEndInfo
* @tc.type: FUNC
* @tc.require:#ICBWNQ
*/
HWTEST_F(RSUifirstFrameRateControlTest, SetAnimationEndInfo008, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    DataBaseRs eventInfo;
    eventInfo.sceneId = "LAUNCHER_APP_LAUNCH_FROM_DOCK";
    control.SetAnimationEndInfo(eventInfo);
    EXPECT_TRUE(control.forceRefreshOnce_);
}

/**
 * @tc.name: JudgeMultiSubSurfaceTest
 * @tc.desc: Test Judge Multi SubSurface
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, JudgeMultiSubSurfaceTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    RSUifirstFrameRateControl control;
    EXPECT_EQ(control.JudgeMultiSubSurface(node), false);
}

/**
 * @tc.name: GetUifirstFrameDropInternalTest
 * @tc.desc: Test Get Uifirst Frame Drop Internal
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, GetUifirstFrameDropInternalTest, TestSize.Level1)
{
    int frameInterval = 0;
    RSUifirstFrameRateControl control;
    EXPECT_EQ(control.GetUifirstFrameDropInternal(frameInterval), false);
    frameInterval = 1;
    EXPECT_EQ(control.GetUifirstFrameDropInternal(frameInterval), true);
}

/**
 * @tc.name: SubThreadFrameDropDecisionTest
 * @tc.desc: Test Sub Thread Frame Drop Decision and Need RS Uifirst Control Frame Drop
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, SubThreadFrameDropDecisionTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    RSUifirstFrameRateControl control;
    EXPECT_EQ(control.SubThreadFrameDropDecision(node), false);
    EXPECT_EQ(control.NeedRSUifirstControlFrameDrop(node), false);
}

/**
 * @tc.name: StartAnimationTest
 * @tc.desc: Test Set and Judge Start Animation
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, StartAnimationTest, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    control.SetStartAnimation(false);
    EXPECT_EQ(control.JudgeStartAnimation(), false);
    control.SetStartAnimation(true);
    EXPECT_EQ(control.JudgeStartAnimation(), true);
}

/**
 * @tc.name: StopAnimationTest
 * @tc.desc: Test Set and Judge StopAnimation
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, StopAnimationTest, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    control.SetStopAnimation(false);
    EXPECT_EQ(control.JudgeStopAnimation(), false);
    control.SetStopAnimation(true);
    EXPECT_EQ(control.JudgeStopAnimation(), true);
}

/**
 * @tc.name: MultTaskAnimationTest
 * @tc.desc: Test Set and Judge MultTaskAnimation
 * @tc.type: FUNC
 * @tc.require:#ICBWNQ
 */
HWTEST_F(RSUifirstFrameRateControlTest, MultTaskAnimationTest, TestSize.Level1)
{
    RSUifirstFrameRateControl control;
    control.SetMultTaskAnimation(false);
    EXPECT_EQ(control.JudgeMultTaskAnimation(), false);
    control.SetMultTaskAnimation(true);
    EXPECT_EQ(control.JudgeMultTaskAnimation(), true);
}

}
