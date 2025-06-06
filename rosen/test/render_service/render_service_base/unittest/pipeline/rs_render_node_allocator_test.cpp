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

#include "pipeline/rs_canvas_render_node.h"
#include "drawable/rs_render_node_allocator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSRenderNodeAllocatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeAllocatorTest::SetUpTestCase() {}
void RSRenderNodeAllocatorTest::TearDownTestCase() {}
void RSRenderNodeAllocatorTest::SetUp() {}
void RSRenderNodeAllocatorTest::TearDown()
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    while (nodeAllocator.nodeAllocator_.size() > 0) {
        nodeAllocator.nodeAllocator_.pop();
    }
}

/**
 * @tc.name: AddNodeToAllocatorTest
 * @tc.desc: test AddNodeToAllocator
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeAllocatorTest, AddNodeToAllocatorTest, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    auto nodePtr = std::make_shared<RSRenderNode>(0);
    EXPECT_FALSE(nodeAllocator.AddNodeToAllocator(nodePtr.get()));

    auto canvasNodePtr = nodeAllocator.CreateRSCanvasRenderNode(0);
    EXPECT_TRUE(nodeAllocator.AddNodeToAllocator(canvasNodePtr.get()));

    auto newCanvasNodePtr = nodeAllocator.CreateRSCanvasRenderNode(1);
    for (int i = 0; i < 512; i++) {
        auto ptr = std::make_shared<RSCanvasRenderNode>(i);
        nodeAllocator.nodeAllocator_.push(ptr.get());
    }
    EXPECT_FALSE(nodeAllocator.AddNodeToAllocator(newCanvasNodePtr.get()));
}

/**
 * @tc.name: CreateRSCanvasRenderNodeTest
 * @tc.desc: test CreateRSCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeAllocatorTest, CreateRSCanvasRenderNodeTest, TestSize.Level1)
{
    auto& nodeAllocator = RSRenderNodeAllocator::Instance();
    while (nodeAllocator.nodeAllocator_.size() > 0) {
        nodeAllocator.nodeAllocator_.pop();
    }
    auto node = nodeAllocator.CreateRSCanvasRenderNode(0);
    ASSERT_NE(node, nullptr);

    nodeAllocator.AddNodeToAllocator(node.get());
    auto newNode = nodeAllocator.CreateRSCanvasRenderNode(1);
    ASSERT_NE(newNode, nullptr);
}
 

} // namespace Rosen
} // namespace OHOS