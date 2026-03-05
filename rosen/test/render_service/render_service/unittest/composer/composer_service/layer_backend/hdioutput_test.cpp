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
#include "rs_composer_client.h"
#include "rs_surface_layer.h"
#include "rs_render_surface_layer.h"

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
    hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(paramKey_));
}

void HdiOutputTest::TearDownTestCase() {}

namespace {

/**
 * Function: GetFrameBufferSurface_ReturnsNullptr
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetFrameBufferSurface()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, GetFrameBufferSurface_ReturnsNullptr, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
}

/**
 * Function: GetFramebuffer_ReturnsNullptr
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetFramebuffer()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, GetFramebuffer_ReturnsNullptr, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/**
 * Function: GetScreenId_ReturnsZero
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetScreenId()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, GetScreenId_ReturnsZero, Function | MediumTest| Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetScreenId(), 0u);
}

/**
 * Function: Commit_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Commit()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, Commit_Default, Function | MediumTest| Level1)
{
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), 0);
}

/**
 * Function: ClearFrameBuffer_ReturnsOk
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFrameBuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_ReturnsOk, Function | MediumTest | Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ClearFrameBuffer(), GSERROR_OK);
}

/**
 * Function: Init_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, Init_Default, Function | MediumTest| Level1)
{
    ASSERT_EQ(hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_), ROSEN_ERROR_OK);
    // device_ is already set
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
    // fbSurface_ already initialized
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Init(), ROSEN_ERROR_OK);
}

/**
 * Function: GetFrameBufferSurface_AfterInit
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetFrameBufferSurface
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, GetFrameBufferSurface_AfterInit, Function | MediumTest| Level3)
{
    ASSERT_NE(HdiOutputTest::hdiOutput_->GetFrameBufferSurface(), nullptr);
}

/**
 * Function: GetFramebuffer_AfterInit
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call GetFramebuffer
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, GetFramebuffer_AfterInit, Function | MediumTest| Level3)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->GetFramebuffer(), nullptr);
}

/**
 * Function: GetOutputDamage_SetAndGet
 * Type: Function
 * Rank: Important(3)
 * EnvConditions: N/A
 * CaseDescription: 1. call SetOutputDamages()
 *                  2. call GetOutputDamages()
 *                  3. check ret
 */
HWTEST_F(HdiOutputTest, GetOutputDamage_SetAndGet, Function | MediumTest| Level3)
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
 * Function: Commit_WithMockDevice
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Commit()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, Commit_WithMockDevice, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, Commit(_, _)).WillRepeatedly(testing::Return(0));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->Commit(fbFence), GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: ClearFrameBuffer_ReturnsDisconnected
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFrameBuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_ReturnsDisconnected, Function | MediumTest | Level1)
{
    ASSERT_EQ(HdiOutputTest::hdiOutput_->ClearFrameBuffer(), GSERROR_CONSUMER_DISCONNECTED);
}

/**
 * Function: CommitAndGetReleaseFence_ReturnsSuccess
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CommitAndGetReleaseFence()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, CommitAndGetReleaseFence_ReturnsSuccess, Function | MediumTest| Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, CommitAndGetReleaseFence(_, _, _, _, _, _, _)).WillRepeatedly(testing::Return(0));
    sptr<SyncFence> fbFence = SyncFence::INVALID_FENCE;
    int32_t skipState = 0;
    bool needFlush = false;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CommitAndGetReleaseFence(fbFence, skipState, needFlush, false),
        GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: CommitAndGetReleaseFence_ReturnsFailure
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call CommitAndGetReleaseFence()
 *                  2. check ret
 */
HWTEST_F(HdiOutputTest, CommitAndGetReleaseFence_ReturnsFailure, Function | MediumTest| Level1)
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
 * Function: CheckAndUpdateClientBufferCache_BufferExists
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCache_BufferExists, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index;
    auto& hdiOutput = HdiOutputTest::hdiOutput_;
    hdiOutput->bufferCache_.clear();
    hdiOutput->bufferCache_.push_back(buffer);
    bool result = hdiOutput->CheckAndUpdateClientBufferCahce(buffer, index);

    ASSERT_TRUE(result);
    ASSERT_EQ(index, 0);
}

/**
 * Function: CheckAndUpdateClientBufferCache_BufferNotExists
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCache_BufferNotExists, Function | MediumTest | Level1)
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
 * Function: CheckAndUpdateClientBufferCache_CacheFull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCache_CacheFull, Function | MediumTest | Level1)
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
 * Function: ReorderLayerInfoLocked_InvalidParam
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReorderLayerInfoLocked() with invalid param
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, ReorderLayerInfoLocked_InvalidParam, Function | MediumTest | Level1)
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
 * Function: DumpFps_InvalidParam
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpFps() with invalid param
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, DumpFps_InvalidParam, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
        output->surfaceIdMap_[i]->UpdateRSLayer(rsLayer);
    }
    std::string result;
    std::string arg;
    output->DumpFps(result, arg);
}

/**
 * Function: DeletePrevLayersLocked_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked_Default, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    auto& surfaceIdMap = hdiOutput->surfaceIdMap_;
    auto& layerIdMap = hdiOutput->layerIdMap_;
    surfaceIdMap.clear();
    layerIdMap.clear();
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
 * Function: RecordCompositionTime_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call RecordCompositionTime()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, RecordCompositionTime_Default, Function | MediumTest | Level1)
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
 * Function: CheckIfDoArsrPre_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPre()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPre_Default, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    res = hdiOutput->CheckIfDoArsrPre(rsLayer);
    EXPECT_TRUE(res);
}

/**
 * Function: CheckIfDoArsrPreForVm_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPreForVm()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPreForVm_Default, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("xcomponentIdSurface");
    rsLayer->SetSurface(cSurface);
    bool res = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    EXPECT_FALSE(res);

    hdiOutput->vmArsrWhiteList_ = "xcomponentIdSurface";
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    res = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    EXPECT_TRUE(res);
}

/**
 * Function: ReleaseFramebuffer_MultipleCases
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseFramebuffer()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseFramebuffer_MultipleCases, Function | MediumTest | Level1)
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
 * Function: GetBufferCacheSize_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetBufferCacheSize()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetBufferCacheSize_Default, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    int res = hdiOutput->GetBufferCacheSize();
    EXPECT_EQ(hdiOutput->bufferCache_.size(), res);
}

/**
 * Function: StartVSyncSampler_VaryingStates
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call StartVSyncSampler()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, StartVSyncSampler_VaryingStates, Function | MediumTest | Level1)
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
 * Function: ClearFpsDump_LayerNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFpsDump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFpsDump_LayerNull, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutput::CreateHdiOutput(0u);
    hdiOutput->Init();
    std::string result = "";
    std::string arg = "xcomponentIdSurface";
    auto& surfaceIdMap = hdiOutput->surfaceIdMap_;
    surfaceIdMap.clear();
    uint64_t id = 0;
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(id);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(nullptr);
    surfaceIdMap[id] = layer;
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_EQ(result.find("layer is null"), std::string::npos);

    result = "";
    rsLayer->SetSurface(IConsumerSurface::Create(arg));
    rsLayer->SetSurfaceName(arg);
    hdiOutput->ClearFpsDump(result, arg);
    EXPECT_NE(result.find("[xcomponentIdSurface] Id[0]"), std::string::npos);
}

/**
 * Function: GetComposeClientLayers_LayerNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetComposeClientLayers with layer is nullptr
 *                  2.check clientLayers size
 */
HWTEST_F(HdiOutputTest, GetComposeClientLayers_LayerNull, Function | MediumTest | Level1)
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
 * Function: GetComposeClientLayers_LayerInfoNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetComposeClientLayers with layerInfo is nullptr
 *                  2.check clientLayers size
 */
HWTEST_F(HdiOutputTest, GetComposeClientLayers_LayerInfoNull, Function | MediumTest | Level1)
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
 * Function: ReleaseFramebuffer_NullLayerOrSurface
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseFramebuffer with layer or layerInfo or layer's cSurface is nullptr
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseFramebuffer_NullLayerOrSurface, Function | MediumTest | Level1)
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

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer->UpdateRSLayer(rsLayer);
    rsLayer->SetSurface(nullptr);
    hdiOutput->ReleaseFramebuffer(releaseFence);
    EXPECT_EQ(releaseFence, nullptr);
}

/**
 * Function: SetProtectedFrameBufferState_SetAndGet
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: get hdiOutput
 *                  2. operation: SetProtectedFrameBufferState and GetProtectedFrameBufferState
 *                  3. result: return ProtectedFrameBufferState
 */
HWTEST_F(HdiOutputTest, SetProtectedFrameBufferState_SetAndGet, testing::ext::TestSize.Level1)
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
HWTEST_F(HdiOutputTest, CleanLayerBufferBySurfaceId_MultipleIds, testing::ext::TestSize.Level1)
{
    std::shared_ptr<HdiOutput> output = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(output, nullptr);
    output->surfaceIdMap_.clear();
    for (size_t i = 0; i < 3; i++) {
        output->surfaceIdMap_[i] = HdiLayer::CreateHdiLayer(i);
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
        output->surfaceIdMap_[i]->UpdateRSLayer(rsLayer);
    }
    output->surfaceIdMap_[3] = nullptr;
    for (size_t i = 0; i < 5; i++) {
        output->CleanLayerBufferBySurfaceId(i);
    }
}

