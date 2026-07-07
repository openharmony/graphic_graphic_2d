/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <parameters.h>

#include "feature/hwc/hpae_offline/rs_offline_processor.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_device.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_buffer.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_thread.h"
#include "feature/hwc/hpae_offline/rs_offline_device.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "feature/hwc/hpae_offline/rs_offline_result.h"
#include "params/rs_surface_render_params.h"

#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "surface_buffer.h"
#include "drawable/rs_surface_render_node_drawable.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

namespace {
constexpr NodeId TEST_NODE_ID = 12345;
constexpr int32_t BUFFER_WIDTH = 1000;
constexpr int32_t BUFFER_HEIGHT = 1000;
std::vector<uint8_t> g_hdrStatucMetadataVec = {10, 215, 35, 63, 195, 245, 168, 62, 154, 153, 153, 62,
    154, 153, 153, 62, 154, 153, 153, 62, 154, 153, 153, 62, 154, 153, 153, 62, 154, 153, 153, 62, 0,
    154, 153, 153, 62, 154, 153, 153, 62, 154, 153, 153, 62};
}

class RSGPUOfflineDeviceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SurfaceBuffer> CreateSurfaceBuffer(int32_t width, int32_t height);
    void SetRSSurfaceHandlerBuffer(sptr<SurfaceBuffer> &surfaceBuffer,
        std::shared_ptr<RSSurfaceHandler> surfaceHandler);
};

