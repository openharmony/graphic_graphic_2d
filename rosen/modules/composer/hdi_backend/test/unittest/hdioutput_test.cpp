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
#include "surface_buffer_impl.h"

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
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ClearFrameBuffer(), GSERROR_CONSUMER_DISCONNECTED);
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
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(testing::Return(0));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    int32_t skipState = 0;
    bool needFlush = false;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CommitAndGetReleaseFence(fbFence, skipState, needFlush, false),
        GRAPHIC_DISPLAY_SUCCESS);
}

/*
* Function: CheckAndUpdateClientBufferCahce001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce001, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index;
    auto &hdiOutput = HdiOutputTest::hdiOutput_;
    hdiOutput->bufferCache_.push_back(buffer);
    bool result = hdiOutput->CheckAndUpdateClientBufferCahce(buffer, index);

    ASSERT_TRUE(result);
    ASSERT_EQ(index, 0);
}

/*
* Function: CheckAndUpdateClientBufferCahce002
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce002, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index;
    auto &hdiOutput = HdiOutputTest::hdiOutput_;
    bool result = hdiOutput->CheckAndUpdateClientBufferCahce(buffer, index);
    ASSERT_FALSE(result);
    ASSERT_EQ(index, 0);
    ASSERT_EQ(hdiOutput->bufferCache_.size(), 1);
    ASSERT_EQ(hdiOutput->bufferCache_[0], buffer);
}

/*
* Function: CheckAndUpdateClientBufferCahce003
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce003, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index;
    auto &hdiOutput = HdiOutputTest::hdiOutput_;
    hdiOutput->bufferCacheCountMax_ = 1;
    hdiOutput->bufferCache_.push_back(new SurfaceBufferImpl());
    bool result = hdiOutput->CheckAndUpdateClientBufferCahce(buffer, index);
    ASSERT_FALSE(result);
    ASSERT_EQ(index, 0);
    ASSERT_EQ(hdiOutput->bufferCache_.size(), 1);
    ASSERT_EQ(hdiOutput->bufferCache_[0], buffer);
}

/*
* Function: ReleaseLayers
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call ReleaseLayers()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, ReleaseLayers, Function | MediumTest | Level1)
{
    auto &map = HdiOutputTest::hdiOutput_->layerIdMap_;
    for (auto &layer : map) {
        layer.second->GetLayerInfo()->SetIsSupportedPresentTimestamp(true);
        EXPECT_EQ(layer.second->GetLayerInfo()->IsSupportedPresentTimestamp(), true);
    }
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    HdiOutputTest::hdiOutput_->ReleaseLayers(fbFence);
}

/*
* Function: DumpHitchs
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call DumpHitchs()
*                  2.check ret
*/
HWTEST_F(HdiOutputTest, DumpHitchs, Function | MediumTest | Level1)
{
    std::vector<LayerInfoPtr> layerInfos;
    for (size_t i = 0; i < 3; i++) {
        auto layerInfo_ = HdiLayerInfo::CreateHdiLayerInfo();
        layerInfo_->SetIsMaskLayer(true);
        layerInfos.emplace_back(layerInfo_);
    }
    HdiOutputTest::hdiOutput_->SetLayerInfo(layerInfos);
    LayerPtr maskLayer_ = HdiLayer::CreateHdiLayer(0);
    maskLayer_->SetLayerStatus(false);
    HdiOutputTest::hdiOutput_->SetMaskLayer(maskLayer_);
    HdiOutputTest::hdiOutput_->SetLayerInfo(layerInfos);
    std::string ret = "";
    HdiOutputTest::hdiOutput_->DumpHitchs(ret, "UniRender");
    EXPECT_EQ(ret, "\n");
    HdiOutputTest::hdiOutput_->DumpFps(ret, "UniRender");
}

/*
* Function: ReorderLayerInfoLocked001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call ReorderLayerInfoLocked() with invalid param
*                  2.no crash
*/
HWTEST_F(HdiOutputTest, ReorderLayerInfoLocked001, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = nullptr;
    }
    std::vector<LayerDumpInfo> dumpLayerInfos;
    output->ReorderLayerInfoLocked(dumpLayerInfos);
}

/*
* Function: DumpFps001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call DumpFps() with invalid param
*                  2.no crash
*/
HWTEST_F(HdiOutputTest, DumpFps001, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        output->surfaceIdMap_[i]->UpdateLayerInfo(HdiLayerInfo::CreateHdiLayerInfo());
    }
    std::string result;
    std::string arg;
    output->DumpFps(result, arg);
}

/*
* Function: DumpCurrentFrameLayers001
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call DumpCurrentFrameLayers()
*                  2.no crash
*/
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers001, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        output->surfaceIdMap_[i]->UpdateLayerInfo(HdiLayerInfo::CreateHdiLayerInfo());
    }
    output->DumpCurrentFrameLayers();
}

/*
 * Function: DeletePrevLayersLocked001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& surfaceIdMap = hdiOutput->surfaceIdMap_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    uint64_t id = 0;
    LayerPtr layer = std::make_shared<HdiLayer>(id);
    layer->isInUsing_ = true;
    surfaceIdMap[id] = layer;
    layerIdMap[id] = layer;

    LayerPtr maskLayer_ = HdiLayer::CreateHdiLayer(0);
    maskLayer_->SetLayerStatus(false);
    hdiOutput->SetMaskLayer(maskLayer_);

    hdiOutput->DeletePrevLayersLocked();
    EXPECT_EQ(surfaceIdMap.count(id), 1);
    EXPECT_EQ(layerIdMap.count(id), 1);

    layer->isInUsing_ = false;
    hdiOutput->DeletePrevLayersLocked();
    EXPECT_EQ(surfaceIdMap.count(id), 0);
    EXPECT_EQ(layerIdMap.count(id), 0);
}

/*
 * Function: RecordCompositionTime001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call RecordCompositionTime()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, RecordCompositionTime001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    int64_t timestamp = 100;
    hdiOutput->RecordCompositionTime(timestamp);
    auto compTimeRcdIndex = hdiOutput->compTimeRcdIndex_;
    if (compTimeRcdIndex == 0) {
        compTimeRcdIndex = hdiOutput->COMPOSITION_RECORDS_NUM - 1;
    } else {
        compTimeRcdIndex -= 1;
    }
    EXPECT_EQ(hdiOutput->compositionTimeRecords_[compTimeRcdIndex], timestamp);
}

/*
 * Function: CheckIfDoArsrPre001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPre()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPre001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    LayerInfoPtr layerInfo = std::make_shared<HdiLayerInfo>();
    bool res = hdiOutput->CheckIfDoArsrPre(nullptr);
    EXPECT_FALSE(res);

    layerInfo->cSurface_ = nullptr;
    res = hdiOutput->CheckIfDoArsrPre(layerInfo);
    EXPECT_FALSE(res);

    layerInfo->cSurface_ = IConsumerSurface::Create("xcomponentIdSurface");
    layerInfo->sbuffer_ = nullptr;
    res = hdiOutput->CheckIfDoArsrPre(layerInfo);
    EXPECT_FALSE(res);

    layerInfo->sbuffer_ = new SurfaceBufferImpl();
    layerInfo->arsrTag_ = true;
    res = hdiOutput->CheckIfDoArsrPre(layerInfo);
    EXPECT_TRUE(res);
}

/*
 * Function: CheckIfDoArsrPreForVm001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPreForVm()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPreForVm001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    LayerInfoPtr layerInfo = std::make_shared<HdiLayerInfo>();
    layerInfo->cSurface_ = IConsumerSurface::Create("xcomponentIdSurface");
    bool res = hdiOutput->CheckIfDoArsrPreForVm(layerInfo);
    EXPECT_FALSE(res);

    hdiOutput->vmArsrWhiteList_ = "xcomponentIdSurface";
    res = res = hdiOutput->CheckIfDoArsrPreForVm(layerInfo);
    EXPECT_TRUE(res);
}

/*
 * Function: ReleaseFramebuffer001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseFramebuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseFramebuffer001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    sptr<SyncFence> fence = new SyncFence(0);
    hdiOutput->currFrameBuffer_ = nullptr;
    int32_t res = hdiOutput->ReleaseFramebuffer(fence);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_NULL_PTR);

    hdiOutput->currFrameBuffer_ = new SurfaceBufferImpl();
    hdiOutput->lastFrameBuffer_ = nullptr;
    res = hdiOutput->ReleaseFramebuffer(fence);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);

    hdiOutput->currFrameBuffer_ = new SurfaceBufferImpl();
    hdiOutput->lastFrameBuffer_ = new SurfaceBufferImpl();
    hdiOutput->fbSurface_ = nullptr;
    res = hdiOutput->ReleaseFramebuffer(fence);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_NULL_PTR);

    hdiOutput->currFrameBuffer_ = new SurfaceBufferImpl();
    hdiOutput->lastFrameBuffer_ = new SurfaceBufferImpl();
    hdiOutput->fbSurface_ = new HdiFramebufferSurface();
    res = hdiOutput->ReleaseFramebuffer(fence);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);
}

/*
 * Function: GetBufferCacheSize001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetBufferCacheSize()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetBufferCacheSize001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    int res = hdiOutput->GetBufferCacheSize();
    EXPECT_EQ(hdiOutput->bufferCache_.size(), res);
}

/*
 * Function: StartVSyncSampler
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call StartVSyncSampler()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, StartVSyncSampler001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    CreateVSyncSampler()->SetVsyncSamplerEnabled(false);
    int32_t res = hdiOutput->StartVSyncSampler(true);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_FAILURE);

    CreateVSyncSampler()->SetVsyncSamplerEnabled(true);
    hdiOutput->sampler_ = nullptr;
    res = hdiOutput->StartVSyncSampler(false);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);

    res = hdiOutput->StartVSyncSampler(true);
    EXPECT_EQ(res, GRAPHIC_DISPLAY_SUCCESS);
}

/*
 * Function: ClearFpsDump001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFpsDump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFpsDump001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    std::string result = "";
    std::string arg = "xcomponentIdSurface";
    auto& surfaceIdMap = hdiOutput->surfaceIdMap_;
    surfaceIdMap.clear();
    uint64_t id = 0;
    LayerPtr layer = std::make_shared<HdiLayer>(id);
    LayerInfoPtr layerInfo = std::make_shared<HdiLayerInfo>();
    layer->layerInfo_ = layerInfo;
    layer->layerInfo_->cSurface_ = nullptr;
    surfaceIdMap[id] = layer;
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_EQ(result, static_cast<std::string>("\nlayer is null.\n"));

    result = "";
    layer->layerInfo_->cSurface_ = IConsumerSurface::Create(arg);
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_EQ(
        result, static_cast<std::string>("\n\n The fps info of surface [xcomponentIdSurface] Id[0] is cleared.\n"));
}

/*
 * Function: GetComposeClientLayers001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetComposeClientLayers with layer is nullptr
 *                  2.check clientLayers size
 */
