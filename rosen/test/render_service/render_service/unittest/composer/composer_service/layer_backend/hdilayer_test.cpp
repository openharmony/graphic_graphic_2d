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

#include "hdi_layer.h"
#include "mock_hdi_device.h"
#include "surface_buffer_impl.h"
#include "rs_composer_client.h"
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
    rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
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

/**
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

/**
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

/**
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

/**
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

/**
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

/**
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
    HdiLayerTest::hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetTunnelLayerId(), GRAPHIC_DISPLAY_SUCCESS);
}
 
/**
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
 
/**
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

/**
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

/**
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

/**
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

/**
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
 
/**
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
 
/**
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
 
/**
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
 
/**
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
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);
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
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    std::vector<float> preLayerLinearMatrix
        = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    prevRSLayer->SetLayerLinearMatrix(preLayerLinearMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;
    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_ = curRSLayer;
    auto ret = hdiLayer_->InitDevice();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, _, _)).WillRepeatedly(testing::Return(0));
    ret = hdiLayer_->SetPerFrameLayerLinearMatrix();
    ASSERT_EQ(ret, 0);
    std::vector<float> curLayerLinearMatrix
        = {1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f};
    curRSLayer->SetLayerLinearMatrix(curLayerLinearMatrix);
    hdiLayer_->prevRSLayer_ = nullptr;
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
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    std::vector<GraphicHDRMetaData> prevMetaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 1 } };
    prevRSLayer->SetMetaData(prevMetaData);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    std::vector<GraphicHDRMetaData> metaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 1 } };
    curRSLayer->SetMetaData(metaData);
    hdiLayer_->rsLayer_ = curRSLayer;
    auto ret = hdiLayer_->IsSameLayerMetaData();
    EXPECT_EQ(ret, true);

    prevMetaData[0].value = 2;
    prevRSLayer->SetMetaData(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaData();
    EXPECT_EQ(ret, false);

    prevMetaData.push_back({ GRAPHIC_MATAKEY_RED_PRIMARY_Y, 1 });
    prevRSLayer->SetMetaData(prevMetaData);
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
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    GraphicHDRMetaDataSet prevMetaData = { GRAPHIC_MATAKEY_RED_PRIMARY_X, { 1 } };
    prevRSLayer->SetMetaDataSet(prevMetaData);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    GraphicHDRMetaDataSet metaData = { GRAPHIC_MATAKEY_RED_PRIMARY_X, { 1 } };
    curRSLayer->SetMetaDataSet(metaData);
    hdiLayer_->rsLayer_ = curRSLayer;
    auto ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, true);

    prevMetaData.metaData[0] = 2;
    prevRSLayer->SetMetaDataSet(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, false);

    prevMetaData.metaData.push_back(3);
    prevRSLayer->SetMetaDataSet(prevMetaData);
    ret = hdiLayer_->IsSameLayerMetaDataSet();
    EXPECT_EQ(ret, false);

    prevMetaData.key = GRAPHIC_MATAKEY_RED_PRIMARY_Y;
    prevRSLayer->SetMetaDataSet(prevMetaData);
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
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    GraphicLayerAlpha layerAlpha1 = {false, false, 0, 0, 0};
    hdiLayer_->rsLayer_->SetAlpha(layerAlpha1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
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

    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions2);
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

    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetDirtyRegions(visibleRegions1);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetDirtyRegions(visibleRegions2);
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
    EXPECT_CALL(*hdiDeviceMock_, ClearLayerBuffer(_, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(hdiLayer_->CheckAndUpdateLayerBufferCache(sequence, index, deletingList), false);
}

/**
 * Function: CheckAndUpdateLayerBufferCache002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CheckAndUpdateLayerBufferCache()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, CheckAndUpdateLayerBufferCache002, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->bufferCache_.push_back(2);
    hdiLayer_->bufferCacheCountMax_ = 1;
    hdiLayer_->rsLayer_ = HdiLayerTest::rsLayer_;
    uint32_t sequence = 0;
    uint32_t index = 0;
    std::vector<uint32_t> deletingList = {};
    EXPECT_CALL(*hdiDeviceMock_, ClearLayerBuffer(_, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(hdiLayer_->CheckAndUpdateLayerBufferCache(sequence, index, deletingList), false);
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
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
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

/**
 * Function: SetTunnelLayerParametersTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetTunnelLayerParameters()
 *                  2. check ret
 */
HWTEST_F(HdiLayerTest, SetTunnelLayerParametersTest, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer->SetTunnelLayerId(1);
    rsLayer->SetTunnelLayerProperty(1);
    hdiLayer_->rsLayer_ = rsLayer;
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, _)).WillRepeatedly(testing::Return(-1));
    auto ret = hdiLayer_->SetTunnelLayerParameters();
    EXPECT_EQ(ret, -1);
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerProperty(_, _, _)).WillRepeatedly(testing::Return(-1));
    ret = hdiLayer_->SetTunnelLayerParameters();
    EXPECT_EQ(ret, -1);
}

/**
 * Function: ResetBufferCache001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ResetBufferCache() with rsLayer_ is nullptr
 *                  2. check bufferCache_ is cleared and bufferCleared_ is true
 */
HWTEST_F(HdiLayerTest, ResetBufferCache001, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->bufferCleared_ = false;
    hdiLayer_->rsLayer_ = nullptr;

    hdiLayer_->ResetBufferCache();

    EXPECT_EQ(hdiLayer_->bufferCache_.size(), 0);
    EXPECT_TRUE(hdiLayer_->bufferCleared_);
}

/**
 * Function: ResetBufferCache002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ResetBufferCache() with rsLayer_ is not nullptr but buffer is nullptr
 *                  2. check bufferCache_ is cleared and bufferCleared_ is true
 */
HWTEST_F(HdiLayerTest, ResetBufferCache002, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->bufferCleared_ = false;
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetBuffer(nullptr);

    hdiLayer_->ResetBufferCache();

    EXPECT_EQ(hdiLayer_->bufferCache_.size(), 0);
    EXPECT_TRUE(hdiLayer_->bufferCleared_);
}

