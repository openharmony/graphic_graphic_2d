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

#define PRIVATE PUBLIC
#include "hdi_output.h"
#undef PRIVATE
#include <gtest/gtest.h>
#include "mock_hdi_device.h"
#include "surface_buffer_impl.h"
#include "rs_render_composer_client.h"
#include "rs_surface_layer.h"

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
    static inline std::vector<std::string> paramKey_ {};
};

void HdiOutputTest::SetUpTestCase()
{
    uint32_t screenId = 0;
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    hdiOutput_->device_ = hdiDeviceMock_;
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}

void HdiOutputTest::TearDownTestCase() {}

namespace {

/**
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

/**
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

/**
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

/**
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
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), 0);
}

/**
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

/**
 * Function: Init001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, Init001, Function | MediumTest| Level1)
{
    // device_ is already set
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    // fbSurface_ already initialized
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/**
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

/**
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

/**
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

/**
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

/**
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

/**
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

/**
 * Function: CommitAndGetReleaseFence002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CommitAndGetReleaseFence()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, CommitAndGetReleaseFence002, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_,
        CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    int32_t skipState = 0;
    bool needFlush = false;

    // Setup buffer cache to verify clearing
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(buffer);

    // Setup layer to verify ResetBufferCache call (branch coverage)
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(0);
    HdiOutputTest::hdiOutput_->layerIdMap_[0] = hdiLayer;
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = nullptr;

    ASSERT_EQ(HdiOutputTest::hdiOutput_->CommitAndGetReleaseFence(fbFence, skipState, needFlush, false),
        GRAPHIC_DISPLAY_FAILURE);

    ASSERT_EQ(HdiOutputTest::hdiOutput_->bufferCache_.size(), 0);

    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
}

/**
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

/**
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

/**
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

/**
 * Function: ReleaseLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseLayers
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseLayers, Function | MediumTest | Level1)
{
    auto &map = HdiOutputTest::hdiOutput_->layerIdMap_;
    for (auto &layer : map) {
        layer.second->GetRSLayer()->SetIsSupportedPresentTimestamp(true);
        EXPECT_EQ(layer.second->GetRSLayer()->IsSupportedPresentTimestamp(), true);
    }

    // ReleaseLayerBuffersInfo releaseLayerInfo;
    // HdiOutputTest::hdiOutput_->ReleaseLayers(releaseLayerInfo);
}

/**
 * Function: DumpHitchs
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpHitchs()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpHitchs, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> maskLayer_ = HdiLayer::CreateHdiLayer(0);
    maskLayer_->SetLayerStatus(false);
    HdiOutputTest::hdiOutput_->SetMaskLayer(maskLayer_);
    std::string ret = "";
    HdiOutputTest::hdiOutput_->DumpHitchs(ret, "UniRender");
    EXPECT_EQ(ret, "\n");
    HdiOutputTest::hdiOutput_->DumpFps(ret, "UniRender");
}

/**
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

/**
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
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
        output->surfaceIdMap_[i]->UpdateRSLayer(rsLayer);
    }
    std::string result;
    std::string arg;
    output->DumpFps(result, arg);
}

/**
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
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(id);
    layer->isInUsing_ = true;
    surfaceIdMap[id] = layer;
    layerIdMap[id] = layer;

    hdiOutput->DeletePrevLayersLocked();
    EXPECT_EQ(surfaceIdMap.count(id), 1);
    EXPECT_EQ(layerIdMap.count(id), 1);

    layer->isInUsing_ = false;
    hdiOutput->DeletePrevLayersLocked();
    EXPECT_EQ(surfaceIdMap.count(id), 0);
    EXPECT_EQ(layerIdMap.count(id), 0);
}

/**
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

/**
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
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    bool res = hdiOutput->CheckIfDoArsrPre(nullptr);
    EXPECT_FALSE(res);

    rsLayer->SetBuffer(nullptr);
    res = hdiOutput->CheckIfDoArsrPre(rsLayer);
    EXPECT_FALSE(res);

    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetBuffer(nullptr);
    res = hdiOutput->CheckIfDoArsrPre(rsLayer);
    EXPECT_FALSE(res);

    rsLayer->SetBuffer(new SurfaceBufferImpl());
    rsLayer->SetLayerArsr(true);
    res = hdiOutput->CheckIfDoArsrPre(rsLayer);
    EXPECT_TRUE(res);
}

/**
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
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("xcomponentIdSurface");
    rsLayer->SetSurface(cSurface);
    bool res = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    EXPECT_FALSE(res);

    hdiOutput->vmArsrWhiteList_ = "xcomponentIdSurface";
    res = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    EXPECT_TRUE(res);
}

/**
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

/**
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

/**
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

/**
 * Function: ClearFpsDump001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFpsDump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFpsDump001, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutput::CreateHdiOutput(0u);
    hdiOutput->Init();
    std::string result = "";
    std::string arg = "xcomponentIdSurface";
    auto& surfaceIdMap = hdiOutput->surfaceIdMap_;
    surfaceIdMap.clear();
    uint64_t id = 0;
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(id);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(nullptr);
    surfaceIdMap[id] = layer;
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_NE(result.find("layer is null"), std::string::npos);

    result = "";
    rsLayer->SetSurface(IConsumerSurface::Create(arg));
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_NE(result.find("[xcomponentIdSurface] Id[0]"), std::string::npos);
}

/**
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
    std::vector<std::shared_ptr<HdiLayer>> clientLayers;
    hdiOutput->GetComposeClientLayers(clientLayers);
    EXPECT_EQ(clientLayers.size(), 0);
}

/**
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
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(id);
    layer->UpdateRSLayer(nullptr);
    layerIdMap[id] = layer;
    std::vector<std::shared_ptr<HdiLayer>> clientLayers;
    hdiOutput->GetComposeClientLayers(clientLayers);
    EXPECT_EQ(clientLayers.size(), 0);
}

/**
 * Function: ReleaseFramebuffer002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseFramebuffer with layer or layerInfo or layer's cSurface is nullptr
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseFramebuffer002, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    uint64_t id = 0;
    layerIdMap[id] = nullptr;
    auto& layersId = hdiOutput->layersId_;
    EXPECT_EQ(layersId.size(), 0);
    sptr<SyncFence> releaseFence = nullptr;
    hdiOutput->ReleaseFramebuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);

    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(id);
    layer->UpdateRSLayer(nullptr);
    layerIdMap[id] = layer;
    hdiOutput->ReleaseFramebuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(nullptr);
    hdiOutput->ReleaseFramebuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);
}

/**
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

/**
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
    output->surfaceIdMap_.clear();
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
        output->surfaceIdMap_[i]->UpdateRSLayer(rsLayer);
    }
    output->surfaceIdMap_[3] = nullptr;
    for (size_t i = 0; i < 5; i++) {
        output->CleanLayerBufferBySurfaceId(i);
    }
}

/**
 * Function: SetRSLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetRSLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, SetRSLayers001, Function | MediumTest | Level3)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    std::vector<std::shared_ptr<RSLayer>> rsLayers;
    // layer is nullptr
    std::shared_ptr<RSLayer> layer1 = nullptr;
    rsLayers.push_back(layer1);
    // layer not nullptr and is maskLayer
    std::shared_ptr<RSLayer> layer2 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer2, nullptr);
    layer2->SetIsMaskLayer(true);
    rsLayers.push_back(layer2);
    // layer not nullptr and not maskLayer, compositionType is SOLID_COLOR
    std::shared_ptr<RSLayer> layer3 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer3, nullptr);
    layer3->SetIsMaskLayer(false);
    layer3->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    rsLayers.push_back(layer3);
    // layer not nullptr and not maskLayer, compositionType is CLIENT, solidLayerCount is 0
    std::shared_ptr<RSLayer> layer4 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer4, nullptr);
    layer4->SetIsMaskLayer(false);
    layer4->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    rsLayers.push_back(layer4);
    // layer not nullptr and not maskLayer, compositionType is CLIENT, solidLayerCount is 1
    std::shared_ptr<RSLayer> layer5 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(layer5, nullptr);
    layer5->SetIsMaskLayer(false);
    layer5->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    rsLayers.push_back(layer5);

    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(0);
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_[0] = hdiLayer;
    HdiOutputTest::hdiOutput_->SetRSLayers(rsLayers);
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
}

/**
 * Function: DirtyRegions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DirtyRegions()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DirtyRegions001, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    rsLayer->SetIsMaskLayer(false);
    HdiOutputTest::hdiOutput_->DirtyRegions(rsLayer);
    rsLayer->SetIsMaskLayer(true);
    auto preMaskLayer = HdiOutputTest::hdiOutput_->maskLayer_;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(0);
    HdiOutputTest::hdiOutput_->maskLayer_ = hdiLayer;
    HdiOutputTest::hdiOutput_->DirtyRegions(rsLayer);
    ASSERT_NE(static_cast<int32_t>(rsLayer->GetDirtyRegions().size()), 0);
    HdiOutputTest::hdiOutput_->maskLayer_ = preMaskLayer;
}

/**
 * Function: CheckSupportCopybitMetadata
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckSupportCopybitMetadata()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckSupportCopybitMetadata001, Function | MediumTest | Level3)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->Init();
    auto ret = HdiOutputTest::hdiOutput_->CheckSupportCopybitMetadata();
    ASSERT_EQ(ret, false);
}

/**
 * Function: AncoTransactionOnComplete
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call AncoTransactionOnComplete()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, AncoTransactionOnComplete001, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = nullptr;
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    rsLayer = std::make_shared<RSSurfaceLayer>();
    const uint32_t ancoFlag = 0x0111;
    rsLayer->SetAncoFlags(ancoFlag);
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    ASSERT_EQ(rsLayer->GetAncoFlags(), ancoFlag);
}

/**
 * Function: RegPrepareComplete
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call RegPrepareComplete()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, RegPrepareComplete001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->onPrepareCompleteCb_ = nullptr;
    HdiOutputTest::hdiOutput_->onPrepareCompleteCbData_ = nullptr;

    auto ret = HdiOutputTest::hdiOutput_->RegPrepareComplete(nullptr, nullptr);
    ASSERT_EQ(ret, ROSEN_ERROR_INVALID_ARGUMENTS);
}

/**
 * Function: DeletePrevLayersLocked
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    uint64_t id = 0;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(id);
    ASSERT_NE(hdiLayer1, nullptr);
    hdiLayer1->SetLayerStatus(true);
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(id + 1);
    ASSERT_NE(hdiLayer2, nullptr);
    hdiLayer2->SetLayerStatus(false);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer1);
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_[id] = hdiLayer1;
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_[id + 1] = hdiLayer2;
    HdiOutputTest::hdiOutput_->DeletePrevLayersLocked();
    ASSERT_NE(static_cast<int32_t>(HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.size()), 0);
    hdiLayer1->SetLayerStatus(false);
    HdiOutputTest::hdiOutput_->DeletePrevLayersLocked();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.size()), 0);
}

/**
 * Function: ResetLayerStatusLocked
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ResetLayerStatusLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(1);
    ASSERT_NE(hdiLayer, nullptr);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer);
    std::shared_ptr<HdiLayer> maskLayer = HdiOutputTest::hdiOutput_->maskLayer_;
    HdiOutputTest::hdiOutput_->maskLayer_ = hdiLayer;
    HdiOutputTest::hdiOutput_->ResetLayerStatusLocked();
    HdiOutputTest::hdiOutput_->maskLayer_ = maskLayer;
    HdiOutputTest::hdiOutput_->layersTobeRelease_.erase(HdiOutputTest::hdiOutput_->layersTobeRelease_.begin());
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->layersTobeRelease_.size()), 0);
}

/**
 * Function: CreateLayerLocked
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked001, Function | MediumTest | Level1)
{
    // hdiLayer init failed
    auto ret = HdiOutputTest::hdiOutput_->CreateLayerLocked(1, nullptr);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_FAILURE);
    // hdiLayer init success
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    rsLayer->SetIsMaskLayer(true);
    rsLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);
    auto arsrPreEnabledForVm = HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_;
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = arsrPreEnabledForVm;
}

/**
 * Function: DumpCurrentFrameLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpCurrentFrameLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers001, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    hdiLayer->UpdateRSLayer(rsLayer);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer;
    HdiOutputTest::hdiOutput_->DumpCurrentFrameLayers();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.erase(1);
}

/**
 * Function: FlushScreen
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call FlushScreen()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, FlushScreen001, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer, nullptr);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    auto preFbSurface = HdiOutputTest::hdiOutput_->fbSurface_;
    HdiOutputTest::hdiOutput_->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    int64_t timestamp = 0;
    Rect damage = {0};
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> buffer = nullptr;
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_NE(HdiOutputTest::hdiOutput_->fbSurface_, nullptr);
    std::vector<std::shared_ptr<HdiLayer>> compClientLayers;
    compClientLayers.push_back(nullptr);
    compClientLayers.push_back(hdiLayer);
    // currFrameBuffer is nullptr
    ASSERT_EQ(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), -1);
    // currFrameBuffer not nullptr
    buffer = new SurfaceBufferImpl();
    while (!HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.empty()) {
        HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.pop();
    }
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    HdiOutputTest::hdiOutput_->bufferCacheCountMax_ = 1;
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers);

    // setscreenclientdamage return error
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_NE(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), 0);

    // setscreenclientdamage return ok
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;

    // bufferCache is true, index == bufferCacheCountMax_
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(buffer);
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers);

    HdiOutputTest::hdiOutput_->fbSurface_ = preFbSurface;
}

/**
 * Function: FlushScreen
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call FlushScreen()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, FlushScreen002, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer, nullptr);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    auto preFbSurface = HdiOutputTest::hdiOutput_->fbSurface_;
    HdiOutputTest::hdiOutput_->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    int64_t timestamp = 0;
    Rect damage = {0};
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    sptr<SurfaceBuffer> buffer = nullptr;
    ASSERT_NE(HdiOutputTest::hdiOutput_->fbSurface_, nullptr);
    std::vector<std::shared_ptr<HdiLayer>> compClientLayers;
    compClientLayers.push_back(nullptr);
    compClientLayers.push_back(hdiLayer);
    // currFrameBuffer not nullptr
    buffer = new SurfaceBufferImpl();
    while (!HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.empty()) {
        HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.pop();
    }
    HdiOutputTest::hdiOutput_->bufferCacheCountMax_ = 1;

    // bufferCache is true, index < bufferCacheCountMax_, SetScreenClientBuffer return err
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(buffer);
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenClientBuffer(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_NE(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), GRAPHIC_DISPLAY_SUCCESS);

    // bufferCache is true, index < bufferCacheCountMax_, SetScreenClientBuffer return okk
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(buffer);
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenClientBuffer(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_EQ(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->fbSurface_ = preFbSurface;
}

/**
 * Function: ReleaseSurfaceBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseSurfaceBuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseSurfaceBuffer001, Function | MediumTest | Level3)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(0);
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    hdiLayer2->UpdateRSLayer(rsLayer2);
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    hdiLayer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    HdiOutputTest::hdiOutput_->layerIdMap_[0] = hdiLayer1;
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer2;
    HdiOutputTest::hdiOutput_->layerIdMap_[2] = hdiLayer3;
    HdiOutputTest::hdiOutput_->layerIdMap_[3] = nullptr;
    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    HdiOutputTest::hdiOutput_->ReleaseSurfaceBuffer(fence);
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
}

/**
 * Function: ReleaseSurfaceBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseSurfaceBuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseSurfaceBuffer002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    ASSERT_NE(fence, nullptr);
    // hdilayer no rslayer
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer1, nullptr);
    // has rslayer but no surface
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    ASSERT_NE(hdiLayer2, nullptr);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    // has rslayer and surface
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    ASSERT_NE(hdiLayer3, nullptr);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface and setUniRenderFlag true
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    ASSERT_NE(hdiLayer4, nullptr);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer4, nullptr);
    hdiLayer4->UpdateRSLayer(rsLayer4);
    rsLayer4->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer4->SetUniRenderFlag(true);

    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;
    HdiOutputTest::hdiOutput_->layersId_.push_back(1);
    HdiOutputTest::hdiOutput_->fences_.push_back(fence);

    HdiOutputTest::hdiOutput_->layerIdMap_[2] = hdiLayer2;
    HdiOutputTest::hdiOutput_->layersId_.push_back(2);
    HdiOutputTest::hdiOutput_->fences_.push_back(fence);

    HdiOutputTest::hdiOutput_->layerIdMap_[3] = hdiLayer3;
    HdiOutputTest::hdiOutput_->layersId_.push_back(3);
    HdiOutputTest::hdiOutput_->fences_.push_back(fence);

    HdiOutputTest::hdiOutput_->layerIdMap_[4] = hdiLayer4;
    HdiOutputTest::hdiOutput_->layersId_.push_back(4);
    HdiOutputTest::hdiOutput_->fences_.push_back(fence);

    HdiOutputTest::hdiOutput_->layersId_.push_back(5);

    HdiOutputTest::hdiOutput_->ReleaseSurfaceBuffer(fence);
}

/**
 * Function: ReleaseSurfaceBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseSurfaceBuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseSurfaceBuffer003, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    ASSERT_NE(fence, nullptr);
    // hdilayer no rslayer
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer1, nullptr);
    // has rslayer but no surface
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    ASSERT_NE(hdiLayer2, nullptr);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    // has rslayer and surface, UniRenderFlag is false
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    ASSERT_NE(hdiLayer3, nullptr);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface, UniRenderFlag is true, prebuffer is not nullptr
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    ASSERT_NE(hdiLayer4, nullptr);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer4, nullptr);
    rsLayer4->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer4->SetPreBuffer(new SurfaceBufferImpl());
    rsLayer4->SetUniRenderFlag(true);
    hdiLayer4->UpdateRSLayer(rsLayer4);

    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer1);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer2);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer3);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), hdiLayer4);
    HdiOutputTest::hdiOutput_->layersTobeRelease_.insert(
        HdiOutputTest::hdiOutput_->layersTobeRelease_.begin(), nullptr);
    HdiOutputTest::hdiOutput_->ReleaseSurfaceBuffer(fence);
}

/**
 * Function: ReleaseLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseLayers001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    ASSERT_NE(fence, nullptr);
    // hdilayer no rslayer
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer1, nullptr);
    // has rslayer but no surface
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    ASSERT_NE(hdiLayer2, nullptr);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    // has rslayer and surface
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    ASSERT_NE(hdiLayer3, nullptr);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface SetIsSupportedPresentTimestamp
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    ASSERT_NE(hdiLayer4, nullptr);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer4, nullptr);
    rsLayer4->SetIsSupportedPresentTimestamp(true);
    hdiLayer4->UpdateRSLayer(rsLayer4);
    rsLayer4->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface SetIsSupportedPresentTimestamp and set buffer
    std::shared_ptr<HdiLayer> hdiLayer5 = HdiLayer::CreateHdiLayer(5);
    ASSERT_NE(hdiLayer5, nullptr);
    std::shared_ptr<RSLayer> rsLayer5 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer5, nullptr);
    rsLayer5->SetIsSupportedPresentTimestamp(true);
    rsLayer5->SetBuffer(new SurfaceBufferImpl());
    hdiLayer5->UpdateRSLayer(rsLayer5);
    rsLayer5->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));

    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;
    HdiOutputTest::hdiOutput_->layerIdMap_[2] = hdiLayer2;
    HdiOutputTest::hdiOutput_->layerIdMap_[3] = hdiLayer3;
    HdiOutputTest::hdiOutput_->layerIdMap_[4] = hdiLayer4;
    HdiOutputTest::hdiOutput_->layerIdMap_[5] = hdiLayer5;
    HdiOutputTest::hdiOutput_->layerIdMap_[6] = nullptr;
    HdiOutputTest::hdiOutput_->ReleaseLayers(fence);
}

/**
 * Function: GetLayersReleaseFenceLocked
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetLayersReleaseFenceLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetLayersReleaseFenceLocked001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
    // hdilayer no rslayer
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;
    HdiOutputTest::hdiOutput_->layersId_.push_back(1);
    HdiOutputTest::hdiOutput_->fences_.push_back(fence);
    HdiOutputTest::hdiOutput_->layersId_.push_back(2);

    auto fences = HdiOutputTest::hdiOutput_->GetLayersReleaseFenceLocked();
    ASSERT_EQ(fences.size(), 1);
}

/**
 * Function: DumpHitchs
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpHitchs()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpHitchs001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    rsLayer1->SetUniRenderFlag(false);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    // hdilayer has rslayer SetUniRenderFlag true
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    rsLayer2->SetUniRenderFlag(true);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[2] = hdiLayer2;

    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_[1] = nullptr;
    std::shared_ptr<HdiLayer> hdiLayer0 = HdiLayer::CreateHdiLayer(0);
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_[0] = hdiLayer0;

    std::string ret = "";
    HdiOutputTest::hdiOutput_->DumpHitchs(ret, "UniRender");
    EXPECT_NE(ret, "\n");
}

/**
 * Function: DumpFps002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpFps()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpFps002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer0 = HdiLayer::CreateHdiLayer(0);
    std::shared_ptr<RSLayer> rsLayer0 = std::make_shared<RSSurfaceLayer>();
    rsLayer0->SetUniRenderFlag(false);
    hdiLayer0->UpdateRSLayer(rsLayer0);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[0] = hdiLayer0;

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    rsLayer1->SetUniRenderFlag(false);
    rsLayer1->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    std::vector<std::string> windowsName1 = { "xcomponentIdSurface", "HdiOutputTest" };
    rsLayer1->SetWindowsName(windowsName1);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    // hdilayer has rslayer SetUniRenderFlag true
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    rsLayer2->SetUniRenderFlag(true);
    rsLayer2->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    std::vector<std::string> windowsName2 = { "HdiOutputTest" };
    rsLayer2->SetWindowsName(windowsName2);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[2] = hdiLayer2;

    std::string ret = "";
    HdiOutputTest::hdiOutput_->DumpFps(ret, "UniRender");
    EXPECT_NE(ret, "\n");

    ret = "";
    HdiOutputTest::hdiOutput_->DumpFps(ret, "xcomponentIdSurface");
    EXPECT_NE(ret, "\n");
}

/**
 * Function: DumpFps003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpFps()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpFps003, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag true, windowsName not find
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    rsLayer3->SetUniRenderFlag(true);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    std::vector<std::string> windowsName3 = {};
    rsLayer3->SetWindowsName(windowsName3);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[0] = hdiLayer3;

    // hdilayer has rslayer SetUniRenderFlag true, windowsName find
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>();
    rsLayer4->SetUniRenderFlag(true);
    rsLayer4->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    std::vector<std::string> windowsName4 = {"xcomponentIdSurface"};
    rsLayer4->SetWindowsName(windowsName4);
    hdiLayer4->UpdateRSLayer(rsLayer4);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer4;

    std::string ret = "";
    HdiOutputTest::hdiOutput_->DumpFps(ret, "UniRender");
    EXPECT_NE(ret, "\n");

    ret = "";
    HdiOutputTest::hdiOutput_->DumpFps(ret, "xcomponentIdSurface");
    EXPECT_NE(ret, "\n");
}

/**
 * Function: DumpCurrentFrameLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpCurrentFrameLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    EXPECT_NE(hdiLayer1, nullptr);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    EXPECT_NE(rsLayer1, nullptr);
    rsLayer1->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;

    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    EXPECT_NE(hdiLayer2, nullptr);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    EXPECT_NE(rsLayer2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    HdiOutputTest::hdiOutput_->layerIdMap_[2] = hdiLayer2;

    HdiOutputTest::hdiOutput_->DumpCurrentFrameLayers();
}

/**
 * Function: Dump002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Dump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, Dump002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    rsLayer1->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    hdiLayer2->UpdateRSLayer(rsLayer2);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[2] = hdiLayer2;

    auto fbSurface = HdiOutputTest::hdiOutput_->fbSurface_;
    HdiOutputTest::hdiOutput_->fbSurface_ = nullptr;

    std::string ret = "";
    HdiOutputTest::hdiOutput_->Dump(ret);
    ASSERT_NE(ret, "");
    HdiOutputTest::hdiOutput_->fbSurface_ = fbSurface;
}

/**
 * Function: GetVsyncSamplerEnabled001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetVsyncSamplerEnabled()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetVsyncSamplerEnabled001, Function | MediumTest | Level1)
{
    auto sampler = HdiOutputTest::hdiOutput_->sampler_;
    HdiOutputTest::hdiOutput_->sampler_ = nullptr;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->sampler_, nullptr);
    HdiOutputTest::hdiOutput_->GetVsyncSamplerEnabled();
    HdiOutputTest::hdiOutput_->sampler_ = sampler;
}

/**
 * Function: CheckIfDoArsrPre002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPre()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPre002, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    rsLayer->SetIsSupportedPresentTimestamp(true);
    auto buffer = new SurfaceBufferImpl();
    BufferHandle *handle = new BufferHandle();
    buffer->SetBufferHandle(handle);
    rsLayer->SetBuffer(buffer);
    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName HdiOutputTest, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName HdiOutputTest, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName xcomponentIdSurface, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName xcomponentIdSurface, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName HdiOutputTest, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName HdiOutputTest, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName xcomponentIdSurface, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName xcomponentIdSurface, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);
}

/**
 * Function: AncoTransactionOnComplete002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call AncoTransactionOnComplete()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, AncoTransactionOnComplete002, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    const uint32_t ancoFlag = 0x0111;
    rsLayer->SetAncoFlags(ancoFlag);
    // no surface no buffer
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    // has surface no buffer
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    // no surface has buffer
    rsLayer->SetSurface(nullptr);
    rsLayer->SetBuffer(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    // has surface has buffer
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetBuffer(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->AncoTransactionOnComplete(rsLayer, SyncFence::InvalidFence());
    ASSERT_EQ(rsLayer->GetAncoFlags(), ancoFlag);
}

/**
 * Function: ReorderRSLayers001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReorderRSLayers()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, ReorderRSLayers001, Function | MediumTest | Level1)
{
    std::vector<std::shared_ptr<RSLayer>> newRSLayers;
    std::shared_ptr<RSLayer> rsLayer1 = nullptr;
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    ASSERT_NE(rsLayer2, nullptr);
    newRSLayers.push_back(rsLayer1);
    newRSLayers.push_back(rsLayer2);
    newRSLayers.push_back(rsLayer1);
    newRSLayers.push_back(rsLayer1);
    HdiOutputTest::hdiOutput_->ReorderRSLayers(newRSLayers);
}

/**
 * Function: DeletePrevLayersLocked003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked003, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    // maskLayer is nullptr
    HdiOutputTest::hdiOutput_->maskLayer_ = nullptr;
    HdiOutputTest::hdiOutput_->DeletePrevLayersLocked();

    // maskLayer not nullptr, layerstatus is false
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(0);
    ASSERT_NE(hdiLayer, nullptr);
    hdiLayer->SetLayerStatus(false);
    HdiOutputTest::hdiOutput_->maskLayer_ = hdiLayer;
    HdiOutputTest::hdiOutput_->DeletePrevLayersLocked();
    // maskLayer not nullptr, layerstatus is true
    HdiOutputTest::hdiOutput_->maskLayer_ = hdiLayer;
    HdiOutputTest::hdiOutput_->maskLayer_->SetLayerStatus(true);
    HdiOutputTest::hdiOutput_->DeletePrevLayersLocked();
}

/**
 * Function: GetRSLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetRSLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetRSLayers001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->layerIdMap_[0] = nullptr;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;
    std::vector<std::shared_ptr<RSLayer>> rsLayers = {};
    HdiOutputTest::hdiOutput_->GetRSLayers(rsLayers);
    ASSERT_NE(static_cast<int32_t>(HdiOutputTest::hdiOutput_->layerIdMap_.size()), 0);
}

/**
 * Function: CheckAndUpdateClientBufferCahce004
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCahce004, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index = 0;
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCacheCountMax_ = 3;
    auto ret = HdiOutputTest::hdiOutput_->CheckAndUpdateClientBufferCahce(buffer, index);
    ASSERT_EQ(ret, false);
}

/**
 * Function: CheckIfDoArsrPreForVm002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPreForVm()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPreForVm002, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    hdiOutput->vmArsrWhiteList_ = "xcomponentIdSurface;HdiOutputTest";

    // getsurface is nullptr
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    bool ret = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    ASSERT_EQ(ret, false);
    // getsurface is not nullptr, not find in vmLayers
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CheckIfDoArsrPreForVm002");
    rsLayer->SetSurface(cSurface);
    ret = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    ASSERT_EQ(ret, false);
    // getsurface is not nullptr, find in vmLayers
    cSurface = IConsumerSurface::Create("xcomponentIdSurface");
    rsLayer->SetSurface(cSurface);
    ret = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    ASSERT_EQ(ret, true);
}

/**
 * Function: SetVsyncSamplerEnabled
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetVsyncSamplerEnabled()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, SetVsyncSamplerEnabled001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->sampler_ = CreateVSyncSampler();
    ASSERT_NE(HdiOutputTest::hdiOutput_->sampler_, nullptr);
    HdiOutputTest::hdiOutput_->SetVsyncSamplerEnabled(false);
}

/**
 * Function: SetPendingMode
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetPendingMode()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, SetPendingMode001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->sampler_ = CreateVSyncSampler();
    ASSERT_NE(HdiOutputTest::hdiOutput_->sampler_, nullptr);
    HdiOutputTest::hdiOutput_->SetPendingMode(0, 0);
}

/**
 * Function: ClearFpsDump002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFpsDump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFpsDump002, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    std::string result = "";
    std::string arg = "xcomponentIdSurface";
    // getSurface is nullptr
    std::shared_ptr<HdiLayer> layer1 = std::make_shared<HdiLayer>(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>();
    layer1->UpdateRSLayer(rsLayer1);
    rsLayer1->SetSurface(nullptr);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = layer1;
    // name != arg
    std::shared_ptr<HdiLayer> layer2 = std::make_shared<HdiLayer>(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>();
    layer2->UpdateRSLayer(rsLayer2);
    rsLayer2->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    HdiOutputTest::hdiOutput_->surfaceIdMap_[2] = layer2;
    // name == arg
    std::shared_ptr<HdiLayer> layer3 = std::make_shared<HdiLayer>(3);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>();
    layer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create(arg));
    HdiOutputTest::hdiOutput_->surfaceIdMap_[3] = layer3;

    HdiOutputTest::hdiOutput_->ClearFpsDump(result, arg);
    EXPECT_NE(result.find("layer is null"), std::string::npos);
}

/**
 * Function: ClearBufferCache
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearBufferCache()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearBufferCache001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    // Test case 1: bufferCache_ is empty, should return early
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    // Test case 2: bufferCache_ is not empty, device_ is nullptr
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    ASSERT_NE(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    // Test case 3: bufferCache_ is not empty, device_ is not nullptr, ClearClientBuffer returns success
    EXPECT_CALL(*hdiDeviceMock_,
        ClearClientBuffer(_)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    // Test case 4: bufferCache_ is not empty, device_ is not nullptr, ClearClientBuffer returns failure
    EXPECT_CALL(*hdiDeviceMock_,
        ClearClientBuffer(_)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);
}

/**
 * Function: GetFrameBufferSurface003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetFrameBufferSurface()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetFrameBufferSurface003, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    ASSERT_NE(HdiOutputTest::hdiOutput_->fbSurface_, nullptr);
    HdiOutputTest::hdiOutput_->fbSurface_->producerSurface_ = nullptr;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->fbSurface_->producerSurface_, nullptr);
    HdiOutputTest::hdiOutput_->GetFrameBufferSurface();
}

/**
 * Function: CheckSupportArsrPreMetadata001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckSupportArsrPreMetadata()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckSupportArsrPreMetadata001, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    static std::vector<std::string> ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CheckSupportArsrPreMetadata(), true);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CheckSupportCopybitMetadata(), true);
}

/**
 * Function: CreateLayerLocked003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CreateLayerLocked()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked003, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    auto vmArsrWhiteList = HdiOutputTest::hdiOutput_->vmArsrWhiteList_;
    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = "HdiOutputTest";

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    rsLayer->SetIsMaskLayer(false);
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    rsLayer->SetLayerCopybit(false);
    std::vector<std::string> ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(false);
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    ret = { "ArsrDoEnhance" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(true);
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    ret = { "ArsrDoEnhance" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(true);
    ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_,
        GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = vmArsrWhiteList;
}

/**
 * Function: CreateLayerLocked004
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CreateLayerLocked()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked004, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    auto vmArsrWhiteList = HdiOutputTest::hdiOutput_->vmArsrWhiteList_;
    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = "HdiOutputTest";

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    rsLayer->SetIsMaskLayer(false);
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    HdiOutputTest::hdiOutput_->arsrPreEnabled_ = true;
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = false;
    rsLayer->SetLayerCopybit(false);
    rsLayer->SetBuffer(new SurfaceBufferImpl());
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    std::vector<std::string> ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->arsrPreEnabled_ = true;
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = false;
    rsLayer->SetLayerCopybit(true);
    rsLayer->SetBuffer(new SurfaceBufferImpl());
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = vmArsrWhiteList;
}

/**
 * Function: UpdateLayerCompType001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call UpdateLayerCompType()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, UpdateLayerCompType001, Function | MediumTest | Level1)
{
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_NE(HdiOutputTest::hdiOutput_->UpdateLayerCompType(), GRAPHIC_DISPLAY_SUCCESS);

    std::vector<uint32_t> layersIds = {1, 2, 3};
    std::vector<int32_t> types = {1, 2};
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(layersIds),
            ::testing::SetArgReferee<2>(types),
            ::testing::Return(GRAPHIC_DISPLAY_SUCCESS)
        ));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_NE(HdiOutputTest::hdiOutput_->UpdateLayerCompType(), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = HdiLayer::CreateHdiLayer(1);
    layersIds = {1, 2};
    types = {1, 2};
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(layersIds),
            ::testing::SetArgReferee<2>(types),
            ::testing::Return(GRAPHIC_DISPLAY_SUCCESS)
        ));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->UpdateLayerCompType(), GRAPHIC_DISPLAY_SUCCESS);
    HdiOutputTest::hdiOutput_->SetScreenPowerOnChanged(false);
}

/**
 * Function: Repaint001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Repaint()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, Repaint001, Function | MediumTest | Level1)
{
    // preProcessLayersComp return success
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(0);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    HdiOutputTest::hdiOutput_->layerIdMap_[0] = layer;

    sptr<SyncFence> fence = nullptr;
    int32_t skipState = GRAPHIC_DISPLAY_FAILURE;
    bool needFlush = false;
    std::vector<uint32_t> layersId = {};
    std::vector<sptr<SyncFence>> fences = {};
    // CommitAndGetReleaseFence ret is GRAPHIC_DISPLAY_FAILURE, skipState is GRAPHIC_DISPLAY_FAILURE
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(fence),
            ::testing::SetArgReferee<2>(skipState),
            ::testing::SetArgReferee<3>(needFlush),
            ::testing::SetArgReferee<4>(layersId),
            ::testing::SetArgReferee<5>(fences),
            ::testing::Return(GRAPHIC_DISPLAY_FAILURE)
        ));
    // UpdateLayerCompType return GRAPHIC_DISPLAY_FAILURE
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->SetScreenPowerOnChanged(false);
    HdiOutputTest::hdiOutput_->Repaint();

    skipState = GRAPHIC_DISPLAY_FAILURE;
    // CommitAndGetReleaseFence ret is GRAPHIC_DISPLAY_FAILURE, skipState is GRAPHIC_DISPLAY_FAILURE
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(fence),
            ::testing::SetArgReferee<2>(skipState),
            ::testing::SetArgReferee<3>(needFlush),
            ::testing::SetArgReferee<4>(layersId),
            ::testing::SetArgReferee<5>(fences),
            ::testing::Return(GRAPHIC_DISPLAY_FAILURE)
        ));
    // UpdateLayerCompType return GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->SetScreenPowerOnChanged(true);
    HdiOutputTest::hdiOutput_->Repaint();
}

/**
 * Function: Repaint002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Repaint()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, Repaint002, Function | MediumTest | Level1)
{
    // preProcessLayersComp return success
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(0);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>();
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    HdiOutputTest::hdiOutput_->layerIdMap_[0] = layer;

    sptr<SyncFence> fence = SyncFence::InvalidFence();
    int32_t skipState = GRAPHIC_DISPLAY_SUCCESS;
    bool needFlush = false;
    std::vector<uint32_t> layersId = {};
    std::vector<sptr<SyncFence>> fences = {};
    // CommitAndGetReleaseFence ret is GRAPHIC_DISPLAY_FAILURE, skipState is GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(fence),
            ::testing::SetArgReferee<2>(skipState),
            ::testing::SetArgReferee<3>(needFlush),
            ::testing::SetArgReferee<4>(layersId),
            ::testing::SetArgReferee<5>(fences),
            ::testing::Return(GRAPHIC_DISPLAY_FAILURE)
        ));
    // UpdateLayerCompType return GRAPHIC_DISPLAY_SUCCESS
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = hdiDeviceMock_;
    HdiOutputTest::hdiOutput_->Repaint();
}

/**
 * Function: SetAncoSrcRect
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetAncoSrcRect() and GetAncoSrcRect()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, SetAncoSrcRect, Function | MediumTest | Level3)
{
    GraphicIRect tmpRect {-1, -1, -1, -1};
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>();
    GraphicIRect srcRect = layer->GetAncoSrcRect();
    EXPECT_EQ(srcRect.x, tmpRect.x);
    EXPECT_EQ(srcRect.y, tmpRect.y);
    EXPECT_EQ(srcRect.w, tmpRect.w);
    EXPECT_EQ(srcRect.h, tmpRect.h);
    srcRect = {-100, -100, 200, 200};
    layer->SetAncoSrcRect(srcRect);
    GraphicIRect srcRectRet = layer->GetAncoSrcRect();
    EXPECT_EQ(srcRect.x, srcRectRet.x);
    EXPECT_EQ(srcRect.y, srcRectRet.y);
    EXPECT_EQ(srcRect.w, srcRectRet.w);
    EXPECT_EQ(srcRect.h, srcRectRet.h);
}

/**
 * Function: UpdateInfosAfterCommit
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call UpdateInfosAfterCommit()
 *                  2.check ret
 */
class MockSyncFence : public OHOS::SyncFence {
public:
    explicit MockSyncFence(int32_t fenceFd) : OHOS::SyncFence(fenceFd) {}
    virtual ~MockSyncFence() = default;
    MOCK_METHOD0(SyncFileReadTimestamp, ns_sec_t());
};
HWTEST_F(HdiOutputTest, UpdateInfosAfterCommitVerifyFramePresentFd, Function | MediumTest | Level1)
{
    sptr<MockSyncFence> fbFence = new MockSyncFence(-1);
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    EXPECT_CALL(*fbFence, SyncFileReadTimestamp()).WillRepeatedly(testing::Return(timeStamp));
    ASSERT_EQ(hdiOutput_->UpdateInfosAfterCommit(fbFence), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(hdiOutput_->GetCurrentFramePresentFd(), fbFence->Get());
    hdiOutput_->historicalPresentfences_.clear();
}

/**
 * Function: GetDisplayClientTargetProperty001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetDisplayClientTargetProperty()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetDisplayClientTargetProperty001, Function | MediumTest | Level1)
{
    int32_t pixelFormat = 0;
    int32_t dataspace = 0;
    ASSERT_NE(hdiOutput_->device_, nullptr);
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(_, _, _)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    ASSERT_EQ(hdiOutput_->GetDisplayClientTargetProperty(pixelFormat, dataspace),
              GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(_, _, _)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    ASSERT_EQ(hdiOutput_->GetDisplayClientTargetProperty(pixelFormat, dataspace),
              GRAPHIC_DISPLAY_NOT_SUPPORT);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
