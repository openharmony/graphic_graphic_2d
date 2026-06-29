/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

class RequestControllerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(RequestControllerTest, Constructor001, TestSize.Level1)
{
    RequestController controller;
}

HWTEST_F(RequestControllerTest, IncStayOnCount001, TestSize.Level1)
{
    RequestController controller;
    controller.IncStayOnCount();
    controller.IncStayOnCount();
    controller.IncStayOnCount();
}

HWTEST_F(RequestControllerTest, IsLongTermOff_DefaultFalse, TestSize.Level1)
{
    RequestController controller;
    ASSERT_FALSE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, IsLongTermOff_After101Off_True, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i <= 101; ++i) {
        controller.Update(false, PlanStatus::OFF, false, false);
    }
    ASSERT_TRUE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, Update_NeedLeave_ResetsNoLeaveCount, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());

    controller.Update(true, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_OFF_IncrementsStayOff, TestSize.Level1)
{
    RequestController controller;
    ASSERT_FALSE(controller.IsLongTermOff());
    for (int i = 0; i < 100; ++i) {
        controller.Update(false, PlanStatus::OFF, false, false);
    }
    ASSERT_FALSE(controller.IsLongTermOff());

    controller.Update(false, PlanStatus::OFF, false, false);
    ASSERT_TRUE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, Update_PREPARE_ResetsStayOff, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 50; ++i) {
        controller.Update(false, PlanStatus::OFF, false, false);
    }
    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, Update_ON_CanDoDirectAndHardware, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, true, true);
}

HWTEST_F(RequestControllerTest, Update_ON_OnlyHardware, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, false, true);
}

HWTEST_F(RequestControllerTest, Update_ON_OnlyDirectComposition, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, true, false);
}