HWTEST_F(HdiOutputTest, GetComposeClientLayers001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    uint64_t id = 0;
    layerIdMap[id] = nullptr;
    std::vector<LayerPtr> clientLayers;
    hdiOutput->GetComposeClientLayers(clientLayers);
    EXPECT_EQ(clientLayers.size(), 0);
}

/*
 * Function: GetComposeClientLayers002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetComposeClientLayers with layerInfo is nullptr
 *                  2.check clientLayers size
 */
HWTEST_F(HdiOutputTest, GetComposeClientLayers002, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    uint64_t id = 0;
    LayerPtr layer = std::make_shared<HdiLayer>(id);
    layer->layerInfo_ = nullptr;
    layerIdMap[id] = layer;
    std::vector<LayerPtr> clientLayers;
    hdiOutput->GetComposeClientLayers(clientLayers);
    EXPECT_EQ(clientLayers.size(), 0);
}

/*
 * Function: ReleaseSurfaceBuffer001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseSurfaceBuffer with layer or layerInfo or layer's cSurface is nullptr
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseSurfaceBuffer001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    uint64_t id = 0;
    layerIdMap[id] = nullptr;
    auto& layersId = hdiOutput->layersId_;
    EXPECT_EQ(layersId.size(), 0);
    sptr<SyncFence> releaseFence = nullptr;
    hdiOutput->ReleaseSurfaceBuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);

    LayerPtr layer = std::make_shared<HdiLayer>(id);
    layer->layerInfo_ = nullptr;
    layerIdMap[id] = layer;
    hdiOutput->ReleaseSurfaceBuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);

    LayerInfoPtr layerInfo = std::make_shared<HdiLayerInfo>();
    layer->layerInfo_ = layerInfo;
    layer->layerInfo_->cSurface_ = nullptr;
    hdiOutput->ReleaseSurfaceBuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);
}

/*
 * Function: SetProtectedFrameBufferState
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: get hdiOutput
 *                  2. operation: SetProtectedFrameBufferState and GetProtectedFrameBufferState
 *                  3. result: return ProtectedFrameBufferState
 */
