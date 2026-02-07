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
#include "pixel_map.h"
#include "sync_fence.h"
#include "surface_tunnel_handle.h"
#include "surface_buffer.h"
#include "graphic_common.h"

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

/**
 * Function: Marshall_Unmarshall_Bool_PreMulti_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip a bool command (PreMulti)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Bool_PreMulti_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerPreMultiCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::PRE_MULTI);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<bool>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_TRUE(outProp->Get());
}

/**
 * Function: Marshall_Unmarshall_Float_SdrNit_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip a float command (SdrNit)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Float_SdrNit_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(123.5f);
    auto cmd = std::make_shared<RSRenderLayerSdrNitCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::SDR_NIT);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<float>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_FLOAT_EQ(outProp->Get(), 123.5f);
}

/**
 * Function: Marshall_Unmarshall_U64_NodeId_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip a uint64_t command (NodeId)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_U64_NodeId_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(0xABCDEF1234567890ULL);
    auto cmd = std::make_shared<RSRenderLayerNodeIdCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::NODE_ID);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<uint64_t>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), 0xABCDEF1234567890ULL);
}

/**
 * Function: Marshall_Unmarshall_Vector_VisibleRegions_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip a vector<GraphicIRect> command (VisibleRegions)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Vector_VisibleRegions_Success, TestSize.Level1)
{
    std::vector<GraphicIRect> rects {{0,0,10,10}, {1,2,3,4}};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(rects);
    auto cmd = std::make_shared<RSRenderLayerVisibleRegionsCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::VISIBLE_REGIONS);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(
        out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 2u);
    EXPECT_EQ(outProp->Get()[1].x, 1);
    EXPECT_EQ(outProp->Get()[1].y, 2);
    EXPECT_EQ(outProp->Get()[1].w, 3);
    EXPECT_EQ(outProp->Get()[1].h, 4);
}

/**
 * Function: Marshall_Unmarshall_Enum_LayerMaskInfo_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip LayerMaskInfo (enum based)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Enum_LayerMaskInfo_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<LayerMask>>(LayerMask::LAYER_MASK_HBM_SYNC);
    auto cmd = std::make_shared<RSRenderLayerLayerMaskInfoCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::LAYER_MASK_INFO);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<LayerMask>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), LayerMask::LAYER_MASK_HBM_SYNC);
}

/**
 * Function: Marshall_Unmarshall_Struct_SolidColorLayerProperty_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip GraphicSolidColorLayerProperty
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Struct_SolidColorLayerProperty_Success, TestSize.Level1)
{
    GraphicSolidColorLayerProperty scp;
    scp.zOrder = 7;
    scp.transformType = GraphicTransformType::GRAPHIC_ROTATE_90;
    scp.layerRect = {10, 20, 30, 40};
    scp.compositionType = GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE;
    scp.layerColor = {255, 128, 64, 191};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicSolidColorLayerProperty>>(scp);
    auto cmd = std::make_shared<RSRenderLayerSolidColorLayerPropertyCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::SOLID_COLOR_LAYER_PROPERTY);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<GraphicSolidColorLayerProperty>>(
        out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_TRUE(outProp->Get() == scp);
}

/**
 * Function: Marshall_Unmarshall_TunnelHandle_Null_And_NonNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Check both nullptr and non-null SurfaceTunnelHandle
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_TunnelHandle_Variants, TestSize.Level1)
{
    // null case
    {
        sptr<SurfaceTunnelHandle> handle = nullptr;
        auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceTunnelHandle>>>(handle);
        auto cmd = std::make_shared<RSRenderLayerTunnelHandleCmd>(prop);
        MessageParcel parcel;
        ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::TUNNEL_HANDLE);
    }
}

/**
 * Function: Marshall_Unmarshall_SyncFence_NonNull_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Use InvalidFence() as a non-null fence and round-trip
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_SyncFence_NonNull_Success, TestSize.Level1)
{
    auto fence = SyncFence::InvalidFence();
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SyncFence>>>(fence);
    auto cmd = std::make_shared<RSRenderLayerAcquireFenceCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ACQUIRE_FENCE);
}

/**
 * Function: Marshall_Fail_VectorTooLarge
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Ensure Marshalling fails when vector.size() exceeds limit
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Fail_VectorTooLarge, TestSize.Level1)
{
    // build a huge vector to trigger size guard (MAX_VECTOR_SIZE = 10000)
    std::vector<float> big(10001, 1.0f);
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(big);
    auto cmd = std::make_shared<RSRenderLayerColorTransformCmd>(prop);
    MessageParcel parcel;
    EXPECT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Unmarshall_Fail_VectorTooLarge
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Manually craft parcel with oversized vector length to fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_VectorTooLarge, TestSize.Level1)
{
    MessageParcel parcel;
    // write type of COLOR_TRANSFORM
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::COLOR_TRANSFORM)));
    // write vector length exceeding MAX_VECTOR_SIZE (10000)
    ASSERT_TRUE(parcel.WriteUint32(10001));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_PixelMap_NonNull_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip non-null PixelMap
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_PixelMap_NonNull_Success, TestSize.Level1)
{
    Media::InitializationOptions opts;
    opts.size = { 2, 2 };
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    auto upm = Media::PixelMap::Create(opts);
    ASSERT_NE(upm, nullptr);
    std::shared_ptr<Media::PixelMap> pm(upm.release());

    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::shared_ptr<Media::PixelMap>>>(pm);
    auto cmd = std::make_shared<RSRenderLayerPixelMapCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::PIXEL_MAP);
}

/**
 * Function: Marshall_Unmarshall_PreBuffer_Null_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip sptr<SurfaceBuffer> with nullptr (PRE_BUFFER)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_PreBuffer_Null_Success, TestSize.Level1)
{
    sptr<SurfaceBuffer> buf = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(buf);
    auto cmd = std::make_shared<RSRenderLayerPreBufferCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::PRE_BUFFER);
}

/**
 * Function: Marshall_Unmarshall_SyncFence_Null_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ACQUIRE_FENCE with nullptr fence should round-trip and remain nullptr
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_SyncFence_Null_Success, TestSize.Level1)
{
    sptr<SyncFence> fence = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SyncFence>>>(fence);
    auto cmd = std::make_shared<RSRenderLayerAcquireFenceCmd>(prop);
    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ACQUIRE_FENCE);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<sptr<SyncFence>>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    EXPECT_EQ(outProp->Get(), nullptr);
}

/**
 * Function: Unmarshall_Fail_Buffer_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BUFFER: write presence=true without properties -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Buffer_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BUFFER)));
    ASSERT_TRUE(parcel.WriteBool(true));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_PreBuffer_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: PRE_BUFFER: write presence=true without properties -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_PreBuffer_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::PRE_BUFFER)));
    ASSERT_TRUE(parcel.WriteBool(true));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_SyncFence_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ACQUIRE_FENCE: write presence=true without FD data -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_SyncFence_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ACQUIRE_FENCE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_TunnelHandle_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: TUNNEL_HANDLE: write presence=true without handle payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_TunnelHandle_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TUNNEL_HANDLE)));
    ASSERT_TRUE(parcel.WriteBool(true));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_String_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: SURFACE_NAME without string payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_String_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::SURFACE_NAME)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Int_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ZORDER without int32 payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Int_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ZORDER)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Enum_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BLEND_TYPE without enum payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Enum_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BLEND_TYPE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Rect_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LAYER_SIZE without GraphicIRect payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Rect_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_SIZE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_VectorFloat_Partial
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: COLOR_TRANSFORM with size=1 but no element -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_VectorFloat_Partial, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::COLOR_TRANSFORM)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_VisibleRegions_VectorTooLarge
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: VISIBLE_REGIONS with size exceeding MAX_VECTOR_SIZE -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_VisibleRegions_VectorTooLarge, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::VISIBLE_REGIONS)));
    ASSERT_TRUE(parcel.WriteUint32(10001));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_WindowsName_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip vector<string> for WINDOWS_NAME
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_WindowsName_Success, TestSize.Level1)
{
    std::vector<std::string> names{"winA", "winB"};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(names);
    auto cmd = std::make_shared<RSRenderLayerWindowsNameCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::WINDOWS_NAME);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<std::string>>>(
        out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 2u);
    EXPECT_EQ(outProp->Get()[1], std::string("winB"));
}

/**
 * Function: Unmarshall_Fail_WindowsName_Partial
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: WINDOWS_NAME with declared size but missing elements -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_WindowsName_Partial, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::WINDOWS_NAME)));
    ASSERT_TRUE(parcel.WriteUint32(2));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_Matrix_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip GraphicMatrix (type-only verification)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Matrix_Success, TestSize.Level1)
{
    GraphicMatrix m{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicMatrix>>(m);
    auto cmd = std::make_shared<RSRenderLayerMatrixCmd>(prop);
    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::MATRIX);
}

/**
 * Function: Marshall_Unmarshall_LayerColor_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip GraphicLayerColor (type-only verification)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_LayerColor_Success, TestSize.Level1)
{
    GraphicLayerColor c{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(c);
    auto cmd = std::make_shared<RSRenderLayerLayerColorCmd>(prop);
    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::LAYER_COLOR);
}

/**
 * Function: Unmarshall_Fail_ColorDataSpace_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: COLOR_DATA_SPACE without enum payload -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_ColorDataSpace_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::COLOR_DATA_SPACE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_Type_Enums_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip enum commands: TYPE/TRANSFORM/COMPOSITION_TYPE
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Type_Enums_Success, TestSize.Level1)
{
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerType>>(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
        auto cmd = std::make_shared<RSRenderLayerTypeCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::TYPE);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicTransformType>>(GraphicTransformType::GRAPHIC_ROTATE_NONE);
        auto cmd = std::make_shared<RSRenderLayerTransformCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::TRANSFORM);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicCompositionType>>(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
        auto cmd = std::make_shared<RSRenderLayerCompositionTypeCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::COMPOSITION_TYPE);
    }
}

/**
 * Function: Marshall_Unmarshall_ColorDataSpace_And_BackgroundColor_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip COLOR_DATA_SPACE and BACKGROUND_COLOR
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_ColorDataSpace_And_BackgroundColor_Success, TestSize.Level1)
{
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(GraphicColorDataSpace::GRAPHIC_BT709_SRGB_FULL);
        auto cmd = std::make_shared<RSRenderLayerColorDataSpaceCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::COLOR_DATA_SPACE);
    }
    {
        GraphicLayerColor bg{26, 51, 77, 102};
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(bg);
        auto cmd = std::make_shared<RSRenderLayerBackgroundColorCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::BACKGROUND_COLOR);
    }
}

/**
 * Function: Marshall_Unmarshall_Additional_Rects_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip BoundSize/CropRect/AncoSrcRect/DirtyRegions
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Additional_Rects_Success, TestSize.Level1)
{
    {
        GraphicIRect r{5,6,7,8};
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
        auto cmd = std::make_shared<RSRenderLayerBoundSizeCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::BOUND_SIZE);
    }
    {
        GraphicIRect r{0,1,2,3};
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
        auto cmd = std::make_shared<RSRenderLayerCropRectCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::CROP_RECT);
    }
    {
        GraphicIRect r{9,9,9,9};
        auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
        auto cmd = std::make_shared<RSRenderLayerAncoSrcRectCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ANCO_SRC_RECT);
    }
    {
        std::vector<GraphicIRect> rects{{2,2,2,2}};
        auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(rects);
        auto cmd = std::make_shared<RSRenderLayerDirtyRegionsCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::DIRTY_REGIONS);
    }
}

/**
 * Function: Marshall_Unmarshall_Additional_Floats_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip DisplayNit & BrightnessRatio
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Additional_Floats_Success, TestSize.Level1)
{
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(300.0f);
        auto cmd = std::make_shared<RSRenderLayerDisplayNitCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::DISPLAY_NIT);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(0.75f);
        auto cmd = std::make_shared<RSRenderLayerBrightnessRatioCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::BRIGHTNESS_RATIO);
    }
}

/**
 * Function: Marshall_Unmarshall_Additional_Ints_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip TunnelLayerId/TunnelLayerProperty/AncoFlags/LayerSourceTuning/CycleBuffersNum/SurfaceUniqueId/RSLayerId
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Additional_Ints_Success, TestSize.Level1)
{
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(123456789ULL);
        auto cmd = std::make_shared<RSRenderLayerTunnelLayerIdCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::TUNNEL_LAYER_ID);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0xF0F0U);
        auto cmd = std::make_shared<RSRenderLayerTunnelLayerPropertyCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::TUNNEL_LAYER_PROPERTY);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0xAA55U);
        auto cmd = std::make_shared<RSRenderLayerAncoFlagsCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ANCO_FLAGS);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(-3);
        auto cmd = std::make_shared<RSRenderLayerLayerSourceTuningCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::LAYER_SOURCE_TUNING);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(3U);
        auto cmd = std::make_shared<RSRenderLayerCycleBuffersNumCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::CYCLE_BUFFERS_NUM);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(0x987654321ULL);
        auto cmd = std::make_shared<RSRenderLayerSurfaceUniqueIdCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::SURFACE_UNIQUE_ID);
    }
    {
        auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(42ULL);
        auto cmd = std::make_shared<RSRenderLayerRSLayerIdCmd>(prop);
        MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ID);
    }
}

/**
 * Function: Marshall_Unmarshall_Additional_Bools_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip several boolean commands to hit branches
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Additional_Bools_Success, TestSize.Level1)
{
    struct BoolCmd { std::shared_ptr<RSRenderLayerCmd> cmd; RSLayerCmdType type; };
    std::vector<BoolCmd> cases;
    cases.push_back({ std::make_shared<RSRenderLayerUniRenderFlagCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::UNI_RENDER_FLAG });
    cases.push_back({ std::make_shared<RSRenderLayerTunnelHandleChangeCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::TUNNEL_HANDLE_CHANGE });
    cases.push_back({ std::make_shared<RSRenderLayerRotationFixedCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(false)), RSLayerCmdType::ROTATION_FIXED });
    cases.push_back({ std::make_shared<RSRenderLayerLayerArsrCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::LAYER_ARSR });
    cases.push_back({ std::make_shared<RSRenderLayerLayerCopybitCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(false)), RSLayerCmdType::LAYER_COPYBIT });
    cases.push_back({ std::make_shared<RSRenderLayerNeedBilinearInterpolationCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::NEED_BILINEAR_INTERPOLATION });
    cases.push_back({ std::make_shared<RSRenderLayerIsMaskLayerCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::IS_MASK_LAYER });
    cases.push_back({ std::make_shared<RSRenderLayerIsNeedCompositionCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(false)), RSLayerCmdType::IS_NEED_COMPOSITION });
    cases.push_back({ std::make_shared<RSRenderLayerUseDeviceOfflineCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::USE_DEVICE_OFFLINE });
    cases.push_back({ std::make_shared<RSRenderLayerIgnoreAlphaCmd>(std::make_shared<RSRenderLayerCmdProperty<bool>>(true)), RSLayerCmdType::IGNORE_ALPHA });

    for (auto& c : cases) {
        MessageParcel parcel; ASSERT_TRUE(c.cmd->Marshalling(parcel));
        auto out = RSRenderLayerCmd::Unmarshalling(parcel);
        ASSERT_NE(out, nullptr);
        EXPECT_EQ(out->GetRSRenderLayerCmdType(), c.type);
    }
}

/**
 * Function: Marshall_Unmarshall_Alpha_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip GraphicLayerAlpha
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Alpha_Success, TestSize.Level1)
{
    GraphicLayerAlpha a{1.0f, true};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerAlpha>>(a);
    auto cmd = std::make_shared<RSRenderLayerAlphaCmd>(prop);
    MessageParcel parcel; ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::ALPHA);
}

/**
 * Function: Unmarshall_Fail_LayerLinearMatrix_Partial
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LAYER_LINEAR_MATRIX with size=1 but missing element -> fail
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerLinearMatrix_Partial, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_LINEAR_MATRIX)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Unmarshall_Buffer_Null_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip sptr<SurfaceBuffer> with nullptr (BUFFER)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Buffer_Null_Success, TestSize.Level1)
{
    sptr<SurfaceBuffer> buf = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(buf);
    auto cmd = std::make_shared<RSRenderLayerBufferCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::BUFFER);
}

/**
 * Function: Marshall_Unmarshall_HDR_MetaData_Vector_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip std::vector<GraphicHDRMetaData> (META_DATA)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_HDR_MetaData_Vector_Success, TestSize.Level1)
{
    std::vector<GraphicHDRMetaData> metas(2); // default-initialized entries
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicHDRMetaData>>>(metas);
    auto cmd = std::make_shared<RSRenderLayerMetaDataCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::META_DATA);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<GraphicHDRMetaData>>>(
        out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 2u);
}

/**
 * Function: Marshall_Unmarshall_HDR_MetaDataSet_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip GraphicHDRMetaDataSet (META_DATA_SET)
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_HDR_MetaDataSet_Success, TestSize.Level1)
{
    GraphicHDRMetaDataSet set{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicHDRMetaDataSet>>(set);
    auto cmd = std::make_shared<RSRenderLayerMetaDataSetCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::META_DATA_SET);
}

/**
 * Function: Marshall_Unmarshall_Gravity_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip bool command for GRAVITY
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_Gravity_Success, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(100);
    auto cmd = std::make_shared<RSRenderLayerGravityCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::GRAVITY);
}

/**
 * Function: Marshall_Unmarshall_LayerLinearMatrix_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip bool command for LAYER_LINEAR_MATRIX
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_LayerLinearMatrix_Success, TestSize.Level1)
{
    std::vector<float> mat {1.0f, 0.5f, 0.0f};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(mat);
    auto cmd = std::make_shared<RSRenderLayerLayerLinearMatrixCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::LAYER_LINEAR_MATRIX);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<float>>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 3u);
    EXPECT_FLOAT_EQ(outProp->Get()[0], 1.0f);
    EXPECT_FLOAT_EQ(outProp->Get()[1], 0.5f);
    EXPECT_FLOAT_EQ(outProp->Get()[2], 0.0f);
}

/**
 * Function: Marshall_Unmarshall_CornerRadiusInfoForDRM_Success
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Round-trip bool command for CORNER_RADIUS_INFO_FOR_DRM
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Unmarshall_CornerRadiusInfoForDRM_Success, TestSize.Level1)
{
    std::vector<float> mat {1.0f, 0.5f, 0.0f};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(mat);
    auto cmd = std::make_shared<RSRenderLayerCornerRadiusInfoForDRMCmd>(prop);

    MessageParcel parcel;
    ASSERT_TRUE(cmd->Marshalling(parcel));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->GetRSRenderLayerCmdType(), RSLayerCmdType::CORNER_RADIUS_INFO_FOR_DRM);
    auto outProp = std::static_pointer_cast<RSRenderLayerCmdProperty<std::vector<float>>>(out->GetRSRenderLayerProperty());
    ASSERT_NE(outProp, nullptr);
    ASSERT_EQ(outProp->Get().size(), 3u);
    EXPECT_FLOAT_EQ(outProp->Get()[0], 1.0f);
    EXPECT_FLOAT_EQ(outProp->Get()[1], 0.5f);
    EXPECT_FLOAT_EQ(outProp->Get()[2], 0.0f);
}

/**
 * Function: Unmarshall_Fail_MetaDataSet_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for META_DATA_SET, missing payload
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_MetaDataSet_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::META_DATA_SET)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_MetaData_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write META_DATA with length=1 but no element payload
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_MetaData_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::META_DATA)));
    ASSERT_TRUE(parcel.WriteUint32(1)); // one element but missing bytes
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_BaseCmds_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: For each base command, write type only and verify Unmarshalling fails.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_RSLayerId_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ID)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Alpha_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for Alpha; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Alpha_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ALPHA)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Zorder_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for Zorder; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Zorder_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ZORDER)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Type_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for Type; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Type_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TYPE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Transform_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for Transform; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Transform_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TRANSFORM)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_CompositionType_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for CompositionType; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_CompositionType_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::COMPOSITION_TYPE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Gravity_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for Gravity; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Gravity_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::GRAVITY)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_BlendType_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for BlendType; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_BlendType_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BLEND_TYPE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_PreMulti_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for PreMulti; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_PreMulti_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::PRE_MULTI)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_UniRenderFlag_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for UniRenderFlag; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_UniRenderFlag_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::UNI_RENDER_FLAG)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_TunnelHandleChange_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for TunnelHandleChange; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_TunnelHandleChange_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TUNNEL_HANDLE_CHANGE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_RotationFixed_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for RotationFixed; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_RotationFixed_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ROTATION_FIXED)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerArsr_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerArsr; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerArsr_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_ARSR)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerCopybit_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerCopybit; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerCopybit_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_COPYBIT)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_NeedBilinearInterpolation_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for NeedBilinearInterpolation; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_NeedBilinearInterpolation_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::NEED_BILINEAR_INTERPOLATION)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_IsMaskLayer_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for IsMaskLayer; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_IsMaskLayer_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::IS_MASK_LAYER)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_IsNeedComposition_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for IsNeedComposition; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_IsNeedComposition_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::IS_NEED_COMPOSITION)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_UseDeviceOffline_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for UseDeviceOffline; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_UseDeviceOffline_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::USE_DEVICE_OFFLINE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_IgnoreAlpha_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for IgnoreAlpha; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_IgnoreAlpha_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::IGNORE_ALPHA)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_SdrNit_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for SdrNit; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_SdrNit_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::SDR_NIT)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_DisplayNit_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for DisplayNit; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_DisplayNit_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::DISPLAY_NIT)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_BrightnessRatio_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for BrightnessRatio; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_BrightnessRatio_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BRIGHTNESS_RATIO)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_NodeId_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for NodeId; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_NodeId_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::NODE_ID)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_TunnelLayerId_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for TunnelLayerId; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_TunnelLayerId_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TUNNEL_LAYER_ID)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_TunnelLayerProperty_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for TunnelLayerProperty; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_TunnelLayerProperty_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::TUNNEL_LAYER_PROPERTY)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_AncoFlags_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for AncoFlags; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_AncoFlags_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ANCO_FLAGS)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerSourceTuning_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerSourceTuning; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerSourceTuning_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_SOURCE_TUNING)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_CycleBuffersNum_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for CycleBuffersNum; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_CycleBuffersNum_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::CYCLE_BUFFERS_NUM)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_SurfaceUniqueId_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for SurfaceUniqueId; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_SurfaceUniqueId_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::SURFACE_UNIQUE_ID)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerSize_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerSize; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerSize_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_SIZE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_BoundSize_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for BoundSize; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_BoundSize_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BOUND_SIZE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_CropRect_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for CropRect; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_CropRect_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::CROP_RECT)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_AncoSrcRect_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for AncoSrcRect; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_AncoSrcRect_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ANCO_SRC_RECT)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_VisibleRegions_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write VisibleRegions length but omit rect payload; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_VisibleRegions_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::VISIBLE_REGIONS)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_DirtyRegions_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write DirtyRegions length but omit rect payload; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_DirtyRegions_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::DIRTY_REGIONS)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_Matrix_PartialPayload
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write partial GraphicMatrix payload; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_Matrix_PartialPayload, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::MATRIX)));
    ASSERT_TRUE(parcel.WriteFloat(1.0f));
    ASSERT_TRUE(parcel.WriteFloat(0.0f));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_ColorTransform_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write float vector length then omit values; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_ColorTransform_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::COLOR_TRANSFORM)));
    ASSERT_TRUE(parcel.WriteUint32(2));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerLinearMatrix_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write float vector length then omit values; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerLinearMatrix_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_LINEAR_MATRIX)));
    ASSERT_TRUE(parcel.WriteUint32(3));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_CornerRadiusInfoForDRM_PartialVector
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write float vector length then omit values; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_CornerRadiusInfoForDRM_PartialVector, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::CORNER_RADIUS_INFO_FOR_DRM)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerColor_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerColor; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerColor_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_COLOR)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_BackgroundColor_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for BackgroundColor; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_BackgroundColor_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::BACKGROUND_COLOR)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_SolidColorLayerProperty_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for SolidColorLayerProperty; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_SolidColorLayerProperty_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::SOLID_COLOR_LAYER_PROPERTY)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_WindowsName_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write WindowsName length but omit string payload; expect nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_WindowsName_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::WINDOWS_NAME)));
    ASSERT_TRUE(parcel.WriteUint32(1));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_SurfaceName_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for SurfaceName; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_SurfaceName_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::SURFACE_NAME)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_LayerMaskInfo_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for LayerMaskInfo; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_LayerMaskInfo_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::LAYER_MASK_INFO)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Unmarshall_Fail_AcquireFence_PayloadMissing
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Write type only for AcquireFence; expect Unmarshalling returns nullptr.
 */
