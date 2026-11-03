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
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"
#include "rs_render_composer_context.h"
#include "rs_render_surface_rcd_layer.h"
#include "rs_render_surface_solid_filled_color_layer.h"
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS;
class RSLayerParcelHelperTest : public Test {};

/**
 * Function: Update_Then_Destroy_NormalLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call UpdateRSLayerCmd on empty context to create layer
 *                  2. verify count becomes 1
 *                  3. call DestroyRSLayerCmd and verify count becomes 0
 */
HWTEST(RSLayerParcelHelperTest, Update_Then_Destroy_NormalLayer, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 100;

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(7);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(nullptr, id, cmd);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);

    OHOS::Rosen::RSLayerParcelHelper::DestroyRSLayerCmd(nullptr, id, cmd);
    OHOS::Rosen::RSLayerParcelHelper::DestroyRSLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 0u);
    OHOS::Rosen::RSLayerParcelHelper::DestroyRSLayerCmd(ctx, id, cmd);
}

/**
 * Function: Update_RCDLayer_Creates_ScreenRCD
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call UpdateRSRCDLayerCmd on empty context
 *                  2. verify RCD layer is created and IsScreenRCDLayer is true
 */
HWTEST(RSLayerParcelHelperTest, Update_RCDLayer_Creates_ScreenRCD, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 200;

    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(nullptr, id, cmd);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(ctx, id, cmd);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsScreenRCDLayer());
}

/**
 * Function: UpdateRSRCD_OnExistingNonRCD_ReplacesWithRCD
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create normal layer via UpdateRSLayerCmd
 *                  2. call UpdateRSRCDLayerCmd with same id
 *                  3. verify layer becomes screen RCD
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSRCD_OnExistingNonRCD_ReplacesWithRCD, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 300;

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(1);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);
    auto layerBefore = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layerBefore, nullptr);
    EXPECT_FALSE(layerBefore->IsScreenRCDLayer());

    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto pixelProp = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto pixelCmd = std::make_shared<RSRenderLayerPixelMapCmd>(pixelProp);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(ctx, id, pixelCmd);

    auto layerAfter = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layerAfter, nullptr);
    EXPECT_TRUE(layerAfter->IsScreenRCDLayer());
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_ContextNullptr_ReturnEarly
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call UpdateRSSolidFilledColorLayerCmd with nullptr context
 *                  2. verify no crash and early return
 *                   Cover branch: context == nullptr (line 4 true)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_ContextNullptr_ReturnEarly, TestSize.Level1)
{
    RSLayerId id = 400;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(5);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    EXPECT_NO_FATAL_FAILURE(
        OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(nullptr, id, cmd));
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_NewLayer_CreatesSolidFilledColorLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call UpdateRSSolidFilledColorLayerCmd on empty context
 *                  2. verify solid filled color layer is created
 *                   Cover branch: context != nullptr (line 4 false), rsLayer == nullptr (line 9 true)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_NewLayer_CreatesSolidFilledColorLayer, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 500;

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(10);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
    EXPECT_EQ(layer->GetZorder(), 10u);
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_ExistingNonSolidFilled_ReplacesWithSolidFilled
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create normal layer via UpdateRSLayerCmd
 *                  2. call UpdateRSSolidFilledColorLayerCmd with same id
 *                  3. verify layer becomes solid filled color layer
 *                   Cover branch: context != nullptr (line 4 false), rsLayer != nullptr (line 9 false),
 *                                  !IsSolidFilledColorLayer() (line 14 true)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_ExistingNonSolidFilled_ReplacesWithSolidFilled,
    TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 600;

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(15);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);

    auto layerBefore = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layerBefore, nullptr);
    EXPECT_FALSE(layerBefore->IsSolidFilledColorLayer());

    auto solidProp = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(20);
    auto solidCmd = std::make_shared<RSRenderLayerZorderCmd>(solidProp);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, solidCmd);

    auto layerAfter = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layerAfter, nullptr);
    EXPECT_TRUE(layerAfter->IsSolidFilledColorLayer());
    EXPECT_EQ(layerAfter->GetZorder(), 20u);
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_ExistingSolidFilled_UpdatesDirectly
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call UpdateRSSolidFilledColorLayerCmd again
 *                  3. verify layer is updated directly (not replaced)
 *                   Cover branch: context != nullptr (line 4 false), rsLayer != nullptr (line 9 false),
 *                                  IsSolidFilledColorLayer() (line 14 false)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_ExistingSolidFilled_UpdatesDirectly, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 700;

    auto prop1 = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(25);
    auto cmd1 = std::make_shared<RSRenderLayerZorderCmd>(prop1);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, cmd1);

    auto layer1 = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer1, nullptr);
    EXPECT_TRUE(layer1->IsSolidFilledColorLayer());
    EXPECT_EQ(layer1->GetZorder(), 25u);

    auto prop2 = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(30);
    auto cmd2 = std::make_shared<RSRenderLayerZorderCmd>(prop2);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, cmd2);

    auto layer2 = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer2, nullptr);
    EXPECT_TRUE(layer2->IsSolidFilledColorLayer());
    EXPECT_EQ(layer2->GetZorder(), 30u);
    EXPECT_EQ(layer1.get(), layer2.get());
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_MultipleUpdates_MaintainsSolidFilledType
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create solid filled color layer
 *                  2. call UpdateRSSolidFilledColorLayerCmd multiple times
 *                  3. verify layer remains solid filled color type
 *                   Cover branch: context != nullptr (line 4 false), rsLayer != nullptr (line 9 false),
 *                                  IsSolidFilledColorLayer() (line 14 false)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_MultipleUpdates_MaintainsSolidFilledType,
    TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 800;

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(35);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, cmd);

    for (int i = 0; i < 5; i++) {
        auto updateProp = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(35 + i);
        auto updateCmd = std::make_shared<RSRenderLayerZorderCmd>(updateProp);
        OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, updateCmd);
    }

    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
}

/**
 * Function: UpdateRSLayerCmd_CommandNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context but a nullptr command
 *                  2. call UpdateRSLayerCmd
 *                  3. verify early return: no layer is created
 *                     (true branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSLayerCmd_CommandNullptr_TrueBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1100;
    std::shared_ptr<RSRenderLayerCmd> nullCmd = nullptr;

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, nullCmd);
    // True branch: command == nullptr, early return keeps the layer count at 0
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 0u);
    EXPECT_EQ(ctx->GetRSRenderLayer(id), nullptr);
}

/**
 * Function: UpdateRSLayerCmd_CommandNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context and a valid (non-null) command
 *                  2. call UpdateRSLayerCmd
 *                  3. verify the function proceeds and creates a layer
 *                     (false branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSLayerCmd_CommandNotNull_FalseBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1101;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(11);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);
    // False branch: command != nullptr, the function proceeds and creates a layer
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->GetZorder(), 11u);
}

/**
 * Function: UpdateRSRCDLayerCmd_CommandNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context but a nullptr command
 *                  2. call UpdateRSRCDLayerCmd
 *                  3. verify early return: no layer is created
 *                     (true branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSRCDLayerCmd_CommandNullptr_TrueBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1200;
    std::shared_ptr<RSRenderLayerCmd> nullCmd = nullptr;

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(ctx, id, nullCmd);
    // True branch: command == nullptr, early return keeps the layer count at 0
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 0u);
    EXPECT_EQ(ctx->GetRSRenderLayer(id), nullptr);
}

/**
 * Function: UpdateRSRCDLayerCmd_CommandNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context and a valid (non-null) command
 *                  2. call UpdateRSRCDLayerCmd
 *                  3. verify the function proceeds and creates an RCD layer
 *                     (false branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSRCDLayerCmd_CommandNotNull_FalseBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1201;
    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSRCDLayerCmd(ctx, id, cmd);
    // False branch: command != nullptr, the function proceeds and creates an RCD layer
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsScreenRCDLayer());
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_CommandNullptr_TrueBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context but a nullptr command
 *                  2. call UpdateRSSolidFilledColorLayerCmd
 *                  3. verify early return: no layer is created
 *                     (true branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_CommandNullptr_TrueBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1300;
    std::shared_ptr<RSRenderLayerCmd> nullCmd = nullptr;

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, nullCmd);
    // True branch: command == nullptr, early return keeps the layer count at 0
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 0u);
    EXPECT_EQ(ctx->GetRSRenderLayer(id), nullptr);
}

/**
 * Function: UpdateRSSolidFilledColorLayerCmd_CommandNotNull_FalseBranch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. provide a valid context and a valid (non-null) command
 *                  2. call UpdateRSSolidFilledColorLayerCmd
 *                  3. verify the function proceeds and creates a solid filled color layer
 *                     (false branch of the new (command == nullptr) condition added in the fix commit)
 */
HWTEST(RSLayerParcelHelperTest, UpdateRSSolidFilledColorLayerCmd_CommandNotNull_FalseBranch, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = 1301;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(22);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSSolidFilledColorLayerCmd(ctx, id, cmd);
    // False branch: command != nullptr, the function proceeds and creates a solid filled color layer
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
    EXPECT_TRUE(layer->IsSolidFilledColorLayer());
    EXPECT_EQ(layer->GetZorder(), 22u);
}
