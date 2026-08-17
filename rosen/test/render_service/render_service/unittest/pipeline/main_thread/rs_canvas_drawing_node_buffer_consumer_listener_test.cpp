/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "pipeline/main_thread/rs_canvas_drawing_node_buffer_consumer_listener.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingNodeBufferConsumerListenerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: OnBufferAvailable_NullSurfaceHandler
 * @tc.desc: Test OnBufferAvailable when surfaceHandler_ is null
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnBufferAvailable_NullSurfaceHandler, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSSurfaceHandler> weakHandler;
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(weakHandler, nodeId);
    // surfaceHandler_ is null, should return early without crash
    listener->OnBufferAvailable();
    ASSERT_EQ(listener->surfaceHandler_.lock(), nullptr);
}

/**
 * @tc.name: OnBufferAvailable_AvailableBufferCountOne
 * @tc.desc: Test OnBufferAvailable when available buffer count is 1 (no DropFirstFlushedBuffer)
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnBufferAvailable_AvailableBufferCountOne, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create("OnBufferAvailableTest1");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();
    ASSERT_NE(mainThread->handler_, nullptr);

    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    // First call: IncreaseAvailableBuffer, count becomes 1, not > 1 so no DropFirstFlushedBuffer
    listener->OnBufferAvailable();
    ASSERT_NE(listener->surfaceHandler_.lock(), nullptr);
    ASSERT_EQ(surfaceHandler->GetAvailableBufferCount(), 1);
}

/**
 * @tc.name: OnBufferAvailable_AvailableBufferCountMoreThanOne
 * @tc.desc: Test OnBufferAvailable when available buffer count > 1 (triggers DropFirstFlushedBuffer)
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnBufferAvailable_AvailableBufferCountMoreThanOne,
    TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create("OnBufferAvailableTest2");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();
    ASSERT_NE(mainThread->handler_, nullptr);

    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    // First call: count = 1
    listener->OnBufferAvailable();
    ASSERT_EQ(surfaceHandler->GetAvailableBufferCount(), 1);
    // Second call: count = 2, > 1 triggers PostTask for DropFirstFlushedBuffer
    listener->OnBufferAvailable();
    ASSERT_EQ(surfaceHandler->GetAvailableBufferCount(), 2);
    usleep(10000);
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: OnBufferAvailable_SurfaceHandlerExpired
 * @tc.desc: Test OnBufferAvailable when surfaceHandler_ expires between calls
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnBufferAvailable_SurfaceHandlerExpired, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    ASSERT_NE(listener->surfaceHandler_.lock(), nullptr);
    // Release the surfaceHandler, weak_ptr expires
    surfaceHandler.reset();
    ASSERT_EQ(listener->surfaceHandler_.lock(), nullptr);
    // Should return early without crash
    listener->OnBufferAvailable();
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: CleanBuffers_NullSurfaceHandler
 * @tc.desc: Test CleanBuffers when surfaceHandler_ is null
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, CleanBuffers_NullSurfaceHandler, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSSurfaceHandler> weakHandler;
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(weakHandler, nodeId);
    // surfaceHandler_ is null, should return early without crash
    listener->OnCleanCache(nullptr);
    listener->OnGoBackground();
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: CleanBuffers_NullConsumer
 * @tc.desc: Test CleanBuffers when consumer is null
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, CleanBuffers_NullConsumer, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create("CleanBuffersNullConsumer");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();

    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    // consumer is null by default, should return early without crash
    listener->OnCleanCache(nullptr);
    listener->OnGoBackground();
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: CleanBuffers_WithConsumer
 * @tc.desc: Test CleanBuffers when consumer is not null, PostTask for cleanup
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, CleanBuffers_WithConsumer, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create("CleanBuffersWithConsumer");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();

    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto consumer = IConsumerSurface::Create();
    ASSERT_NE(consumer, nullptr);
    surfaceHandler->SetConsumer(consumer);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    // consumer is not null, should PostTask for cleanup without crash
    listener->OnCleanCache(nullptr);
    listener->OnGoBackground();
    usleep(10000);
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: OnTunnelHandleChangeTest
 * @tc.desc: Test OnTunnelHandleChange does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnTunnelHandleChangeTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    listener->OnTunnelHandleChange();
    ASSERT_NE(listener, nullptr);
}

/**
 * @tc.name: OnTransformChangeTest
 * @tc.desc: Test OnTransformChange does not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnTransformChangeTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(nodeId);
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(surfaceHandler, nodeId);
    listener->OnTransformChange();
    ASSERT_NE(listener, nullptr);
}
} // namespace OHOS::Rosen
