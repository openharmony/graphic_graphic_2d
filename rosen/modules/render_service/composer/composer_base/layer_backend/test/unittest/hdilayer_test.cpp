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

#include <gtest/gtest.h>

#include "hdi_layer.h"
#include "mock_hdi_device.h"
#include "surface_buffer_impl.h"
#include "rs_render_composer_client.h"
#include "rs_surface_layer.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

#define WIDTH_VAL 50
#define HEIGHT_VAL 50

namespace OHOS {
namespace Rosen {
class HdiLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiLayer> hdiLayer_;
    static inline std::shared_ptr<RSLayer> rsLayer_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline std::vector<std::string> paramKey_ {};
};

void HdiLayerTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    rsLayer_ = std::make_shared<RSSurfaceLayer>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    rsLayer_->SetSurface(cSurface);
    GraphicIRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    GraphicIRect dstRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    rsLayer_->SetLayerSize(dstRect);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect);
    rsLayer_->SetDirtyRegions(dirtyRegions);
    rsLayer_->SetCropRect(srcRect);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(srcRect);
    rsLayer_->SetVisibleRegions(visibleRegions);
    GraphicLayerAlpha layerAlpha = {false, false, 0, 0, 0};
    rsLayer_->SetAlpha(layerAlpha);
    rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_NONE);
    rsLayer_->SetLayerMaskInfo(LayerMask::LAYER_MASK_NORMAL);
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer_->SetBuffer(buffer, fence);

    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerZorder(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPreMulti(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetLayerColorDataSpace(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaData(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerProperty(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetPresentTimestamp(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMaskInfo(_, _, _)).WillRepeatedly(testing::Return(0));
}

void HdiLayerTest::TearDownTestCase() {}

namespace {
class MockSurfaceBuffer : public SurfaceBufferImpl {
public:
    MOCK_CONST_METHOD0(GetBufferHandle, BufferHandle*());
};

/*
* Function: SetHdiDeviceMock001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetHdiDeviceMock()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetHdiDeviceMock001, Function | MediumTest| Level1)
{
    // Set hdiLayer_->device_ to nullptr
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(nullptr), GRAPHIC_DISPLAY_NULL_PTR);
    // Set hdiLayer_->device_ to hdiDeviceMock_
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(hdiDeviceMock_), GRAPHIC_DISPLAY_SUCCESS);
    // Reset hdiLayer_->device_ to hdiDeviceMock_
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(hdiDeviceMock_), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: Init001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Init()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, Init001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(nullptr), false);
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, _)).WillRepeatedly(testing::Return(1));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::rsLayer_), false);
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::rsLayer_), true);
}

/*
* Function: GetReleaseFence001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetReleaseFence()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, GetReleaseFence001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(), GRAPHIC_DISPLAY_FAILURE);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetReleaseFence().GetRefPtr()->Get(), -1);
    HdiLayerTest::hdiLayer_->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    HdiLayerTest::hdiLayer_->UpdateRSLayer(rsLayer_);
    HdiLayerTest::hdiLayer_->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    HdiLayerTest::hdiLayer_->RecordPresentTime(0);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetReleaseFence().GetRefPtr()->Get(), -1);
}

/*
* Function: SetHdiLayerInfo002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetHdiLayerInfo002(true)
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetHdiLayerInfo002, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(false), GRAPHIC_DISPLAY_SUCCESS);
    hdiLayer_->SavePrevRSLayer();
    hdiLayer_->prevRSLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(false), GRAPHIC_DISPLAY_SUCCESS);
    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(false), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetLayerTunnelHandle001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerTunnelHandle()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetLayerTunnelHandle001, Function | MediumTest| Level3)
{
    HdiLayerTest::rsLayer_->SetTunnelHandleChange(true);
    HdiLayerTest::rsLayer_->SetTunnelHandle(nullptr);
    HdiLayerTest::hdiLayer_->UpdateRSLayer(HdiLayerTest::rsLayer_);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetLayerTunnelHandle(), GRAPHIC_DISPLAY_SUCCESS);

    HdiLayerTest::rsLayer_->SetTunnelHandle(new SurfaceTunnelHandle());
    HdiLayerTest::hdiLayer_->UpdateRSLayer(HdiLayerTest::rsLayer_);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetLayerTunnelHandle(), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: SetTunnelLayerId001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelLayerId()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetTunnelLayerId001, Function | MediumTest| Level1)
{
    HdiLayerTest::hdiLayer_->prevRSLayer_ = nullptr;
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetTunnelLayerId(), GRAPHIC_DISPLAY_SUCCESS);
    HdiLayerTest::hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>();
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetTunnelLayerId(), GRAPHIC_DISPLAY_SUCCESS);
}
 
/*
* Function: SetTunnelLayerId002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelLayerId()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetTunnelLayerId002, Function | MediumTest| Level1)
{
    uint32_t devId = 1;
    uint32_t layerId = 2;
    uint64_t tunnelId = 3;
 
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(devId, layerId, tunnelId)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetTunnelLayerId(), 0);
}
 
/*
* Function: SetTunnelLayerProperty001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetTunnelLayerProperty()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetTunnelLayerProperty001, Function | MediumTest| Level1)
{
    uint32_t devId = 1;
    uint32_t layerId = 2;
    uint32_t property = 3;
 
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerProperty(devId, layerId, property)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetTunnelLayerProperty(), 0);
}

/*
* Function: GetLayerStatus001
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerStatus()
*                  2. call GetLayerStatus()
*                  3. check ret
*/
HWTEST_F(HdiLayerTest, GetLayerStatus001, Function | MediumTest| Level3)
{
    bool isUsing = true;
    HdiLayerTest::hdiLayer_->SetLayerStatus(isUsing);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetLayerStatus(), true);

    isUsing = false;
    HdiLayerTest::hdiLayer_->SetLayerStatus(isUsing);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->GetLayerStatus(), false);
    std::string ret = "";
    hdiLayer_->SelectHitchsInfo("", ret);
    hdiLayer_->SelectHitchsInfo("surface", ret);

    sptr<SyncFence> fbAcquireFence = nullptr;
    hdiLayer_->MergeWithFramebufferFence(fbAcquireFence);
    hdiLayer_->MergeWithLayerFence(fbAcquireFence);
    fbAcquireFence = new SyncFence(-1);
    hdiLayer_->MergeWithFramebufferFence(fbAcquireFence);
    hdiLayer_->MergeWithLayerFence(fbAcquireFence);
    hdiLayer_->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    hdiLayer_->SetLayerPresentTimestamp();
    sptr<SyncFence> releaseFence = new SyncFence(-1);
    hdiLayer_->SetReleaseFence(releaseFence);
    hdiLayer_->SetReleaseFence(nullptr);
    std::string dumpStr = "";
    hdiLayer_->Dump(dumpStr);
    hdiLayer_->DumpByName("surface", dumpStr);
    hdiLayer_->DumpMergedResult(ret);
    EXPECT_CALL(*hdiDeviceMock_, ClearLayerBuffer(_, _)).WillRepeatedly(testing::Return(0));
    hdiLayer_->ClearBufferCache();
}

/*
* Function: CloseLayer001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call CloseLayer()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, CreateLayer001, Function | MediumTest| Level1)
{
    uint32_t layerId = 1;
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, layerId)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::rsLayer_), true);
    auto tmpSurface = HdiLayerTest::hdiLayer_->GetRSLayer()->GetSurface();
    HdiLayerTest::rsLayer_->SetSurface(nullptr);
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, layerId)).WillRepeatedly(testing::Return(0));
    HdiLayerTest::rsLayer_->SetSurface(tmpSurface);
    EXPECT_CALL(*hdiDeviceMock_, CloseLayer(_, _)).WillRepeatedly(testing::Return(1));
}

/*
* Function: ClearBufferCache001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ClearBufferCache()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, ClearBufferCache001, Function | MediumTest| Level1)
{
    HdiLayerTest::hdiLayer_->ClearBufferCache();
    EXPECT_EQ(hdiLayer_->bufferCache_.size(), 0);
}

/*
 * Function: SetPerFrameParameters001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters001, Function | MediumTest| Level1)
{
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
}
 
/*
 * Function: SetPerFrameParameters002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters002, Function | MediumTest| Level1)
{
    paramKey_.clear();
    rsLayer_->SetTunnelLayerId(0);
    rsLayer_->SetTunnelLayerProperty(0);
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}
 
/*
 * Function: SetPerFrameParameters003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters003, Function | MediumTest| Level1)
{
    paramKey_.clear();
    rsLayer_->SetTunnelLayerId(1);
    rsLayer_->SetTunnelLayerProperty(0);
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}
 
/*
 * Function: SetPerFrameParameters004
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters004, Function | MediumTest| Level1)
{
    paramKey_.clear();
    rsLayer_->SetTunnelLayerId(0);
    rsLayer_->SetTunnelLayerProperty(1);
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}
 
/*
 * Function: SetPerFrameParameters005
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters005, Function | MediumTest| Level1)
{
    paramKey_.clear();
    rsLayer_->SetTunnelLayerId(1);
    rsLayer_->SetTunnelLayerProperty(1);
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}

/**
 * Function: SetLayerBuffer001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerBuffer()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer, Function | MediumTest| Level1)
{
    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>();
    auto rsLayer = std::make_shared<RSSurfaceLayer>();
    hdiLayer->prevRSLayer_ = prevRSLayer;
    hdiLayer->rsLayer_ = rsLayer;
    auto res = hdiLayer->SetLayerBuffer();
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> newBuffer = new SurfaceBufferImpl();
    newBuffer->Alloc(requestConfig);
    sptr<SyncFence> newFence = new SyncFence(-1);
    prevRSLayer->SetBuffer(newBuffer, newFence);
    rsLayer->SetBuffer(newBuffer, newFence);
    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);
    hdiLayer->doLayerInfoCompare_ = true;
    hdiLayer->bufferCleared_ = true;
    hdiLayer->SetLayerBuffer();
    EXPECT_FALSE(hdiLayer->bufferCleared_);

    hdiLayer->bufferCleared_ = false;
    res = hdiLayer->SetLayerBuffer();

    sptr<SurfaceBuffer> oldBuffer = new SurfaceBufferImpl();
    oldBuffer->Alloc(requestConfig);
    sptr<SyncFence> oldFence = new SyncFence(-1);
    prevRSLayer->SetBuffer(oldBuffer, oldFence);
    res = hdiLayer->SetLayerBuffer();
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);

    auto mockBuffer = new MockSurfaceBuffer();
    EXPECT_CALL(*mockBuffer, GetBufferHandle()).WillRepeatedly(testing::Return(nullptr));
    rsLayer->SetBuffer(mockBuffer, newFence);
    res = hdiLayer->SetLayerBuffer();
    EXPECT_EQ(res, GRAPHIC_DISPLAY_NULL_PTR);
}

/**
 * Function: SelectHitchsInfoTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SelectHitchsInfoTest()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SelectHitchsInfoTest, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    std::string ret = "";
    std::vector<std::string> windowsNameTest = {"testName1", "testName2", "testName3"};
    FPSInfo test1 = {1, windowsNameTest};
    FPSInfo test2 = {1, windowsNameTest};
    hdiLayer_->presentTimeRecords_[0] = test1;
    hdiLayer_->presentTimeRecords_[1] = test2;
    hdiLayer_->SelectHitchsInfo("testName1", ret);
    ret = "";
    hdiLayer_->SelectHitchsInfo("testName2", ret);
    EXPECT_NE(ret, "");
    hdiLayer_->presentTimeRecords_[1].presentTime = 70 * 1000000;
    hdiLayer_->SelectHitchsInfo("testName1", ret);
    EXPECT_NE(ret, "");
    hdiLayer_->presentTimeRecords_[1].presentTime = 35 * 1000000;
    hdiLayer_->SelectHitchsInfo("testName1", ret);
    EXPECT_NE(ret, "");
    hdiLayer_->presentTimeRecords_[1].presentTime = 20 * 1000000;
    hdiLayer_->SelectHitchsInfo("testName1", ret);
    EXPECT_NE(ret, "");
}

/**
 * Function: SetPerFrameLayerLinearMatrixTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetPerFrameLayerLinearMatrix()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetPerFrameLayerLinearMatrixTest, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    auto prevRsLayer = std::make_shared<RSSurfaceLayer>();
    std::vector<float> preLayerLinearMatrix
        = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    prevRsLayer->SetLayerLinearMatrix(preLayerLinearMatrix);
    hdiLayer_->prevRSLayer_ = prevRsLayer;
    auto curRsLayer = std::make_shared<RSSurfaceLayer>();
    hdiLayer_->rsLayer_ = curRsLayer;
    auto ret = hdiLayer_->InitDevice();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, _, _)).WillRepeatedly(testing::Return(0));
    ret = hdiLayer_->SetPerFrameLayerLinearMatrix();
    ASSERT_EQ(ret, 0);
    std::vector<float> curLayerLinearMatrix
        = { 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    curRsLayer->SetLayerLinearMatrix(curLayerLinearMatrix);
    ret = hdiLayer_->SetPerFrameLayerLinearMatrix();
    ASSERT_EQ(ret, 0);
}

/**
 * Function: IsSameLayerMetaDataTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call IsSameLayerMetaData()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, IsSameLayerMetaDataTest, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    auto prevRsLayer = std::make_shared<RSSurfaceLayer>();
    std::vector<GraphicHDRMetaData> prevMetaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 1 } };
    prevRsLayer->SetMetaData(prevMetaData);
    hdiLayer_->prevRSLayer_ = prevRsLayer;

    auto curRsLayer = std::make_shared<RSSurfaceLayer>();
    std::vector<GraphicHDRMetaData> metaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 1 } };
    curRsLayer->SetMetaData(metaData);
    hdiLayer_->rsLayer_ = curRsLayer;
    auto ret = hdiLayer_->IsSameLayerMetaData();
    EXPECT_EQ(ret, true);

    prevMetaData[0].value = 2;
    prevRsLayer->SetMetaData(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaData();
    EXPECT_EQ(ret, false);

    prevMetaData.push_back({ GRAPHIC_MATAKEY_GREEN_PRIMARY_X, 1 });
    prevRsLayer->SetMetaData(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaData();
    EXPECT_EQ(ret, false);
}

/**
 * Function: IsSameLayerMetaDataSetTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call IsSameLayerMetaDataSet()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, IsSameLayerMetaDataSetTest, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    auto prevRsLayer = std::make_shared<RSSurfaceLayer>();
    GraphicHDRMetaDataSet prevMetaData = { GRAPHIC_MATAKEY_RED_PRIMARY_X, { 1 } };
    prevRsLayer->SetMetaDataSet(prevMetaData);
    hdiLayer_->prevRSLayer_ = prevRsLayer;

    auto curRsLayer = std::make_shared<RSSurfaceLayer>();
    GraphicHDRMetaDataSet metaData = { GRAPHIC_MATAKEY_RED_PRIMARY_X, { 1 } };
    curRsLayer->SetMetaDataSet(metaData);
    hdiLayer_->rsLayer_ = curRsLayer;
    auto ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, true);

    prevMetaData.metaData[0] = 2;
    prevRsLayer->SetMetaDataSet(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, false);

    prevMetaData.metaData.push_back(3);
    prevRsLayer->SetMetaDataSet(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, false);

    prevMetaData.key = GRAPHIC_MATAKEY_RED_PRIMARY_Y;
    prevRsLayer->SetMetaDataSet(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, false);
}

/**
 * Function: SetLayerAlpha001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerAlpha()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->doLayerInfoCompare_ = true;
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>();
    GraphicLayerAlpha layerAlpha1 = {false, false, 0, 0, 0};
    hdiLayer_->rsLayer_->SetAlpha(layerAlpha1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>();
    GraphicLayerAlpha layerAlpha2 = {true, true, 0, 0, 0};
    hdiLayer_->prevRSLayer_->SetAlpha(layerAlpha2);
    ASSERT_EQ(hdiLayer_->SetLayerAlpha(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerVisibleRegion()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->doLayerInfoCompare_ = true;
    GraphicIRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    std::vector<GraphicIRect> visibleRegions1 = {};
    visibleRegions1.push_back(srcRect);
    std::vector<GraphicIRect> visibleRegions2 = {};
    visibleRegions2.push_back(srcRect);
    visibleRegions2.push_back(srcRect);

    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>();
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>();
    hdiLayer_->prevRSLayer_->SetVisibleRegions(visibleRegions2);
    ASSERT_EQ(hdiLayer_->SetLayerVisibleRegion(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerDirtyRegion()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->doLayerInfoCompare_ = true;
    GraphicIRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    std::vector<GraphicIRect> visibleRegions1 = {};
    visibleRegions1.push_back(srcRect);
    std::vector<GraphicIRect> visibleRegions2 = {};
    visibleRegions2.push_back(srcRect);
    visibleRegions2.push_back(srcRect);

    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>();
    hdiLayer_->rsLayer_->SetDirtyRegions(visibleRegions1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>();
    hdiLayer_->prevRSLayer_->SetDirtyRegions(visibleRegions2);
    ASSERT_EQ(hdiLayer_->SetLayerDirtyRegion(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: CheckAndUpdateLayerBufferCache001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CheckAndUpdateLayerBufferCache()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, CheckAndUpdateLayerBufferCache001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->bufferCache_.push_back(2);
    hdiLayer_->bufferCacheCountMax_ = 1;
    uint32_t sequence = 0;
    uint32_t index = 0;
    std::vector<uint32_t> deletingList = {};
    ASSERT_EQ(hdiLayer_->CheckAndUpdateLayerBufferCahce(sequence, index, deletingList), false);
}

/**
 * Function: SetLayerPresentTimestamp001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerPresentTimestamp()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetLayerPresentTimestamp001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->InitDevice();
    ASSERT_NE(hdiLayer_->device_, nullptr);
    hdiLayer_->supportedPresentTimestamptype_ = GRAPHIC_DISPLAY_PTS_TIMESTAMP;
    auto ret = hdiLayer_->SetLayerPresentTimestamp();
    ASSERT_NE(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: RecordMergedPresentTime001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetLayerPresentTimestamp()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, RecordMergedPresentTime001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->InitDevice();
    ASSERT_NE(hdiLayer_->device_, nullptr);
    hdiLayer_->RecordMergedPresentTime(0);
}

/**
 * Function: ClearBufferCache002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ClearBufferCache()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, ClearBufferCache002, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->rsLayer_ = nullptr;
    hdiLayer_->ClearBufferCache();
}

} // namespace
} // namespace Rosen
} // namespace OHOS
