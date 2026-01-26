#include <gtest/gtest.h>
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"
#include "rs_render_composer_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerParcelTemplateTest : public Test {};

/**
 * Function: Template_Marshalling_Unmarshalling_Basic
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSUpdateRSLayerCmd with null command
 *                  2. verify marshalling fails (helper rejects null)
 */
HWTEST_F(RSLayerParcelTemplateTest, Template_Marshalling_Unmarshalling_Basic, TestSize.Level1)
{
    std::shared_ptr<RSRenderLayerCmd> dummyCmd;
    RSUpdateRSLayerCmd cmd(static_cast<RSLayerId>(1u), dummyCmd);
    MessageParcel parcel;
    ASSERT_FALSE(cmd.Marshalling(parcel));
}

/**
 * Function: Template_Unmarshalling_Fail_EmptyParcel
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call Unmarshalling on empty parcel
 *                  2. expect nullptr (read fails)
 */
HWTEST_F(RSLayerParcelTemplateTest, Template_Unmarshalling_Fail_EmptyParcel, TestSize.Level1)
{
    MessageParcel empty;
    std::unique_ptr<RSLayerParcel> out(RSUpdateRSLayerCmd::Unmarshalling(empty));
    ASSERT_EQ(out, nullptr);
}

/**
 * Function: Template_ApplyRSLayerCmd_AddsLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. apply RSUpdateRSLayerCmd to empty context
 *                  2. verify a layer is added for given id
 */
HWTEST_F(RSLayerParcelTemplateTest, Template_ApplyRSLayerCmd_AddsLayer, TestSize.Level1)
{
    auto ctx = std::make_shared<RSRenderComposerContext>();
    RSLayerId id = static_cast<RSLayerId>(321u);
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(9);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    RSUpdateRSLayerCmd cmd(id, zCmd);

    cmd.ApplyRSLayerCmd(ctx);
    auto layer = ctx->GetRSRenderLayer(id);
    ASSERT_NE(layer, nullptr);
}
} // namespace OHOS::Rosen