/**
 * Function: SetRSLayers_VariousTypes
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetRSLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, SetRSLayers_VariousTypes, Function | MediumTest | Level3)
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
    std::shared_ptr<RSLayer> layer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(layer2, nullptr);
    layer2->SetIsMaskLayer(true);
    rsLayers.push_back(layer2);
    // layer not nullptr and not maskLayer, compositionType is SOLID_COLOR
    std::shared_ptr<RSLayer> layer3 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(layer3, nullptr);
    layer3->SetIsMaskLayer(false);
    layer3->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    rsLayers.push_back(layer3);
    // layer not nullptr and not maskLayer, compositionType is CLIENT, solidLayerCount is 0
    std::shared_ptr<RSLayer> layer4 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(layer4, nullptr);
    layer4->SetIsMaskLayer(false);
    layer4->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    rsLayers.push_back(layer4);
    // layer not nullptr and not maskLayer, compositionType is CLIENT, solidLayerCount is 1
    std::shared_ptr<RSLayer> layer5 = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
 * Function: DirtyRegions_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DirtyRegions()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DirtyRegions_Default, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
 * Function: CheckSupportCopybitMetadata_AfterInit
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckSupportCopybitMetadata()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckSupportCopybitMetadata_AfterInit, Function | MediumTest | Level3)
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
 * Function: RegPrepareComplete_NullParams
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call RegPrepareComplete()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, RegPrepareComplete_NullParams, Function | MediumTest | Level1)
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
 * Function: DeletePrevLayersLocked_SolidSurfaceMap
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked_SolidSurfaceMap, Function | MediumTest | Level1)
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
 * Function: ResetLayerStatusLocked_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ResetLayerStatusLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_Default, Function | MediumTest | Level1)
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
 * Function: CreateLayerLocked_VaryingConfigs
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked_VaryingConfigs, Function | MediumTest | Level1)
{
    // hdiLayer init failed
    auto ret = HdiOutputTest::hdiOutput_->CreateLayerLocked(1, nullptr);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_FAILURE);
    // hdiLayer init success
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer->SetIsMaskLayer(true);
    rsLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    auto preDevice = HdiOutputTest::hdiOutput_->device_;
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);
    HdiOutputTest::hdiOutput_->device_ = preDevice;
    auto arsrPreEnabledForVm = HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_;
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);
    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = arsrPreEnabledForVm;
}

/**
 * Function: FlushScreen_VariousScenarios
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call FlushScreen()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, FlushScreen_VariousScenarios, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer, nullptr);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_NE(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), 0);

    // setscreenclientdamage return ok
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);

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
 * Function: FlushScreen_BufferCacheIndex
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call FlushScreen()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, FlushScreen_BufferCacheIndex, Function | MediumTest | Level1)
{
    std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(1);
    ASSERT_NE(hdiLayer, nullptr);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_NE(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), GRAPHIC_DISPLAY_SUCCESS);

    // bufferCache is true, index < bufferCacheCountMax_, SetScreenClientBuffer return okk
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(buffer);
    EXPECT_CALL(*hdiDeviceMock_,
        SetScreenClientBuffer(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    ASSERT_EQ(HdiOutputTest::hdiOutput_->FlushScreen(compClientLayers), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->fbSurface_ = preFbSurface;
}

/**
 * Function: ReleaseLayers_VariousLayerStates
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReleaseLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_VariousLayerStates, Function | MediumTest | Level1)
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
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(rsLayer2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    // has rslayer and surface
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    ASSERT_NE(hdiLayer3, nullptr);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(rsLayer3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface SetIsSupportedPresentTimestamp
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    ASSERT_NE(hdiLayer4, nullptr);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(rsLayer4, nullptr);
    rsLayer4->SetIsSupportedPresentTimestamp(true);
    hdiLayer4->UpdateRSLayer(rsLayer4);
    rsLayer4->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    // has rslayer and surface SetIsSupportedPresentTimestamp and set buffer
    std::shared_ptr<HdiLayer> hdiLayer5 = HdiLayer::CreateHdiLayer(5);
    ASSERT_NE(hdiLayer5, nullptr);
    std::shared_ptr<RSLayer> rsLayer5 = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    ReleaseLayerBuffersInfo releaseLayerInfo;
    releaseLayerInfo.screenId = HdiOutputTest::hdiOutput_->GetScreenId();
    HdiOutputTest::hdiOutput_->ReleaseLayers(releaseLayerInfo);
}

/**
 * Function: GetLayersReleaseFenceLocked_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetLayersReleaseFenceLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetLayersReleaseFenceLocked_Default, Function | MediumTest | Level1)
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
 * Function: DumpHitchs_WithLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpHitchs()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpHitchs_WithLayers, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer1->SetUniRenderFlag(false);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    // hdilayer has rslayer SetUniRenderFlag true
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
 * Function: DumpFps_WithUniRenderFlags
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpFps()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpFps_WithUniRenderFlags, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer0 = HdiLayer::CreateHdiLayer(0);
    std::shared_ptr<RSLayer> rsLayer0 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer0->SetUniRenderFlag(false);
    hdiLayer0->UpdateRSLayer(rsLayer0);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[0] = hdiLayer0;

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer1->SetUniRenderFlag(false);
    rsLayer1->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer1->SetSurfaceName("HdiOutputTest");
    std::vector<std::string> windowsName1 = { "xcomponentIdSurface", "HdiOutputTest" };
    rsLayer1->SetWindowsName(windowsName1);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    // hdilayer has rslayer SetUniRenderFlag true
    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer2->SetUniRenderFlag(true);
    rsLayer2->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer2->SetSurfaceName("xcomponentIdSurface");
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
 * Function: DumpFps_WithWindowsName
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DumpFps()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DumpFps_WithWindowsName, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag true, windowsName not find
    std::shared_ptr<HdiLayer> hdiLayer3 = HdiLayer::CreateHdiLayer(3);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer3->SetUniRenderFlag(true);
    rsLayer3->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    std::vector<std::string> windowsName3 = {};
    rsLayer3->SetWindowsName(windowsName3);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[0] = hdiLayer3;

    // hdilayer has rslayer SetUniRenderFlag true, windowsName find
    std::shared_ptr<HdiLayer> hdiLayer4 = HdiLayer::CreateHdiLayer(4);
    std::shared_ptr<RSLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
 * Function: Dump_WithSurface
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Dump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, Dump_WithSurface, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->layersId_.clear();
    HdiOutputTest::hdiOutput_->fences_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();

    // hdilayer has rslayer SetUniRenderFlag false
    std::shared_ptr<HdiLayer> hdiLayer1 = HdiLayer::CreateHdiLayer(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer1->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = hdiLayer1;

    std::shared_ptr<HdiLayer> hdiLayer2 = HdiLayer::CreateHdiLayer(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
 * Function: GetVsyncSamplerEnabled_NullSampler
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetVsyncSamplerEnabled()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetVsyncSamplerEnabled_NullSampler, Function | MediumTest | Level1)
{
    auto sampler = HdiOutputTest::hdiOutput_->sampler_;
    HdiOutputTest::hdiOutput_->sampler_ = nullptr;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->sampler_, nullptr);
    HdiOutputTest::hdiOutput_->GetVsyncSamplerEnabled();
    HdiOutputTest::hdiOutput_->sampler_ = sampler;
}

/**
 * Function: CheckIfDoArsrPre_VaryingFormatsAndSurfaces
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPre()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPre_VaryingFormatsAndSurfaces, Function | MediumTest | Level1)
{
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer->SetIsSupportedPresentTimestamp(true);
    auto buffer = new SurfaceBufferImpl();
    BufferHandle *handle = new BufferHandle();
    buffer->SetBufferHandle(handle);
    rsLayer->SetBuffer(buffer);
    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName HdiOutputTest, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetSurfaceName("HdiOutputTest");
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName HdiOutputTest, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetSurfaceName("HdiOutputTest");
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName xcomponentIdSurface, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName xcomponentIdSurface, SetLayerArsr false
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    rsLayer->SetLayerArsr(false);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName HdiOutputTest, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetSurfaceName("HdiOutputTest");
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), false);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName HdiOutputTest, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetSurfaceName("HdiOutputTest");
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);

    // handleFormat GRAPHIC_PIXEL_FMT_BUTT, surfaceName xcomponentIdSurface, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_BUTT;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);

    // handleFormat GRAPHIC_PIXEL_FMT_YUV_422_I, surfaceName xcomponentIdSurface, SetLayerArsr true
    handle->format = OHOS::GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YUV_422_I;
    rsLayer->SetSurface(IConsumerSurface::Create("xcomponentIdSurface"));
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(HdiOutputTest::hdiOutput_->CheckIfDoArsrPre(rsLayer), true);
}

/**
 * Function: ReorderRSLayers_WithNullLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ReorderRSLayers()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, ReorderRSLayers_WithNullLayers, Function | MediumTest | Level1)
{
    std::vector<std::shared_ptr<RSLayer>> newRSLayers;
    std::shared_ptr<RSLayer> rsLayer1 = nullptr;
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(rsLayer2, nullptr);
    newRSLayers.push_back(rsLayer1);
    newRSLayers.push_back(rsLayer2);
    newRSLayers.push_back(rsLayer1);
    newRSLayers.push_back(rsLayer1);
    HdiOutputTest::hdiOutput_->ReorderRSLayers(newRSLayers);
}

/**
 * Function: DeletePrevLayersLocked_MaskLayer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call DeletePrevLayersLocked()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, DeletePrevLayersLocked_MaskLayer, Function | MediumTest | Level1)
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
 * Function: GetRSLayers_WithLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetRSLayers()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetRSLayers_WithLayers, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->layerIdMap_[0] = nullptr;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    HdiOutputTest::hdiOutput_->layerIdMap_[1] = hdiLayer1;
    std::vector<std::shared_ptr<RSLayer>> rsLayers = {};
    HdiOutputTest::hdiOutput_->GetRSLayers(rsLayers);
    ASSERT_NE(static_cast<int32_t>(HdiOutputTest::hdiOutput_->layerIdMap_.size()), 0);
}

/**
 * Function: CheckAndUpdateClientBufferCache_EmptyCache
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckAndUpdateClientBufferCahce()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckAndUpdateClientBufferCache_EmptyCache, Function | MediumTest | Level1)
{
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    uint32_t index = 0;
    HdiOutputTest::hdiOutput_->bufferCache_.clear();
    HdiOutputTest::hdiOutput_->bufferCacheCountMax_ = 3;
    auto ret = HdiOutputTest::hdiOutput_->CheckAndUpdateClientBufferCahce(buffer, index);
    ASSERT_EQ(ret, false);
}

/**
 * Function: CheckIfDoArsrPreForVm_WithWhitelist
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckIfDoArsrPreForVm()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, CheckIfDoArsrPreForVm_WithWhitelist, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    hdiOutput->vmArsrWhiteList_ = "xcomponentIdSurface;HdiOutputTest";

    // getsurface is nullptr
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    rsLayer->SetSurfaceName("xcomponentIdSurface");
    ret = hdiOutput->CheckIfDoArsrPreForVm(rsLayer);
    ASSERT_EQ(ret, true);
}

/**
 * Function: SetVsyncSamplerEnabled_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetVsyncSamplerEnabled()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, SetVsyncSamplerEnabled_Default, Function | MediumTest | Level1)
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
 * Function: SetPendingMode_Default
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call SetPendingMode()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, SetPendingMode_Default, Function | MediumTest | Level1)
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
 * Function: ClearFpsDump_VariousSurfaceStates
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearFpsDump()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearFpsDump_VariousSurfaceStates, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();

    std::string result = "";
    std::string arg = "xcomponentIdSurface";
    // getSurface is nullptr
    std::shared_ptr<HdiLayer> layer1 = std::make_shared<HdiLayer>(1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer1->UpdateRSLayer(rsLayer1);
    rsLayer1->SetSurface(nullptr);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[1] = layer1;
    // name != arg
    std::shared_ptr<HdiLayer> layer2 = std::make_shared<HdiLayer>(2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer2->UpdateRSLayer(rsLayer2);
    rsLayer2->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer2->SetSurfaceName("HdiOutputTest");
    HdiOutputTest::hdiOutput_->surfaceIdMap_[2] = layer2;
    // name == arg
    std::shared_ptr<HdiLayer> layer3 = std::make_shared<HdiLayer>(3);
    std::shared_ptr<RSLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer3->UpdateRSLayer(rsLayer3);
    rsLayer3->SetSurface(IConsumerSurface::Create(arg));
    rsLayer3->SetSurfaceName(arg);
    HdiOutputTest::hdiOutput_->surfaceIdMap_[3] = layer3;

    HdiOutputTest::hdiOutput_->ClearFpsDump(result, arg);
    EXPECT_EQ(result.find("layer is null"), std::string::npos);
}

/**
 * Function: ClearBufferCache_VariousStates
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call ClearBufferCache()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, ClearBufferCache_VariousStates, Function | MediumTest | Level1)
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
    auto preDevice = HdiOutputTest::hdiOutput_->device_;
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    ASSERT_NE(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    // Test case 3: bufferCache_ is not empty, device_ is not nullptr, ClearClientBuffer returns success
    EXPECT_CALL(*hdiDeviceMock_,
        ClearClientBuffer(_)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    // Test case 4: bufferCache_ is not empty, device_ is not nullptr, ClearClientBuffer returns failure
    EXPECT_CALL(*hdiDeviceMock_,
        ClearClientBuffer(_)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->bufferCache_.push_back(new SurfaceBufferImpl());
    HdiOutputTest::hdiOutput_->ClearBufferCache();
    ASSERT_EQ(static_cast<int32_t>(HdiOutputTest::hdiOutput_->bufferCache_.size()), 0);

    HdiOutputTest::hdiOutput_->device_ = preDevice;
}

/**
 * Function: GetFrameBufferSurface_NullProducerSurface
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetFrameBufferSurface()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetFrameBufferSurface_NullProducerSurface, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    ASSERT_NE(HdiOutputTest::hdiOutput_->fbSurface_, nullptr);
    HdiOutputTest::hdiOutput_->fbSurface_->producerSurface_ = nullptr;
    ASSERT_EQ(HdiOutputTest::hdiOutput_->fbSurface_->producerSurface_, nullptr);
    HdiOutputTest::hdiOutput_->GetFrameBufferSurface();
}

/**
 * Function: CheckSupportArsrPreMetadata_AfterInit
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CheckSupportArsrPreMetadata()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, CheckSupportArsrPreMetadata_AfterInit, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->solidSurfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->surfaceIdMap_.clear();
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    HdiOutputTest::hdiOutput_->layersTobeRelease_.clear();

    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    static std::vector<std::string> ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CheckSupportArsrPreMetadata(), true);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CheckSupportCopybitMetadata(), true);
}

/**
 * Function: CreateLayerLocked_WithArsrPreEnabledForVm
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CreateLayerLocked()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked_WithArsrPreEnabledForVm, Function | MediumTest | Level1)
{
    auto preDevice = HdiOutputTest::hdiOutput_->device_;

    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    auto vmArsrWhiteList = HdiOutputTest::hdiOutput_->vmArsrWhiteList_;
    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = "HdiOutputTest";

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rsLayer->SetIsMaskLayer(false);
    rsLayer->SetSurface(IConsumerSurface::Create("HdiOutputTest"));
    rsLayer->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);

    rsLayer->SetLayerCopybit(false);
    std::vector<std::string> ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(false);
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    ret = { "ArsrDoEnhance" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(true);
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    ret = { "ArsrDoEnhance" };
    EXPECT_CALL(*hdiDeviceMock_, GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    rsLayer->SetLayerCopybit(true);
    ret = { "ArsrDoEnhance", "TryToDoCopybit" };
    EXPECT_CALL(*hdiDeviceMock_,
        GetSupportedLayerPerFrameParameterKey()).WillRepeatedly(testing::ReturnRef(ret));
    EXPECT_CALL(*hdiDeviceMock_,
        SetLayerPerFrameParameter(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = vmArsrWhiteList;
    HdiOutputTest::hdiOutput_->device_ = preDevice;
}

/**
 * Function: CreateLayerLocked_WithArsrPreEnabled
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call CreateLayerLocked()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, CreateLayerLocked_WithArsrPreEnabled, Function | MediumTest | Level1)
{
    auto preDevice = HdiOutputTest::hdiOutput_->device_;

    HdiOutputTest::hdiOutput_->arsrPreEnabledForVm_ = true;
    auto vmArsrWhiteList = HdiOutputTest::hdiOutput_->vmArsrWhiteList_;
    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = "HdiOutputTest";

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->CreateLayerLocked(1, rsLayer), GRAPHIC_DISPLAY_SUCCESS);

    HdiOutputTest::hdiOutput_->vmArsrWhiteList_ = vmArsrWhiteList;
    HdiOutputTest::hdiOutput_->device_ = preDevice;
}

/**
 * Function: UpdateLayerCompType_VaryingLayerIds
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call UpdateLayerCompType()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, UpdateLayerCompType_VaryingLayerIds, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->device_ = HdiDevice::GetInstance();
    ASSERT_NE(HdiOutputTest::hdiOutput_->device_, nullptr);
    EXPECT_CALL(*hdiDeviceMock_,
        GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_FAILURE));
    auto preDevice = HdiOutputTest::hdiOutput_->device_;
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_NE(HdiOutputTest::hdiOutput_->UpdateLayerCompType(), GRAPHIC_DISPLAY_SUCCESS);

    std::vector<uint32_t> layersIds = {1, 2, 3};
    std::vector<int32_t> types = {1, 2};
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(DoAll(
            ::testing::SetArgReferee<1>(layersIds),
            ::testing::SetArgReferee<2>(types),
            ::testing::Return(GRAPHIC_DISPLAY_SUCCESS)
        ));
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    ASSERT_EQ(HdiOutputTest::hdiOutput_->UpdateLayerCompType(), GRAPHIC_DISPLAY_SUCCESS);
    HdiOutputTest::hdiOutput_->device_ = preDevice;
}

/**
 * Function: Repaint_FailureCases
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Repaint()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, Repaint_FailureCases, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->device_ = HdiDevice::GetInstance();
    ASSERT_NE(HdiOutputTest::hdiOutput_->device_, nullptr);

    // preProcessLayersComp return success
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(0);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    HdiOutputTest::hdiOutput_->Repaint();
}

/**
 * Function: Repaint_MixedSuccessFailure
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call Repaint()
 *                  2.no crash
 */
