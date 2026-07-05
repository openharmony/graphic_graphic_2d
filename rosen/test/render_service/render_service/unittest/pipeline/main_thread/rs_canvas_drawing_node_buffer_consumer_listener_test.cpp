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
#include "pipeline/rs_context.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
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
 * @tc.name: OnBufferAvailableTest
 * @tc.desc: Test OnBufferAvailable for all branches
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingNodeBufferConsumerListenerTest, OnBufferAvailableTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create("OnBufferAvailableTest");
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    runner->Run();
    ASSERT_NE(mainThread->handler_, nullptr);

    NodeId nodeId = 1;
    std::weak_ptr<RSContext> weakContext;
    auto listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(weakContext, nodeId);
    listener->OnBufferAvailable();
    ASSERT_EQ(listener->rsContext_.lock(), nullptr);

    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(context, nodeId);
    listener->OnBufferAvailable();
    ASSERT_NE(listener->rsContext_.lock(), nullptr);
    ASSERT_EQ(listener->node_.lock(), nullptr);

    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, context);
    listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(context, nodeId);
    listener->node_ = node;
    listener->OnBufferAvailable();
    ASSERT_NE(listener->node_.lock(), nullptr);

    listener = std::make_shared<RSCanvasDrawingNodeBufferConsumerListener>(context, nodeId);
    listener->node_ = node;
    node->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(nodeId);
    listener->OnBufferAvailable();
    ASSERT_NE(listener->node_.lock()->surfaceHandler_, nullptr);
    ASSERT_EQ(listener->node_.lock()->surfaceHandler_->GetAvailableBufferCount(), 1);
    listener->OnBufferAvailable();
    ASSERT_NE(listener->node_.lock()->surfaceHandler_, nullptr);
    ASSERT_EQ(listener->node_.lock()->surfaceHandler_->GetAvailableBufferCount(), 2);
    usleep(10000);
    listener->OnBufferAvailable();
    listener->node_.lock()->surfaceHandler_ = nullptr;
    usleep(10000);
    ASSERT_EQ(listener->node_.lock()->surfaceHandler_, nullptr);
}
} // namespace OHOS::Rosen
