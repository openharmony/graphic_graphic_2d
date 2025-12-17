/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "feature/power_off_render_skip/rs_power_off_render_skip_manager.h"

#include "parameters.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_screen_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPowerOffRenderSkipTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    NodeId curNodeId_ = 1;
    void RegisterScreenNode(
        RSRenderNodeMap& renderNodeMap, ScreenId screenId, ScreenPowerStatus powerStatus, bool disableControl);
};

void RSPowerOffRenderSkipTest::SetUpTestCase() {}
void RSPowerOffRenderSkipTest::TearDownTestCase() {}
void RSPowerOffRenderSkipTest::SetUp() {}
void RSPowerOffRenderSkipTest::TearDown() {}

void RSPowerOffRenderSkipTest::RegisterScreenNode(
    RSRenderNodeMap& renderNodeMap, ScreenId screenId, ScreenPowerStatus powerStatus, bool disableControl)
{
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(curNodeId_++, screenId, context);
    screenNode->screenProperty_.powerStatus_ = powerStatus;
    screenNode->screenProperty_.disablePowerOffRenderControl_ = disableControl;
    renderNodeMap.RegisterRenderNode(screenNode);
}

/*
 * @tc.name: CheckRenderSkipStatus_001
 * @tc.desc: Test CheckRenderSkipStatus, if single screen on, nothing to skip
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(RSPowerOffRenderSkipTest, CheckRenderSkipStatus_001, TestSize.Level1)
{
    RSRenderNodeMap renderNodeMap;
    ScreenId screenId = 1;
    RegisterScreenNode(renderNodeMap, screenId, ScreenPowerStatus::POWER_STATUS_ON, false);
    RSPowerOffRenderSkipManager::Instance().CheckRenderSkipStatus(renderNodeMap);
    // with wrong screen id, nothing to skip
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(INVALID_SCREEN_ID));
    // with correct screen id, but screen is on, nothing to skip
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(screenId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetAllScreenRenderSkipStatus());
}

/*
 * @tc.name: CheckRenderSkipStatus_002
 * @tc.desc: Test CheckRenderSkipStatus, if single screen off, skip render
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(RSPowerOffRenderSkipTest, CheckRenderSkipStatus_002, TestSize.Level1)
{
    RSRenderNodeMap renderNodeMap;
    ScreenId screenId = 1;
    RegisterScreenNode(renderNodeMap, screenId, ScreenPowerStatus::POWER_STATUS_OFF, false);
    RSPowerOffRenderSkipManager::Instance().CheckRenderSkipStatus(renderNodeMap);
    // with correct screen id, but screen is off, should skip render
    ASSERT_TRUE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(screenId));
    ASSERT_TRUE(RSPowerOffRenderSkipManager::Instance().GetAllScreenRenderSkipStatus());
}

/*
 * @tc.name: CheckRenderSkipStatus_003
 * @tc.desc: Test CheckRenderSkipStatus, multiple screens with different status
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(RSPowerOffRenderSkipTest, CheckRenderSkipStatus_003, TestSize.Level1)
{
    RSRenderNodeMap renderNodeMap;
    ScreenId powerOnId = 1;
    ScreenId powerOffId = 2;
    RegisterScreenNode(renderNodeMap, powerOnId, ScreenPowerStatus::POWER_STATUS_ON, false);
    RegisterScreenNode(renderNodeMap, powerOffId, ScreenPowerStatus::POWER_STATUS_OFF, false);
    RSPowerOffRenderSkipManager::Instance().CheckRenderSkipStatus(renderNodeMap);

    // only the correct power off screen id should skip render
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOnId));
    ASSERT_TRUE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOffId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetAllScreenRenderSkipStatus());
}

/*
 * @tc.name: CheckRenderSkipStatus_004
 * @tc.desc: Test CheckRenderSkipStatus, if render control is disabled.
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(RSPowerOffRenderSkipTest, CheckRenderSkipStatus_004, TestSize.Level1)
{
    RSRenderNodeMap renderNodeMap;
    ScreenId powerOnId = 1;
    ScreenId powerOffId = 2;
    RegisterScreenNode(renderNodeMap, powerOnId, ScreenPowerStatus::POWER_STATUS_ON, true);
    RegisterScreenNode(renderNodeMap, powerOffId, ScreenPowerStatus::POWER_STATUS_OFF, true);
    RSPowerOffRenderSkipManager::Instance().CheckRenderSkipStatus(renderNodeMap);

    // render skip is disabled for both screens
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOnId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOffId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetAllScreenRenderSkipStatus());
}

/*
 * @tc.name: CheckRenderSkipStatus_005
 * @tc.desc: Test CheckRenderSkipStatus, if render control is disabled by sysProp.
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(RSPowerOffRenderSkipTest, CheckRenderSkipStatus_005, TestSize.Level1)
{
    RSRenderNodeMap renderNodeMap;
    ScreenId powerOnId = 1;
    ScreenId powerOffId = 2;
    RegisterScreenNode(renderNodeMap, powerOnId, ScreenPowerStatus::POWER_STATUS_ON, false);
    RegisterScreenNode(renderNodeMap, powerOffId, ScreenPowerStatus::POWER_STATUS_OFF, false);
    RSPowerOffRenderSkipManager::Instance().CheckRenderSkipStatus(renderNodeMap);

    bool sysProp = RSSystemProperties::GetSkipDisplayIfScreenOffEnabled();
    system::SetParameter("rosen.graphic.screenoffskipdisplayenabled", "0");
    // render skip is disabled for both screens
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(INVALID_SCREEN_ID));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOnId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetScreenRenderSkipStatus(powerOffId));
    ASSERT_FALSE(RSPowerOffRenderSkipManager::Instance().GetAllScreenRenderSkipStatus());
    // restore sysProp
    system::SetParameter("rosen.graphic.screenoffskipdisplayenabled", sysProp ? "1" : "0");
}
}