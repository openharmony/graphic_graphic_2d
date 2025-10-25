/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include <regex>
#include "limit_number.h"
#include "parameters.h"
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#include "feature_cfg/feature_param/performance_feature/video_metadata_param.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "iconsumer_surface.h"
#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace OHOS::Rosen {
class TvMetadataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
};

void TvMetadataTest::SetUpTestCase()
{
    RSTvMetadataManager::Instance().Reset();
}
void TvMetadataTest::TearDownTestCase() {}
void TvMetadataTest::SetUp() {}
void TvMetadataTest::TearDown() {}

class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

static std::shared_ptr<RSSurfaceOhos> CreateRsSurfaceOhos(void)
{
    auto rsSurface = std::make_shared<RSSurfaceOhosVulkan>(IConsumerSurface::Create());
    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    if (cSurface == nullptr) {
        return nullptr;
    }
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    if (pSurface == nullptr) {
        return nullptr;
    }
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    if (nativeWindowBuffer == nullptr) {
        return nullptr;
    }
    rsSurface->mSurfaceList.emplace_back(nativeWindowBuffer);
    return rsSurface;
}

/**
 * @tc.name: RecordAndCombineMetadata_001
 * @tc.desc: Test RecordAndCombineMetadata
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, RecordAndCombineMetadata_001, TestSize.Level1)
{
    TvPQMetadata metadata = { 0 };
    metadata.sceneTag = 1;
    metadata.uiFrameCnt = 60;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata);
    TvPQMetadata metadata2 = { 0 };
    metadata2.vidFrameCnt = 24;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata2);
    TvPQMetadata metadata3 = { 0 };
    metadata3.dpPixFmt = 2;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata3);
    auto outMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(1, outMetadata.sceneTag);
    ASSERT_EQ(60, outMetadata.uiFrameCnt);
    ASSERT_EQ(24, outMetadata.vidFrameCnt);
    ASSERT_EQ(2, outMetadata.dpPixFmt);
}

/**
 * @tc.name: CopyTvMetadataToSurface_001
 * @tc.desc: Test CopyTvMetadataToSurface
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, CopyTvMetadataToSurface_001, TestSize.Level1)
{
    TvPQMetadata metadata = { 0 };
    metadata.sceneTag = 1;
    metadata.uiFrameCnt = 60;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata);
    TvPQMetadata metadata2 = { 0 };
    metadata2.vidFrameCnt = 24;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata2);
    TvPQMetadata metadata3 = { 0 };
    metadata3.dpPixFmt = 2;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata3);

    auto rsSurface = CreateRsSurfaceOhos();
    ASSERT_NE(rsSurface, nullptr);
    RSTvMetadataManager::Instance().CopyTvMetadataToSurface(rsSurface);

    auto outBuffer = rsSurface->GetCurrentBuffer();
    ASSERT_EQ(true, outBuffer != nullptr);
    TvPQMetadata tvMetadata = { 0 };
    MetadataHelper::GetVideoTVMetadata(outBuffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(60, tvMetadata.uiFrameCnt);
    ASSERT_EQ(24, tvMetadata.vidFrameCnt);
    ASSERT_EQ(2, tvMetadata.dpPixFmt);

    // after copy, metadata reset
    auto outMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(0, outMetadata.sceneTag);
    ASSERT_EQ(0, outMetadata.uiFrameCnt);
    ASSERT_EQ(0, outMetadata.vidFrameCnt);
    ASSERT_EQ(0, outMetadata.dpPixFmt);

    RSTvMetadataManager::Instance().cachedSurfaceNodeId_ = 1;
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    ASSERT_NE(renderThreadParams, nullptr);
    renderThreadParams->cachedSurfaceNodeId_ = 1;
    renderThreadParams->cachedSurfaceNodeOnTheTree_ = true;
    RSUniRenderThread::Instance().Sync(std::move(renderThreadParams));
    RSTvMetadataManager::Instance().CopyTvMetadataToSurface(rsSurface);
    MetadataHelper::GetVideoTVMetadata(outBuffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(24, tvMetadata.vidFrameCnt);
    ASSERT_EQ(0, tvMetadata.dpPixFmt);
}

/**
 * @tc.name: CopyFromLayersToSurface_001
 * @tc.desc: Test CopyFromLayersToSurface
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, CopyFromLayersToSurface_001, TestSize.Level1)
{
    auto rsSurface = CreateRsSurfaceOhos();
    auto outBuffer = rsSurface->GetCurrentBuffer();
    ASSERT_NE(rsSurface, nullptr);
    ASSERT_EQ(true, outBuffer != nullptr);

    // test empty layers
    std::vector<LayerInfoPtr> layers;
    RSTvMetadataManager::CopyFromLayersToSurface(layers, rsSurface);
    TvPQMetadata tvMetadata = { 0 };
    MetadataHelper::GetVideoTVMetadata(outBuffer, tvMetadata);
    ASSERT_EQ(0, tvMetadata.sceneTag);
    ASSERT_EQ(0, tvMetadata.uiFrameCnt);
    ASSERT_EQ(0, tvMetadata.vidFrameCnt);
    ASSERT_EQ(0, tvMetadata.dpPixFmt);

    // test normal layers
    auto rsSurface1 = CreateRsSurfaceOhos();
    ASSERT_NE(rsSurface1, nullptr);
    auto buffer = rsSurface1->GetCurrentBuffer();
    ASSERT_NE(buffer, nullptr);
    TvPQMetadata layerTvMetadata = { 0 };
    layerTvMetadata.sceneTag = 1;
    layerTvMetadata.uiFrameCnt = 60;
    layerTvMetadata.vidFrameCnt = 24;
    layerTvMetadata.dpPixFmt = 2;
    MetadataHelper::SetVideoTVMetadata(buffer, layerTvMetadata);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    layer->SetBuffer(buffer, acquireFence);
    layers.emplace_back(layer);

    RSTvMetadataManager::CopyFromLayersToSurface(layers, rsSurface);
    MetadataHelper::GetVideoTVMetadata(outBuffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(60, tvMetadata.uiFrameCnt);
    ASSERT_EQ(24, tvMetadata.vidFrameCnt);
    ASSERT_EQ(2, tvMetadata.dpPixFmt);
}

/**
 * @tc.name: ResetDpPixelFormat_001
 * @tc.desc: Test ResetDpPixelFormat
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, ResetDpPixelFormat_001, TestSize.Level1)
{
    TvPQMetadata metadata = { 0 };
    metadata.sceneTag = 1;
    metadata.uiFrameCnt = 60;
    metadata.dpPixFmt = 2;
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(metadata);
    RSTvMetadataManager::Instance().ResetDpPixelFormat();
    auto outMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(1, outMetadata.sceneTag);
    ASSERT_EQ(60, outMetadata.uiFrameCnt);
    ASSERT_EQ(0, outMetadata.dpPixFmt);
}

/**
 * @tc.name: UpdateTvMetadata_001
 * @tc.desc: Test UpdateTvMetadata
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, UpdateTvMetadata_001, TestSize.Level1)
{
    auto rsSurface = CreateRsSurfaceOhos();
    auto buffer = rsSurface->GetCurrentBuffer();
    ASSERT_NE(rsSurface, nullptr);
    ASSERT_EQ(true, buffer != nullptr);

    std::string bundleName = "com.test.app";
    std::string val = "1";
    VideoMetadataParam::AddVideoMetadataApp(bundleName, val);
    RectI screenRect(0, 0, 3840, 2160);
    RSLayerInfo layerInfo;
    layerInfo.dstRect.w = 3840;
    layerInfo.dstRect.h = 2160;
    RSSurfaceRenderParams params(0);
    params.RecordScreenRect(screenRect);
    params.SetLayerInfo(layerInfo);
    params.bundleName_ = bundleName;
    CM_ColorSpaceInfo colorSpaceInfo;
    colorSpaceInfo.primaries = COLORPRIMARIES_BT2020;
    MetadataHelper::SetColorSpaceInfo(buffer, colorSpaceInfo);
    CM_HDR_Metadata_Type hdrMetadataType = CM_VIDEO_HDR_VIVID;
    MetadataHelper::SetHDRMetadataType(buffer, hdrMetadataType);
    MetadataHelper::SetSceneTag(buffer, 1);
    RSTvMetadataManager::Instance().UpdateTvMetadata(params, buffer);
    TvPQMetadata tvMetadata = { 0 };
    MetadataHelper::GetVideoTVMetadata(buffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(2, tvMetadata.vidWinSize);
    ASSERT_EQ(4, tvMetadata.colorimetry);
    ASSERT_EQ(3, tvMetadata.hdr);
}

/**
 * @tc.name: RecordTvMetadata_001
 * @tc.desc: Test RecordTvMetadata
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, RecordTvMetadata_001, TestSize.Level1)
{
    auto rsSurface = CreateRsSurfaceOhos();
    auto buffer = rsSurface->GetCurrentBuffer();
    ASSERT_NE(rsSurface, nullptr);
    ASSERT_EQ(true, buffer != nullptr);

    std::string bundleName = "com.test.app";
    std::string val = "1";
    VideoMetadataParam::AddVideoMetadataApp(bundleName, val);
    RectI screenRect(0, 0, 3840, 2160);
    RSLayerInfo layerInfo;
    layerInfo.dstRect.w = 3840;
    layerInfo.dstRect.h = 2160;
    RSSurfaceRenderParams params(0);
    params.RecordScreenRect(screenRect);
    params.SetLayerInfo(layerInfo);
    params.bundleName_ = bundleName;
    CM_ColorSpaceInfo colorSpaceInfo;
    colorSpaceInfo.primaries = COLORPRIMARIES_BT2020;
    MetadataHelper::SetColorSpaceInfo(buffer, colorSpaceInfo);
    CM_HDR_Metadata_Type hdrMetadataType = CM_VIDEO_HDR_VIVID;
    MetadataHelper::SetHDRMetadataType(buffer, hdrMetadataType);
    MetadataHelper::SetSceneTag(buffer, 1);
    RSTvMetadataManager::Instance().RecordTvMetadata(params, buffer);
    TvPQMetadata tvMetadata = RSTvMetadataManager::Instance().GetMetadata();
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(2, tvMetadata.vidWinSize);
    ASSERT_EQ(4, tvMetadata.colorimetry);
    ASSERT_EQ(3, tvMetadata.hdr);
}
 
/**
 * @tc.name: CombineMetadataForAllLayers_001
 * @tc.desc: Test CombineMetadataForAllLayers
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, CombineMetadataForAllLayers_001, TestSize.Level1)
{
    std::vector<LayerInfoPtr> layers;
    auto uniRenderSurface = CreateRsSurfaceOhos();
    ASSERT_NE(uniRenderSurface, nullptr);
    auto unitRenderBuffer = uniRenderSurface->GetCurrentBuffer();
    ASSERT_NE(unitRenderBuffer, nullptr);
    TvPQMetadata uniRenderTvMetadata = { 0 };
    uniRenderTvMetadata.sceneTag = 1;
    uniRenderTvMetadata.uiFrameCnt = 60;
    uniRenderTvMetadata.vidFrameCnt = 24;
    uniRenderTvMetadata.dpPixFmt = 2;
    MetadataHelper::SetVideoTVMetadata(unitRenderBuffer, uniRenderTvMetadata);
    LayerInfoPtr uniRenderLayer = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    uniRenderLayer->SetBuffer(unitRenderBuffer, acquireFence);
    uniRenderLayer->SetUniRenderFlag(true);
    layers.emplace_back(uniRenderLayer);
    RSTvMetadataManager::Instance().CombineMetadataForAllLayers(layers);
    TvPQMetadata tvMetadata = { 0 };
    MetadataHelper::GetVideoTVMetadata(unitRenderBuffer, tvMetadata);
    ASSERT_EQ(1, tvMetadata.sceneTag);
    ASSERT_EQ(60, tvMetadata.uiFrameCnt);
    ASSERT_EQ(24, tvMetadata.vidFrameCnt);
    ASSERT_EQ(2, tvMetadata.dpPixFmt);
}
 
/**
 * @tc.name: CombineMetadataForAllLayers_002
 * @tc.desc: Test CombineMetadataForAllLayers
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, CombineMetadataForAllLayers_002, TestSize.Level1)
{
    std::vector<LayerInfoPtr> layers;
    auto uniRenderSurface = CreateRsSurfaceOhos();
    ASSERT_NE(uniRenderSurface, nullptr);
    auto unitRenderBuffer = uniRenderSurface->GetCurrentBuffer();
    ASSERT_NE(unitRenderBuffer, nullptr);
    TvPQMetadata uniRenderTvMetadata = { 0 };
    uniRenderTvMetadata.sceneTag = 0;
    uniRenderTvMetadata.uiFrameCnt = 60;
    uniRenderTvMetadata.vidFrameCnt = 24;
    uniRenderTvMetadata.dpPixFmt = 2;
    MetadataHelper::SetVideoTVMetadata(unitRenderBuffer, uniRenderTvMetadata);
    LayerInfoPtr uniRenderLayer = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<SyncFence> acquireFence1 = SyncFence::INVALID_FENCE;
    uniRenderLayer->SetBuffer(unitRenderBuffer, acquireFence1);
    uniRenderLayer->SetUniRenderFlag(true);
    layers.emplace_back(uniRenderLayer);
 
    auto selfDrawSurface = CreateRsSurfaceOhos();
    ASSERT_NE(selfDrawSurface, nullptr);
    auto selfDrawBuffer = selfDrawSurface->GetCurrentBuffer();
    ASSERT_NE(selfDrawBuffer, nullptr);
    TvPQMetadata selfDrawTvMetadata = { 0 };
    selfDrawTvMetadata.sceneTag = 1;
    selfDrawTvMetadata.uiFrameCnt = 70;
    selfDrawTvMetadata.vidFrameCnt = 25;
    selfDrawTvMetadata.dpPixFmt = 0;
    MetadataHelper::SetVideoTVMetadata(selfDrawBuffer, selfDrawTvMetadata);
    LayerInfoPtr selfDrawLayer = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<SyncFence> acquireFence2 = SyncFence::INVALID_FENCE;
    selfDrawLayer->SetBuffer(selfDrawBuffer, acquireFence2);
    selfDrawLayer->SetUniRenderFlag(false);
    layers.emplace_back(selfDrawLayer);
    
    RSTvMetadataManager::Instance().CombineMetadataForAllLayers(layers);
    TvPQMetadata tvMetadata1 = { 0 };
    MetadataHelper::GetVideoTVMetadata(selfDrawBuffer, tvMetadata1);
    ASSERT_EQ(1, tvMetadata1.sceneTag);
    ASSERT_EQ(60, tvMetadata1.uiFrameCnt);
    ASSERT_EQ(25, tvMetadata1.vidFrameCnt);
    ASSERT_EQ(2, tvMetadata1.dpPixFmt);
}

/**
 * @tc.name: IsSdpInfoApp_001
 * @tc.desc: Test IsSdpInfoApp
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, IsSdpInfoApp_001, TestSize.Level1)
{
    std::string appName = "com.example.app";
    std::string val = "1";
    VideoMetadataParam::AddVideoMetadataApp(appName, val);
    bool result = RSTvMetadataManager::IsSdpInfoAppId(appName);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsSdpInfoApp_002
 * @tc.desc: Test IsSdpInfoApp
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, IsSdpInfoApp_002, TestSize.Level1)
{
    std::string appName = "com.example.app";
    std::string val = "1";
    VideoMetadataParam::AddVideoMetadataApp(appName, val);
    bool result = RSTvMetadataManager::IsSdpInfoAppId("com.other.app");
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetUniRenderThreadParam_001
 * @tc.desc: Test SetUniRenderThreadParam
 * @tc.type: FUNC
 * @tc.require: issueIBNN9I
 */
HWTEST_F(TvMetadataTest, SetUniRenderThreadParam_001, TestSize.Level1)
{
    auto surfaceNode1 = std::make_shared<RSSurfaceRenderNode>(1);
    RSTvMetadataManager::Instance().cachedSurfaceNodeId_ = 1;
    surfaceNode1->SetIsOnTheTree(true);
    RSMainThread::Instance()->AddSelfDrawingNodes(surfaceNode1);
    auto renderThreadParams = std::make_unique<RSRenderThreadParams>();
    RSTvMetadataManager::Instance().SetUniRenderThreadParam(renderThreadParams);
    ASSERT_EQ(1, renderThreadParams->cachedSurfaceNodeId_);
    ASSERT_EQ(true, renderThreadParams->cachedSurfaceNodeOnTheTree_);
}
}