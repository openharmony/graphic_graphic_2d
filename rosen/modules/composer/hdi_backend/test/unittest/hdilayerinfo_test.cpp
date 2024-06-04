/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "iconsumer_surface.h"
#include "hdi_layer_info.h"
#include "surface_tunnel_handle.h"
#include "sync_fence.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiLayerInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiLayerInfo> hdiLayerInfo_;
};

void HdiLayerInfoTest::SetUpTestCase()
{
    hdiLayerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
}

void HdiLayerInfoTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: GetZorder001
 * @tc.desc: Verify the GetZorder of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetZorder001, Function | MediumTest| Level3)
{
    HdiLayerInfoTest::hdiLayerInfo_->SetZorder(1);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetZorder(), 1u);
}

/**
 * @tc.name: GetSurface001
 * @tc.desc: Verify the GetSurface of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetSurface001, Function | MediumTest| Level3)
{
    sptr<IConsumerSurface> surface = nullptr;
    HdiLayerInfoTest::hdiLayerInfo_->SetSurface(surface);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetSurface(), nullptr);
}

/**
 * @tc.name: GetBuffer001
 * @tc.desc: Verify the GetBuffer of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetBuffer001, Function | MediumTest| Level3)
{
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> sbuffer = nullptr;
    HdiLayerInfoTest::hdiLayerInfo_->SetBuffer(sbuffer, acquireFence);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBuffer(), nullptr);
}

/**
 * @tc.name: GetAcquireFence001
 * @tc.desc: Verify the GetAcquireFence of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetAcquireFence001, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->GetAcquireFence(), nullptr);
}

/**
 * @tc.name: GetAlpha001
 * @tc.desc: Verify the GetAlpha of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetAlpha001, Function | MediumTest| Level3)
{
    GraphicLayerAlpha layerAlpha = {
        .enGlobalAlpha = true,
        .enPixelAlpha = true,
        .alpha0 = 0,
        .alpha1 = 0,
        .gAlpha = 0,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetAlpha(layerAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().enGlobalAlpha, layerAlpha.enGlobalAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().enPixelAlpha, layerAlpha.enPixelAlpha);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().alpha0, layerAlpha.alpha0);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().alpha1, layerAlpha.alpha1);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetAlpha().gAlpha, layerAlpha.gAlpha);
}

/**
 * @tc.name: GetTransformType001
 * @tc.desc: Verify the GetTransformType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetTransformType001, Function | MediumTest| Level3)
{
    GraphicTransformType type = GraphicTransformType::GRAPHIC_ROTATE_90;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), GraphicTransformType::GRAPHIC_ROTATE_90);

    type = GraphicTransformType::GRAPHIC_ROTATE_180;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), GraphicTransformType::GRAPHIC_ROTATE_180);

    type = GraphicTransformType::GRAPHIC_ROTATE_270;
    HdiLayerInfoTest::hdiLayerInfo_->SetTransform(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTransformType(), GraphicTransformType::GRAPHIC_ROTATE_270);
}

/**
 * @tc.name: GetCompositionType001
 * @tc.desc: Verify the GetCompositionType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetCompositionType001, Function | MediumTest| Level3)
{
    GraphicCompositionType type = GRAPHIC_COMPOSITION_CLIENT;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), GRAPHIC_COMPOSITION_CLIENT);

    type = GRAPHIC_COMPOSITION_DEVICE;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), GRAPHIC_COMPOSITION_DEVICE);

    type = GRAPHIC_COMPOSITION_CURSOR;
    HdiLayerInfoTest::hdiLayerInfo_->SetCompositionType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCompositionType(), GRAPHIC_COMPOSITION_CURSOR);
}

/**
 * @tc.name: GetVisibleRegion001
 * @tc.desc: Verify the GetVisibleRegions of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetVisibleRegions001, Function | MediumTest| Level3)
{
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    std::vector<GraphicIRect> inVisibles;
    inVisibles.emplace_back(iRect);
    HdiLayerInfoTest::hdiLayerInfo_->SetVisibleRegions(inVisibles);
    const std::vector<GraphicIRect>& outVisibles = HdiLayerInfoTest::hdiLayerInfo_->GetVisibleRegions();
    ASSERT_EQ(outVisibles.size(), 1);
    ASSERT_EQ(outVisibles[0].x, iRect.x);
    ASSERT_EQ(outVisibles[0].y, iRect.y);
    ASSERT_EQ(outVisibles[0].w, iRect.w);
    ASSERT_EQ(outVisibles[0].h, iRect.h);
}

/**
 * @tc.name: GetDirtyRegion001
 * @tc.desc: Verify the GetDirtyRegions of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetDirtyRegion001, Function | MediumTest| Level3)
{
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    std::vector<GraphicIRect> inDirtyRegions;
    inDirtyRegions.emplace_back(iRect);
    HdiLayerInfoTest::hdiLayerInfo_->SetDirtyRegions(inDirtyRegions);
    const std::vector<GraphicIRect>& outDirtyRegions = HdiLayerInfoTest::hdiLayerInfo_->GetDirtyRegions();
    ASSERT_EQ(outDirtyRegions.size(), 1);
    ASSERT_EQ(outDirtyRegions[0].x, iRect.x);
    ASSERT_EQ(outDirtyRegions[0].y, iRect.y);
    ASSERT_EQ(outDirtyRegions[0].w, iRect.w);
    ASSERT_EQ(outDirtyRegions[0].h, iRect.h);
}

/**
 * @tc.name: GetBlendType001
 * @tc.desc: Verify the GetBlendType of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetBlendType001, Function | MediumTest| Level3)
{
    GraphicBlendType type = GraphicBlendType::GRAPHIC_BLEND_CLEAR;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_CLEAR);

    type = GraphicBlendType::GRAPHIC_BLEND_SRC;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_SRC);

    type = GraphicBlendType::GRAPHIC_BLEND_SRCOVER;
    HdiLayerInfoTest::hdiLayerInfo_->SetBlendType(type);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetBlendType(), GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
}

/**
 * @tc.name: GetCropRect001
 * @tc.desc: Verify the GetCropRect of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetCropRect001, Function | MediumTest| Level3)
{
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetCropRect(iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetCropRect().h, iRect.h);
}

/**
 * @tc.name: GetLayerSize001
 * @tc.desc: Verify the GetLayerSize of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, GetLayerSize001, Function | MediumTest| Level3)
{
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    HdiLayerInfoTest::hdiLayerInfo_->SetLayerSize(iRect);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().x, iRect.x);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().y, iRect.y);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().w, iRect.w);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetLayerSize().h, iRect.h);
}

/**
 * @tc.name: IsPreMulti001
 * @tc.desc: Verify the IsPreMulti of hdilayerinfo
 * @tc.type:FUNC
 * @tc.require:AR000GGP0P
 * @tc.author:
 */
HWTEST_F(HdiLayerInfoTest, IsPreMulti001, Function | MediumTest| Level3)
{
    HdiLayerInfoTest::hdiLayerInfo_->SetPreMulti(true);
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->IsPreMulti(), false);

    HdiLayerInfoTest::hdiLayerInfo_->SetPreMulti(false);
    ASSERT_NE(HdiLayerInfoTest::hdiLayerInfo_->IsPreMulti(), true);
}

/*
* Function: SetTunnelHandleChange and GetTunnelHandleChange
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetTunnelHandleChange with default
*                  2. call SetTunnelHandleChange
*                  3. call GetTunnelHandleChange and check ret
 */
HWTEST_F(HdiLayerInfoTest, TunnelHandleChange001, Function | MediumTest | Level1)
{
    bool change = HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandleChange();
    ASSERT_EQ(change, false);
    HdiLayerInfoTest::hdiLayerInfo_->SetTunnelHandleChange(true);
    change = HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandleChange();
    ASSERT_EQ(change, true);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetTunnelHandle with default
* @tc.require: issueI5GMZN issueI5IWHW
 */
HWTEST_F(HdiLayerInfoTest, TunnelHandle001, Function | MediumTest | Level1)
{
    sptr<SurfaceTunnelHandle> handle = HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandle();
    ASSERT_EQ(handle, nullptr);
}

/*
* Function: SetTunnelHandle and GetTunnelHandle
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelHandle
*                  2. call GetTunnelHandle and check ret
* @tc.require: issueI5GMZN issueI5IWHW
 */
HWTEST_F(HdiLayerInfoTest, TunnelHandle002, Function | MediumTest | Level1)
{
    sptr<SurfaceTunnelHandle> handle = HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandle();
    ASSERT_EQ(handle, nullptr);

    sptr<SurfaceTunnelHandle> tunnelHandle = new SurfaceTunnelHandle;

    uint32_t reserveInts = 1;
    GraphicExtDataHandle *handleSet = AllocExtDataHandle(reserveInts);
    ASSERT_EQ(tunnelHandle->SetHandle(handleSet), OHOS::GSERROR_OK);
    ASSERT_NE(tunnelHandle, nullptr);
    HdiLayerInfoTest::hdiLayerInfo_->SetTunnelHandle(tunnelHandle);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandle()->GetHandle()->fd,
              tunnelHandle->GetHandle()->fd);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandle()->GetHandle()->reserveInts,
              tunnelHandle->GetHandle()->reserveInts);
    ASSERT_EQ(HdiLayerInfoTest::hdiLayerInfo_->GetTunnelHandle()->GetHandle()->reserve[0],
              tunnelHandle->GetHandle()->reserve[0]);
    free(handleSet);
}

/*
* Function: SetColorTransform and GetColorTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetColorTransform
*                  2. call GetColorTransform and check ret
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, ColorTransform001, Function | MediumTest | Level1)
{
    std::vector<float> matrix = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    HdiLayerInfoTest::hdiLayerInfo_->SetColorTransform(matrix);
    const std::vector<float>& transform = HdiLayerInfoTest::hdiLayerInfo_->GetColorTransform();
    ASSERT_EQ(transform.size(), 9);
}

/*
* Function: SetLayerColor and GetColorTransform
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerColor
*                  2. call SetLayerColor and check ret
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, LayerColor001, Function | MediumTest | Level1)
{
    const uint32_t COLOR_R = 155;
    const uint32_t COLOR_G = 224;
    const uint32_t COLOR_B = 88;
    const uint32_t COLOR_A = 128;

    GraphicLayerColor layercolor = {
        .r = COLOR_R,
        .g = COLOR_G,
        .b = COLOR_B,
        .a = COLOR_A
    };

    HdiLayerInfoTest::hdiLayerInfo_->SetLayerColor(layercolor);
    GraphicLayerColor color = HdiLayerInfoTest::hdiLayerInfo_->GetLayerColor();
    ASSERT_EQ(color.r, layercolor.r);
    ASSERT_EQ(color.g, layercolor.g);
    ASSERT_EQ(color.b, layercolor.b);
    ASSERT_EQ(color.a, layercolor.a);
}


/*
* Function: SetColorDataSpace and GetColorDataSpace
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetColorDataSpace with default
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, ColorDataSpace001, Function | MediumTest | Level1)
{
    GraphicColorDataSpace colorSpace = HdiLayerInfoTest::hdiLayerInfo_->GetColorDataSpace();
    ASSERT_EQ(colorSpace, GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN);
}

/*
* Function: SetColorDataSpace and GetColorDataSpace
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetColorDataSpace
*                  2. call GetColorDataSpace and check ret
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, ColorDataSpace002, Function | MediumTest | Level1)
{
    GraphicColorDataSpace colorSpaceSet = GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3;
    HdiLayerInfoTest::hdiLayerInfo_->SetColorDataSpace(colorSpaceSet);
    GraphicColorDataSpace colorSpaceGet = HdiLayerInfoTest::hdiLayerInfo_->GetColorDataSpace();
    ASSERT_EQ(colorSpaceSet, colorSpaceGet);
}

/*
* Function: SetMetaData and GetMetaData
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaData
*                  2. call GetMetaData and check ret
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, MetaData001, Function | MediumTest | Level1)
{
    std::vector<GraphicHDRMetaData> metaData = {{GRAPHIC_MATAKEY_RED_PRIMARY_X, 1}};
    HdiLayerInfoTest::hdiLayerInfo_->SetMetaData(metaData);
    std::vector<GraphicHDRMetaData> metaDataGet = HdiLayerInfoTest::hdiLayerInfo_->GetMetaData();
    ASSERT_EQ(metaData[0].key, metaDataGet[0].key);
    ASSERT_EQ(metaData[0].value, metaDataGet[0].value);
}

/*
* Function: SetMetaDataSet and GetMetaDataSet
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetMetaDataSet
*                  2. call GetMetaDataSet and check ret
* @tc.require: issueI5H317
 */
HWTEST_F(HdiLayerInfoTest, MetaDataSet001, Function | MediumTest | Level1)
{
    GraphicHDRMetaDataSet metaDataSet = {GraphicHDRMetadataKey::GRAPHIC_MATAKEY_RED_PRIMARY_X, {1, 2, 3}};
    HdiLayerInfoTest::hdiLayerInfo_->SetMetaDataSet(metaDataSet);
    GraphicHDRMetaDataSet metaDataSetGet = HdiLayerInfoTest::hdiLayerInfo_->GetMetaDataSet();
    ASSERT_EQ(metaDataSet.key, metaDataSetGet.key);
    ASSERT_EQ(metaDataSet.metaData[0], metaDataSetGet.metaData[0]);
    ASSERT_EQ(metaDataSet.metaData[1], metaDataSetGet.metaData[1]);
    ASSERT_EQ(metaDataSet.metaData[2], metaDataSetGet.metaData[2]);
}

/*
* Function: SetIsSupportedPresentTimestamp and IsSupportedPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call IsSupportedPresentTimestamp with default
* @tc.require: issueI5I57K
 */
HWTEST_F(HdiLayerInfoTest, IsSupportedPresentTimestamp001, Function | MediumTest | Level1)
{
    bool isSupported = HdiLayerInfoTest::hdiLayerInfo_->IsSupportedPresentTimestamp();
    ASSERT_EQ(isSupported, false);
}

/*
* Function: SetIsSupportedPresentTimestamp and IsSupportedPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetIsSupportedPresentTimestamp
*                  2. call IsSupportedPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
HWTEST_F(HdiLayerInfoTest, IsSupportedPresentTimestamp002, Function | MediumTest | Level1)
{
    HdiLayerInfoTest::hdiLayerInfo_->SetIsSupportedPresentTimestamp(true);
    bool isSupported = HdiLayerInfoTest::hdiLayerInfo_->IsSupportedPresentTimestamp();
    ASSERT_EQ(isSupported, true);
    HdiLayerInfoTest::hdiLayerInfo_->SetIsSupportedPresentTimestamp(false);
    isSupported = HdiLayerInfoTest::hdiLayerInfo_->IsSupportedPresentTimestamp();
    ASSERT_EQ(isSupported, false);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetPresentTimestamp with default
* @tc.require: issueI5I57K
 */
HWTEST_F(HdiLayerInfoTest, PresentTimestamp001, Function | MediumTest | Level1)
{
    GraphicPresentTimestamp timestamp = HdiLayerInfoTest::hdiLayerInfo_->GetPresentTimestamp();
    ASSERT_EQ(timestamp.type, GRAPHIC_DISPLAY_PTS_UNSUPPORTED);
    ASSERT_EQ(timestamp.time, 0);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp
*                  2. call GetPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
HWTEST_F(HdiLayerInfoTest, PresentTimestamp002, Function | MediumTest | Level1)
{
    GraphicPresentTimestamp timestampSet = {GRAPHIC_DISPLAY_PTS_DELAY, 1};  // mock data for test
    HdiLayerInfoTest::hdiLayerInfo_->SetPresentTimestamp(timestampSet);
    GraphicPresentTimestamp timestampGet = HdiLayerInfoTest::hdiLayerInfo_->GetPresentTimestamp();
    ASSERT_EQ(timestampSet.type, timestampGet.type);
    ASSERT_EQ(timestampSet.time, timestampGet.time);
}

/*
* Function: SetPresentTimestamp and GetPresentTimestamp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetPresentTimestamp
*                  2. call GetPresentTimestamp and check ret
* @tc.require: issueI5I57K
 */
HWTEST_F(HdiLayerInfoTest, PresentTimestamp003, Function | MediumTest | Level1)
{
    GraphicPresentTimestamp timestampSet = {GRAPHIC_DISPLAY_PTS_TIMESTAMP, 10};  // mock data for test
    HdiLayerInfoTest::hdiLayerInfo_->SetPresentTimestamp(timestampSet);
    GraphicPresentTimestamp timestampGet = HdiLayerInfoTest::hdiLayerInfo_->GetPresentTimestamp();
    ASSERT_EQ(timestampSet.type, timestampGet.type);
    ASSERT_EQ(timestampSet.time, timestampGet.time);
}

/*
* Function: SetLayerMaskInfo and GetLayerMaskInfo
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerMaskInfo
*                  2. call GetLayerMaskInfo and check ret
* @tc.require: issueI6WBXO
 */
HWTEST_F(HdiLayerInfoTest, SetLayerMaskInfo001, Function | MediumTest | Level1)
{
    HdiLayerInfo::LayerMask layerMask = HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL;
    HdiLayerInfoTest::hdiLayerInfo_->SetLayerMaskInfo(layerMask);
    HdiLayerInfo::LayerMask Get = HdiLayerInfoTest::hdiLayerInfo_->GetLayerMaskInfo();
    ASSERT_EQ(layerMask, Get);
}

/*
* Function: SetLayerMaskInfo and GetLayerMaskInfo
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerMaskInfo
*                  2. call GetLayerMaskInfo and check ret
* @tc.require: issueI6WBXO
 */
HWTEST_F(HdiLayerInfoTest, SetLayerMaskInfo002, Function | MediumTest | Level1)
{
    HdiLayerInfo::LayerMask layerMask = HdiLayerInfo::LayerMask::LAYER_MASK_HBM_SYNC;
    HdiLayerInfoTest::hdiLayerInfo_->SetLayerMaskInfo(layerMask);
    HdiLayerInfo::LayerMask Get = HdiLayerInfoTest::hdiLayerInfo_->GetLayerMaskInfo();
    ASSERT_EQ(layerMask, Get);
}
} // namespace
} // namespace Rosen
} // namespace OHOS