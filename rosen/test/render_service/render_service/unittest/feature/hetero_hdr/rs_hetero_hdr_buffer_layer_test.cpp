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

#include "common/rs_common_def.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_manager.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_buffer_layer.h"
#include "feature/hdr/hetero_hdr/rs_hetero_hdr_hpae.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "hetero_hdr/rs_hdr_pattern_manager.h"  // rs base
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "hetero_hdr/rs_hdr_vulkan_task.h"
#include "screen_manager/rs_screen.h"
#include "transaction/rs_interfaces.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
using namespace std;

namespace OHOS::Rosen {
class RSHeteroHDRBufferLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHeteroHDRBufferLayerTest::SetUpTestCase()
{
    RS_LOGI("************************RSHeteroHDRBufferLayerTest SetUpTestCase************************");
}
void RSHeteroHDRBufferLayerTest::TearDownTestCase()
{
    RS_LOGI("************************RSHeteroHDRBufferLayerTest TearDownTestCase************************");
}
void RSHeteroHDRBufferLayerTest::SetUp() { RS_LOGI("SetUp------------------------------------"); }
void RSHeteroHDRBufferLayerTest::TearDown() { RS_LOGI("TearDown------------------------------------"); }

class MockRSHeteroHDRBufferLayer : public RSHeteroHDRBufferLayer {
public:
    MockRSHeteroHDRBufferLayer(const std::string& name, NodeId layerId) :  RSHeteroHDRBufferLayer(name, layerId) {}
    ~MockRSHeteroHDRBufferLayer() = default;
};

/**
 * @tc.name: PrepareHDRDstBufferTest
 * @tc.desc: Test PrepareHDRDstBuffer
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, PrepareHDRDstBufferTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);

    auto surfaceDrawableAdapter = RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode);
    ASSERT_NE(surfaceDrawableAdapter, nullptr);
    surfaceNode->InitRenderParams();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawableAdapter->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->buffer_ = OHOS::SurfaceBuffer::Create();
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    surfaceParams->GetBuffer()->Alloc(requestConfig);
    surfaceParams->GetBuffer()->SetSurfaceBufferScalingMode(ScalingMode::SCALING_MODE_FREEZE);
    RSLayerInfo layerInfo;
    layerInfo.srcRect.w = 1;
    layerInfo.srcRect.h = 1;
    layerInfo.dstRect.w = 1;
    layerInfo.dstRect.h = 1;
    surfaceParams->SetLayerInfo(layerInfo);

    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    sptr<SurfaceBuffer> surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(nullptr, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);

    RSHeteroHDRManager::Instance().curHandleStatus_ = HdrStatus::HDR_VIDEO;
    layerInfo.transformType = GRAPHIC_ROTATE_90;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);

    layerInfo.transformType = GRAPHIC_ROTATE_180;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);

    layerInfo.transformType = GRAPHIC_ROTATE_270;
    surfaceParams->SetLayerInfo(layerInfo);
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);

    RSHeteroHDRManager::Instance().curHandleStatus_ = HdrStatus::AI_HDR_VIDEO_GAINMAP;
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);

    RSHeteroHDRManager::Instance().curHandleStatus_ = HdrStatus::NO_HDR;
    surfaceBuffer = mockRSHeteroHDRBufferLayer->PrepareHDRDstBuffer(surfaceParams, 0);
    EXPECT_EQ(surfaceBuffer, nullptr);
}

/**
 * @tc.name: ConsumeAndUpdateBufferTest
 * @tc.desc: Test ConsumeAndUpdateBuffer
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, ConsumeAndUpdateBufferTest, TestSize.Level1)
{
    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", 1234);
    mockRSHeteroHDRBufferLayer->surfaceHandler_ = nullptr;
    mockRSHeteroHDRBufferLayer->ConsumeAndUpdateBuffer();

    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);

    mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    mockRSHeteroHDRBufferLayer->ConsumeAndUpdateBuffer();
}

/**
 * @tc.name: RequestSurfaceBufferTest
 * @tc.desc: Test RequestSurfaceBuffer
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, RequestSurfaceBufferTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);

    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    mockRSHeteroHDRBufferLayer->surfaceCreated_ = false;
    BufferRequestConfig BufferConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    int32_t releaseFence = -1;
    BufferConfig.width = 100000000;
    BufferConfig.height = 100000000;
    auto surfaceBuffer = mockRSHeteroHDRBufferLayer->RequestSurfaceBuffer(BufferConfig, releaseFence);
    EXPECT_EQ(surfaceBuffer, nullptr);

    mockRSHeteroHDRBufferLayer->surfaceCreated_ = true;
    BufferConfig.width = 100000000;
    BufferConfig.height = 100000000;
    surfaceBuffer = mockRSHeteroHDRBufferLayer->RequestSurfaceBuffer(BufferConfig, releaseFence);
    EXPECT_EQ(surfaceBuffer, nullptr);

    mockRSHeteroHDRBufferLayer->surfaceCreated_ = true;
    BufferConfig.width = 100;
    BufferConfig.height = 100;
    surfaceBuffer = mockRSHeteroHDRBufferLayer->RequestSurfaceBuffer(BufferConfig, releaseFence);
    EXPECT_NE(surfaceBuffer, nullptr);

    mockRSHeteroHDRBufferLayer->surfaceCreated_ = false;
    mockRSHeteroHDRBufferLayer->surfaceHandler_ = nullptr;
    surfaceBuffer = mockRSHeteroHDRBufferLayer->RequestSurfaceBuffer(BufferConfig, releaseFence);
    EXPECT_EQ(surfaceBuffer, nullptr);

    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    int32_t acquireFence = -1;
    BufferFlushConfig flushConfig;
    mockRSHeteroHDRBufferLayer->FlushSurfaceBuffer(buffer, acquireFence, flushConfig);

    sptr<SurfaceBuffer> buffer1 = OHOS::SurfaceBuffer::Create();
    mockRSHeteroHDRBufferLayer->FlushHDRDstBuffer(buffer1);
}

/**
 * @tc.name: CreateSurfaceTest
 * @tc.desc: Test CreateSurface
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, CreateSurfaceTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    sptr<IBufferConsumerListener> listener;
    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    bool ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, false);

    mockRSHeteroHDRBufferLayer->surfaceHandler_->SetConsumer(IConsumerSurface::Create("test consumer"));
    mockRSHeteroHDRBufferLayer->surface_ = nullptr;
    ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, false);

    mockRSHeteroHDRBufferLayer->surfaceHandler_->SetConsumer(nullptr);
    mockRSHeteroHDRBufferLayer->surface_ = Surface::CreateSurfaceAsConsumer("test_surface");
    ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, false);

    mockRSHeteroHDRBufferLayer->surfaceHandler_->SetConsumer(IConsumerSurface::Create("test consumer"));
    mockRSHeteroHDRBufferLayer->surface_ = Surface::CreateSurfaceAsConsumer("test_surface");
    ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, true);

    mockRSHeteroHDRBufferLayer->surfaceHandler_ = nullptr;
    ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: ReleaseBufferTest
 * @tc.desc: Test ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, ReleaseBufferTest, TestSize.Level1)
{
    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", 1234);
    mockRSHeteroHDRBufferLayer->surfaceHandler_ = nullptr;
    mockRSHeteroHDRBufferLayer->ReleaseBuffer();

    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);

    mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    mockRSHeteroHDRBufferLayer->ReleaseBuffer();

    mockRSHeteroHDRBufferLayer->surfaceHandler_->SetConsumer(IConsumerSurface::Create("test consumer"));
    mockRSHeteroHDRBufferLayer->ReleaseBuffer();
    
    mockRSHeteroHDRBufferLayer->ConsumeAndUpdateBuffer();
    mockRSHeteroHDRBufferLayer->ReleaseBuffer();

    mockRSHeteroHDRBufferLayer->ConsumeAndUpdateBuffer();
    mockRSHeteroHDRBufferLayer->ReleaseBuffer();
}

/**
 * @tc.name: CleanCacheTest
 * @tc.desc: Test CleanCache
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(RSHeteroHDRBufferLayerTest, CleanCacheTest, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
 
    auto mockRSHeteroHDRBufferLayer = make_shared<MockRSHeteroHDRBufferLayer>("ut test", surfaceNode->GetId());
    mockRSHeteroHDRBufferLayer->CleanCache();

    sptr<IBufferConsumerListener> listener;
    mockRSHeteroHDRBufferLayer->surfaceHandler_->SetConsumer(IConsumerSurface::Create("test consumer"));
    mockRSHeteroHDRBufferLayer->surface_ = Surface::CreateSurfaceAsConsumer("test_surface");
    bool ret = mockRSHeteroHDRBufferLayer->CreateSurface(listener);
    EXPECT_EQ(ret, true);
    mockRSHeteroHDRBufferLayer->CleanCache();
 
    mockRSHeteroHDRBufferLayer->surfaceHandler_ = nullptr;
    mockRSHeteroHDRBufferLayer->CleanCache();
}

} // namespace OHOS::Rosen