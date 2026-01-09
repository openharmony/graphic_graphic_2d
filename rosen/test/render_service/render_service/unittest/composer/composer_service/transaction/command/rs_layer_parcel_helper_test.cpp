/**
 * Function: FileHeader_RSLayerParcelHelper_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Cover RSLayerParcelHelper Update/Destroy/UpdateRSRCDLayerCmd paths.
 */

#include <gtest/gtest.h>
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"
#include "rs_render_composer_context.h"
#include "rs_render_surface_rcd_layer.h"
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

    OHOS::Rosen::RSLayerParcelHelper::UpdateRSLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 1u);

    OHOS::Rosen::RSLayerParcelHelper::DestroyRSLayerCmd(ctx, id, cmd);
    EXPECT_EQ(ctx->GetRSRenderLayerCount(), 0u);
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
