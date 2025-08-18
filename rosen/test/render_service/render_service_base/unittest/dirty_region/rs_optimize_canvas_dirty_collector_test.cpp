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

#include <parameter.h>
#include <parameters.h>

#include "dirty_region/rs_optimize_canvas_dirty_collector.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
const NodeId TARGET_NODE_ID = 9999999999;
const NodeId INVALID_NODE_ID = 99999999999;

class RSOptimizeCanvasDirtyCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOptimizeCanvasDirtyCollectorTest::SetUpTestCase() {}
void RSOptimizeCanvasDirtyCollectorTest::TearDownTestCase() {}
void RSOptimizeCanvasDirtyCollectorTest::SetUp() {}
void RSOptimizeCanvasDirtyCollectorTest::TearDown() {}

/*
 * @tc.name: CalcCmdlistDrawRegionFromOpItem001
 * @tc.desc: Test function CalcCmdlistDrawRegionFromOpItem when modifier's cmdlistDrawRegion is empty
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSOptimizeCanvasDirtyCollectorTest, CalcCmdlistDrawRegionFromOpItem001, TestSize.Level1)
{
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CONTENT_STYLE, property, id, ModifierNG::RSPropertyType::CONTENT_STYLE);
    ASSERT_NE(modifier, nullptr);
    auto cmdlistDrawRegion = RSOptimizeCanvasDirtyCollector::GetInstance().CalcCmdlistDrawRegionFromOpItem(modifier);
    ASSERT_EQ(cmdlistDrawRegion.IsEmpty(), true);
}

/*
 * @tc.name: CalcCmdlistDrawRegionFromOpItem002
 * @tc.desc: Test function CalcCmdlistDrawRegionFromOpItem when modifier's cmdlistDrawRegion is not empty
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSOptimizeCanvasDirtyCollectorTest, CalcCmdlistDrawRegionFromOpItem002, TestSize.Level1)
{
    auto optimizeCanvasDrawRegionEnabled = RSSystemProperties::GetOptimizeCanvasDrawRegionEnabled();
    if (!optimizeCanvasDrawRegionEnabled) {
        system::SetParameter("rosen.graphic.optimizeCanvasDrawRegion.enabled", "1");
    }

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    ASSERT_NE(drawCmdList, nullptr);
    ASSERT_EQ(drawCmdList->GetCmdlistDrawRegion().IsEmpty(), true);
    drawCmdList->SetWidth(1);
    drawCmdList->SetHeight(1);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::CONTENT_STYLE, property, id, ModifierNG::RSPropertyType::CONTENT_STYLE);
    ASSERT_NE(modifier, nullptr);
    auto cmdlistDrawRegion = RSOptimizeCanvasDirtyCollector::GetInstance().CalcCmdlistDrawRegionFromOpItem(modifier);
    ASSERT_EQ(cmdlistDrawRegion.IsEmpty(), true);
}

/*
 * @tc.name: SetOptimizeCanvasDirtyPidList001
 * @tc.desc: Test function SetOptimizeCanvasDirtyPidList when pidList is not include targetPid
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSOptimizeCanvasDirtyCollectorTest, SetOptimizeCanvasDirtyPidList001, TestSize.Level1)
{
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);

    NodeId nodeId = 1;
    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(nodeId));
    
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/*
 * @tc.name: SetOptimizeCanvasDirtyPidList002
 * @tc.desc: Test function SetOptimizeCanvasDirtyPidList when pidList include targetPid
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSOptimizeCanvasDirtyCollectorTest, SetOptimizeCanvasDirtyPidList002, TestSize.Level1)
{
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);

    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(TARGET_NODE_ID));
    
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), true);
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}

/*
 * @tc.name: IsOptimizeCanvasDirtyEnabled001
 * @tc.desc: Test function IsOptimizeCanvasDirtyEnabled
 * @tc.type: FUNC
 * @tc.require: issueICSPON
 */
HWTEST_F(RSOptimizeCanvasDirtyCollectorTest, IsOptimizeCanvasDirtyEnabled001, TestSize.Level1)
{
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);

    std::vector<pid_t> pidList;
    pidList.emplace_back(ExtractPid(TARGET_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), true);
    pidList.assign(1, ExtractPid(INVALID_NODE_ID));
    RSOptimizeCanvasDirtyCollector::GetInstance().SetOptimizeCanvasDirtyPidList(pidList);
    ASSERT_EQ(RSOptimizeCanvasDirtyCollector::GetInstance().IsOptimizeCanvasDirtyEnabled(TARGET_NODE_ID), false);
}
} // namespace OHOS::Rosen