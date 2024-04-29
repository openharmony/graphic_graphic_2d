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
#include "mock_hdi_device.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HdiOutputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void HdiOutputTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
}

void HdiOutputTest::TearDownTestCase() {}

namespace {

/*
* Function: GetFrameBufferSurface001
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
* Function: GetFramebuffer001
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
* Function: GetScreenId001
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
* Function: Commit001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Commit()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, Commit001, Function | MediumTest| Level1)
{
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), ROSEN_ERROR_NOT_INIT);
}

/*
* Function: ClearFrameBuffer001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call ClearFrameBuffer()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, ClearFrameBuffer001, Function | MediumTest | Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ClearFrameBuffer(), GSERROR_OK);
}

/*
* Function: Init001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Init()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, Init001, Function | MediumTest| Level1)
{
    ASSERT_EQ(hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_), ROSEN_ERROR_OK);
    // device_ is already set
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    // fbSurface_ already initialized
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/*
* Function: GetFrameBufferSurface002
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
* Function: GetFramebuffer002
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
* Function: GetOutputDamage001
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
* Function: GetDirectClientCompEnableStatus001
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
* Function: GetDirectClientCompEnableStatus002
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

/*
* Function: Commit002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call Commit()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, Commit002, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, Commit(_, _)).WillRepeatedly(testing::Return(0));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: ClearFrameBuffer002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call ClearFrameBuffer()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, ClearFrameBuffer002, Function | MediumTest | Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ClearFrameBuffer(), GSERROR_INVALID_OPERATING);
}

/*
* Function: CommitAndGetReleaseFence001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call CommitAndGetReleaseFence()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, CommitAndGetReleaseFence001, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _)).WillRepeatedly(testing::Return(0));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    int32_t skipState = 0;
    bool needFlush = false;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CommitAndGetReleaseFence(fbFence, skipState, needFlush),
        GRAPHIC_DISPLAY_SUCCESS);
}
} // namespace
} // namespace Rosen
} // namespace OHOS