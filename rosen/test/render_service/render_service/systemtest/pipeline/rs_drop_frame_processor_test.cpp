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
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_render_service_listener.h"

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
 * @tc.desc: DropFrameProcess system test.
 * @tc.type: FUNC
 * @tc.require: issueI5VQWQ
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
    rsNode->SetIsOnTheTree(true);
    ASSERT_TRUE(rsNode->IsOnTheTree());

    csurf = IConsumerSurface::Create(config.name);
    ASSERT_NE(csurf, nullptr);
    rsNode->SetConsumer(csurf);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(surfaceRenderNode);
    ASSERT_NE(listener, nullptr);
    SurfaceError ret = csurf->RegisterConsumerListener(listener);
    ASSERT_EQ(ret, SURFACE_ERROR_OK);
    producer = csurf->GetProducer();
    ASSERT_NE(producer, nullptr);
    psurf = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurf, nullptr);
    psurf->SetQueueSize(4); // only test 4 frames
    ASSERT_EQ(4, static_cast<int>(psurf->GetQueueSize()));

    // request&&flush 3 buffer, make dirtyList size equal queuesize -1
    for (int i = 0; i < 3; i++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        GSError ret = psurf->RequestBuffer(buffer, requestFence, requestConfig);
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
        ASSERT_NE(buffer, nullptr);
        sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
        ret = psurf->FlushBuffer(buffer, flushFence, flushConfig);
        ASSERT_EQ(ret, OHOS::GSERROR_OK);
        usleep(40000); // every frame wait 40 microseconds
    }
    
    // create RSHardwareProcessor
    RSBaseRenderUtil::DropFrameProcess(*rsNode.get());
    ASSERT_EQ(2, rsNode->GetAvailableBufferCount());
}
} // namespace OHOS::Rosen