HWTEST_F(HdiOutputTest, SetProtectedFrameBufferState_001, testing::ext::TestSize.Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);
    hdiOutput->SetProtectedFrameBufferState(true);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), true);
}

/*
* Function:  CleanLayerBufferBySurfaceId
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1.call CleanLayerBufferBySurfaceId() with valid param, no crash
*                  2.call CleanLayerBufferBySurfaceId() with invalid param, no crash
*/
HWTEST_F(HdiOutputTest, CleanLayerBufferBySurfaceId_001, testing::ext::TestSize.Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        output->surfaceIdMap_[i]->UpdateLayerInfo(HdiLayerInfo::CreateHdiLayerInfo());
    }
    for (size_t i = 0; i < 3; i++) {
        output->CleanLayerBufferBySurfaceId(i);
    }
    // clean the nonexistent surface ID.
    output->CleanLayerBufferBySurfaceId(4);
}

/*
 * Function: InitLoadOptParams001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call InitLoadOptParams
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, InitLoadOptParams001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    LoadOptParamsForHdiOutput params;
    params.switchParams[IS_MERGE_FENCE_SKIPPED] = true;

    hdiOutput->InitLoadOptParams(params);
    EXPECT_TRUE(hdiOutput->isMergeFenceSkipped_);
}

/*
 * Function: ANCOTransactionOnComplete001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ANCOTransactionOnComplete()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, ANCOTransactionOnComplete001, Function | MediumTest | Level3)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    LayerInfoPtr layerInfo = nullptr;
    output->ANCOTransactionOnComplete(layerInfo, nullptr);
    layerInfo = std::make_shared<HdiLayerInfo>();
    output->ANCOTransactionOnComplete(layerInfo, nullptr);
    layerInfo->SetAncoFlags(static_cast<uint32_t>(HdiAncoFlags::ANCO_NATIVE_NODE));
    output->ANCOTransactionOnComplete(layerInfo, nullptr);
    auto previousReleaseFence = sptr<SyncFence>::MakeSptr(-1);
    auto consumer = IConsumerSurface::Create("xcomponentIdSurface");
    layerInfo->SetSurface(consumer);
    output->ANCOTransactionOnComplete(layerInfo, previousReleaseFence);
    auto buffer = new SurfaceBufferImpl();
    layerInfo->SetBuffer(buffer, nullptr);
    output->ANCOTransactionOnComplete(layerInfo, previousReleaseFence);
}
} // namespace
} // namespace Rosen
} // namespace OHOS