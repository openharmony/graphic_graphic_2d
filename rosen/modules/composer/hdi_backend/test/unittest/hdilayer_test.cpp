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
#include <gtest/gtest.h>
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
    static inline LayerInfoPtr layerInfo_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline std::vector<std::string> paramKey_{};
};

void HdiLayerTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    layerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create();
    layerInfo_->SetSurface(cSurface);
    GraphicIRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    GraphicIRect dstRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    layerInfo_->SetLayerSize(dstRect);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.emplace_back(srcRect);
    layerInfo_->SetDirtyRegions(dirtyRegions);
    layerInfo_->SetCropRect(srcRect);
    std::vector<GraphicIRect> visibleRegions;
    visibleRegions.emplace_back(srcRect);
    layerInfo_->SetVisibleRegions(visibleRegions);
    GraphicLayerAlpha layerAlpha = {false, false, 0, 0, 0};
    layerInfo_->SetAlpha(layerAlpha);
    layerInfo_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layerInfo_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_NONE);
    layerInfo_->SetLayerMaskInfo(HdiLayerInfo::LayerMask::LAYER_MASK_NORMAL);
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    sptr<SyncFence> fence = new SyncFence(1);
    layerInfo_->SetBuffer(buffer, fence);

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
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::layerInfo_), false);
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::layerInfo_), true);
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
    HdiLayerTest::hdiLayer_->UpdateLayerInfo(layerInfo_);
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
    bool doLayerCompare = true;
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(doLayerCompare), GRAPHIC_DISPLAY_SUCCESS);
    hdiLayer_->SavePrevLayerInfo();
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(doLayerCompare), GRAPHIC_DISPLAY_SUCCESS);
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
    HdiLayerTest::layerInfo_->SetTunnelHandleChange(true);
    HdiLayerTest::layerInfo_->SetTunnelHandle(nullptr);
    HdiLayerTest::hdiLayer_->UpdateLayerInfo(HdiLayerTest::layerInfo_);
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _)).WillRepeatedly(testing::Return(0));
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetLayerTunnelHandle(), GRAPHIC_DISPLAY_SUCCESS);

    HdiLayerTest::layerInfo_->SetTunnelHandle(new SurfaceTunnelHandle());
    HdiLayerTest::hdiLayer_->UpdateLayerInfo(HdiLayerTest::layerInfo_);
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
    HdiLayerTest::hdiLayer_->prevLayerInfo_ = nullptr;
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
 
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerId(devId, layerId, tunnelId)).WillOnce(Return(0));
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
 
    EXPECT_CALL(*hdiDeviceMock_, SetTunnelLayerProperty(devId, layerId, property)).WillOnce(Return(0));
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
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::layerInfo_), true);
    auto tmpSurface = HdiLayerTest::hdiLayer_->GetLayerInfo()->GetSurface();
    HdiLayerTest::layerInfo_->SetSurface(nullptr);
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _, layerId)).WillRepeatedly(testing::Return(0));
    HdiLayerTest::layerInfo_->SetSurface(tmpSurface);
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
    layerInfo_->SetTunnelLayerId(0);
    layerInfo_->SetTunnelLayerProperty(0);
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
    layerInfo_->SetTunnelLayerId(1);
    layerInfo_->SetTunnelLayerProperty(0);
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
    layerInfo_->SetTunnelLayerId(0);
    layerInfo_->SetTunnelLayerProperty(1);
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
    layerInfo_->SetTunnelLayerId(1);
    layerInfo_->SetTunnelLayerProperty(1);
    int32_t result = HdiLayerTest::hdiLayer_->SetPerFrameParameters();
    EXPECT_EQ(result, GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}
} // namespace
} // namespace Rosen
} // namespace OHOS