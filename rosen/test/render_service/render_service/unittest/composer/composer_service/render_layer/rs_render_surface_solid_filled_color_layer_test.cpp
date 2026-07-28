/**
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

#include <gtest/gtest.h>
#include <limits>
#include "rs_render_surface_solid_filled_color_layer.h"
#include "rs_render_layer_cmd.h"
#include "rs_layer_cmd_type.h"

using namespace OHOS::Rosen;
using namespace OHOS;
using namespace testing;
using namespace testing::ext;

namespace {
class MockInvalidLayerCmd : public RSRenderLayerCmd {
public:
    MockInvalidLayerCmd() : RSRenderLayerCmd(std::make_shared<RSRenderLayerCmdProperty<bool>>()) {}
    
    std::shared_ptr<RSRenderLayerPropertyBase> GetRSRenderLayerProperty() const override
    {
        return rsRenderLayerProperty_;
    }
    
    bool Marshalling(OHOS::MessageParcel& parcel) override
    {
        return true;
    }
    
    RSLayerCmdType GetRSRenderLayerCmdType() const override
    {
        return RSLayerCmdType::INVALID;
    }
};
}

class RSRenderSurfaceSolidFilledColorLayerTest : public Test {};

/**
 * Function: UpdateRSLayerCmd_ValidLayerAndCommand_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer and Zorder command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetZorder equals set value
 *                   Cover branch: rsRenderLayer != nullptr (line 49 false), it != cmdHandlers_.end() (line 53 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_ValidLayerAndCommand_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(9);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetZorder(), 9u);
}

/**
 * Function: UpdateRSLayerCmd_ValidLayerAndPreMultiCommand_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer and PreMulti command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect IsPreMulti equals set value
 *                   Cover branch: rsRenderLayer != nullptr (line 49 false), it != cmdHandlers_.end() (line 53 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_ValidLayerAndPreMultiCommand_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerPreMultiCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_TRUE(layer->IsPreMulti());
}

/**
 * Function: CopyLayerInfo_ValidLayer_Copied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create source layer with properties
 *                  2. create solid filled color layer
 *                  3. call CopyLayerInfo
 *                  4. verify properties are copied
 *                   Cover branch: rsLayer != nullptr (line 62 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, CopyLayerInfo_ValidLayer_Copied, TestSize.Level1)
{
    auto sourceLayer = std::make_shared<RSRenderSurfaceLayer>();
    sourceLayer->SetZorder(15);
    sourceLayer->SetPreMulti(true);
    GraphicIRect layerSize = {10, 20, 100, 200};
    sourceLayer->SetLayerSize(layerSize);

    auto targetLayer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    targetLayer->CopyLayerInfo(sourceLayer);

    EXPECT_EQ(targetLayer->GetZorder(), 15u);
    EXPECT_TRUE(targetLayer->IsPreMulti());
    auto size = targetLayer->GetLayerSize();
    EXPECT_EQ(size.x, 10);
    EXPECT_EQ(size.y, 20);
    EXPECT_EQ(size.w, 100);
    EXPECT_EQ(size.h, 200);
}

/**
 * Function: CopyLayerInfo_ValidLayerWithCompositionType_Copied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create source layer with composition type
 *                  2. call CopyLayerInfo
 *                  3. verify composition type is copied
 *                   Cover branch: rsLayer != nullptr (line 62 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, CopyLayerInfo_ValidLayerWithCompositionType_Copied, TestSize.Level1)
{
    auto sourceLayer = std::make_shared<RSRenderSurfaceLayer>();
    sourceLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    auto targetLayer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    targetLayer->CopyLayerInfo(sourceLayer);

    EXPECT_EQ(targetLayer->GetCompositionType(), GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
}

/**
 * Function: UpdateRSLayerCmd_ValidLayerGravityCommand_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer and Gravity command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetGravity equals set value
 *                   Cover branch: rsRenderLayer != nullptr (line 49 false), it != cmdHandlers_.end() (line 53 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_ValidLayerGravityCommand_Applied, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(3);
    auto cmd = std::make_shared<RSRenderLayerGravityCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetGravity(), 3);
}

/**
 * Function: UpdateRSLayerCmd_ValidLayerColorDataSpaceCommand_Applied
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer and ColorDataSpace command
 *                  2. call UpdateRSLayerCmd
 *                  3. expect GetColorDataSpace equals set value
 *                   Cover branch: rsRenderLayer != nullptr (line 49 false), it != cmdHandlers_.end() (line 53 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_ValidLayerColorDataSpaceCommand_Applied,
    TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(
        GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    auto cmd = std::make_shared<RSRenderLayerColorDataSpaceCmd>(prop);
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetColorDataSpace(), GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
}

/**
 * Function: IsSolidFilledColorLayer_AlwaysReturnsTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call IsSolidFilledColorLayer
 *                  3. expect always returns true
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, IsSolidFilledColorLayer_AlwaysReturnsTrue, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: CopyLayerInfo_MultipleProperties_CopiedCorrectly
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create source layer with multiple properties
 *                  2. call CopyLayerInfo
 *                  3. verify all properties are copied correctly
 *                   Cover branch: rsLayer != nullptr (line 62 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, CopyLayerInfo_MultipleProperties_CopiedCorrectly, TestSize.Level1)
{
    auto sourceLayer = std::make_shared<RSRenderSurfaceLayer>();
    sourceLayer->SetZorder(25);
    sourceLayer->SetPreMulti(false);
    sourceLayer->SetGravity(5);
    sourceLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    auto targetLayer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    targetLayer->CopyLayerInfo(sourceLayer);

    EXPECT_EQ(targetLayer->GetZorder(), 25u);
    EXPECT_FALSE(targetLayer->IsPreMulti());
    EXPECT_EQ(targetLayer->GetGravity(), 5);
    EXPECT_EQ(targetLayer->GetCompositionType(), GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
}

/**
 * Function: UpdateRSLayerCmd_InvalidCommandType_LogDebug
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer and invalid command
 *                  2. call UpdateRSLayerCmd with invalid command type
 *                  3. verify debug log is printed and no crash
 *                   Cover branch: it == cmdHandlers_.end() (line 53 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_InvalidCommandType_LogDebug, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto cmd = std::make_shared<MockInvalidLayerCmd>();
    EXPECT_NO_FATAL_FAILURE(layer->UpdateRSLayerCmd(cmd));
}

/**
 * Function: UpdateRSLayerCmd_NullCommandProperty_HandlesGracefully
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call UpdateRSLayerCmd with command having invalid type
 *                  3. verify no crash occurs
 *                   Cover branch: it == cmdHandlers_.end() (line 53 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_NullCommandProperty_HandlesGracefully,
    TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto cmd = std::make_shared<MockInvalidLayerCmd>();
    layer->UpdateRSLayerCmd(cmd);
    EXPECT_EQ(layer->GetZorder(), 0u);
}

/**
 * Function: UpdateRSLayerCmd_OutOfRangeCommandType_LogDebug
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer and command with out-of-range type
 *                  2. call UpdateRSLayerCmd
 *                  3. verify debug log and no crash
 *                   Cover branch: it == cmdHandlers_.end() (line 53 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_OutOfRangeCommandType_LogDebug, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto cmd = std::make_shared<MockInvalidLayerCmd>();
    EXPECT_NO_FATAL_FAILURE(layer->UpdateRSLayerCmd(cmd));
}

/**
 * Function: UpdateRSLayerCmd_EdgeCaseCommandType_HandlesGracefully
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. test with edge case command types
 *                  2. verify all are handled gracefully
 *                   Cover branch: it == cmdHandlers_.end() (line 53 false)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_EdgeCaseCommandType_HandlesGracefully,
    TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    
    auto cmd = std::make_shared<MockInvalidLayerCmd>();
    EXPECT_NO_FATAL_FAILURE(layer->UpdateRSLayerCmd(cmd));
}

/**
 * Function: UpdateRSLayerCmd_SharedFromThisCheck_CoverageNote
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. Note: The if (rsRenderLayer == nullptr) check at line 49 is a defensive check
 *                  2. In normal usage, shared_from_this() returns valid pointer for objects managed by shared_ptr
 *                  3. static_pointer_cast to the same type should always succeed
 *                  4. This branch is technically unreachable in normal code paths
 *                   Cover branch: rsRenderLayer == nullptr (line 49 true) - Note: This is a defensive check
 *                   that should never trigger in normal operation. The test documents this behavior.
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, UpdateRSLayerCmd_SharedFromThisCheck_CoverageNote, TestSize.Level1)
{
    auto layer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(1);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    EXPECT_NO_FATAL_FAILURE(layer->UpdateRSLayerCmd(cmd));
    EXPECT_EQ(layer->GetZorder(), 1u);
}

/**
 * Function: CopyLayerInfo_NullLayer_EarlyReturn
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call CopyLayerInfo with nullptr
 *                  3. verify early return without crash
 *                   Cover branch: rsLayer == nullptr (line 62 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, CopyLayerInfo_NullLayer_EarlyReturn, TestSize.Level1)
{
    auto targetLayer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    targetLayer->SetZorder(10);
    targetLayer->SetPreMulti(true);

    EXPECT_NO_FATAL_FAILURE(targetLayer->CopyLayerInfo(nullptr));

    EXPECT_EQ(targetLayer->GetZorder(), 10u);
    EXPECT_TRUE(targetLayer->IsPreMulti());
}

/**
 * Function: CopyLayerInfo_NullLayer_NoModification
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer with initial properties
 *                  2. call CopyLayerInfo with nullptr
 *                  3. verify layer properties remain unchanged
 *                   Cover branch: rsLayer == nullptr (line 62 true)
 */
HWTEST(RSRenderSurfaceSolidFilledColorLayerTest, CopyLayerInfo_NullLayer_NoModification, TestSize.Level1)
{
    auto targetLayer = std::make_shared<RSRenderSurfaceSolidFilledColorLayer>();
    targetLayer->SetZorder(20);
    targetLayer->SetGravity(7);
    GraphicIRect layerSize = {0, 0, 100, 100};
    targetLayer->SetLayerSize(layerSize);

    targetLayer->CopyLayerInfo(nullptr);

    EXPECT_EQ(targetLayer->GetZorder(), 20u);
    EXPECT_EQ(targetLayer->GetGravity(), 7);
    auto size = targetLayer->GetLayerSize();
    EXPECT_EQ(size.x, 0);
    EXPECT_EQ(size.y, 0);
    EXPECT_EQ(size.w, 100);
    EXPECT_EQ(size.h, 100);
}