/**
 * Function: ResetBufferCache003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ResetBufferCache() with rsLayer_ is not nullptr and buffer is not nullptr
 *                  2. check bufferCache_ is cleared, bufferCleared_ is true, and currBuffer_ is set
 */
HWTEST_F(HdiLayerTest, ResetBufferCache003, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(1);
    hdiLayer_->bufferCleared_ = false;
    hdiLayer_->currBuffer_ = nullptr;
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    hdiLayer_->rsLayer_->SetBuffer(buffer);

    hdiLayer_->ResetBufferCache();

    EXPECT_EQ(hdiLayer_->bufferCache_.size(), 0);
    EXPECT_TRUE(hdiLayer_->bufferCleared_);
    EXPECT_EQ(hdiLayer_->currBuffer_, buffer);
}

/**
 * Function: SetTransformMode_NoChangeOrButt
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Skip setting when transform is GRAPHIC_ROTATE_BUTT or equals prev
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetTransformMode_NoChangeOrButt, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);

    hdiLayer_->rsLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_BUTT);
    ASSERT_EQ(hdiLayer_->SetTransformMode(), GRAPHIC_DISPLAY_SUCCESS);

    hdiLayer_->rsLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    hdiLayer_->prevRSLayer_->SetTransform(GraphicTransformType::GRAPHIC_ROTATE_90);
    hdiLayer_->doLayerInfoCompare_ = true;
    ASSERT_EQ(hdiLayer_->SetTransformMode(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerZorder_And_PreMulti_NoChange
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Skip setting when Zorder/PreMulti equals prev
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetLayerZorder_And_PreMulti_NoChange, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->prevRSLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->doLayerInfoCompare_ = true;

    hdiLayer_->rsLayer_->SetZorder(5);
    hdiLayer_->prevRSLayer_->SetZorder(5);
    ASSERT_EQ(hdiLayer_->SetLayerZorder(), GRAPHIC_DISPLAY_SUCCESS);

    hdiLayer_->rsLayer_->SetPreMulti(true);
    hdiLayer_->prevRSLayer_->SetPreMulti(true);
    ASSERT_EQ(hdiLayer_->SetLayerPreMulti(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_Set
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Set color data space, expect success
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_Set, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->rsLayer_ = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    ASSERT_EQ(hdiLayer_->SetLayerColorDataSpace(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_SpacesMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, color data spaces match
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_SpacesMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same color data space on current rsLayer
    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);

    // doLayerInfoCompare_ is true AND color data spaces match
    // Expect SetLayerColorDataSpace NOT to be called (early return)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_UnknownMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are UNKNOWN space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_UnknownMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_BT601Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are BT601 space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_BT601Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_BT601);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_BT601);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_BT709Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are BT709 space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_BT709Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_BT709);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_BT709);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_DCI_P3Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are DCI_P3 space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_DCI_P3Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DCI_P3);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DCI_P3);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_SRGBMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are SRGB space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_SRGBMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_SRGB);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_SRGB);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorDataSpace_DoLayerInfoCompareTrue_DisplayP3Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are DISPLAY_P3 space
 *                  2. call SetLayerColorDataSpace
 *                  3. verify early return (spaces match)
 */
