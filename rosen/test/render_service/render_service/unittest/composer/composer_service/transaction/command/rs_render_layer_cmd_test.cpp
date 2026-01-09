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

/**
 * Function: Unmarshall_Fail_EmptyParcel
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. attempt to unmarshal from empty parcel
 *                  2. expect returned command is nullptr
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_EmptyParcel, TestSize.Level1)
{
    MessageParcel empty;
    auto out = RSRenderLayerCmd::Unmarshalling(empty);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_UnknownType
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. write unknown command type id into parcel
 *                  2. expect returned command is nullptr
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_UnknownType, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteUint16(static_cast<uint16_t>(9999));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_Enum_BlendType_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build BlendType command and marshal
 *                  2. unmarshal back and verify type/value
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Enum_BlendType_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicBlendType>>(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    auto cmd = std::make_shared<RSRenderLayerBlendTypeCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));

    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::BLEND_TYPE);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<GraphicBlendType>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
}

/**
 * Function: Marshall_Unmarshall_Struct_Rect_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build LayerSize command with GraphicIRect
 *                  2. unmarshal and verify values
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Struct_Rect_Success, TestSize.Level1)
{
    GraphicIRect rect {1, 2, 3, 4};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(rect);
    auto cmd = std::make_shared<RSRenderLayerLayerSizeCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::LAYER_SIZE);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<GraphicIRect>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get().x, 1);
    EXPECT_EQ(outProp->Get().y, 2);
    EXPECT_EQ(outProp->Get().w, 3);
    EXPECT_EQ(outProp->Get().h, 4);
}

/**
 * Function: Marshall_Unmarshall_Vector_Float_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build ColorTransform command with float vector
 *                  2. unmarshal and verify contents
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Vector_Float_Success, TestSize.Level1)
{
    std::vector<float> mat {1.0f, 0.5f, 0.0f};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(mat);
    auto cmd = std::make_shared<RSRenderLayerColorTransformCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::COLOR_TRANSFORM);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<float>>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 3u);
    EXPECT_FLOAT_EQ(outProp->Get()[0], 1.0f);
    EXPECT_FLOAT_EQ(outProp->Get()[1], 0.5f);
    EXPECT_FLOAT_EQ(outProp->Get()[2], 0.0f);
}

/**
 * Function: Marshall_Unmarshall_String_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build SurfaceName command with string
 *                  2. unmarshal and verify value
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_String_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::string>>(std::string("surface-abc"));
    auto cmd = std::make_shared<RSRenderLayerSurfaceNameCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::SURFACE_NAME);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::string>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), std::string("surface-abc"));
}
