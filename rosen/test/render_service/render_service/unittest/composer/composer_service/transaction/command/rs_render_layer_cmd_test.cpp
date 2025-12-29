/**
 * Function: FileHeader_RSRenderLayerCmd_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Cover RSRenderLayerCmd Marshalling/Unmarshalling for base command and PixelMap.
 */

#include <gtest/gtest.h>
#include "message_parcel.h"
#include "rs_render_layer_cmd.h"

using namespace OHOS::Rosen;
using namespace OHOS;
using namespace testing;
using namespace testing::ext;
class RSRenderLayerCmdTest : public Test {};
/**
 * Function: Marshall_Unmarshall_Zorder_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build Zorder command and marshal into parcel
 *                  2. unmarshal back to command
 *                  3. verify type/value matched
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Zorder_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(5);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));

    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ZORDER);

    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<int32_t>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), 5);
}

/**
 * Function: Marshall_Unmarshall_PixelMap_Null_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build PixelMap command with nullptr value
 *                  2. marshal and unmarshal
 *                  3. verify type equals PIXEL_MAP
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_PixelMap_Null_Success, TestSize.Level1)
{
    std::shared_ptr<Media::PixelMap> pm = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));

    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::PIXEL_MAP);
}
