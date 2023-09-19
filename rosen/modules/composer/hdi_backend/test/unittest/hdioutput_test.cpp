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
    hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
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
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), ROSEN_ERROR_OK);
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
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: CreateLayer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call CreateLayer
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, CreateLayer, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  CreateLayer start";
    uint64_t surfaceId = 0;
    uint32_t screenId_ = HdiOutputTest::hdiOutput_->GetScreenId();
    const LayerInfoPtr layerInfo;
    LayerPtr layer = HdiLayer::CreateHdiLayer(screenId_);
    bool ret = layer->Init(layerInfo);
    ASSERT_FALSE(ret);
    auto res = HdiOutputTest::hdiOutput_->CreateLayer(surfaceId, layerInfo);
    ASSERT_EQ(res, GRAPHIC_DISPLAY_FAILURE);
    GTEST_LOG_(INFO) << "HdiOutputTest  CreateLayer end";
}

/*
* Function: PreProcessLayersComp
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call PreProcessLayersComp
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, PreProcessLayersComp, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  PreProcessLayersComp start";
    bool needFlush;
    int32_t ret = 0;
    std::unordered_map<uint32_t, LayerPtr> layerIdMap_ = HdiOutputTest::hdiOutput_->GetLayers();
    for (auto iter = layerIdMap_.begin(); iter != layerIdMap_.end(); ++iter) {
        const LayerPtr& layer = iter->second;
        ret = layer->SetHdiLayerInfo();
        ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
        auto res = HdiOutputTest::hdiOutput_->PreProcessLayersComp(needFlush);
        ASSERT_EQ(res, 0);
    }
    GTEST_LOG_(INFO) << "HdiOutputTest  PreProcessLayersComp end";
}

/*
* Function: CheckAndUpdateClientBufferCahce001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call CheckAndUpdateClientBufferCahce
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce001, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  CheckAndUpdateClientBufferCahce001 start";
    sptr<SurfaceBuffer> buffer;
    uint32_t index = 0;
    uint32_t bufferCacheCountMax_ = 100;
    std::vector<sptr<SurfaceBuffer>> bufferCache_;
    for (uint32_t i = 0; i < bufferCacheCountMax_; i++) {
        auto ret = bufferCache_[i];
        ASSERT_EQ(ret, buffer);
        index = i;
        auto res = HdiOutputTest::hdiOutput_->CheckAndUpdateClientBufferCahce(buffer, index);
        ASSERT_TRUE(res);
    }
    GTEST_LOG_(INFO) << "HdiOutputTest  CheckAndUpdateClientBufferCahce001 end";
}

/*
* Function: CheckAndUpdateClientBufferCahce002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call CheckAndUpdateClientBufferCahce
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce002, Function | MediumTest | Level3)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  CheckAndUpdateClientBufferCahce002 start";
    sptr<SurfaceBuffer> buffer;
    uint32_t index = 0;
    uint32_t bufferCacheCountMax_ = 100;
    uint32_t bufferCacheIndex_ = 200;
    bool ret = (bufferCacheIndex_ >= bufferCacheCountMax_);
    ASSERT_TRUE(ret);
    auto result = HdiOutputTest::hdiOutput_->CheckAndUpdateClientBufferCahce(buffer, index);
    ASSERT_FALSE(result);
    GTEST_LOG_(INFO) << "HdiOutputTest  CheckAndUpdateClientBufferCahce002 end";
}

/*
* Function: FlushScreen
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call FlushScreen()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, FlushScreen, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  FlushScreen start";
    std::vector<LayerPtr> compClientLayers;
    sptr<HdiFramebufferSurface> fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    auto fbEntry = HdiOutputTest::hdiOutput_->GetFramebuffer();
    sptr<SurfaceBuffer> currFrameBuffer_ = nullptr;
    ASSERT_EQ(currFrameBuffer_, nullptr);
    auto res = HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers);
    ASSERT_EQ(res, -1);
    GTEST_LOG_(INFO) << "HdiOutputTest  FlushScreen end";
}

/*
* Function: UpdateInfosAfterCommit
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call UpdateInfosAfterCommit()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, UpdateInfosAfterCommit, Function | MediumTest | Level1)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  UpdateInfosAfterCommit start";
    sptr<SyncFence> fbFence;
    bool startSample = false;
    sptr<VSyncSampler> sampler_ = CreateVSyncSampler();
    bool enabled = false;
    sampler_->SetHardwareVSyncStatus(enabled);
    bool alreadyStartSample = sampler_->GetHardwareVSyncStatus();
    bool res = (startSample && !alreadyStartSample);
    ASSERT_FALSE(res);
    ASSERT_NE(HdiOutputTest::hdiOutput_->UpdateInfosAfterCommit(fbFence), -1);
    GTEST_LOG_(INFO) << "HdiOutputTest  UpdateInfosAfterCommit end";
}

/*
* Function: ReleaseFramebuffer
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. call ReleaseFramebuffer()
*                  2. check ret
*/
HWTEST_F(HdiOutputTest, ReleaseFramebuffer, Function | MediumTest| Level1)
{
    GTEST_LOG_(INFO) << "HdiOutputTest  ReleaseFramebuffer start";
    sptr<SyncFence> releaseFence;
    sptr<SurfaceBuffer> lastFrameBuffer_ = SurfaceBuffer::Create();
    bool res = (lastFrameBuffer_ != nullptr);
    ASSERT_EQ(res, true);
    int32_t ret = HdiOutputTest::hdiOutput_->ReleaseFramebuffer(releaseFence);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_NULL_PTR);
    GTEST_LOG_(INFO) << "HdiOutputTest  ReleaseFramebuffer end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS