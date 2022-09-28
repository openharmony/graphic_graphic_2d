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
};

void HdiLayerTest::SetUpTestCase()
{
    hdiLayer_ = HdiLayer::CreateHdiLayer(0);
    LayerInfoPtr layerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<Surface> cSurface = Surface::CreateSurfaceAsConsumer();
    layerInfo_->SetSurface(cSurface);
    sptr<IBufferProducer> producer = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    IRect srcRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    IRect dstRect = {0, 0, WIDTH_VAL, HEIGHT_VAL};
    layerInfo_->SetLayerSize(dstRect);
    layerInfo_->SetDirtyRegion(srcRect);
    layerInfo_->SetCropRect(srcRect);
    layerInfo_->SetVisibleRegion(1, srcRect);
    LayerAlpha layerAlpha = {false, false, 0, 0, 0};
    layerInfo_->SetAlpha(layerAlpha);
    layerInfo_->SetCompositionType(CompositionType::COMPOSITION_DEVICE);
    layerInfo_->SetBlendType(BlendType::BLEND_NONE);
    hdiLayer_->UpdateLayerInfo(layerInfo_);
}

void HdiLayerTest::TearDownTestCase() {}

namespace {
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
    LayerInfoPtr layerInfoNull = nullptr;
    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(layerInfoNull), false);

    ASSERT_EQ(HdiLayerTest::hdiLayer_->Init(HdiLayerTest::layerInfo_), false);
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
}

} // namespace
} // namespace Rosen
} // namespace OHOS