sptr<SurfaceBuffer> RSGPUOfflineDeviceTest::CreateSurfaceBuffer(int32_t width, int32_t height)
{
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    BufferRequestConfig bufConfig = {
        .width = width,
        .height = height,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA |
            BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE,
        .timeout = 0,
        .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    surfaceBuffer->Alloc(bufConfig);
    return surfaceBuffer;
}

void RSGPUOfflineDeviceTest::SetRSSurfaceHandlerBuffer(sptr<SurfaceBuffer> &surfaceBuffer,
    std::shared_ptr<RSSurfaceHandler> surfaceHandler)
{
    surfaceBuffer->SetMetadata(ATTRKEY_HDR_STATIC_METADATA, g_hdrStatucMetadataVec);
    sptr<SyncFence> fence = SyncFence::InvalidFence();
    int64_t timestamp = 0;
    Rect damage = {0, 0, BUFFER_WIDTH, BUFFER_HEIGHT};
    std::shared_ptr<RSRcdSurfaceRenderNode::BufferOwnerCount> buffferOwnerCount = 
        std::make_shared<RSRcdSurfaceRenderNode::BufferOwnerCount>();
    surfaceHandler->SetBuffer(surfaceBuffer, fence, damage, timestamp, buffferOwnerCount);
}

void RSGPUOfflineDeviceTest::SetUpTestCase() {}
void RSGPUOfflineDeviceTest::TearDownTestCase() {}
void RSGPUOfflineDeviceTest::SetUp() {}
void RSGPUOfflineDeviceTest::TearDown() {}

/**
 * @tc.name: GetSingleBufferModeNullHandlerTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeNullHandlerTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto result = device->GetSingleBufferMode(nullptr);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeNullConsumerTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeNullConsumerTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeValidConsumerNoneTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeValidConsumerNoneTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto consumer = IConsumerSurface::Create("test_switch_none");
    if (consumer == nullptr) {
        return;
    }
    surfaceHandler->SetConsumer(consumer);
    consumer->SetSingleBufferMode(SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeValidConsumerToSingleTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeValidConsumerToSingleTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto consumer = IConsumerSurface::Create("test_switch_single");
    if (consumer == nullptr) {
        return;
    }
    surfaceHandler->SetConsumer(consumer);
    consumer->SetSingleBufferMode(SingleBufferMode::SINGLE_BUFFER_MODE_TO_SINGLE);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeValidConsumerToMultiTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeValidConsumerToMultiTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto consumer = IConsumerSurface::Create("test_switch_multi");
    if (consumer == nullptr) {
        return;
    }
    surfaceHandler->SetConsumer(consumer);
    consumer->SetSingleBufferMode(SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeValidConsumerMaxValueTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeValidConsumerMaxValueTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto consumer = IConsumerSurface::Create("test_switch_max");
    if (consumer == nullptr) {
        return;
    }
    surfaceHandler->SetConsumer(consumer);
    consumer->SetSingleBufferMode(SingleBufferMode::SINGLE_BUFFER_MODE_MAX_VALUE);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: GetSingleBufferModeNegativeTest001
 * @tc.desc: Verify function GetSingleBufferMode
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, GetSingleBufferModeNegativeTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(TEST_NODE_ID);
    auto consumer = IConsumerSurface::Create("test_switch_negative");
    if (consumer == nullptr) {
        return;
    }
    surfaceHandler->SetConsumer(consumer);
    consumer->SetSingleBufferMode(SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    auto result = device->GetSingleBufferMode(surfaceHandler);
    EXPECT_EQ(result, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: UpdateContextSkipDrawSwitchTypeNotNoneTest001
 * @tc.desc: Verify function UpdateContext
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, UpdateContextSkipDrawSwitchTypeNotNoneTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = TEST_NODE_ID;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode->InitRenderParams();
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    sptr<SurfaceBuffer> surfaceBuffer = CreateSurfaceBuffer(BUFFER_WIDTH, BUFFER_HEIGHT);
    SetRSSurfaceHandlerBuffer(surfaceBuffer, surfaceHandler);

    auto offlineContext = device->GetOrCreateOfflineContext(TEST_NODE_ID);
    EXPECT_NE(offlineContext, nullptr);

    offlineContext->hasDrawn = false;
    bool firstResult = device->UpdateContext(surfaceNode, offlineContext);
    EXPECT_TRUE(firstResult);
    EXPECT_FALSE(offlineContext->skipDraw);

    offlineContext->hasDrawn = true;
    auto saveDrawParams = offlineContext->drawParams;
    saveDrawParams.switchType = SingleBufferMode::SINGLE_BUFFER_MODE_TO_SINGLE;
    offlineContext->drawParams = saveDrawParams;
    bool senconResult = device->UpdateContext(surfaceNode, offlineContext);
    EXPECT_TRUE(senconResult);
    EXPECT_FALSE(offlineContext->skipDraw);
}

/**
 * @tc.name: UpdateContextSkipDrawSwitchTypeNoneTest001
 * @tc.desc: Verify function UpdateContext
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, UpdateContextSkipDrawSwitchTypeNoneTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = TEST_NODE_ID;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode->InitRenderParams();
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    sptr<SurfaceBuffer> surfaceBuffer = CreateSurfaceBuffer(BUFFER_WIDTH, BUFFER_HEIGHT);
    SetRSSurfaceHandlerBuffer(surfaceBuffer, surfaceHandler);

    auto offlineContext = device->GetOrCreateOfflineContext(TEST_NODE_ID);
    EXPECT_NE(offlineContext, nullptr);

    offlineContext->hasDrawn = false;
    bool firstResult = device->UpdateContext(surfaceNode, offlineContext);
    EXPECT_TRUE(firstResult);

    offlineContext->hasDrawn = true;
    auto saveDrawParams = offlineContext->drawParams;
    EXPECT_EQ(saveDrawParams.switchType, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
    offlineContext->drawParams = saveDrawParams;
    bool senconResult = device->UpdateContext(surfaceNode, offlineContext);
    EXPECT_TRUE(senconResult);
    EXPECT_TRUE(offlineContext->skipDraw);
}

/**
 * @tc.name: UpdateContextSkipDrawSwitchTypeToMultiTest001
 * @tc.desc: Verify function UpdateContext
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, UpdateContextSkipDrawSwitchTypeToMultiTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = TEST_NODE_ID;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode->InitRenderParams();
    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    sptr<SurfaceBuffer> surfaceBuffer = CreateSurfaceBuffer(BUFFER_WIDTH, BUFFER_HEIGHT);
    SetRSSurfaceHandlerBuffer(surfaceBuffer, surfaceHandler);

    auto offlineContext = device->GetOrCreateOfflineContext(TEST_NODE_ID);
    EXPECT_NE(offlineContext, nullptr);

    offlineContext->hasDrawn = false;
    device->UpdateContext(surfaceNode, offlineContext);

    offlineContext->hasDrawn = true;
    offlineContext->drawParams.switchType = SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI;

    bool result = device->UpdateContext(surfaceNode, offlineContext);
    EXPECT_TRUE(result);
    EXPECT_FALSE(offlineContext->skipDraw);
    EXPECT_FALSE(offlineContext->hasDrawn);
}

/**
 * @tc.name: CollectDrawParamsSwitchTypeTest001
 * @tc.desc: Verify function CollectDrawParams
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, CollectDrawParamsSwitchTypeTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);

    RSSurfaceRenderNodeConfig config;
    config.id = TEST_NODE_ID;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    surfaceNode->InitRenderParams();

    auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
    RSSurfaceRenderParams params(0);
    
    auto drawParams = device->CollectDrawParams(surfaceNode, surfaceHandler, params);
    EXPECT_EQ(drawParams.switchType, SingleBufferMode::SINGLE_BUFFER_MODE_NONE);
}

/**
 * @tc.name: DrawHDRImagePassesSwitchTypeTest001
 * @tc.desc: Verify function CollectDrawParams
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, DrawHDRImagePassesSwitchTypeTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    RSSurfaceRenderParams surfaceParams(0);
    auto bufferOwnerCount = std::make_shared<RSSurfaceHander::BufferOwenerCount>();
    bufferOwnerCount->bufferId_ = 888u;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    Rect damageRect = {0, 0, 100, 100 };
    surfaceParams.SetBuffer(buffer,bufferOwnerCount, damageRect);

    GPUOfflineSubThreadData taskContext;
    taskContext.offlineBuffer = std::make_shared<RSGPUOfflineBuffer>("text", TEST_NODE_ID);
    taskContext.drawParams.resultWidth = 1920;
    taskContext.drawParams.resultHeight = 1080;
    taskContext.drawParams.targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    taskContext.drawParams.switchType = SingleBufferMode::SINGLE_BUFFER_MODE_TO_SINGLE;
    
    device->offlineThread_.Start();
    bool ret = device->DrawHDRImage(surfaceParams, taskContext);
    device->offlineThread_.Stop();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: DrawHDRImageSwitchTypeToMultiTest001
 * @tc.desc: Verify function CollectDrawParams
 * @tc.type: FUNC
 * @tc.require: issuesIBT79X
 */
HWTEST_F(RSGPUOfflineDeviceTest, DrawHDRImageSwitchTypeToMultiTest001, Level1)
{
    auto device = std::make_shared<RSGPUOfflineDevice>();
    EXPECT_NE(device, nullptr);

    RSSurfaceRenderParams surfaceParams(0);
    auto bufferOwnerCount = std::make_shared<RSSurfaceHander::BufferOwenerCount>();
    bufferOwnerCount->bufferId_ = 888u;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    Rect damageRect = {0, 0, 100, 100 };
    surfaceParams.SetBuffer(buffer,bufferOwnerCount, damageRect);

    GPUOfflineSubThreadData taskContext;
    taskContext.offlineBuffer = std::make_shared<RSGPUOfflineBuffer>("text", TEST_NODE_ID);
    taskContext.drawParams.resultWidth = 1920;
    taskContext.drawParams.resultHeight = 1080;
    taskContext.drawParams.targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    taskContext.drawParams.switchType = SingleBufferMode::SINGLE_BUFFER_MODE_TO_MULTI;
    
    device->offlineThread_.Start();
    bool ret = device->DrawHDRImage(surfaceParams, taskContext);
    device->offlineThread_.Stop();
    EXPECT_TRUE(ret);
}