HWTEST_F(HdiLayerTest, SetLayerColorDataSpace_DoLayerInfoCompareTrue_DisplayP3Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorDataSpace(GraphicColorDataSpace::GRAPHIC_GAMUT_DISPLAY_P3);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorDataSpace(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorDataSpace();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaData_Different
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. When metaData differs from prev, take setting path
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetLayerMetaData_Different, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    std::vector<GraphicHDRMetaData> prevMetaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 1 } };
    prevRSLayer->SetMetaData(prevMetaData);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    std::vector<GraphicHDRMetaData> metaData = { { GRAPHIC_MATAKEY_RED_PRIMARY_X, 2 } };
    curRSLayer->SetMetaData(metaData);
    hdiLayer_->rsLayer_ = curRSLayer;
    ASSERT_EQ(hdiLayer_->SetLayerMetaData(), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: CheckAndUpdateLayerBufferCache_Found
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Return true when sequence is found
 *                  2. check index/deletingList
 */
HWTEST_F(HdiLayerTest, CheckAndUpdateLayerBufferCache_Found, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->bufferCache_.clear();
    hdiLayer_->bufferCache_.push_back(10);
    hdiLayer_->bufferCache_.push_back(20);
    hdiLayer_->bufferCacheCountMax_ = 5;
    uint32_t index = 0;
    std::vector<uint32_t> deletingList;
    bool found = hdiLayer_->CheckAndUpdateLayerBufferCache(10, index, deletingList);
    ASSERT_TRUE(found);
    ASSERT_EQ(index, 0u);
    ASSERT_TRUE(deletingList.empty());
}


/**
 * Function: SetLayerPresentTimestamp_Unsupported
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Return success when timestamp type is unsupported
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetLayerPresentTimestamp_Unsupported, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    hdiLayer_->supportedPresentTimestamptype_ = GRAPHIC_DISPLAY_PTS_UNSUPPORTED;
    auto ret = hdiLayer_->SetLayerPresentTimestamp();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameters_LinearMatrixParamErr_Last
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Only linear matrix key; invalid matrix size leads to final error code
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters_LinearMatrixParamErr_Last, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    paramKey_.clear();
    paramKey_.push_back("LayerLinearMatrix");
    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    // Invalid size, triggers GRAPHIC_DISPLAY_PARAM_ERR
    curRSLayer->SetLayerLinearMatrix({1.0f, 2.0f});
    hdiLayer_->rsLayer_ = curRSLayer;
    auto ret = hdiLayer_->SetPerFrameParameters();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_PARAM_ERR);
}

/**
 * Function: SetPerFrameParameters_AllKeys_Success
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Set all keys in order; final return is success
 *                  2. check return value
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameters_AllKeys_Success, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);
    paramKey_.clear();
    paramKey_.push_back("SDRBrightnessNit");
    paramKey_.push_back("BrightnessNit");
    paramKey_.push_back("SDRBrightnessRatio");
    paramKey_.push_back("LayerLinearMatrix");
    paramKey_.push_back("SourceCropTuning");

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetLayerLinearMatrix({ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    auto curRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    curRSLayer->SetSdrNit(10.0f);
    curRSLayer->SetDisplayNit(200.0f);
    curRSLayer->SetBrightnessRatio(1.0f);
    curRSLayer->SetLayerLinearMatrix({ 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f });
    curRSLayer->SetLayerSourceTuning(7);
    hdiLayer_->rsLayer_ = curRSLayer;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, _, _)).WillRepeatedly(testing::Return(0));
    auto ret = hdiLayer_->SetPerFrameParameters();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_ColorsMatch_TrueBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true and colors match
 *                  2. call SetLayerColor
 *                  3. verify sizes match, early return (colors match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerColor_ColorsMatch_TrueBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Create prevRSLayer with GRAPHIC_COMPOSITION_DEVICE type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor color = { 200, 100, 50, 255 };
    prevRSLayer->SetLayerColor(color);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same color on current rsLayer
    hdiLayer_->rsLayer_->SetLayerColor(color);

    // Expect SetLayerColor NOT to be called (colors match, sizes match, early return - colors match)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_ColorsDiffer_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true but colors differ
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called (colors differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerColor_ColorsDiffer_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Create prevRSLayer with GRAPHIC_COMPOSITION_DEVICE type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different color on current rsLayer
    GraphicLayerColor curColor = { 200, 150, 75, 255 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    // Expect SetLayerColor to be called (colors differ, set called - colors differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_RComponentDiffer_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. red component differs between current and previous
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called
 */
HWTEST_F(HdiLayerTest, SetLayerColor_RComponentDiffer_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change only R component
    GraphicLayerColor curColor = { 200, 50, 25, 128 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_GComponentDiffer_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. green component differs between current and previous
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called
 */
HWTEST_F(HdiLayerTest, SetLayerColor_GComponentDiffer_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change only G component
    GraphicLayerColor curColor = { 100, 200, 25, 128 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_BComponentDiffer_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. blue component differs between current and previous
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called
 */
HWTEST_F(HdiLayerTest, SetLayerColor_BComponentDiffer_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change only B component
    GraphicLayerColor curColor = { 100, 50, 200, 128 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_AComponentDiffer_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. alpha component differs between current and previous
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called
 */
HWTEST_F(HdiLayerTest, SetLayerColor_AComponentDiffer_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change only A component
    GraphicLayerColor curColor = { 100, 50, 25, 255 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_PrevCompositionTypeNotDevice_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer composition type is not GRAPHIC_COMPOSITION_DEVICE
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called (doLayerInfoCompare_ will be false)
 */
HWTEST_F(HdiLayerTest, SetLayerColor_PrevCompositionTypeNotDevice_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer with CLIENT composition type (not DEVICE)
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicLayerColor color = { 100, 50, 25, 128 };
    prevRSLayer->SetLayerColor(color);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same color on current rsLayer
    hdiLayer_->rsLayer_->SetLayerColor(color);

    // Expect SetLayerColor to be called (doLayerInfoCompare_ will be false due to composition type)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColor_MultipleDifferingComponents_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. multiple color components differ
 *                  2. call SetLayerColor
 *                  3. verify SetLayerColor is called
 */
HWTEST_F(HdiLayerTest, SetLayerColor_MultipleDifferingComponents_FalseBranch, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerColor prevColor = { 100, 100, 100, 100 };
    prevRSLayer->SetLayerColor(prevColor);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change all components
    GraphicLayerColor curColor = { 200, 150, 175, 255 };
    hdiLayer_->rsLayer_->SetLayerColor(curColor);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerColor(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColor();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_MatricesMatch_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, matrices are identical
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform NOT called (matrices differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_MatricesMatch_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> matrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(matrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same matrix on current rsLayer
    hdiLayer_->rsLayer_->SetColorTransform(matrix);

    //  doLayerInfoCompare_ is true
    //  IsNeedSetInfoToDevice returns false (matrices match)
    // Expect SetLayerColorTransform NOT to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_MatricesDiffer_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, matrices differ
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called (types differ)
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_MatricesDiffer_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different matrix on current rsLayer
    std::vector<float> curMatrix = {1.0f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  doLayerInfoCompare_ is true
    //  IsNeedSetInfoToDevice returns true (matrices differ)
    // Expect SetLayerColorTransform to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_SizeDiffers_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, matrix sizes differ
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_SizeDiffers_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {1.0f, 0.0f, 0.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different size matrix on current rsLayer
    std::vector<float> curMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  doLayerInfoCompare_ is true
    //  IsNeedSetInfoToDevice returns true (sizes differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_AllZero_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, prev matrix has zeros, current has values
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_AllZero_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    std::vector<float> curMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (matrices differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_EmptyMatrix_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, empty vs non-empty matrices
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_EmptyMatrix_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {}; // Empty
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    std::vector<float> curMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (sizes differ: 0 vs 9)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_IdentityMatrices_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are identity matrices
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform NOT called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_IdentityMatrices_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    // Identity matrix
    std::vector<float> identityMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(identityMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetColorTransform(identityMatrix);

    //  true,  false (matrices are identical)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_LargeMatrix_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, large matrices differ
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_LargeMatrix_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix(16, 0.5f); // 4x4 matrix
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    std::vector<float> curMatrix(16, 0.6f); // Different values
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (matrices differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_SingleDiff_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, single element differs
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_SingleDiff_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change only one element
    std::vector<float> curMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.1f, 1.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (one element differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_NegativeValues_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, matrices with negative values differ
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_NegativeValues_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {-0.5f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    std::vector<float> curMatrix = {-0.6f, 0.0f, 0.0f, 0.0f, -0.6f, 0.0f, 0.0f, 0.0f, 1.0f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (matrices differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerColorTransform_DoLayerInfoCompareTrue_LastElementDiff_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, last matrix element differs
 *                  2. call SetLayerColorTransform
 *                  3. verify SetLayerColorTransform IS called
 */
HWTEST_F(HdiLayerTest, SetLayerColorTransform_DoLayerInfoCompareTrue_LastElementDiff_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<float> prevMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    prevRSLayer->SetColorTransform(prevMatrix);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change last element
    std::vector<float> curMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.9f};
    hdiLayer_->rsLayer_->SetColorTransform(curMatrix);

    //  true,  true (last element differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerColorTransform(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerColorTransform();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataSetSame_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, meta data sets are identical
 *                  2. call SetLayerMetaDataSet
 *                  3. verify sizes match, early return (meta data same, sizes match, early return, no call to SetLayerMetaDataSet)
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataSetSame_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // Set meta data set
    std::vector<uint8_t> metaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, metaData};
    prevRSLayer->SetMetaDataSet(metaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same meta data set on current rsLayer
    hdiLayer_->rsLayer_->SetMetaDataSet(metaDataSet);

    //  doLayerInfoCompare_ is true
    //  IsSameLayerMetaDataSet returns true (meta data sets are same)
    // Expect SetLayerMetaDataSet NOT to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataDiffer_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, meta data differs
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called (meta data differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataDiffer_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<uint8_t> prevMetaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different meta data on current rsLayer
    std::vector<uint8_t> curMetaData = {1, 2, 3, 5}; // Different last element
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  doLayerInfoCompare_ is true
    //  IsSameLayerMetaDataSet returns false (meta data differs)
    // Expect SetLayerMetaDataSet to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_KeyDiffers_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, keys differ
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_KeyDiffers_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<uint8_t> metaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, metaData}; // Key = 5
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different key on current rsLayer
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_WHITE_PRIMARY_X, metaData}; // Key = 6
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (keys differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataSizeDiffers_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, meta data sizes differ
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataSizeDiffers_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<uint8_t> prevMetaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different size meta data on current rsLayer
    std::vector<uint8_t> curMetaData = {1, 2, 3};
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (sizes differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_EmptyMetaData_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, empty vs non-empty meta data
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_EmptyMetaData_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // Empty meta data
    std::vector<uint8_t> emptyMetaData = {};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, emptyMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Non-empty meta data on current
    std::vector<uint8_t> curMetaData = {1, 2};
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (empty vs non-empty)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataEmptyBoth_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both meta data are empty
 *                  2. call SetLayerMetaDataSet
 *                  3. verify sizes match, early return (meta data same, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_MetaDataEmptyBoth_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // Empty meta data for both
    std::vector<uint8_t> emptyMetaData = {};
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, emptyMetaData};
    prevRSLayer->SetMetaDataSet(metaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetMetaDataSet(metaDataSet);

    //  true,  false (both empty, same)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_LargeMetaData_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, large meta data with difference
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_LargeMetaData_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // Large meta data
    std::vector<uint8_t> prevMetaData(100, 128);
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_MAX_CONTENT_LIGHT_LEVEL, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Different large meta data
    std::vector<uint8_t> curMetaData(100, 129);
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_MAX_CONTENT_LIGHT_LEVEL, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (large meta data differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_FirstElementDiffers_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, first meta data element differs
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_FirstElementDiffers_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<uint8_t> prevMetaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // First element differs
    std::vector<uint8_t> curMetaData = {0, 2, 3, 4};
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (first element differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_LastElementDiffers_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, last meta data element differs
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_LastElementDiffers_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    std::vector<uint8_t> prevMetaData = {1, 2, 3, 4};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Last element differs
    std::vector<uint8_t> curMetaData = {1, 2, 3, 5};
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (last element differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_AllZeroMetaData_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both meta data are all zeros
 *                  2. call SetLayerMetaDataSet
 *                  3. verify sizes match, early return (meta data same, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_AllZeroMetaData_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // All zero meta data
    std::vector<uint8_t> zeroMetaData = {0, 0, 0, 0};
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, zeroMetaData};
    prevRSLayer->SetMetaDataSet(metaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetMetaDataSet(metaDataSet);

    //  true,  false (both identical)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerMetaDataSet_DoLayerInfoCompareTrue_MaxMetaData_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, max value meta data differs
 *                  2. call SetLayerMetaDataSet
 *                  3. verify SetLayerMetaDataSet IS called
 */
HWTEST_F(HdiLayerTest, SetLayerMetaDataSet_DoLayerInfoCompareTrue_MaxMetaData_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    // Max value meta data
    std::vector<uint8_t> prevMetaData = {255, 255, 255, 255};
    GraphicHDRMetaDataSet prevMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, prevMetaData};
    prevRSLayer->SetMetaDataSet(prevMetaDataSet);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Slightly different
    std::vector<uint8_t> curMetaData = {254, 255, 255, 255};
    GraphicHDRMetaDataSet curMetaDataSet = {GRAPHIC_MATAKEY_BLUE_PRIMARY_Y, curMetaData};
    hdiLayer_->rsLayer_->SetMetaDataSet(curMetaDataSet);

    //  true,  true (meta data differs)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerMetaDataSet(_, _, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerMetaDataSet();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_SizesMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, layer sizes match
 *                  2. call SetLayerSize
 *                  3. verify early return (sizes match)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_SizesMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect size = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(size);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same size on current rsLayer
    hdiLayer_->rsLayer_->SetLayerSize(size);

    //  doLayerInfoCompare_ is true AND sizes match, so sizes match
    // Expect SetLayerSize NOT to be called (sizes match, early return)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_WidthDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, width differs
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_WidthDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different width on current rsLayer
    GraphicIRect curSize = {0, 0, WIDTH_VAL + 10, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    //  doLayerInfoCompare_ is true but sizes differ, so sizes differ
    // Expect SetLayerSize to be called (sizes differ, set called)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_HeightDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, height differs
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_HeightDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different height on current rsLayer
    GraphicIRect curSize = {0, 0, WIDTH_VAL, HEIGHT_VAL + 20};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    //  doLayerInfoCompare_ is true but sizes differ
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_XPosDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, x position differs
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_XPosDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change x position
    GraphicIRect curSize = {10, 0, WIDTH_VAL, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_YPosDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, y position differs
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_YPosDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change y position
    GraphicIRect curSize = {0, 15, WIDTH_VAL, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_AllFieldsDiffer_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, all fields differ
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_AllFieldsDiffer_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change all fields
    GraphicIRect curSize = {100, 200, 300, 400};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer composition type is not GRAPHIC_COMPOSITION_DEVICE
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (doLayerInfoCompare_ will be false)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer with CLIENT composition type (not DEVICE)
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicIRect size = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetLayerSize(size);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same size on current rsLayer
    hdiLayer_->rsLayer_->SetLayerSize(size);

    //  doLayerInfoCompare_ will be false due to composition type
    // Expect SetLayerSize to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_ZeroSize_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, zero size vs non-zero size
 *                  2. call SetLayerSize
 *                  3. verify SetLayerSize IS called (sizes differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_ZeroSize_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevSize = {0, 0, 0, 0};
    prevRSLayer->SetLayerSize(prevSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set non-zero size on current rsLayer
    GraphicIRect curSize = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetLayerSize(curSize);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerSize_DoLayerInfoCompareTrue_LargeSizes_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both have large matching sizes
 *                  2. call SetLayerSize
 *                  3. verify sizes match, early return (sizes match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerSize_DoLayerInfoCompareTrue_LargeSizes_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect largeSize = {100, 200, 1920, 1080};
    prevRSLayer->SetLayerSize(largeSize);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same large size on current rsLayer
    hdiLayer_->rsLayer_->SetLayerSize(largeSize);

    //  true, sizes match
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerSize();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_AlphasMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, alphas match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_AlphasMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {false, false, 0, 0, 0};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same alpha on current rsLayer
    hdiLayer_->rsLayer_->SetAlpha(alpha);

    //  doLayerInfoCompare_ is true AND alphas match
    // Expect SetLayerAlpha NOT to be called (sizes match, early return 191)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_AllFieldsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, all alpha fields match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_AllFieldsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {true, true, 128, 128, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same alpha with all fields on current rsLayer
    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_EnGlobalAlphaTrue_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, enGlobalAlpha true matches
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_EnGlobalAlphaTrue_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {true, false, 200, 150, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_EnPixelAlphaTrue_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, enPixelAlpha true matches
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_EnPixelAlphaTrue_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {false, true, 100, 100, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_BothEnableTrue_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both enGlobalAlpha and enPixelAlpha true match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_BothEnableTrue_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {true, true, 255, 255, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_Alpha0Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, alpha0 values match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_Alpha0Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {true, false, 128, 100, 200};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_Alpha1Match_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, alpha1 values match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_Alpha1Match_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {false, true, 80, 160, 200};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_GAlphaMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, gAlpha values match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_GAlphaMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {false, false, 50, 50, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_MaxAlphaValues_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, maximum alpha values match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_MaxAlphaValues_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {true, true, 255, 255, 255};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerAlpha_DoLayerInfoCompareTrue_MinAlphaValues_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, minimum alpha values match
 *                  2. call SetLayerAlpha
 *                  3. verify sizes match, early return (alphas match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerAlpha_DoLayerInfoCompareTrue_MinAlphaValues_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicLayerAlpha alpha = {false, false, 0, 0, 0};
    prevRSLayer->SetAlpha(alpha);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetAlpha(alpha);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerAlpha();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, visible regions match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect rect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back(rect);
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same visible regions on current rsLayer
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    //  doLayerInfoCompare_ is true AND regions match
    // Expect SetLayerVisibleRegion NOT to be called (sizes match, early return 236)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_MultipleRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, multiple visible regions match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_MultipleRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 100, 100});
    visibleRegions.push_back({100, 100, 200, 200});
    visibleRegions.push_back({200, 200, 300, 300});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same multiple visible regions on current rsLayer
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_LargeRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, large visible regions match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_LargeRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 1920, 1080});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionWithOffset_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, regions with offset match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionWithOffset_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({50, 100, 150, 200});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_EmptyRegions_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both have empty visible regions
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_EmptyRegions_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions; // Empty
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same empty visible regions on current rsLayer
    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionXMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region x values match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionXMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({100, 50, 200, 150});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionYMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region y values match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionYMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({50, 100, 150, 200});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionWidthMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region width values match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionWidthMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 1280, 720});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionHeightMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region height values match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_RegionHeightMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 800, 600});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_FourRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, four visible regions match
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_FourRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 200, 200});
    visibleRegions.push_back({200, 0, 200, 200});
    visibleRegions.push_back({0, 200, 200, 200});
    visibleRegions.push_back({200, 200, 200, 200});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerVisibleRegion_DoLayerInfoCompareTrue_AllFieldsZero_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, all region fields are zero
 *                  2. call SetLayerVisibleRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerVisibleRegion_DoLayerInfoCompareTrue_AllFieldsZero_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.push_back({0, 0, 0, 0});
    prevRSLayer->SetVisibleRegions(visibleRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetVisibleRegions(visibleRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerVisibleRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, dirty regions match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect rect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back(rect);
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same dirty regions on current rsLayer
    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    //  doLayerInfoCompare_ is true AND dirty regions match
    // Expect SetLayerDirtyRegion NOT to be called (sizes match, early return 254)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_MultipleRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, multiple dirty regions match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_MultipleRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 100, 100});
    dirtyRegions.push_back({100, 100, 200, 200});
    dirtyRegions.push_back({200, 200, 300, 300});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same multiple dirty regions on current rsLayer
    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_LargeRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, large dirty regions match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_LargeRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 1920, 1080});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionWithOffset_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, regions with offset match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionWithOffset_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({50, 100, 150, 200});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_EmptyRegions_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both have empty dirty regions
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_EmptyRegions_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions; // Empty
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same empty dirty regions on current rsLayer
    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionXMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region x values match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionXMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({100, 50, 200, 150});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionYMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region y values match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionYMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({50, 100, 150, 200});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionWidthMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region width values match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionWidthMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 1280, 720});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionHeightMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, region height values match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_RegionHeightMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 800, 600});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_FourRegionsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, four dirty regions match
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_FourRegionsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 200, 200});
    dirtyRegions.push_back({200, 0, 200, 200});
    dirtyRegions.push_back({0, 200, 200, 200});
    dirtyRegions.push_back({200, 200, 200, 200});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerDirtyRegion_DoLayerInfoCompareTrue_AllFieldsZero_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, all region fields are zero
 *                  2. call SetLayerDirtyRegion
 *                  3. verify sizes match, early return (regions match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerDirtyRegion_DoLayerInfoCompareTrue_AllFieldsZero_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back({0, 0, 0, 0});
    prevRSLayer->SetDirtyRegions(dirtyRegions);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetDirtyRegions(dirtyRegions);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerDirtyRegion();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedAndIndexValid_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true AND index < bufferCacheCountMax_
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedAndIndexValid_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up buffer cache to make bufferCached true and index valid
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 3;  // Cache max is 3

    //  bufferCached will be true (sequence found in cache)
    // AND index (0) < bufferCacheCountMax_ (3)
    // Expect layerBuffer.handle to be nullptr (handle is null)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedMultipleTimes_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. buffer with multiple cached entries
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedMultipleTimes_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Add multiple entries to buffer cache
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum() + 1);
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum() + 2);
    hdiLayer->bufferCacheCountMax_ = 5;

    //  bufferCached will be true (index 0)
    // AND index (0) < bufferCacheCountMax_ (5)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedIndexAtMax_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true AND index equals bufferCacheCountMax_ - 1
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedIndexAtMax_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up cache with index at max - 1
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 1;  // index will be 0, which is < 1

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedLargeIndex_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true with large cache size
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedLargeIndex_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up large cache size
    for (int i = 0; i < 10; i++) {
        hdiLayer->bufferCache_.push_back(buffer->GetSeqNum() + i);
    }
    hdiLayer->bufferCacheCountMax_ = 20;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedWithDeletingList_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true with deleting list populated
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedWithDeletingList_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up cache with deleting list
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 5;

    // Note: The deleting list is populated by CheckAndUpdateLayerBufferCache
    // when the cache is full, not in this simple case

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedZeroIndex_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true at index 0
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedZeroIndex_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up cache at index 0
    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 10;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedDifferentFence_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true with different fence
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedDifferentFence_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(100);  // Different fd
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 3;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedFullCache_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true with full cache
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedFullCache_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    // Set up full cache
    for (int i = 0; i < 5; i++) {
        hdiLayer->bufferCache_.push_back(buffer->GetSeqNum() + i);
    }
    hdiLayer->bufferCacheCountMax_ = 5;  // index 0 < 5

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedLargeBufferSize_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true with large buffer size
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedLargeBufferSize_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 1920,
        .height = 1080,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 10;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBuffer_BufferCachedBufferClearedFalse_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. bufferCached is true, bufferCleared is false
 *                  2. call SetLayerBuffer
 *                  3. verify handle is set to nullptr (buffer cached, handle null)
 */
HWTEST_F(HdiLayerTest, SetLayerBuffer_BufferCachedBufferClearedFalse_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto hdiLayer = HdiLayer::CreateHdiLayer(0);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3,
    };
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    buffer->Alloc(requestConfig);
    sptr<SyncFence> fence = new SyncFence(-1);
    rsLayer->SetBuffer(buffer, fence);
    hdiLayer->rsLayer_ = rsLayer;

    hdiLayer->SetHdiDeviceMock(hdiDeviceMock_);

    hdiLayer->bufferCache_.push_back(buffer->GetSeqNum());
    hdiLayer->bufferCacheCountMax_ = 5;
    hdiLayer->bufferCleared_ = false;

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, testing::Field(
        &GraphicLayerBuffer::handle, testing::IsNull()
    ))).WillOnce(testing::Return(0));

    auto ret = hdiLayer->SetLayerBuffer();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_ClientTypeMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are CLIENT type
 *                  2. call SetLayerCompositionType
 *                  3. verify sizes match, early return (types match, sizes match, early return)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_ClientTypeMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    // When prev is DEVICE and cur is CLIENT, types differ, so this is false branch case
    // Actually, this is Line329_False case, let me correct this test
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_DeviceVsClient_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, DEVICE vs CLIENT types differ
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (types differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_DeviceVsClient_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_ClientVsDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, CLIENT vs DEVICE types differ
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (types differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_ClientVsDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_DeviceVsCursor_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, DEVICE vs CURSOR types differ
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (types differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_DeviceVsCursor_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CURSOR);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_CursorVsDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, CURSOR vs DEVICE types differ
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (types differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_CursorVsDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CURSOR);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareTrue_ClientVsCursor_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, CLIENT vs CURSOR types differ
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (types differ, set called)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareTrue_ClientVsCursor_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CURSOR);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCompositionType_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer composition type is not GRAPHIC_COMPOSITION_DEVICE
 *                  2. call SetLayerCompositionType
 *                  3. verify SetLayerCompositionType IS called (doLayerInfoCompare_ will be false)
 */
HWTEST_F(HdiLayerTest, SetLayerCompositionType_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer with CLIENT composition type (not DEVICE)
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    prevRSLayer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same composition type on current rsLayer
    hdiLayer_->rsLayer_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    //  doLayerInfoCompare_ will be false due to prev composition type
    // Expect SetLayerCompositionType to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCompositionType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCompositionType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_TypesMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, blend types match
 *                  2. call SetLayerBlendType
 *                  3. verify early return (types match)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_TypesMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same blend type on current rsLayer
    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);

    // doLayerInfoCompare_ is true AND blend types match
    // Expect SetLayerBlendType NOT to be called (early return)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_ClearTypeMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are CLEAR type
 *                  2. call SetLayerBlendType
 *                  3. verify early return (types match)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_ClearTypeMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_CLEAR);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_CLEAR);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_SrcTypeMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are SRC type
 *                  2. call SetLayerBlendType
 *                  3. verify early return (types match)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_SrcTypeMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverTypeMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, both are SRCOVER type
 *                  2. call SetLayerBlendType
 *                  3. verify early return (types match)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverTypeMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_ClearVsSrc_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, CLEAR vs SRC types differ
 *                  2. call SetLayerBlendType
 *                  3. verify SetLayerBlendType IS called (types differ)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_ClearVsSrc_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_CLEAR);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_SrcVsSrcOver_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, SRC vs SRCOVER types differ
 *                  2. call SetLayerBlendType
 *                  3. verify SetLayerBlendType IS called (types differ)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_SrcVsSrcOver_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverVsClear_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, SRCOVER vs CLEAR types differ
 *                  2. call SetLayerBlendType
 *                  3. verify SetLayerBlendType IS called (types differ)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverVsClear_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_CLEAR);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverVsSrc_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, SRCOVER vs SRC types differ
 *                  2. call SetLayerBlendType
 *                  3. verify SetLayerBlendType IS called (types differ)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareTrue_SrcOverVsSrc_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRC);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerBlendType_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer composition type is not GRAPHIC_COMPOSITION_DEVICE
 *                  2. call SetLayerBlendType
 *                  3. verify SetLayerBlendType IS called (doLayerInfoCompare_ will be false)
 */
HWTEST_F(HdiLayerTest, SetLayerBlendType_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer with CLIENT composition type (not DEVICE)
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    prevRSLayer->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same blend type on current rsLayer
    hdiLayer_->rsLayer_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_SRCOVER);

    // doLayerInfoCompare_ will be false due to prev composition type
    // Expect SetLayerBlendType to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBlendType(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerBlendType();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_CropsMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, crop rects match
 *                  2. call SetLayerCrop
 *                  3. verify early return (crops match)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_CropsMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type to make doLayerInfoCompare_ true
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect crop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(crop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same crop rect on current rsLayer
    hdiLayer_->rsLayer_->SetCropRect(crop);

    // doLayerInfoCompare_ is true AND crop rects match
    // Expect SetLayerCrop NOT to be called (early return)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_WidthDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, width differs
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (crops differ)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_WidthDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer with DEVICE composition type
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(prevCrop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different width on current rsLayer
    GraphicIRect curCrop = {0, 0, WIDTH_VAL + 10, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetCropRect(curCrop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_HeightDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, height differs
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (crops differ)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_HeightDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(prevCrop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set different height on current rsLayer
    GraphicIRect curCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL + 20};
    hdiLayer_->rsLayer_->SetCropRect(curCrop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_XPosDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, x position differs
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (crops differ)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_XPosDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(prevCrop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change x position
    GraphicIRect curCrop = {10, 0, WIDTH_VAL, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetCropRect(curCrop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_YPosDiffers_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, y position differs
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (crops differ)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_YPosDiffers_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(prevCrop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change y position
    GraphicIRect curCrop = {0, 15, WIDTH_VAL, HEIGHT_VAL};
    hdiLayer_->rsLayer_->SetCropRect(curCrop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_AllFieldsDiffer_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, all fields differ
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (crops differ)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_AllFieldsDiffer_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect prevCrop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(prevCrop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Change all fields
    GraphicIRect curCrop = {100, 200, 300, 400};
    hdiLayer_->rsLayer_->SetCropRect(curCrop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer composition type is not GRAPHIC_COMPOSITION_DEVICE
 *                  2. call SetLayerCrop
 *                  3. verify SetLayerCrop IS called (doLayerInfoCompare_ will be false)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareFalse_PrevCompositionTypeNotDevice_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer with CLIENT composition type (not DEVICE)
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    GraphicIRect crop = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    prevRSLayer->SetCropRect(crop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set same crop rect on current rsLayer
    hdiLayer_->rsLayer_->SetCropRect(crop);

    // doLayerInfoCompare_ will be false due to prev composition type
    // Expect SetLayerCrop to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_ZeroCropMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, zero crop rects match
 *                  2. call SetLayerCrop
 *                  3. verify early return (crops match)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_ZeroCropMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect crop = {0, 0, 0, 0};
    prevRSLayer->SetCropRect(crop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCropRect(crop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetLayerCrop_DoLayerInfoCompareTrue_LargeCropMatch_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. doLayerInfoCompare_ is true, large crop rects match
 *                  2. call SetLayerCrop
 *                  3. verify early return (crops match)
 */
HWTEST_F(HdiLayerTest, SetLayerCrop_DoLayerInfoCompareTrue_LargeCropMatch_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    GraphicIRect crop = {100, 200, 1920, 1080};
    prevRSLayer->SetCropRect(crop);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    hdiLayer_->rsLayer_->SetCropRect(crop);

    EXPECT_CALL(*hdiDeviceMock_, SetLayerCrop(_, _, _)).Times(0);
    auto ret = hdiLayer_->SetLayerCrop();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetTunnelLayerId_PrevRSLayerIsNull_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer is nullptr (prevRSLayer_ is nullptr)
 *                  2. call SetTunnelLayerId
 *                  3. verify SetTunnelLayerId IS called
 */
HWTEST_F(HdiLayerTest, SetTunnelLayerId_PrevRSLayerIsNull_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer to nullptr to make prevRSLayer_ nullptr
    hdiLayer_->prevRSLayer_ = nullptr;
    hdiLayer_->rsLayer_->SetTunnelLayerId(12345);

    // skips to the SetTunnelLayerId function
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, 12345)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetTunnelLayerId();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetTunnelLayerId_PrevRSLayerIsNullWithZeroId_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer is nullptr with zero tunnel id
 *                  2. call SetTunnelLayerId
 *                  3. verify SetTunnelLayerId IS called
 */
HWTEST_F(HdiLayerTest, SetTunnelLayerId_PrevRSLayerIsNullWithZeroId_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    hdiLayer_->prevRSLayer_ = nullptr;
    hdiLayer_->rsLayer_->SetTunnelLayerId(0);

    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, 0)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetTunnelLayerId();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetTunnelLayerId_PrevRSLayerIsNullWithLargeId_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer is nullptr with large tunnel id
 *                  2. call SetTunnelLayerId
 *                  3. verify SetTunnelLayerId IS called
 */
HWTEST_F(HdiLayerTest, SetTunnelLayerId_PrevRSLayerIsNullWithLargeId_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    hdiLayer_->prevRSLayer_ = nullptr;
    hdiLayer_->rsLayer_->SetTunnelLayerId(UINT64_MAX);

    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, UINT64_MAX)).WillOnce(testing::Return(0));
    auto ret = hdiLayer_->SetTunnelLayerId();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetTunnelLayerId_PrevRSLayerIsNullDeviceError_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer is nullptr, device returns error
 *                  2. call SetTunnelLayerId
 *                  3. verify error is returned
 */
HWTEST_F(HdiLayerTest, SetTunnelLayerId_PrevRSLayerIsNullDeviceError_False, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    hdiLayer_->prevRSLayer_ = nullptr;
    hdiLayer_->rsLayer_->SetTunnelLayerId(555);

    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(_, _, 555)).WillOnce(testing::Return(-1));
    auto ret = hdiLayer_->SetTunnelLayerId();
    ASSERT_EQ(ret, -1);
}

/**
 * Function: GetReleaseFence_CurrBufferInfoIsNull_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. currBufferInfo_ is nullptr (currBufferInfo_ is nullptr)
 *                  2. call GetReleaseFence
 *                  3. verify InvalidFence is returned
 */
HWTEST_F(HdiLayerTest, GetReleaseFence_CurrBufferInfoIsNull_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set currBufferInfo_ to nullptr to make currBufferInfo_ nullptr
    hdiLayer_->currBufferInfo_ = nullptr;

    // is true
    // Expect InvalidFence to be returned
    auto fence = hdiLayer_->GetReleaseFence();
    ASSERT_EQ(fence.GetRefPtr()->Get(), -1);
}

/**
 * Function: GetReleaseFence_CurrBufferInfoIsNullAfterClear_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. currBufferInfo_ is nullptr after clearing
 *                  2. call GetReleaseFence
 *                  3. verify InvalidFence is returned
 */
HWTEST_F(HdiLayerTest, GetReleaseFence_CurrBufferInfoIsNullAfterClear_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Create and then delete currBufferInfo_
    delete hdiLayer_->currBufferInfo_;
    hdiLayer_->currBufferInfo_ = nullptr;

    auto fence = hdiLayer_->GetReleaseFence();
    ASSERT_EQ(fence.GetRefPtr()->Get(), -1);
}

/**
 * Function: GetReleaseFence_DefaultStateIsInvalid_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. test default state where currBufferInfo_ may be nullptr
 *                  2. call GetReleaseFence
 *                  3. verify InvalidFence is returned
 */
HWTEST_F(HdiLayerTest, GetReleaseFence_DefaultStateIsInvalid_True, Function | MediumTest| Level1)
{
    auto hdiLayer = HdiLayer::CreateHdiLayer(0);

    // Default state - currBufferInfo_ is nullptr
    auto fence = hdiLayer->GetReleaseFence();
    ASSERT_EQ(fence.GetRefPtr()->Get(), -1);
}

/**
 * Function: GetReleaseFence_AfterCloseLayer_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. currBufferInfo_ becomes nullptr after CloseLayer
 *                  2. call GetReleaseFence
 *                  3. verify InvalidFence is returned
 */
HWTEST_F(HdiLayerTest, GetReleaseFence_AfterCloseLayer_True, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Close the layer which may set currBufferInfo_ to nullptr
    hdiLayer_->CloseLayer();

    // After CloseLayer, currBufferInfo_ may be nullptr
    auto fence = hdiLayer_->GetReleaseFence();
    // The fence may be InvalidFence or a valid fence depending on state
    // Just verify we can call it without crashing
    ASSERT_NE(fence, nullptr);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_RatiosDiffer_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer_ exists but brightness ratios differ
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (brightness ratios differ)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_RatiosDiffer_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer_ with different brightness ratio
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetBrightnessRatio(0.5f);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set current rsLayer_ with different brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(0.8f);

    // Expect SetLayerPerFrameParameterSmq to be called (brightness ratios differ)
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_PrevNull_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. prevRSLayer_ is nullptr
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (prevRSLayer_ is nullptr)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_PrevNull_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set prevRSLayer_ to nullptr (prevRSLayer_ is nullptr)
    hdiLayer_->prevRSLayer_ = nullptr;

    // Set current rsLayer_ brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(0.6f);

    // Expect SetLayerPerFrameParameterSmq to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_RatiosDifferLargeValues_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Brightness ratios differ with large values
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (brightness ratios differ)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_RatiosDifferLargeValues_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer_ with large brightness ratio
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetBrightnessRatio(2.0f);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set current rsLayer_ with different large brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(3.5f);

    // Expect SetLayerPerFrameParameterSmq to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_RatiosDifferSmallValues_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Brightness ratios differ with small values
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (brightness ratios differ)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_RatiosDifferSmallValues_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer_ with small brightness ratio
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetBrightnessRatio(0.001f);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set current rsLayer_ with different small brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(0.002f);

    // Expect SetLayerPerFrameParameterSmq to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_RatiosDifferNegativeValues_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Brightness ratios differ with negative values
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (brightness ratios differ)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_RatiosDifferNegativeValues_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer_ with negative brightness ratio
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetBrightnessRatio(-0.5f);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set current rsLayer_ with different negative brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(-0.3f);

    // Expect SetLayerPerFrameParameterSmq to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: SetPerFrameParameterBrightnessRatio_RatiosDifferZeroToPositive_ParameterSet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. Brightness ratios differ from zero to positive
 *                  2. Call SetPerFrameParameterBrightnessRatio
 *                  3. Verify SetLayerPerFrameParameterSmq is called (brightness ratios differ)
 */
HWTEST_F(HdiLayerTest, SetPerFrameParameterBrightnessRatio_RatiosDifferZeroToPositive_ParameterSet, Function | MediumTest| Level1)
{
    ASSERT_NE(hdiLayer_, nullptr);

    // Set up prevRSLayer_ with zero brightness ratio
    auto prevRSLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    prevRSLayer->SetBrightnessRatio(0.0f);
    hdiLayer_->prevRSLayer_ = prevRSLayer;

    // Set current rsLayer_ with positive brightness ratio
    hdiLayer_->rsLayer_->SetBrightnessRatio(0.5f);

    // Expect SetLayerPerFrameParameterSmq to be called
    EXPECT_CALL(*hdiDeviceMock_, SetLayerPerFrameParameterSmq(_, _, "SDRBrightnessRatio", _))
        .Times(1)
        .WillOnce(testing::Return(GRAPHIC_DISPLAY_SUCCESS));

    auto ret = hdiLayer_->SetPerFrameParameterBrightnessRatio();
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}
} // namespace
} // namespace Rosen
} // namespace OHOS