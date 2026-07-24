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
#include "drawable/rs_misc_drawable.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "render_thread/rs_render_thread_visitor.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSRootRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSRootRenderNodeTest::SetUpTestCase() {}
void RSRootRenderNodeTest::TearDownTestCase() {}
void RSRootRenderNodeTest::SetUp() {}
void RSRootRenderNodeTest::TearDown() {}

/**
 * @tc.name: settings
 * @tc.desc: test results of settings
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, settings, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSRootRenderNode rsRootRenderNode(nodeId, context);

    rsRootRenderNode.AttachRSSurfaceNode(nodeId);
    EXPECT_EQ(rsRootRenderNode.GetRSSurfaceNodeId(), 0);

    float width = 1.0;
    float height = 2.0;
    rsRootRenderNode.UpdateSuggestedBufferSize(width, height);
    EXPECT_EQ(rsRootRenderNode.GetSuggestedBufferWidth(), 1.0);
    EXPECT_EQ(rsRootRenderNode.GetSuggestedBufferHeight(), 2.0);
}

/**
 * @tc.name: Prepare
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Prepare, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSRootRenderNode rsRootRenderNode(nodeId, context);
    rsRootRenderNode.Prepare(visitor);

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsRootRenderNode.Prepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Process, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSRootRenderNode rsRootRenderNode(nodeId, context);
    rsRootRenderNode.Process(visitor);

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsRootRenderNode.Process(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: QuickPrepare
 * @tc.desc: test results of QuickPrepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, QuickPrepare, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSRootRenderNode rsRootRenderNode(nodeId, context);
    rsRootRenderNode.QuickPrepare(visitor);

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsRootRenderNode.QuickPrepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Destructor001
 * @tc.desc: Test RSRootRenderNode destructor with CHILDREN drawable and valid context
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Destructor001, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    NodeId nodeId = 1;
    {
        auto rootRenderNode = std::make_shared<RSRootRenderNode>(nodeId, context);
        ASSERT_NE(rootRenderNode, nullptr);
        auto childrenDrawable = DrawableV2::RSChildrenDrawable::OnGenerate(*rootRenderNode);
        if (childrenDrawable == nullptr) {
            childrenDrawable = std::make_shared<DrawableV2::RSChildrenDrawable>();
        }
        rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)] = childrenDrawable;
        ASSERT_NE(rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)], nullptr);
    }
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Destructor002
 * @tc.desc: Test RSRootRenderNode destructor without CHILDREN drawable (no drawableVec populated)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Destructor002, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    NodeId nodeId = 2;
    {
        auto rootRenderNode = std::make_shared<RSRootRenderNode>(nodeId, context);
        ASSERT_NE(rootRenderNode, nullptr);
    }
    {
        std::weak_ptr<RSContext> expiredContext;
        auto rootRenderNode = std::make_shared<RSRootRenderNode>(nodeId + 1, expiredContext);
        ASSERT_NE(rootRenderNode, nullptr);
        auto childrenDrawable = std::make_shared<DrawableV2::RSChildrenDrawable>();
        rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)] = childrenDrawable;
        ASSERT_NE(rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)], nullptr);
    }
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Destructor003
 * @tc.desc: Test RSRootRenderNode destructor with RT task runner set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRootRenderNodeTest, Destructor003, TestSize.Level1)
{
    auto context = std::make_shared<RSContext>();
    bool taskExecuted = false;
    context->SetRTTaskRunner([&taskExecuted](const std::function<void()>& task) {
        if (task) {
            task();
            taskExecuted = true;
        }
    });
    NodeId nodeId = 3;
    {
        auto rootRenderNode = std::make_shared<RSRootRenderNode>(nodeId, context);
        ASSERT_NE(rootRenderNode, nullptr);
        auto childrenDrawable = std::make_shared<DrawableV2::RSChildrenDrawable>();
        rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)] = childrenDrawable;
        ASSERT_NE(rootRenderNode->GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::CHILDREN)], nullptr);
    }
    EXPECT_TRUE(taskExecuted);
}

} // namespace Rosen
} // namespace OHOS