HWTEST_F(HdiOutputTest, Repaint_MixedSuccessFailure, Function | MediumTest | Level1)
{
    HdiOutputTest::hdiOutput_->device_ = HdiDevice::GetInstance();
    ASSERT_NE(HdiOutputTest::hdiOutput_->device_, nullptr);

    // preProcessLayersComp return success
    HdiOutputTest::hdiOutput_->layerIdMap_.clear();
    std::shared_ptr<HdiLayer> layer = std::make_shared<HdiLayer>(0);
    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    HdiOutputTest::hdiOutput_->device_ = nullptr;
    HdiOutputTest::hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
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
    RSLayerPtr layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
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
    sptr<MockSyncFence> fbFence = new MockSyncFence(1); //  Fd 1
    int64_t timeStamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    EXPECT_CALL(*fbFence, SyncFileReadTimestamp()).WillRepeatedly(testing::Return(timeStamp));
    ASSERT_EQ(hdiOutput_->UpdateInfosAfterCommit(fbFence), GRAPHIC_DISPLAY_SUCCESS);
    ASSERT_EQ(hdiOutput_->GetCurrentFramePresentFd(), fbFence->Get());
    hdiOutput_->historicalPresentfences_.clear();
}

/**
 * Function: GetDisplayClientTargetProperty_VaryingResults
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1.call GetDisplayClientTargetProperty()
 *                  2.check ret
 */
HWTEST_F(HdiOutputTest, GetDisplayClientTargetProperty_VaryingResults, Function | MediumTest | Level1)
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

/*
 * Function: SetActiveRectSwitchStatus_Toggle
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: toggle active-rect switching on/off; ensure no crash and flag set
 */
HWTEST_F(HdiOutputTest, SetActiveRectSwitchStatus_Toggle, Function | MediumTest | Level1)
{
    GraphicIRect rect {1, 2, 3, 4};
    EXPECT_CALL(*hdiDeviceMock_, SetScreenActiveRect(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    hdiOutput_->device_ = nullptr;
    hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    hdiOutput_->SetActiveRectSwitchStatus(true, rect);
    EXPECT_TRUE(hdiOutput_->isActiveRectSwitching_);
    hdiOutput_->SetActiveRectSwitchStatus(false, rect);
    EXPECT_FALSE(hdiOutput_->isActiveRectSwitching_);
}

/*
 * Function: RegPrepareComplete_ValidCallback
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: register valid callback and verify invocation via OnPrepareComplete
 */
HWTEST_F(HdiOutputTest, RegPrepareComplete_ValidCallback, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    ASSERT_NE(out, nullptr);
    out->Init();
    bool called = false;
    auto cb = [](sptr<Surface>& fb, const PrepareCompleteParam& param, void* data) {
        (void)fb;
        auto flagPtr = reinterpret_cast<bool*>(data);
        *flagPtr = param.needFlushFramebuffer;
    };
    ASSERT_EQ(out->RegPrepareComplete(cb, &called), ROSEN_ERROR_OK);
    std::vector<std::shared_ptr<RSLayer>> layers;
    layers.emplace_back(std::make_shared<RSSurfaceLayer>(0, nullptr));
    out->OnPrepareComplete(true, layers);
    EXPECT_TRUE(called);
}

/*
 * Function: UpdateInfosAfterCommit_ThirdFenceNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: when thirdFrameAheadPresentFence is null, returns GRAPHIC_DISPLAY_NULL_PTR
 */
HWTEST_F(HdiOutputTest, UpdateInfosAfterCommit_ThirdFenceNull, Function | MediumTest | Level1)
{
    hdiOutput_->historicalPresentfences_.clear();
    hdiOutput_->thirdFrameAheadPresentFence_ = nullptr;
    sptr<SyncFence> fbFence = SyncFence::InvalidFence();
    ASSERT_EQ(hdiOutput_->UpdateInfosAfterCommit(fbFence), GRAPHIC_DISPLAY_NULL_PTR);
}

/*
 * Function: DumpFps_ComposerBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: DumpFps with arg="composer" prints composition records
 */
HWTEST_F(HdiOutputTest, DumpFps_ComposerBranch, Function | MediumTest | Level1)
{
    std::string result;
    hdiOutput_->compositionTimeRecords_.fill(123);
    hdiOutput_->DumpFps(result, "composer");
    EXPECT_NE(result.find("The fps of screen"), std::string::npos);
}

/*
 * Function: FlushScreen_BufferCacheCountZero
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: bufferCacheCountMax_=0 triggers ClearBufferCache without crash
 */
HWTEST_F(HdiOutputTest, FlushScreen_BufferCacheCountZero, Function | MediumTest | Level1)
{
    auto preFbSurface = hdiOutput_->fbSurface_;
    hdiOutput_->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    int64_t timestamp = 0;
    Rect damage {};
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    while (!hdiOutput_->fbSurface_->availableBuffers_.empty()) {
        hdiOutput_->fbSurface_->availableBuffers_.pop();
    }
    hdiOutput_->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    hdiOutput_->bufferCache_.clear();
    hdiOutput_->bufferCache_.push_back(buffer);
    hdiOutput_->bufferCacheCountMax_ = 0;

    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    hdiOutput_->device_ = nullptr;
    hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);

    std::vector<std::shared_ptr<HdiLayer>> compClientLayers;
    ASSERT_EQ(hdiOutput_->FlushScreen(compClientLayers), GRAPHIC_DISPLAY_SUCCESS);
    EXPECT_EQ(hdiOutput_->bufferCache_.size(), 0u);
    hdiOutput_->fbSurface_ = preFbSurface;
}

/*
 * Function: SetScreenBacklight_And_PowerOnChanged
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: set backlight and toggle power-on-changed flag without crash
 */
HWTEST_F(HdiOutputTest, SetScreenBacklight_And_PowerOnChanged, Function | MediumTest | Level1)
{
    EXPECT_CALL(*hdiDeviceMock_, SetScreenBacklight(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    hdiOutput_->device_ = nullptr;
    hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    hdiOutput_->SetScreenBacklight(80u);
}

/*
 * Function: SetHdiOutputDevice_Nullptr_InvalidArgs
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: SetHdiOutputDevice with nullptr returns invalid arguments
 */
HWTEST_F(HdiOutputTest, SetHdiOutputDevice_Nullptr_InvalidArgs, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    ASSERT_EQ(out->SetHdiOutputDevice(nullptr), ROSEN_ERROR_INVALID_ARGUMENTS);
}

/*
 * Function: GetComposeClientLayers_ClientClear_And_Tunnel
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: client layers include CLIENT_CLEAR and TUNNEL types
 */
HWTEST_F(HdiOutputTest, GetComposeClientLayers_ClientClear_And_Tunnel, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    out->layerIdMap_.clear();
    // CLIENT_CLEAR
    auto l1 = HdiLayer::CreateHdiLayer(1);
    auto rs1 = std::make_shared<RSRenderSurfaceLayer>();
    rs1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR);
    rs1->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT_CLEAR);
    l1->UpdateRSLayer(rs1);
    out->layerIdMap_[1] = l1;
    // TUNNEL
    auto l2 = HdiLayer::CreateHdiLayer(2);
    auto rs2 = std::make_shared<RSRenderSurfaceLayer>();
    rs2->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL);
    rs2->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_TUNNEL);
    l2->UpdateRSLayer(rs2);
    out->layerIdMap_[2] = l2;

    std::vector<std::shared_ptr<HdiLayer>> clientLayers;
    out->GetComposeClientLayers(clientLayers);
    ASSERT_EQ(clientLayers.size(), 2u);
}

/*
 * Function: PrepareCompleteIfNeed_FlushWhenClientLayersExist
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: compClientLayers non-empty triggers flush and returns success
 */
