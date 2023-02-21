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
    static inline Mock::HdiDevice* hdiDeviceMock_;
};

void HdiLayerTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    layerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<Surface> cSurface = Surface::CreateSurfaceAsConsumer();
    layerInfo_->SetSurface(cSurface);
    IRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    IRect dstRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    layerInfo_->SetLayerSize(dstRect);
    layerInfo_->SetDirtyRegion(srcRect);
    layerInfo_->SetCropRect(srcRect);
    layerInfo_->SetVisibleRegion(1, srcRect);
    GraphicLayerAlpha layerAlpha = {false, false, 0, 0, 0};
    layerInfo_->SetAlpha(layerAlpha);
    layerInfo_->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layerInfo_->SetBlendType(GraphicBlendType::GRAPHIC_BLEND_NONE);

    hdiDeviceMock_ = Mock::HdiDevice::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, SetLayerAlpha(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerSize(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetTransformMode(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerVisibleRegion(_, _, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerDirtyRegion(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, SetLayerBuffer(_, _, _, _)).WillRepeatedly(testing::Return(0));
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
    EXPECT_CALL(*hdiDeviceMock_, SetLayerTunnelHandle(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedPresentTimestampType(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetPresentTimestamp(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, CreateLayer(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, CloseLayer(_, _)).WillRepeatedly(testing::Return(0));
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
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(nullptr), DISPLAY_NULL_PTR);
    // Set hdiLayer_->device_ to hdiDeviceMock_
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(hdiDeviceMock_), DISPLAY_SUCCESS);
    // Reset hdiLayer_->device_ to hdiDeviceMock_
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiDeviceMock(hdiDeviceMock_), DISPLAY_SUCCESS);
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
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::layerInfo_), true);
}

/*
* Function: SetHdiLayerInfo001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetHdiLayerInfo()
*                  2. check ret
*/
HWTEST_F(HdiLayerTest, SetHdiLayerInfo001, Function | MediumTest| Level1)
{
    hdiLayer_->UpdateLayerInfo(layerInfo_);
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(), DISPLAY_SUCCESS);

    hdiLayer_->SavePrevLayerInfo();
    ASSERT_EQ(HdiLayerTest::hdiLayer_->SetHdiLayerInfo(), DISPLAY_SUCCESS);
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

    sptr<SyncFence> fbAcquireFence = new SyncFence(-1);
    hdiLayer_->MergeWithFramebufferFence(fbAcquireFence);
    hdiLayer_->UpdateCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    std::string dumpStr = "";
    hdiLayer_->Dump(dumpStr);
}

} // namespace
} // namespace Rosen
} // namespace OHOS