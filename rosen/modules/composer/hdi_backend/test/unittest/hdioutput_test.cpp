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

#include "hdi_output.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiOutputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiOutput> hdiOutput_;
};

void HdiOutputTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
}

void HdiOutputTest::TearDownTestCase() {}

namespace {

/*
* Function: GetFrameBufferSurface
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetFrameBufferSurface()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, GetFrameBufferSurface001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
}

/*
* Function: GetFramebuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, GetFramebuffer001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/*
* Function: ReleaseFramebuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, ReleaseFramebuffer001, Function | MediumTest| Level1)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ReleaseFramebuffer(buffer, releaseFence), -1);
}

/*
* Function: GetScreenId
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call GetScreenId()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, GetScreenId001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetScreenId(), 0u);
}

/*
* Function: Init
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Init()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, Init001, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    // fbSurface_ already initialized
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/*
* Function: ReleaseFramebuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, ReleaseFramebuffer002, Function | MediumTest| Level1)
{
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> releaseFence;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ReleaseFramebuffer(buffer, releaseFence), 0);
}

/*
* Function: Init
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetOutputDamages()
*                  2. call GetOutputDamages()
*                  3. check ret
*/
HWTEST_F(HdiOutputTest, GetOutputDamage001, Function | MediumTest| Level3)
{
    GraphicIRect iRect = {
        .x = 0,
        .y = 0,
        .w = 800,
        .h = 600,
    };
    std::vector<GraphicIRect> inDamages;
    inDamages.emplace_back(iRect);
    HdiOutputTest::hdiOutput_->SetOutputDamages(inDamages);
    const std::vector<GraphicIRect>& outDamages = HdiOutputTest::hdiOutput_->GetOutputDamages();
    ASSERT_EQ(outDamages.size(), 1);
    ASSERT_EQ(outDamages[0].x, iRect.x);
    ASSERT_EQ(outDamages[0].y, iRect.y);
    ASSERT_EQ(outDamages[0].w, iRect.w);
    ASSERT_EQ(outDamages[0].h, iRect.h);
}

/*
* Function: GetLayerCompCapacity
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call SetLayerCompCapacity
*                  2. call GetLayerCompCapacity
*                  3. check ret
*/
HWTEST_F(HdiOutputTest, GetLayerCompCapacity001, Function | MediumTest| Level3)
{
    uint32_t layerCompositionCapacity = 8;
    HdiOutputTest::hdiOutput_->SetLayerCompCapacity(layerCompositionCapacity);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetLayerCompCapacity(), 8u);
}

/*
* Function: GetFrameBufferSurface
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetFrameBufferSurface
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, GetFrameBufferSurface002, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiOutputTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
}

/*
* Function: GetFramebuffer
* Type: Function
* Rank: Important(3)
* EnvConditions: N/A
* CaseDescription: 1. call GetFramebuffer
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, GetFramebuffer002, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/*
* Function: GetDirectClientCompEnableStatus
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetDirectClientCompEnableStatus
*                  2. call GetDirectClientCompEnableStatus
*                  3. check ret
 */
HWTEST_F(HdiOutputTest, GetDirectClientCompEnableStatus001, Function | MediumTest| Level1)
{
    bool enablStatus = false;
    HdiOutputTest::hdiOutput_->SetDirectClientCompEnableStatus(enablStatus);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetDirectClientCompEnableStatus(), false);
}

/*
* Function: GetDirectClientCompEnableStatus
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call SetDirectClientCompEnableStatus
*                  2. call GetDirectClientCompEnableStatus
*                  3. check ret
 */
HWTEST_F(HdiOutputTest, GetDirectClientCompEnableStatus002, Function | MediumTest| Level1)
{
    bool enablStatus = true;
    HdiOutputTest::hdiOutput_->SetDirectClientCompEnableStatus(enablStatus);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetDirectClientCompEnableStatus(), true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS