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
#include "surface.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_hardware_processor.h"
#include "pipeline/rs_processor_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDropFrameProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    static inline sptr<Surface> csurf = nullptr;
    static inline sptr<IBufferProducer> producer = nullptr;
    static inline sptr<Surface> psurf = nullptr;
    static inline std::shared_ptr<RSSurfaceRenderNode> rsNode = nullptr;
    static inline std::shared_ptr<RSSurfaceRenderNode> rsParentNode = nullptr;
};

void RSDropFrameProcessorTest::SetUpTestCase() {}

void RSDropFrameProcessorTest::TearDownTestCase()
{
    csurf = nullptr;
    producer = nullptr;
    psurf = nullptr;
    rsNode = nullptr;
    rsParentNode = nullptr;
}

void RSDropFrameProcessorTest::SetUp() {}
void RSDropFrameProcessorTest::TearDown() {}

/**
 * @tc.name: TestDropFrame001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSDropFrameProcessorTest, TestDropFrame001, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    rsNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsNode, nullptr);

    // add the node on tree, and visible default true
    // so that RSRenderServiceListener will increase AvailableBufferCount
    rsParentNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsParentNode, nullptr);
    rsParentNode->AddChild(rsNode);
    ASSERT_TRUE(rsNode->IsOnTheTree());

    csurf = Surface::CreateSurfaceAsConsumer(config.name);
    ASSERT_NE(csurf, nullptr);
    rsNode->SetConsumer(csurf);
    printf("SetUpTestCase::CreateNodeAndSurface node id:%llu name:%s surface id:%llu \n",
        rsNode->GetId(), rsNode->GetName().c_str(), csurf->GetUniqueId());
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    ASSERT_NE(listener, nullptr);
    SurfaceError ret = csurf->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    producer = csurf->GetProducer();
    ASSERT_NE(producer, nullptr);
    psurf = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurf, nullptr);
    psurf->SetQueueSize(3); // only test 3 frames
    ASSERT_EQ(3, static_cast<int>(psurf->GetQueueSize()));

    // request&&flush 3 buffer make queue size full
    for (int i = 0; i < 3; i ++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
        ASSERT_NE(buffer, nullptr);
        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
        printf("RSDropFrameProcessorTest::TestDropFrame001 frame %d finish \n", i);
        printf("RSDropFrameProcessorTest::TestDropFrame001 AvailableCount: %d \n", rsNode->GetAvailableBufferCount());
        sleep(4); // every frame wait 4 seconds
    }
    
    // create RSHardwareProcessor
    auto rsHdProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::HARDWARE_COMPOSITE);
    ASSERT_NE(rsHdProcessor, nullptr);
    rsHdProcessor->ProcessSurface(*rsNode);
    ASSERT_EQ(1, rsNode->GetAvailableBufferCount());
}
} // namespace OHOS::Rosen