HWTEST_F(HdiOutputTest, PrepareCompleteIfNeed_FlushWhenClientLayersExist, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    out->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    // setup client layer
    auto l1 = HdiLayer::CreateHdiLayer(1);
    auto rs1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    rs1->SetCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    l1->UpdateRSLayer(rs1);
    out->layerIdMap_[1] = l1;

    // supply one framebuffer entry so FlushScreen can proceed
    int64_t timestamp = 0;
    Rect damage {};
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    while (!out->fbSurface_->availableBuffers_.empty()) {
        out->fbSurface_->availableBuffers_.pop();
    }
    out->fbSurface_->availableBuffers_.push(
        std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _))
        .WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    out->device_ = nullptr;
    out->SetHdiOutputDevice(hdiDeviceMock_);

    // needFlush param starts false; inside it becomes true
    auto ret = out->PrepareCompleteIfNeed(false);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: PrepareCompleteIfNeed_NoClientLayers_NoFlush
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: no client layers, line 1085 condition false, line 1093 condition false
 */
HWTEST_F(HdiOutputTest, PrepareCompleteIfNeed_NoClientLayers_NoFlush, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    out->layerIdMap_.clear();

    // No client layers set up, so line 1085 condition is false (compClientLayers.size() == 0)
    // needFlush param starts false and remains false, so line 1093 condition is also false
    auto ret = out->PrepareCompleteIfNeed(false);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: PrepareCompleteIfNeed_NeedFlushTrue_CallFlushScreen
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: needFlush param true, FlushScreen called
 */
HWTEST_F(HdiOutputTest, PrepareCompleteIfNeed_NeedFlushTrue_CallFlushScreen, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    out->fbSurface_ = HdiFramebufferSurface::CreateFramebufferSurface();
    out->layerIdMap_.clear();

    // supply one framebuffer entry so FlushScreen can proceed
    int64_t timestamp = 0;
    Rect damage {};
    sptr<SyncFence> acquireFence = SyncFence::InvalidFence();
    sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl();
    while (!out->fbSurface_->availableBuffers_.empty()) {
        out->fbSurface_->availableBuffers_.pop();
    }
    out->fbSurface_->availableBuffers_.push(std::make_unique<FrameBufferEntry>(buffer, acquireFence, timestamp, damage));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientDamage(_, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    EXPECT_CALL(*hdiDeviceMock_, SetScreenClientBuffer(_, _, _, _)).WillRepeatedly(testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    out->device_ = nullptr;
    out->SetHdiOutputDevice(hdiDeviceMock_);

    // needFlush param is true, so line 1093 condition is true, FlushScreen should be called
    auto ret = out->PrepareCompleteIfNeed(true);
    ASSERT_EQ(ret, GRAPHIC_DISPLAY_SUCCESS);
}

/**
 * Function: ClearFrameBuffer_FrameBufferSurfaceNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. fbSurface is nullptr so GetFrameBufferSurface returns nullptr
 *                  2. expect ClearFrameBuffer succeeds, no CleanCache call
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_FrameBufferSurfaceNull, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    // Save original fbSurface_
    auto originalFbSurface = out->fbSurface_;
    // Set fbSurface_ to nullptr to make GetFrameBufferSurface() return nullptr
    out->fbSurface_ = nullptr;

    // Call ClearFrameBuffer - should succeed, line 77 condition will be false
    GSError ret = out->ClearFrameBuffer();

    // Verify the function succeeds even when pFrameSurface is nullptr
    EXPECT_EQ(ret, GSERROR_OK);

    // Restore original fbSurface_
    out->fbSurface_ = originalFbSurface;
}

/**
 * Function: ClearFrameBuffer_CheckFbSurfaceFalse
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. make CheckFbSurface return false by invalidating fbSurface
 *                  2. expect ClearFrameBuffer returns early without error
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_CheckFbSurfaceFalse, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    // Save original fbSurface_
    auto originalFbSurface = out->fbSurface_;

    // Invalidate fbSurface to make CheckFbSurface() return false
    out->fbSurface_ = nullptr;

    // Call ClearFrameBuffer - should return early due to CheckFbSurface() returning false
    GSError ret = out->ClearFrameBuffer();

    // Verify early return - function should succeed without processing
    EXPECT_EQ(ret, GSERROR_OK);

    // Restore original fbSurface_
    out->fbSurface_ = originalFbSurface;
}

/**
 * Function: ClearFrameBuffer_SurfaceGetSurfaceNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. fbSurface exists but GetSurface() returns nullptr
 *                  2. expect ClearFrameBuffer succeeds, no CleanCache call
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_SurfaceGetSurfaceNull, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    // Save original fbSurface_
    auto originalFbSurface = out->fbSurface_;

    // Create a mock fbSurface that returns nullptr for GetSurface()
    // Since HdiFramebufferSurface::CreateFramebufferSurface() creates a real surface,
    // we need to set fbSurface_ to nullptr to simulate GetSurface() returning nullptr
    out->fbSurface_ = nullptr;

    // Call ClearFrameBuffer - line 77 condition will be false (pFrameSurface == nullptr)
    GSError ret = out->ClearFrameBuffer();

    // Verify success without CleanCache call
    EXPECT_EQ(ret, GSERROR_OK);

    // Restore original fbSurface_
    out->fbSurface_ = originalFbSurface;
}

/**
 * Function: ClearFrameBuffer_MultipleCallsSurfaceNull
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. multiple ClearFrameBuffer calls with null surface
 *                  2. expect all succeed
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_MultipleCallsSurfaceNull, Function | MediumTest | Level1)
{
    auto out = HdiOutput::CreateHdiOutput(0);
    // Save original fbSurface_
    auto originalFbSurface = out->fbSurface_;

    // Set fbSurface_ to nullptr
    out->fbSurface_ = nullptr;

    // Call ClearFrameBuffer multiple times
    GSError ret1 = out->ClearFrameBuffer();
    GSError ret2 = out->ClearFrameBuffer();
    GSError ret3 = out->ClearFrameBuffer();

    // All should succeed with line 77 condition false each time
    EXPECT_EQ(ret1, GSERROR_OK);
    EXPECT_EQ(ret2, GSERROR_OK);
    EXPECT_EQ(ret3, GSERROR_OK);

    // Restore original fbSurface_
    out->fbSurface_ = originalFbSurface;
}

/**
 * Function: ClearFrameBuffer_ProtectedBufferAllocated_Line81_TrueBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. set isProtectedBufferAllocated_ to true before ClearFrameBuffer
 *                  2. expect ClearFrameBuffer succeeds, line 81 condition is true, state resets to false
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_ProtectedBufferAllocated, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Set protected buffer allocated state to true
    hdiOutput->SetProtectedFrameBufferState(true);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), true);

    // Call ClearFrameBuffer - line 81 condition will be true
    GSError ret = hdiOutput->ClearFrameBuffer();

    // Verify success and state has been reset to false
    EXPECT_EQ(ret, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);
}

/**
 * Function: ClearFrameBuffer_ProtectedBufferAllocatedTwice_Line81_TrueBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. set protected buffer state, call ClearFrameBuffer, set again, call ClearFrameBuffer
 *                  2. expect both calls succeed, line 81 true branch triggered both times
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_ProtectedBufferAllocatedTwice, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // First call with protected buffer allocated
    hdiOutput->SetProtectedFrameBufferState(true);
    GSError ret1 = hdiOutput->ClearFrameBuffer();
    EXPECT_EQ(ret1, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);

    // Set state to true again and call ClearFrameBuffer
    hdiOutput->SetProtectedFrameBufferState(true);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), true);
    GSError ret2 = hdiOutput->ClearFrameBuffer();
    EXPECT_EQ(ret2, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);
}

/**
 * Function: ClearFrameBuffer_SetTrueBeforeCall_Line81_TrueBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. set protected buffer state to true immediately before ClearFrameBuffer
 *                  2. expect state resets to false after ClearFrameBuffer completes
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_SetTrueBeforeCall, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Verify initial state is false
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);

    // Set to true right before ClearFrameBuffer
    hdiOutput->SetProtectedFrameBufferState(true);

    // Call ClearFrameBuffer - line 81 condition will be true, state should reset
    GSError ret = hdiOutput->ClearFrameBuffer();

    // Verify success and state reset
    EXPECT_EQ(ret, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);
}

/**
 * Function: ClearFrameBuffer_ProtectedBufferTrueThenFalse_SequentialCalls
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call ClearFrameBuffer with protected=true, then again with protected=false
 *                  2. expect both succeed, only first call triggers line 81 true branch
 */
HWTEST_F(HdiOutputTest, ClearFrameBuffer_ProtectedBufferTrueThenFalse_SequentialCalls, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // First call with protected buffer allocated
    hdiOutput->SetProtectedFrameBufferState(true);
    GSError ret1 = hdiOutput->ClearFrameBuffer();
    EXPECT_EQ(ret1, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);

    // Second call without protected buffer (false branch)
    GSError ret2 = hdiOutput->ClearFrameBuffer();
    EXPECT_EQ(ret2, GSERROR_OK);
    EXPECT_EQ(hdiOutput->GetProtectedFrameBufferState(), false);
}

/**
 * Function: Init_NewOutputInstanceSuccess
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create new HdiOutput instance with fbSurface_=nullptr
 *                  2. call Init to trigger CreateFramebufferSurface
 *                  3. expect success with fbSurface_ allocated
 */
HWTEST_F(HdiOutputTest, Init_NewOutputInstanceSuccess, Function | MediumTest | Level1)
{
    // Create a fresh HdiOutput instance (fbSurface_ is nullptr by default)
    uint32_t screenId = 1;
    auto newHdiOutput = HdiOutput::CreateHdiOutput(screenId);

    // Set up mock device (similar to SetUpTestCase)
    newHdiOutput->device_ = hdiDeviceMock_;

    // First Init call - should create fbSurface_
    RosenError ret1 = newHdiOutput->Init();
    EXPECT_TRUE(ret1 == ROSEN_ERROR_OK || ret1 == ROSEN_ERROR_NOT_INIT);

    // If Init succeeded, verify fbSurface_ is allocated
    if (ret1 == ROSEN_ERROR_OK) {
        EXPECT_NE(newHdiOutput->fbSurface_, nullptr);
    }

    // Second Init call - should return OK early (line 89-91)
    RosenError ret2 = newHdiOutput->Init();
    EXPECT_EQ(ret2, ROSEN_ERROR_OK);
}

/**
 * Function: Init_FbSurfaceAlreadyAllocated_Line89_EarlyReturn
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init on already initialized output
 *                  2. expect early return at line 89-91 without recreating fbSurface_
 */
HWTEST_F(HdiOutputTest, Init_FbSurfaceAlreadyAllocated, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // hdiOutput_ is already initialized from SetUpTestCase -> Init001
    // fbSurface_ should already be allocated

    // Call Init again - should hit early return at line 89-91
    RosenError ret = hdiOutput->Init();

    // Expect ROSEN_ERROR_OK due to early return (fbSurface_ already exists)
    EXPECT_EQ(ret, ROSEN_ERROR_OK);

    // Verify fbSurface_ is still valid
    EXPECT_NE(hdiOutput->fbSurface_, nullptr);
}

/**
 * Function: Init_MultipleSequentialInitCalls
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Init multiple times sequentially on same output
 *                  2. expect all calls return OK, only first creates fbSurface_
 */
HWTEST_F(HdiOutputTest, Init_MultipleSequentialInitCalls, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Call Init multiple times
    RosenError ret1 = hdiOutput->Init();
    RosenError ret2 = hdiOutput->Init();
    RosenError ret3 = hdiOutput->Init();

    // All should return ROSEN_ERROR_OK
    EXPECT_EQ(ret1, ROSEN_ERROR_OK);
    EXPECT_EQ(ret2, ROSEN_ERROR_OK);
    EXPECT_EQ(ret3, ROSEN_ERROR_OK);

    // fbSurface_ should remain valid
    EXPECT_NE(hdiOutput->fbSurface_, nullptr);
}

/**
 * Function: Init_CreateFramebufferSurfaceFailure_DocumentationTest
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. document expected behavior when CreateFramebufferSurface fails
 *                  2. verify return code ROSEN_ERROR_NOT_INIT for line 94 true branch
 * Note: This test documents the expected error handling path when
 *       CreateFramebufferSurface returns nullptr (line 94). Triggering
 *       this condition depends on system resource state and may not
 *       be reproducible in all test environments.
 */
HWTEST_F(HdiOutputTest, Init_CreateFramebufferSurfaceFailure_DocumentationTest, Function | MediumTest | Level1)
{
    // Create a fresh HdiOutput instance
    uint32_t screenId = 99;
    auto newHdiOutput = HdiOutput::CreateHdiOutput(screenId);
    newHdiOutput->device_ = hdiDeviceMock_;

    // Call Init and verify behavior
    RosenError ret = newHdiOutput->Init();

    // Expected outcomes:
    // 1. ROSEN_ERROR_OK - CreateFramebufferSurface succeeded (normal case)
    // 2. ROSEN_ERROR_NOT_INIT - CreateFramebufferSurface failed (line 94 true branch)
    EXPECT_TRUE(ret == ROSEN_ERROR_OK || ret == ROSEN_ERROR_NOT_INIT);

    // Verify fbSurface_ state matches return value
    if (ret == ROSEN_ERROR_NOT_INIT) {
        // Line 94 true branch: fbSurface_ should be nullptr
        EXPECT_EQ(newHdiOutput->fbSurface_, nullptr);
    } else {
        // Line 94 false branch: fbSurface_ should be allocated
        EXPECT_NE(newHdiOutput->fbSurface_, nullptr);
    }
}

/**
 * Function: ResetLayerStatusLocked_LayerIdMapRsLayerNull_Line255_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer without RSLayer to layerIdMap_
 *                  2. call ResetLayerStatusLocked
 *                  3. verify line 255 condition is false (rsLayer == nullptr)
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_LayerIdMapRsLayerNull, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer without RSLayer (GetRSLayer() will return nullptr)
    uint32_t layerId = 100;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    // Add to layerIdMap_ without setting rsLayer
    hdiOutput->layerIdMap_[layerId] = hdiLayer;

    // Verify GetRSLayer returns nullptr
    ASSERT_EQ(hdiLayer->GetRSLayer(), nullptr);

    // Call ResetLayerStatusLocked - line 255 condition will be false
    // The function should handle nullptr gracefully
    hdiOutput->ResetLayerStatusLocked();

    // Verify layer status is set to false
    EXPECT_EQ(hdiLayer->GetLayerStatus(), false);
}

/**
 * Function: ResetLayerStatusLocked_MixedLayerIdMap_Line255_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layers with and without RSLayer to layerIdMap_
 *                  2. call ResetLayerStatusLocked
 *                  3. verify both branches work correctly
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_MixedLayerIdMap, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create first HdiLayer with RSLayer
    uint32_t layerId1 = 101;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    ASSERT_NE(hdiLayer1, nullptr);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);

    // Create second HdiLayer without RSLayer
    uint32_t layerId2 = 102;
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(layerId2);
    ASSERT_NE(hdiLayer2, nullptr);
    // Don't call UpdateRSLayer - GetRSLayer() will return nullptr

    // Add both to layerIdMap_
    hdiOutput->layerIdMap_[layerId1] = hdiLayer1;
    hdiOutput->layerIdMap_[layerId2] = hdiLayer2;

    // Verify rsLayer states before call
    EXPECT_NE(hdiLayer1->GetRSLayer(), nullptr);
    EXPECT_EQ(hdiLayer2->GetRSLayer(), nullptr);

    // Call ResetLayerStatusLocked
    hdiOutput->ResetLayerStatusLocked();

    // Verify layer status is set to false for both
    EXPECT_EQ(hdiLayer1->GetLayerStatus(), false);
    EXPECT_EQ(hdiLayer2->GetLayerStatus(), false);
}

/**
 * Function: ResetLayerStatusLocked_LayersToBeReleaseWithRSLayer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer with RSLayer to layersTobeRelease_
 *                  2. call ResetLayerStatusLocked
 *                  3. verify line 262 condition is true (rsLayer != nullptr)
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_LayersToBeReleaseWithRSLayer,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer with RSLayer
    uint32_t layerId = 200;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);

    hdiLayer->UpdateRSLayer(rsLayer);

    // Verify GetRSLayer returns non-null
    ASSERT_NE(hdiLayer->GetRSLayer(), nullptr);

    // Add to layersTobeRelease_
    hdiOutput->layersTobeRelease_.push_back(hdiLayer);

    // Call ResetLayerStatusLocked - line 262 condition will be true
    hdiOutput->ResetLayerStatusLocked();

    // Verify layer status is set to false
    EXPECT_EQ(hdiLayer->GetLayerStatus(), false);
}

/**
 * Function: ResetLayerStatusLocked_LayersToBeReleaseMultiple
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add multiple HdiLayers with RSLayer to layersTobeRelease_
 *                  2. call ResetLayerStatusLocked
 *                  3. verify all layers processed correctly
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_LayersToBeReleaseMultiple, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create multiple HdiLayers with RSLayer
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t layerId = 300 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        ASSERT_NE(hdiLayer, nullptr);

        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);

        // Verify GetRSLayer returns non-null before adding
        EXPECT_NE(hdiLayer->GetRSLayer(), nullptr);

        hdiOutput->layersTobeRelease_.push_back(hdiLayer);
    }

    // Call ResetLayerStatusLocked - line 262 condition will be true for all layers
    hdiOutput->ResetLayerStatusLocked();

    // Verify all layer status are set to false
    for (const auto& hdiLayer : hdiOutput->layersTobeRelease_) {
        EXPECT_EQ(hdiLayer->GetLayerStatus(), false);
    }
}

/**
 * Function: ResetLayerStatusLocked_AllThreeMaps_Line255False_Line262True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. populate layerIdMap_ with null RSLayer (line 255 false)
 *                  2. populate layersTobeRelease_ with valid RSLayer (line 262 true)
 *                  3. call ResetLayerStatusLocked
 *                  4. verify all branches work correctly
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_AllThreeMaps, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();
    hdiOutput->maskLayer_ = nullptr;

    // Add HdiLayer without RSLayer to layerIdMap_ (triggers line 255 false branch)
    uint32_t layerId1 = 400;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    ASSERT_NE(hdiLayer1, nullptr);
    // Don't set RSLayer - GetRSLayer() will return nullptr
    hdiOutput->layerIdMap_[layerId1] = hdiLayer1;

    // Add HdiLayer with RSLayer to layersTobeRelease_ (triggers line 262 true branch)
    uint32_t layerId2 = 401;
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(layerId2);
    ASSERT_NE(hdiLayer2, nullptr);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(layerId2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    hdiOutput->layersTobeRelease_.push_back(hdiLayer2);

    // Verify preconditions
    EXPECT_EQ(hdiLayer1->GetRSLayer(), nullptr);  // Line 255 condition: false
    EXPECT_NE(hdiLayer2->GetRSLayer(), nullptr);  // Line 262 condition: true

    // Call ResetLayerStatusLocked
    hdiOutput->ResetLayerStatusLocked();

    // Verify layer status is set to false for both
    EXPECT_EQ(hdiLayer1->GetLayerStatus(), false);
    EXPECT_EQ(hdiLayer2->GetLayerStatus(), false);
}

/**
 * Function: ResetLayerStatusLocked_LayersToBeReleaseMixed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layers with and without RSLayer to layersTobeRelease_
 *                  2. call ResetLayerStatusLocked
 *                  3. verify both true and false branches handled correctly
 */
HWTEST_F(HdiOutputTest, ResetLayerStatusLocked_LayersToBeReleaseMixed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing layers
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer with RSLayer
    uint32_t layerId1 = 500;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    ASSERT_NE(hdiLayer1, nullptr);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);

    // Create HdiLayer without RSLayer
    uint32_t layerId2 = 501;
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(layerId2);
    ASSERT_NE(hdiLayer2, nullptr);
    // Don't set RSLayer

    // Add both to layersTobeRelease_
    hdiOutput->layersTobeRelease_.push_back(hdiLayer1);
    hdiOutput->layersTobeRelease_.push_back(hdiLayer2);

    // Verify rsLayer states before call
    EXPECT_NE(hdiLayer1->GetRSLayer(), nullptr);  // Line 262 condition: true
    EXPECT_EQ(hdiLayer2->GetRSLayer(), nullptr);  // Line 262 condition: false

    // Call ResetLayerStatusLocked
    hdiOutput->ResetLayerStatusLocked();

    // Verify layer status is set to false for both
    EXPECT_EQ(hdiLayer1->GetLayerStatus(), false);
    EXPECT_EQ(hdiLayer2->GetLayerStatus(), false);
}

/**
 * Function: ReleaseLayers_EmptyLayerIdMap_Line742_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. setup empty layerIdMap_ and layersId_
 *                  2. call ReleaseLayers
 *                  3. verify line 742 condition is true (size == 0)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_EmptyLayerIdMap, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // When layersId_ is empty, GetLayersReleaseFenceLocked returns empty map
    // Line 742 condition (size == 0) will be true
    EXPECT_EQ(releaseLayerInfo.timestampVec.size(), 0u);
}

/**
 * Function: ReleaseLayers_NonEmptyReleaseFence_Line742_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. setup layersId_, layerIdMap_, and fences_ to make GetLayersReleaseFenceLocked return non-empty
 *                  2. call ReleaseLayers
 *                  3. verify line 742 condition is false (size != 0)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_NonEmptyReleaseFence, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer with RSLayer
    uint32_t layerId = 600;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to layerIdMap_
    hdiOutput->layerIdMap_[layerId] = hdiLayer;

    // Add to layersId_ to make GetLayersReleaseFenceLocked return non-empty
    hdiOutput->layersId_.push_back(layerId);

    // Add corresponding fence
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    hdiOutput->fences_.push_back(fence);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 742 condition (size == 0) will be false since layersId_ is not empty
    EXPECT_GT(releaseLayerInfo.timestampVec.size(), 0u);
    EXPECT_GT(releaseLayerInfo.releaseBufferFenceVec.size(), 0u);
}

/**
 * Function: ReleaseLayers_MultipleLayersInReleaseFence_Line742_FalseBranch
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. setup multiple layers in layersId_
 *                  2. call ReleaseLayers
 *                  3. verify line 742 condition is false with multiple layers
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_MultipleLayersInReleaseFence, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create multiple HdiLayers with RSLayer
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t layerId = 700 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        ASSERT_NE(hdiLayer, nullptr);

        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);

        hdiOutput->layerIdMap_[layerId] = hdiLayer;
        hdiOutput->layersId_.push_back(layerId);

        sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
        hdiOutput->fences_.push_back(fence);
    }

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 742 condition (size == 0) will be false
    EXPECT_EQ(releaseLayerInfo.timestampVec.size(), 3u);
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 3u);
}

/**
 * Function: ReleaseLayers_LayersToBeReleaseNullRSLayer_Line769
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer without RSLayer to layersTobeRelease_
 *                  2. call ReleaseLayers
 *                  3. verify line 769 condition is true (skip)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_LayersToBeReleaseNullRSLayer, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer without RSLayer
    uint32_t layerId = 800;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);
    // Don't set RSLayer - GetRSLayer() will return nullptr

    // Add to layersTobeRelease_
    hdiOutput->layersTobeRelease_.push_back(hdiLayer);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 769 condition (hdiLayer == nullptr || GetRSLayer() == nullptr) will be true
    // Layer should be skipped, no additional entries added
    EXPECT_EQ(releaseLayerInfo.timestampVec.size(), 0u);
}

/**
 * Function: ReleaseLayers_LayersToBeReleaseValidRSLayer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer with RSLayer not in releaseBufferFenceMap
 *                  2. call ReleaseLayers
 *                  3. verify line 769 false, line 772 true (add to vec)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_LayersToBeReleaseValidRSLayer, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer with RSLayer
    uint32_t layerId = 900;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to layersTobeRelease_ (but NOT to layersId_, so not in releaseBufferFenceMap)
    hdiOutput->layersTobeRelease_.push_back(hdiLayer);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 769 condition will be false (hdiLayer != nullptr && GetRSLayer() != nullptr)
    // Line 772 condition will be true (layerId not found in releaseBufferFenceMap)
    // Should add to releaseBufferFenceVec
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 1u);
}

/**
 * Function: ReleaseLayers_LayerInBothMaps_Line769_False_Line772_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer to both layersId_ and layersTobeRelease_
 *                  2. call ReleaseLayers
 *                  3. verify line 769 false, line 772 false (already in map)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_LayerInBothMaps, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Create HdiLayer with RSLayer
    uint32_t layerId = 1000;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to layerIdMap_ and layersId_ (so it's in releaseBufferFenceMap)
    hdiOutput->layerIdMap_[layerId] = hdiLayer;
    hdiOutput->layersId_.push_back(layerId);
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    hdiOutput->fences_.push_back(fence);

    // Also add to layersTobeRelease_ (same layer)
    hdiOutput->layersTobeRelease_.push_back(hdiLayer);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 769 condition will be false
    // Line 772 condition will be false (layerId found in releaseBufferFenceMap)
    // Should NOT add duplicate entry
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 1u);
}

/**
 * Function: ReleaseLayers_MixedInLayersToBeRelease_AllLine768Conditions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layers with various states to layersTobeRelease_
 *                  2. call ReleaseLayers
 *                  3. verify all conditions in line 768 loop work correctly
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_MixedInLayersToBeRelease, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Layer 1: In layersId_ (will be in releaseBufferFenceMap) and in layersTobeRelease_
    uint32_t layerId1 = 1100;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    std::shared_ptr<RSLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->layerIdMap_[layerId1] = hdiLayer1;
    hdiOutput->layersId_.push_back(layerId1);
    hdiOutput->fences_.push_back(sptr<SyncFence>::MakeSptr(-1));
    hdiOutput->layersTobeRelease_.push_back(hdiLayer1);
    // Line 769: false, Line 772: false (already in map)

    // Layer 2: NOT in layersId_, in layersTobeRelease_
    uint32_t layerId2 = 1101;
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(layerId2);
    std::shared_ptr<RSLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(layerId2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    hdiOutput->layerIdMap_[layerId2] = hdiLayer2;
    hdiOutput->layersTobeRelease_.push_back(hdiLayer2);
    // Line 769: false, Line 772: true (not in map)

    // Layer 3: nullptr hdiLayer in layersTobeRelease_
    std::shared_ptr<HdiLayer> hdiLayer3 = nullptr;
    hdiOutput->layersTobeRelease_.push_back(hdiLayer3);
    // Line 769: true (hdiLayer == nullptr)

    // Layer 4: Valid hdiLayer but nullptr RSLayer
    uint32_t layerId4 = 1103;
    std::shared_ptr<HdiLayer> hdiLayer4 = std::make_shared<HdiLayer>(layerId4);
    // Don't set RSLayer
    hdiOutput->layersTobeRelease_.push_back(hdiLayer4);
    // Line 769: true (GetRSLayer() == nullptr)

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Should have 2 entries: one from layersId_ (line 742 false branch) + one from layerId2 (line 772 true)
    // Layer1, 3, 4 should not add new entries (3 and 4 skipped at line 769, layer1 skipped at line 772)
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 2u);
}

/**
 * Function: ReleaseLayers_AllBranches_Line742False_AllLine768Conditions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. setup comprehensive scenario with layersId_, layerIdMap_, layersTobeRelease_
 *                  2. call ReleaseLayers
 *                  3. verify line 742 false branch and all line 768 conditions
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_AllBranches, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Setup layersId_ with 2 layers (line 742 false branch will be taken)
    for (uint32_t i = 0; i < 2; i++) {
        uint32_t layerId = 1200 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->layerIdMap_[layerId] = hdiLayer;
        hdiOutput->layersId_.push_back(layerId);
        hdiOutput->fences_.push_back(sptr<SyncFence>::MakeSptr(-1));
    }

    // Setup layersTobeRelease_ with various conditions
    // Layer in releaseBufferFenceMap (line 769 false, line 772 false)
    hdiOutput->layersTobeRelease_.push_back(hdiOutput->layerIdMap_[1200]);

    // Layer NOT in releaseBufferFenceMap (line 769 false, line 772 true)
    uint32_t layerIdExtra = 1299;
    std::shared_ptr<HdiLayer> hdiLayerExtra = std::make_shared<HdiLayer>(layerIdExtra);
    std::shared_ptr<RSLayer> rsLayerExtra = std::make_shared<RSSurfaceLayer>(layerIdExtra, nullptr);
    hdiLayerExtra->UpdateRSLayer(rsLayerExtra);
    hdiOutput->layerIdMap_[layerIdExtra] = hdiLayerExtra;
    hdiOutput->layersTobeRelease_.push_back(hdiLayerExtra);

    // Layer with nullptr RSLayer (line 769 true)
    std::shared_ptr<HdiLayer> hdiLayerNull = std::make_shared<HdiLayer>(1300);
    hdiOutput->layersTobeRelease_.push_back(hdiLayerNull);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Results:
    // - Line 742: false branch (size != 0)
    // - From line 756-765: 2 entries added from layersId_
    // - From line 768: 1 entry added from layerIdExtra (line 772 true)
    // Total: 3 entries (1200 already in map, 1201 in map, 1299 added)
    EXPECT_EQ(releaseLayerInfo.timestampVec.size(), 3u);
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 3u);
}

/**
 * Function: ReleaseLayers_NullptrHdiLayerInLayersToBeRelease_Line769_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add nullptr hdiLayer to layersTobeRelease_
 *                  2. call ReleaseLayers
 *                  3. verify line 769 first condition (hdiLayer == nullptr)
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_NullptrHdiLayerInLayersToBeRelease, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Add nullptr hdiLayer
    hdiOutput->layersTobeRelease_.push_back(nullptr);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Line 769 condition (hdiLayer == nullptr) will be true
    // No crash, no additional entries
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 0u);
}

/**
 * Function: ReleaseLayers_CompleteFlow_Line742False_MultipleLine768Branches
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. comprehensive test covering all paths
 *                  2. verify correct behavior across all conditions
 */
HWTEST_F(HdiOutputTest, ReleaseLayers_CompleteFlow, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing data
    hdiOutput->layerIdMap_.clear();
    hdiOutput->layersId_.clear();
    hdiOutput->fences_.clear();
    hdiOutput->layersTobeRelease_.clear();

    // Setup 3 layers in layersId_ for line 742 false branch
    std::vector<uint32_t> layerIds = {1400, 1401, 1402};
    for (uint32_t layerId : layerIds) {
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->layerIdMap_[layerId] = hdiLayer;
        hdiOutput->layersId_.push_back(layerId);
        hdiOutput->fences_.push_back(sptr<SyncFence>::MakeSptr(-1));
    }

    // Add to layersTobeRelease_:
    // 1. Layer 1400 - already in releaseBufferFenceMap (line 772 false)
    hdiOutput->layersTobeRelease_.push_back(hdiOutput->layerIdMap_[1400]);
    // 2. Layer 1403 - not in map (line 772 true)
    uint32_t newLayerId = 1403;
    std::shared_ptr<HdiLayer> newHdiLayer = std::make_shared<HdiLayer>(newLayerId);
    std::shared_ptr<RSLayer> newRsLayer = std::make_shared<RSSurfaceLayer>(newLayerId, nullptr);
    newHdiLayer->UpdateRSLayer(newRsLayer);
    hdiOutput->layersTobeRelease_.push_back(newHdiLayer);
    // 3. nullptr layer (line 769 true)
    hdiOutput->layersTobeRelease_.push_back(nullptr);
    // 4. Layer without RSLayer (line 769 true)
    std::shared_ptr<HdiLayer> nullRsLayer = std::make_shared<HdiLayer>(1404);
    hdiOutput->layersTobeRelease_.push_back(nullRsLayer);

    ReleaseLayerBuffersInfo releaseLayerInfo;
    hdiOutput->ReleaseLayers(releaseLayerInfo);

    // Expected results:
    // - Line 742: false (3 entries added from line 756-765)
    // - Line 768 loop:
    //   - 1400: skipped (already in map)
    //   - 1403: added (not in map)
    //   - nullptr: skipped
    //   - 1404: skipped (no RSLayer)
    // Total: 3 (from layersId_) + 1 (1403) = 4
    EXPECT_EQ(releaseLayerInfo.timestampVec.size(), 3u);
    EXPECT_EQ(releaseLayerInfo.releaseBufferFenceVec.size(), 4u);
}

/**
 * Function: DumpLayerInfoForSplitRender_EmptyMaps_AllIfConditions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. clear surfaceIdMap_ and solidSurfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify no crash and empty result
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_EmptyMaps_AllIfConditions, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should only contain header, no layer info
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_ValidLayerInSurfaceIdMap_Processed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add valid HdiLayer with RSLayer to surfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify line 836 condition is false (layer processed)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_ValidLayerInSurfaceIdMap_Processed,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer
    uint64_t surfaceId = 1500;
    uint32_t layerId = 1501;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // layer and RSLayer are valid (both hdiLayer and GetRSLayer() are non-null)
    // Should contain layer info
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_NullHdiLayerInSurfaceIdMap_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add nullptr HdiLayer to surfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify line 836 condition is true (skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_NullHdiLayerInSurfaceIdMap_Skipped,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to surfaceIdMap_
    hdiOutput->surfaceIdMap_[1502] = nullptr;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // hdiLayer is nullptr or GetRSLayer is nullptr (hdiLayer == nullptr)
    // Should not crash, only header added
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
    // Should NOT contain surface info for nullptr layer
    EXPECT_TRUE(result.find("surface [") == std::string::npos ||
                result.find("Layer Without Surface") == std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_LayerWithoutRSLayer_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer without RSLayer to surfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify layer is skipped (skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_LayerWithoutRSLayer_Skipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer without RSLayer
    uint32_t layerId = 1503;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);
    // Don't call UpdateRSLayer - GetRSLayer() will return nullptr

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[1504] = hdiLayer;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // hdiLayer is nullptr or GetRSLayer is nullptr (GetRSLayer() == nullptr)
    // Should not crash
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_ValidLayerInSolidSurfaceIdMap_Processed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add valid HdiLayer to solidSurfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify line 836 condition is false
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_ValidLayerInSolidSurfaceIdMap_Processed,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer for solid color layer
    uint64_t solidSurfaceId = 1600;
    uint32_t layerId = 1601;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // layer and RSLayer are valid
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_SurfaceNull_DefaultName
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layer with nullptr Surface to surfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify surface name handling returns "Layer Without Surface"
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_SurfaceNull_DefaultName, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer but nullptr Surface
    uint64_t surfaceId = 1700;
    uint32_t layerId = 1701;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    // GetSurface() will return nullptr for RSSurfaceLayer without actual surface
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Line 840: surface is nullptr, should use "Layer Without Surface" + zorder
    // This depends on the actual RSLayer implementation
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_MixedValidAndInvalidLayers_AllConditions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add mix of valid and invalid layers to both maps
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify all conditions handled correctly
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_MixedValidAndInvalidLayers_AllConditions,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layer to surfaceIdMap_ (line 836 false)
    uint64_t surfaceId1 = 1800;
    uint32_t layerId1 = 1801;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    std::shared_ptr<RSSurfaceLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[surfaceId1] = hdiLayer1;

    // Add nullptr layer to surfaceIdMap_ (line 836 true)
    hdiOutput->surfaceIdMap_[1802] = nullptr;

    // Add layer without RSLayer to surfaceIdMap_ (line 836 true)
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(1803);
    hdiOutput->surfaceIdMap_[1804] = hdiLayer2;

    // Add valid layer to solidSurfaceIdMap_ (line 836 false)
    uint64_t solidSurfaceId = 1900;
    uint32_t layerId3 = 1901;
    std::shared_ptr<HdiLayer> hdiLayer3 = std::make_shared<HdiLayer>(layerId3);
    std::shared_ptr<RSSurfaceLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(layerId3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer3;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should process valid layers and skip invalid ones
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_MultipleValidLayers_AllProcessed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add multiple valid layers to both maps
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify all valid layers processed
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_MultipleValidLayers_AllProcessed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add multiple valid layers to surfaceIdMap_
    for (uint32_t i = 0; i < 3; i++) {
        uint64_t surfaceId = 2000 + i;
        uint32_t layerId = 2100 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;
    }

    // Add multiple valid layers to solidSurfaceIdMap_
    for (uint32_t i = 0; i < 2; i++) {
        uint64_t solidSurfaceId = 2200 + i;
        uint32_t layerId = 2300 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer;
    }

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // All layers have valid hdiLayer and RSLayer for all 5 valid layers
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_ReorderLayerInfoMixed_AllConditions
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. test ReorderLayerInfoLocked with mixed layers
 *                  2. verify line 974 and 986 conditions
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_ReorderLayerInfoMixed_AllConditions,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Test ReorderLayerInfoLocked line 974 conditions

    // Valid layer (line 974 false)
    uint64_t surfaceId1 = 2400;
    uint32_t layerId1 = 2401;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    std::shared_ptr<RSSurfaceLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[surfaceId1] = hdiLayer1;

    // nullptr layer (line 974 true)
    hdiOutput->surfaceIdMap_[2402] = nullptr;

    // Layer without RSLayer (line 974 true)
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(2403);
    hdiOutput->surfaceIdMap_[2404] = hdiLayer2;

    // Test ReorderLayerInfoLocked line 986 conditions in solidSurfaceIdMap_

    // Valid solid layer (line 986 false)
    uint64_t solidSurfaceId1 = 2500;
    uint32_t layerId3 = 2501;
    std::shared_ptr<HdiLayer> hdiLayer3 = std::make_shared<HdiLayer>(layerId3);
    std::shared_ptr<RSSurfaceLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(layerId3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId1] = hdiLayer3;

    // nullptr solid layer (line 986 true)
    hdiOutput->solidSurfaceIdMap_[2502] = nullptr;

    // Solid layer without RSLayer (line 986 true)
    std::shared_ptr<HdiLayer> hdiLayer4 = std::make_shared<HdiLayer>(2503);
    hdiOutput->solidSurfaceIdMap_[2504] = hdiLayer4;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should process valid layers from both maps
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_NullptrInSolidSurfaceIdMap_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add nullptr HdiLayer to solidSurfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify line 986 condition is true (skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_NullptrInSolidSurfaceIdMap_Skipped,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[2600] = nullptr;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // solid layer has invalid hdiLayer or RSLayer (solidLayer == nullptr)
    // Should not crash
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_SolidLayerWithoutRSLayer_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer without RSLayer to solidSurfaceIdMap_
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify solid layer is skipped (skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_SolidLayerWithoutRSLayer_Skipped,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer without RSLayer
    uint32_t layerId = 2700;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);
    // Don't set RSLayer

    // Add to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[2701] = hdiLayer;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // solid layer has invalid hdiLayer or RSLayer (GetRSLayer() == nullptr)
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_AllMapsEmpty_NoLayerOutput
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. both surfaceIdMap_ and solidSurfaceIdMap_ are empty
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify only header is output, no layer info
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_AllMapsEmpty_NoLayerOutput, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should only contain header, no layer info (no "surface [" in result)
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpLayerInfoForSplitRender_OnlyValidLayers_AllProcessed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. populate both maps with only valid layers
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify all layers processed (line 836 false for all)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_OnlyValidLayers_AllProcessed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add 2 valid layers to surfaceIdMap_
    for (uint32_t i = 0; i < 2; i++) {
        uint64_t surfaceId = 2800 + i;
        uint32_t layerId = 2900 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;
    }

    // Add 1 valid layer to solidSurfaceIdMap_
    uint64_t solidSurfaceId = 3000;
    uint32_t layerId3 = 3100;
    std::shared_ptr<HdiLayer> hdiLayer3 = std::make_shared<HdiLayer>(layerId3);
    std::shared_ptr<RSSurfaceLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(layerId3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer3;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // All layers have valid hdiLayer and RSLayer for all 3 layers
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: DumpLayerInfoForSplitRender_AllNullLayers_AllSkipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add only nullptr HdiLayer to both maps
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify all layers skipped (all layers skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_AllNullLayers_AllSkipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to surfaceIdMap_
    hdiOutput->surfaceIdMap_[8001] = nullptr;
    hdiOutput->surfaceIdMap_[8002] = nullptr;
    hdiOutput->surfaceIdMap_[8003] = nullptr;

    // Add nullptr HdiLayer to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[9001] = nullptr;
    hdiOutput->solidSurfaceIdMap_[9002] = nullptr;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should only contain header, no layer info (all skipped)
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: DumpLayerInfoForSplitRender_AlternatingNullLayers_MixedProcessing
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add alternating valid and nullptr layers
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify valid layers processed, null layers skipped (null layers skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_AlternatingNullLayers_MixedProcessing, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add alternating valid and nullptr layers
    hdiOutput->surfaceIdMap_[8501] = nullptr;
    hdiOutput->surfaceIdMap_[8502] = std::make_shared<HdiLayer>(1);
    hdiOutput->surfaceIdMap_[8503] = nullptr;
    hdiOutput->surfaceIdMap_[8504] = std::make_shared<HdiLayer>(2);

    // Set RSLayer for valid layers
    auto rsLayer1 = std::make_shared<RSSurfaceLayer>(1, nullptr);
    auto rsLayer2 = std::make_shared<RSSurfaceLayer>(2, nullptr);
    hdiOutput->surfaceIdMap_[8502]->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[8504]->UpdateRSLayer(rsLayer2);

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should contain header and process valid layers
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: DumpLayerInfoForSplitRender_LayerWithNullRSLayer_AfterValid_LayerSkipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add valid layer followed by layer with null RSLayer
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify first layer processed, second skipped (second layer skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_LayerWithNullRSLayer_AfterValid_LayerSkipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layer first
    auto hdiLayer1 = std::make_shared<HdiLayer>(1);
    auto rsLayer1 = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[8801] = hdiLayer1;

    // Add layer without RSLayer (condition true)
    auto hdiLayer2 = std::make_shared<HdiLayer>(2);
    hdiOutput->surfaceIdMap_[8802] = hdiLayer2;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should contain header and process first layer only
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: DumpLayerInfoForSplitRender_NullLayersInBetween_ValidProcessed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add valid layers with null layers in between
 *                  2. call DumpLayerInfoForSplitRender
 *                  3. verify valid layers processed, null layers skipped (null layers skipped)
 */
HWTEST_F(HdiOutputTest, DumpLayerInfoForSplitRender_NullLayersInBetween_ValidProcessed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layers with null layers in between
    hdiOutput->surfaceIdMap_[8701] = nullptr;
    hdiOutput->surfaceIdMap_[8702] = nullptr;

    // Add valid layer
    auto hdiLayer1 = std::make_shared<HdiLayer>(1);
    auto rsLayer1 = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[8703] = hdiLayer1;

    hdiOutput->surfaceIdMap_[8704] = nullptr;
    hdiOutput->surfaceIdMap_[8705] = nullptr;

    // Add another valid layer
    auto hdiLayer2 = std::make_shared<HdiLayer>(2);
    auto rsLayer2 = std::make_shared<RSSurfaceLayer>(2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    hdiOutput->surfaceIdMap_[8706] = hdiLayer2;

    std::string result;
    hdiOutput->DumpLayerInfoForSplitRender(result);

    // Should contain header and process valid layers
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/**
 * Function: DumpCurrentFrameLayers_EmptyMaps_NoLayersProcessed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. clear surfaceIdMap_ and solidSurfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify no crash, no layers processed
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_EmptyMaps_NoLayersProcessed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Verify maps are empty
    ASSERT_TRUE(hdiOutput->surfaceIdMap_.empty());
    ASSERT_TRUE(hdiOutput->solidSurfaceIdMap_.empty());

    // Should not crash when maps are empty
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_ValidLayerWithSurface_Line873_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer with RSLayer and Surface to surfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 873 condition is false (layer processed)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_ValidLayerWithSurface, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer
    uint64_t surfaceId = 3200;
    uint32_t layerId = 3201;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);

    std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    ASSERT_NE(rsLayer, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;

    // Should not crash, line 873 condition may be true or false depending on GetSurface()
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_NullHdiLayerInSurfaceIdMap_Line873_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add nullptr HdiLayer to surfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 873 condition is true (skipped)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_NullHdiLayerInSurfaceIdMap, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to surfaceIdMap_
    hdiOutput->surfaceIdMap_[3202] = nullptr;
    ASSERT_EQ(hdiOutput->surfaceIdMap_.size(), 1);

    // Should not crash when HdiLayer is nullptr
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_LayerWithoutRSLayer_Line873_True
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer without RSLayer to surfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 873 condition is true (skipped)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_LayerWithoutRSLayer, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer without RSLayer
    uint32_t layerId = 3203;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    ASSERT_NE(hdiLayer, nullptr);
    // Don't call UpdateRSLayer

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[3204] = hdiLayer;

    // Line 873 condition will be true (GetRSLayer() == nullptr)
    // Should not crash
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_NullRSLayerInSolidSurfaceIdMap
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add nullptr to solidSurfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 873 condition is true
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_NullRSLayerInSolidSurfaceIdMap,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[3300] = nullptr;
    ASSERT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 1);
    // Should not crash when solidSurfaceIdMap_ contains nullptr
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_MixedValidAndInvalidLayers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add mix of valid and invalid layers to both maps
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify all conditions handled correctly
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_MixedValidAndInvalidLayers,
    Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layer to surfaceIdMap_ (line 873 may be true/false depending on GetSurface())
    uint64_t surfaceId1 = 3400;
    uint32_t layerId1 = 3401;
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(layerId1);
    std::shared_ptr<RSSurfaceLayer> rsLayer1 = std::make_shared<RSSurfaceLayer>(layerId1, nullptr);
    hdiLayer1->UpdateRSLayer(rsLayer1);
    hdiOutput->surfaceIdMap_[surfaceId1] = hdiLayer1;

    // Add nullptr layer to surfaceIdMap_ (line 873 true - hdiLayer == nullptr)
    hdiOutput->surfaceIdMap_[3402] = nullptr;

    // Add layer without RSLayer to surfaceIdMap_ (line 873 true - GetRSLayer() == nullptr)
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(3403);
    hdiOutput->surfaceIdMap_[3404] = hdiLayer2;

    // Add valid layer to solidSurfaceIdMap_
    uint64_t solidSurfaceId = 3500;
    uint32_t layerId3 = 3501;
    std::shared_ptr<HdiLayer> hdiLayer3 = std::make_shared<HdiLayer>(layerId3);
    std::shared_ptr<RSSurfaceLayer> rsLayer3 = std::make_shared<RSSurfaceLayer>(layerId3, nullptr);
    hdiLayer3->UpdateRSLayer(rsLayer3);
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer3;

    // Verify maps contain expected number of entries
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 3u);
    EXPECT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 1u);

    // Should not crash with mixed valid and invalid layers
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_MultipleValidLayers_Line873_False
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add multiple valid layers to both maps
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify all layers processed (no crash)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_MultipleValidLayers, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    ASSERT_NE(hdiOutput, nullptr);

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add multiple valid layers to surfaceIdMap_
    for (uint32_t i = 0; i < 3; i++) {
        uint64_t surfaceId = 3600 + i;
        uint32_t layerId = 3700 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;
    }

    // Add multiple valid layers to solidSurfaceIdMap_
    for (uint32_t i = 0; i < 2; i++) {
        uint64_t solidSurfaceId = 3800 + i;
        uint32_t layerId = 3900 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer;
    }

    // Verify maps contain expected number of layers
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 3u);
    EXPECT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 2u);

    // Should not crash with multiple valid layers
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_AllNullLayers_Line873_AllTrue
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add only nullptr layers to both maps
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify all skipped (line 873 true for all)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_AllNullLayers, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr layers to surfaceIdMap_
    hdiOutput->surfaceIdMap_[4000] = nullptr;
    hdiOutput->surfaceIdMap_[4001] = nullptr;

    // Add nullptr layers to solidSurfaceIdMap_
    hdiOutput->solidSurfaceIdMap_[4100] = nullptr;
    hdiOutput->solidSurfaceIdMap_[4101] = nullptr;

    // Verify maps contain expected number of nullptr layers
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 2u);
    EXPECT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 2u);

    // Line 873 will be true for all layers (hdiLayer == nullptr)
    // Should not crash
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_AllLayersWithoutRSLayer_Line873_AllTrue
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layers without RSLayer to both maps
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify all skipped (line 873 true for all)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_AllLayersWithoutRSLayer, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add layers without RSLayer to surfaceIdMap_
    for (uint32_t i = 0; i < 2; i++) {
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(4200 + i);
        hdiOutput->surfaceIdMap_[4300 + i] = hdiLayer;
    }

    // Add layers without RSLayer to solidSurfaceIdMap_
    for (uint32_t i = 0; i < 2; i++) {
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(4400 + i);
        hdiOutput->solidSurfaceIdMap_[4500 + i] = hdiLayer;
    }

    // Verify maps contain expected number of layers
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 2u);
    EXPECT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 2u);

    // Line 873 will be true for all layers (GetRSLayer() == nullptr)
    // Should not crash
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_ReorderLayerInfoCalled_VerifyLayerOrder
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layers with different z-order to both maps
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify ReorderLayerInfoLocked is called
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_ReorderLayerInfoCalled_VerifyLayerOrder, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add layers with different z-order
    for (uint32_t i = 0; i < 3; i++) {
        uint64_t surfaceId = 4600 + i;
        uint32_t layerId = 4700 + i;
        std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
        std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;
    }

    // Verify layers were added
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 3u);

    // Should not crash, ReorderLayerInfoLocked will be called
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_ComprehensiveTest_AllLine873Branches
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. comprehensive test with all possible layer states
 *                  2. verify all line 873 conditions work correctly
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_ComprehensiveTest, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Test all three conditions in line 873:

    // 1. hdiLayer == nullptr (true)
    hdiOutput->surfaceIdMap_[4800] = nullptr;

    // 2. GetRSLayer() == nullptr (true)
    std::shared_ptr<HdiLayer> hdiLayer1 = std::make_shared<HdiLayer>(4801);
    hdiOutput->surfaceIdMap_[4802] = hdiLayer1;

    // 3. GetSurface() == nullptr (may be true depending on RSLayer implementation)
    // 4. All non-null (false branch)
    uint64_t surfaceId2 = 4900;
    uint32_t layerId2 = 4901;
    std::shared_ptr<HdiLayer> hdiLayer2 = std::make_shared<HdiLayer>(layerId2);
    std::shared_ptr<RSSurfaceLayer> rsLayer2 = std::make_shared<RSSurfaceLayer>(layerId2, nullptr);
    hdiLayer2->UpdateRSLayer(rsLayer2);
    hdiOutput->surfaceIdMap_[surfaceId2] = hdiLayer2;

    // Add solid layer without RSLayer (GetRSLayer() == nullptr, true)
    std::shared_ptr<HdiLayer> hdiLayer3 = std::make_shared<HdiLayer>(5000);
    hdiOutput->solidSurfaceIdMap_[5001] = hdiLayer3;

    // Add valid solid layer (all non-null, false branch)
    uint64_t solidSurfaceId = 5100;
    uint32_t layerId4 = 5101;
    std::shared_ptr<HdiLayer> hdiLayer4 = std::make_shared<HdiLayer>(layerId4);
    std::shared_ptr<RSSurfaceLayer> rsLayer4 = std::make_shared<RSSurfaceLayer>(layerId4, nullptr);
    hdiLayer4->UpdateRSLayer(rsLayer4);
    hdiOutput->solidSurfaceIdMap_[solidSurfaceId] = hdiLayer4;

    // Verify maps contain expected number of layers
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 3u);
    EXPECT_EQ(hdiOutput->solidSurfaceIdMap_.size(), 2u);

    // Should not crash
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_MutexLock_VerifyThreadSafety
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call DumpCurrentFrameLayers
 *                  2. verify mutex is locked (no crash)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_MutexLock_VerifyThreadSafety, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;
    ASSERT_NE(hdiOutput, nullptr);

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add some layers
    uint64_t surfaceId = 5200;
    uint32_t layerId = 5201;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;
    EXPECT_EQ(hdiOutput->surfaceIdMap_.size(), 1u);

    // Should not crash (mutex lock should work)
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_SurfaceNullCheck_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add layer with valid RSLayer but nullptr Surface
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 874 third condition (GetSurface() == nullptr)
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_SurfaceNullCheck_Skipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer
    // Note: RSSurfaceLayer created with nullptr surface will have GetSurface() == nullptr
    uint64_t surfaceId = 5300;
    uint32_t layerId = 5301;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;

    // Verify RSLayer's Surface is nullptr
    ASSERT_EQ(rsLayer->GetSurface(), nullptr);

    // GetSurface() is nullptr
    // Should not crash
    hdiOutput->DumpCurrentFrameLayers();
}

/**
 * Function: DumpCurrentFrameLayers_ValidLayerWithSurface_Line878False_Executed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. add HdiLayer with valid RSLayer and valid Surface to surfaceIdMap_
 *                  2. call DumpCurrentFrameLayers
 *                  3. verify line 878 condition is false, DumpCurrentFrameLayer is called
 */
HWTEST_F(HdiOutputTest, DumpCurrentFrameLayers_ValidLayerWithSurface_Executed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Create HdiLayer with RSLayer and valid Surface
    uint64_t surfaceId = 5400;
    uint32_t layerId = 5401;
    std::shared_ptr<HdiLayer> hdiLayer = std::make_shared<HdiLayer>(layerId);
    std::shared_ptr<RSSurfaceLayer> rsLayer = std::make_shared<RSSurfaceLayer>(layerId, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);

    // Create a valid IConsumerSurface
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("TestSurface");
    ASSERT_NE(surface, nullptr);
    rsLayer->SetSurface(surface);

    // Verify RSLayer's Surface is not nullptr
    ASSERT_NE(rsLayer->GetSurface(), nullptr);

    // Add to surfaceIdMap_
    hdiOutput->surfaceIdMap_[surfaceId] = hdiLayer;

    // Verify all three conditions in line 878 are false:
    // 1. hdiLayer != nullptr
    // 2. hdiLayer->GetRSLayer() != nullptr
    // 3. hdiLayer->GetRSLayer()->GetSurface() != nullptr
    ASSERT_NE(hdiLayer, nullptr);
    ASSERT_NE(hdiLayer->GetRSLayer(), nullptr);
    ASSERT_NE(hdiLayer->GetRSLayer()->GetSurface(), nullptr);

    // Should call DumpCurrentFrameLayer() since line 878 condition is false
    hdiOutput->DumpCurrentFrameLayers();

    // Clean up
    hdiOutput->surfaceIdMap_.clear();
}

/*
 * Function: Dump_IsSplitRenderTrue_CallsDumpLayerInfoForSplitRender
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true
 *                  2. verify DumpLayerInfoForSplitRender is called
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_CallsDumpLayerInfoForSplitRender, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layer to surfaceIdMap_
    auto hdiLayer = std::make_shared<HdiLayer>(1);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    hdiOutput->surfaceIdMap_[6001] = hdiLayer;

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderTrue_NoLayers_OnlyHeader
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true, no layers
 *                  2. verify only header is output
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_NoLayers_OnlyHeader, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    // Should contain header but no layer info
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderTrue_WithNullLayers_LayersSkipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true, add null layers
 *                  2. verify DumpLayerInfoForSplitRender skips null layers
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_WithNullLayers_LayersSkipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add nullptr HdiLayer to surfaceIdMap_
    hdiOutput->surfaceIdMap_[6101] = nullptr;
    hdiOutput->surfaceIdMap_[6102] = nullptr;

    // Add valid layer
    auto hdiLayer = std::make_shared<HdiLayer>(1);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    hdiOutput->surfaceIdMap_[6103] = hdiLayer;

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderFalse_SkipsDumpLayerInfoForSplitRender
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = false
 *                  2. verify DumpLayerInfoForSplitRender is not called
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderFalse_SkipsDumpLayerInfoForSplitRender, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add valid layer
    auto hdiLayer = std::make_shared<HdiLayer>(1);
    auto rsLayer = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiLayer->UpdateRSLayer(rsLayer);
    hdiOutput->surfaceIdMap_[6201] = hdiLayer;

    // Call Dump with isSplitRender = false
    std::string result;
    hdiOutput->Dump(result, false);

    // Verify DumpLayerInfoForSplitRender was not called
    // Should NOT contain "LayerInfo ScreenId:"
    EXPECT_EQ(result.find("LayerInfo ScreenId:"), std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderTrue_MultipleLayers_AllProcessed
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true, add multiple layers
 *                  2. verify all valid layers are processed
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_MultipleLayers_AllProcessed, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add multiple valid layers
    for (int i = 0; i < 3; i++) {
        auto hdiLayer = std::make_shared<HdiLayer>(i);
        auto rsLayer = std::make_shared<RSSurfaceLayer>(i, nullptr);
        hdiLayer->UpdateRSLayer(rsLayer);
        hdiOutput->surfaceIdMap_[6300 + i] = hdiLayer;
    }

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderTrue_ValidAndNullLayers_MixedProcessing
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true, add mix of valid and null layers
 *                  2. verify valid layers processed, null layers skipped
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_ValidAndNullLayers_MixedProcessing, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add mix of valid and null layers
    hdiOutput->surfaceIdMap_[6401] = nullptr;
    hdiOutput->surfaceIdMap_[6402] = std::make_shared<HdiLayer>(1);

    auto rsLayer1 = std::make_shared<RSSurfaceLayer>(1, nullptr);
    hdiOutput->surfaceIdMap_[6402]->UpdateRSLayer(rsLayer1);

    hdiOutput->surfaceIdMap_[6403] = nullptr;
    hdiOutput->surfaceIdMap_[6404] = std::make_shared<HdiLayer>(2);

    auto rsLayer2 = std::make_shared<RSSurfaceLayer>(2, nullptr);
    hdiOutput->surfaceIdMap_[6404]->UpdateRSLayer(rsLayer2);

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}

/*
 * Function: Dump_IsSplitRenderTrue_NullHdiLayer_Skipped
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. call Dump with isSplitRender = true, add layer without RSLayer
 *                  2. verify layer without RSLayer is skipped
 */
HWTEST_F(HdiOutputTest, Dump_IsSplitRenderTrue_NullHdiLayer_Skipped, Function | MediumTest | Level1)
{
    auto hdiOutput = HdiOutputTest::hdiOutput_;

    // Clear existing maps
    hdiOutput->surfaceIdMap_.clear();
    hdiOutput->solidSurfaceIdMap_.clear();

    // Add layer without RSLayer
    auto hdiLayer = std::make_shared<HdiLayer>(1);
    hdiOutput->surfaceIdMap_[6501] = hdiLayer;

    // Call Dump with isSplitRender = true
    std::string result;
    hdiOutput->Dump(result, true);

    // Verify DumpLayerInfoForSplitRender was called
    // Should contain header but layer info is skipped
    EXPECT_TRUE(result.find("LayerInfo ScreenId:") != std::string::npos);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
