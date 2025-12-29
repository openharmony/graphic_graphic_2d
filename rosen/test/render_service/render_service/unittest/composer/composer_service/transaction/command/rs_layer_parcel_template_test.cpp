#include <gtest/gtest.h>
#include "rs_surface_layer_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerParcelTemplateTest : public Test {};

/**
 * Function: Template_Marshalling_Unmarshalling_Basic
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSUpdateRSLayerCmd with minimal params
 *                  2. marshalling and unmarshalling back
 */
HWTEST_F(RSLayerParcelTemplateTest, Template_Marshalling_Unmarshalling_Basic, TestSize.Level1)
{
    std::shared_ptr<RSRenderLayerCmd> dummyCmd;
    RSUpdateRSLayerCmd cmd(static_cast<RSLayerId>(1u), dummyCmd);
    MessageParcel parcel;
    ASSERT_TRUE(cmd.Marshalling(parcel));
    RSLayerParcel* out = RSUpdateRSLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    delete out;
}
} // namespace OHOS::Rosen