HWTEST_F(RequestControllerTest, Update_ON_NoFlags, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_SizeNotThree, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::LEAVE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_LEAVE_SizeThreeAvgLeq5_ThresholdDoubles, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 3; ++i) {
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

HWTEST_F(RequestControllerTest, Update_LEAVE_SizeGt3_PopFront, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 4; ++i) {
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

HWTEST_F(RequestControllerTest, Update_LEAVE_AvgGt5_NoThresholdDouble, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 10; ++j) {
            controller.IncStayOnCount();
        }
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

HWTEST_F(RequestControllerTest, Update_LEAVE_DoDirectExpectedFalse_ThresholdDoubles, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, true, false);
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_ConditionTrue_ThresholdDoubles, TestSize.Level1)
{
    RequestController controller;
    controller.IncStayOnCount();
    controller.Update(false, PlanStatus::ON, false, false);
    controller.Update(false, PlanStatus::LEAVE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_LEAVE_ConditionFalse_ThresholdHalved, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_HardwareEnabledHigh_ThresholdHalved, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, false, true);
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_FullCycleWithAssertions, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, true, true);
    controller.Update(false, PlanStatus::ON, false, false);
    for (int i = 0; i < 2; ++i) {
        controller.IncStayOnCount();
    }
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, CheckNeedRequest_Default_False, TestSize.Level1)
{
    RequestController controller;
    ASSERT_FALSE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, CheckNeedRequest_AfterOneUpdate_True, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, CheckNeedRequest_AfterNeedLeave_False, TestSize.Level1)
{
    RequestController controller;
    controller.Update(true, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, CheckNeedRequest_ThresholdIncreased_NeedsMoreCalls, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 3; ++i) {
        controller.Update(false, PlanStatus::ON, true, false);
    }
    controller.Update(false, PlanStatus::LEAVE, false, false);

    controller.Update(true, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());

    for (int i = 0; i < 3; ++i) {
        controller.Update(false, PlanStatus::PREPARE, false, false);
        ASSERT_FALSE(controller.CheckNeedRequest());
    }
    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Reset_ClearsLongTermOff, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 101; ++i) {
        controller.Update(false, PlanStatus::OFF, false, false);
    }
    ASSERT_TRUE(controller.IsLongTermOff());

    controller.Reset();
    ASSERT_FALSE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, Reset_ResetsThresholdAndClearDeque, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 3; ++i) {
        controller.Update(false, PlanStatus::ON, true, false);
    }
    controller.Update(false, PlanStatus::LEAVE, false, false);

    controller.Update(true, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Reset();
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Update(false, PlanStatus::PREPARE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, FullCycle_AllMethods, TestSize.Level1)
{
    RequestController controller;

    ASSERT_FALSE(controller.CheckNeedRequest());
    ASSERT_FALSE(controller.IsLongTermOff());

    controller.IncStayOnCount();
    controller.IncStayOnCount();

    controller.Update(false, PlanStatus::ON, false, false);
    controller.Update(false, PlanStatus::LEAVE, false, false);

    ASSERT_TRUE(controller.CheckNeedRequest());

    controller.Update(false, PlanStatus::OFF, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());

    controller.Update(true, PlanStatus::OFF, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Reset();
    ASSERT_FALSE(controller.IsLongTermOff());
    ASSERT_FALSE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_LEAVE_ClampMin, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_ClampMax, TestSize.Level1)
{
    RequestController controller;
    for (int j = 0; j < 12; ++j) {
        for (int i = 0; i < 3; ++i) {
            controller.IncStayOnCount();
        }
        controller.Update(false, PlanStatus::ON, true, false);
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

HWTEST_F(RequestControllerTest, Update_LEAVE_ManyLeaves_ThresholdClamps, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 32; ++i) {
        controller.IncStayOnCount();
        controller.IncStayOnCount();
        controller.IncStayOnCount();
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

HWTEST_F(RequestControllerTest, Update_LEAVE_NoDirectOnly, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::ON, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());

    controller.Update(false, PlanStatus::LEAVE, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, NeedLeaveAndNonLeaveSequence, TestSize.Level1)
{
    RequestController controller;
    controller.Update(true, PlanStatus::PREPARE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Update(false, PlanStatus::ON, false, false);
    ASSERT_TRUE(controller.CheckNeedRequest());

    controller.Update(true, PlanStatus::ON, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_OFF_And_LEAVE_Sequence, TestSize.Level1)
{
    RequestController controller;
    for (int i = 0; i < 50; ++i) {
        controller.Update(false, PlanStatus::OFF, false, false);
    }
    ASSERT_FALSE(controller.IsLongTermOff());

    controller.Update(false, PlanStatus::LEAVE, false, false);
    ASSERT_FALSE(controller.IsLongTermOff());
}

HWTEST_F(RequestControllerTest, CheckNeedRequest_ThresholdCheck, TestSize.Level1)
{
    RequestController controller;
    ASSERT_FALSE(controller.CheckNeedRequest());

    controller.Update(true, PlanStatus::LEAVE, false, false);
    ASSERT_FALSE(controller.CheckNeedRequest());
}

HWTEST_F(RequestControllerTest, Update_AllPlanStatusCombinations, TestSize.Level1)
{
    RequestController controller;
    controller.Update(false, PlanStatus::OFF, false, false);
    controller.Update(false, PlanStatus::PREPARE, false, false);
    controller.Update(false, PlanStatus::ON, false, false);
    controller.Update(false, PlanStatus::LEAVE, false, false);
}

HWTEST_F(RequestControllerTest, Update_LEAVE_MultipleCycles, TestSize.Level1)
{
    RequestController controller;
    for (int cycle = 0; cycle < 5; ++cycle) {
        for (int i = 0; i < 2; ++i) {
            controller.IncStayOnCount();
        }
        controller.Update(false, PlanStatus::ON, false, false);
        controller.Update(false, PlanStatus::LEAVE, false, false);
    }
}

} // namespace
} // namespace OHOS::Rosen