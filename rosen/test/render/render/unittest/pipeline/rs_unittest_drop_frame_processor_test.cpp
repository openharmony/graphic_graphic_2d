/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "pipeline/render_thread/rs_base_render_util.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "surface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDropFrameProcessorExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    static inline sptr<IConsumerSurface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> psurf = nullptr;
    static inline std::shared_ptr<RSSurfaceRenderNode> rsNode = nullptr;
    static inline std::shared_ptr<RSSurfaceRenderNode> rsParentNode = nullptr;
};

void RSDropFrameProcessorExtTest::SetUpTestCase() {}

void RSDropFrameProcessorExtTest::TearDownTestCase()
{
    csurf = nullptr;
    producer = nullptr;
    psurf = nullptr;
    rsNode = nullptr;
    rsParentNode = nullptr;
}

void RSDropFrameProcessorExtTest::SetUp() {}
void RSDropFrameProcessorExtTest::TearDown() {}

/**
 * @tc.name: DropFrameProcessorExtTest001
 * @tc.desc: Test drop frame when pointer is null.
 * @tc.type: FUNC
 * @tc.require: issueI5VQWQ
 */
HWTEST_F(RSDropFrameProcessorExtTest, DropFrameProcessorExtTest001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    rsNode->GetRSSurfaceHandler()->SetConsumer(nullptr);
    GSError result = RSBaseRenderUtil::DropFrameProcess(*rsNode->GetRSSurfaceHandler());
    ASSERT_EQ(result, OHOS::GSERROR_NO_CONSUMER);
}

/**
 * @tc.name: DropFrameProcessorExtTest002
 * @tc.desc: Create node
 * @tc.type: Test if AcquireBuffer is not OK.
 * @tc.require: issueI5VQWQ
 */
HWTEST_F(RSDropFrameProcessorExtTest, DropFrameProcessorExtTest002, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsNode, nullptr);
    rsNode->InitRenderParams();

    // add the node on tree, and visible default true
    // so that RSRenderServiceListener will increase AvailableBufferCount
    rsParentNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsParentNode, nullptr);
    rsParentNode->InitRenderParams();

    rsParentNode->AddChild(rsNode);
    rsNode->SetIsOnTheTree(true);

    csurf = IConsumerSurface::Create(config.name);
    rsNode->GetRSSurfaceHandler()->SetConsumer(csurf);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    csurf->RegisterConsumerListener(listener);
    producer = csurf->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(4); // only test 4 frames

    // request&&flush 3 buffer, make dirtyList size equal queuesize -1
    for (int i = 0; i < 3; i ++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
        sleep(4); // every frame wait 4 seconds
    }
    const auto& surfaceConsumer = rsNode->GetRSSurfaceHandler()->GetConsumer();
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    GSError ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    while (ret == OHOS::GSERROR_OK) {
        ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    }
    // create RSHardwareProcessor
    GSError result = RSBaseRenderUtil::DropFrameProcess(*rsNode->GetRSSurfaceHandler());
    ASSERT_EQ(result, OHOS::GSERROR_NO_BUFFER);
}

/**
 * @tc.name: DropFrameProcessorExtTest003
 * @tc.desc: Test DropFrameProcess is OK.
 * @tc.type: FUNC
 * @tc.require: issueI5VQWQ
 */
HWTEST_F(RSDropFrameProcessorExtTest, DropFrameProcessorExtTest003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsNode, nullptr);
    rsNode->InitRenderParams();

    // add the node on tree, and visible default true
    // so that RSRenderServiceListener will increase AvailableBufferCount
    rsParentNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsParentNode, nullptr);
    rsParentNode->InitRenderParams();

    rsParentNode->AddChild(rsNode);
    rsNode->SetIsOnTheTree(true);

    csurf = IConsumerSurface::Create(config.name);
    rsNode->GetRSSurfaceHandler()->SetConsumer(csurf);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    csurf->RegisterConsumerListener(listener);
    producer = csurf->GetProducer();
    psurf = Surface::CreateSurfaceAsProducer(producer);
    psurf->SetQueueSize(4); // only test 4 frames

    // request&&flush 3 buffer, make dirtyList size equal queuesize -1
    for (int i = 0; i < 3; i ++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        [[maybe_unused]] GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
        sleep(4); // every frame wait 4 seconds
    }

    // create RSHardwareProcessor
    GSError result = RSBaseRenderUtil::DropFrameProcess(*rsNode->GetRSSurfaceHandler());
    ASSERT_EQ(result, OHOS::GSERROR_OK);
}
} // namespace OHOS::Rosen