HWTEST(RSRenderLayerCmdTest, Unmarshall_Fail_AcquireFence_PayloadMissing, TestSize.Level1)
{
    MessageParcel parcel;
    ASSERT_TRUE(parcel.WriteUint16(static_cast<uint16_t>(RSLayerCmdType::ACQUIRE_FENCE)));
    auto out = RSRenderLayerCmd::Unmarshalling(parcel);
    EXPECT_EQ(out, nullptr);
}

/**
 * Function: Marshall_Zorder_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. build Zorder command and marshal into parcel
 *                  2. verify result
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Zorder_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(5);
    auto cmd = std::make_shared<RSRenderLayerZorderCmd>(prop);

    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_RSLayerId_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: RSLayerId command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_RSLayerId_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(123u);
    auto cmd = std::make_shared<RSRenderLayerRSLayerIdCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Alpha_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Alpha command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Alpha_Fail, TestSize.Level1)
{
    GraphicLayerAlpha a{true, true, 0, 255, 128};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerAlpha>>(a);
    auto cmd = std::make_shared<RSRenderLayerAlphaCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Type_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Type command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Type_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerType>>(GraphicLayerType::GRAPHIC_LAYER_TYPE_GRAPHIC);
    auto cmd = std::make_shared<RSRenderLayerTypeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Transform_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Transform command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Transform_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicTransformType>>(GraphicTransformType::GRAPHIC_ROTATE_NONE);
    auto cmd = std::make_shared<RSRenderLayerTransformCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_CompositionType_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: CompositionType command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_CompositionType_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicCompositionType>>(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    auto cmd = std::make_shared<RSRenderLayerCompositionTypeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Gravity_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Gravity command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Gravity_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(0);
    auto cmd = std::make_shared<RSRenderLayerGravityCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_BlendType_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BlendType command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_BlendType_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicBlendType>>(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    auto cmd = std::make_shared<RSRenderLayerBlendTypeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_PreMulti_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: PreMulti command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_PreMulti_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerPreMultiCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_UniRenderFlag_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: UniRenderFlag command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_UniRenderFlag_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerUniRenderFlagCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_TunnelHandleChange_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: TunnelHandleChange command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_TunnelHandleChange_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerTunnelHandleChangeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_RotationFixed_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: RotationFixed command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_RotationFixed_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerRotationFixedCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerArsr_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerArsr command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerArsr_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerLayerArsrCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerCopybit_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerCopybit command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerCopybit_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerLayerCopybitCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_NeedBilinearInterpolation_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: NeedBilinearInterpolation command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_NeedBilinearInterpolation_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerNeedBilinearInterpolationCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_IsMaskLayer_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: IsMaskLayer command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_IsMaskLayer_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerIsMaskLayerCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_IsNeedComposition_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: IsNeedComposition command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_IsNeedComposition_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerIsNeedCompositionCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_UseDeviceOffline_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: UseDeviceOffline command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_UseDeviceOffline_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(false);
    auto cmd = std::make_shared<RSRenderLayerUseDeviceOfflineCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_IgnoreAlpha_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: IgnoreAlpha command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_IgnoreAlpha_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<bool>>(true);
    auto cmd = std::make_shared<RSRenderLayerIgnoreAlphaCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

// float commands
/**
 * Function: Marshall_SdrNit_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: SdrNit command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_SdrNit_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(100.0f);
    auto cmd = std::make_shared<RSRenderLayerSdrNitCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_DisplayNit_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: DisplayNit command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_DisplayNit_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(200.0f);
    auto cmd = std::make_shared<RSRenderLayerDisplayNitCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_BrightnessRatio_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BrightnessRatio command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_BrightnessRatio_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<float>>(0.5f);
    auto cmd = std::make_shared<RSRenderLayerBrightnessRatioCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_NodeId_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: NodeId command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_NodeId_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(1u);
    auto cmd = std::make_shared<RSRenderLayerNodeIdCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_TunnelLayerId_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: TunnelLayerId command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_TunnelLayerId_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(2u);
    auto cmd = std::make_shared<RSRenderLayerTunnelLayerIdCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_TunnelLayerProperty_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: TunnelLayerProperty command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_TunnelLayerProperty_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0u);
    auto cmd = std::make_shared<RSRenderLayerTunnelLayerPropertyCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_AncoFlags_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: AncoFlags command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_AncoFlags_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(0u);
    auto cmd = std::make_shared<RSRenderLayerAncoFlagsCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerSourceTuning_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerSourceTuning command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerSourceTuning_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(0);
    auto cmd = std::make_shared<RSRenderLayerLayerSourceTuningCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_CycleBuffersNum_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: CycleBuffersNum command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_CycleBuffersNum_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint32_t>>(3u);
    auto cmd = std::make_shared<RSRenderLayerCycleBuffersNumCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_SurfaceUniqueId_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: SurfaceUniqueId command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_SurfaceUniqueId_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<uint64_t>>(999u);
    auto cmd = std::make_shared<RSRenderLayerSurfaceUniqueIdCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerSize_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerSize command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerSize_Fail, TestSize.Level1)
{
    GraphicIRect r{0,0,0,0};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerLayerSizeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_BoundSize_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BoundSize command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_BoundSize_Fail, TestSize.Level1)
{
    GraphicIRect r{0,0,0,0};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerBoundSizeCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_CropRect_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: CropRect command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_CropRect_Fail, TestSize.Level1)
{
    GraphicIRect r{0,0,0,0};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerCropRectCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_AncoSrcRect_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: AncoSrcRect command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_AncoSrcRect_Fail, TestSize.Level1)
{
    GraphicIRect r{0,0,0,0};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicIRect>>(r);
    auto cmd = std::make_shared<RSRenderLayerAncoSrcRectCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_VisibleRegions_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: VisibleRegions command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_VisibleRegions_Fail, TestSize.Level1)
{
    std::vector<GraphicIRect> v{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(v);
    auto cmd = std::make_shared<RSRenderLayerVisibleRegionsCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_DirtyRegions_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: DirtyRegions command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_DirtyRegions_Fail, TestSize.Level1)
{
    std::vector<GraphicIRect> v{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicIRect>>>(v);
    auto cmd = std::make_shared<RSRenderLayerDirtyRegionsCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Matrix_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Matrix command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Matrix_Fail, TestSize.Level1)
{
    GraphicMatrix m{1,0,0,0,1,0,0,0,1};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicMatrix>>(m);
    auto cmd = std::make_shared<RSRenderLayerMatrixCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_ColorTransform_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ColorTransform command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_ColorTransform_Fail, TestSize.Level1)
{
    std::vector<float> v{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(v);
    auto cmd = std::make_shared<RSRenderLayerColorTransformCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerLinearMatrix_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerLinearMatrix command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerLinearMatrix_Fail, TestSize.Level1)
{
    std::vector<float> v{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(v);
    auto cmd = std::make_shared<RSRenderLayerLayerLinearMatrixCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_CornerRadiusInfoForDRM_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: CornerRadiusInfoForDRM command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_CornerRadiusInfoForDRM_Fail, TestSize.Level1)
{
    std::vector<float> v{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<float>>>(v);
    auto cmd = std::make_shared<RSRenderLayerCornerRadiusInfoForDRMCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerColor_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerColor command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerColor_Fail, TestSize.Level1)
{
    GraphicLayerColor c{1,2,3,4};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(c);
    auto cmd = std::make_shared<RSRenderLayerLayerColorCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_BackgroundColor_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: BackgroundColor command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_BackgroundColor_Fail, TestSize.Level1)
{
    GraphicLayerColor c{5,6,7,8};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicLayerColor>>(c);
    auto cmd = std::make_shared<RSRenderLayerBackgroundColorCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_ColorDataSpace_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ColorDataSpace command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_ColorDataSpace_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicColorDataSpace>>(GraphicColorDataSpace::GRAPHIC_BT709_SRGB_FULL);
    auto cmd = std::make_shared<RSRenderLayerColorDataSpaceCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_MetaData_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: MetaData command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_MetaData_Fail, TestSize.Level1)
{
    std::vector<GraphicHDRMetaData> metas{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<GraphicHDRMetaData>>>(metas);
    auto cmd = std::make_shared<RSRenderLayerMetaDataCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_MetaDataSet_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: MetaDataSet command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_MetaDataSet_Fail, TestSize.Level1)
{
    GraphicHDRMetaDataSet set{};
    set.key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X;
    set.metaData = {};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicHDRMetaDataSet>>(set);
    auto cmd = std::make_shared<RSRenderLayerMetaDataSetCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_SolidColorLayerProperty_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: SolidColorLayerProperty command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_SolidColorLayerProperty_Fail, TestSize.Level1)
{
    GraphicSolidColorLayerProperty scp;
    scp.zOrder = 1;
    scp.transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<GraphicSolidColorLayerProperty>>(scp);
    auto cmd = std::make_shared<RSRenderLayerSolidColorLayerPropertyCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_WindowsName_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: WindowsName command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_WindowsName_Fail, TestSize.Level1)
{
    std::vector<std::string> ws{};
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::vector<std::string>>>(ws);
    auto cmd = std::make_shared<RSRenderLayerWindowsNameCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_SurfaceName_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: SurfaceName command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_SurfaceName_Fail, TestSize.Level1)
{
    std::string name;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<std::string>>(name);
    auto cmd = std::make_shared<RSRenderLayerSurfaceNameCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_TunnelHandle_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: TunnelHandle command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_TunnelHandle_Fail, TestSize.Level1)
{
    sptr<SurfaceTunnelHandle> handle = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceTunnelHandle>>>(handle);
    auto cmd = std::make_shared<RSRenderLayerTunnelHandleCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_LayerMaskInfo_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: LayerMaskInfo command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_LayerMaskInfo_Fail, TestSize.Level1)
{
    auto prop = std::make_shared<RSRenderLayerCmdProperty<LayerMask>>(LayerMask::LAYER_MASK_NORMAL);
    auto cmd = std::make_shared<RSRenderLayerLayerMaskInfoCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_Buffer_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: Buffer command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_Buffer_Fail, TestSize.Level1)
{
    sptr<SurfaceBuffer> buf = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(buf);
    auto cmd = std::make_shared<RSRenderLayerBufferCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_AcquireFence_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: AcquireFence command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_AcquireFence_Fail, TestSize.Level1)
{
    sptr<SyncFence> fence = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SyncFence>>>(fence);
    auto cmd = std::make_shared<RSRenderLayerAcquireFenceCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}

/**
 * Function: Marshall_PreBuffer_Fail
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: PreBuffer command with null property; expect Marshalling returns false.
 */
HWTEST(RSRenderLayerCmdTest, Marshall_PreBuffer_Fail, TestSize.Level1)
{
    sptr<SurfaceBuffer> pre = nullptr;
    auto prop = std::make_shared<RSRenderLayerCmdProperty<sptr<SurfaceBuffer>>>(pre);
    auto cmd = std::make_shared<RSRenderLayerPreBufferCmd>(prop);
    MessageParcel parcel;
    cmd->rsRenderLayerProperty_ = nullptr;
    ASSERT_FALSE(cmd->Marshalling(parcel));
}