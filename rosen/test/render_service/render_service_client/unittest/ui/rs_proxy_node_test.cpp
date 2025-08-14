/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui/rs_proxy_node.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProxyNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSProxyNodeTest::SetUpTestCase() {}
void RSProxyNodeTest::TearDownTestCase() {}
void RSProxyNodeTest::SetUp() {}
void RSProxyNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle001, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    node->ResetContextVariableCache();
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle002, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    node->GetName();
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle003, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    //for test
    int index = 1;
    node->AddChild(child1, index);
    node->RemoveChild(child1);
    node->ClearChildren();
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle004, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    const Vector4f& bounds = {0.1};
    float positionX = 0.1;
    float positionY = 0.1;
    float width = 0.1;
    float height = 0.1;
    node->SetBounds(bounds);
    node->SetBounds(positionX, positionY, width, height);
    node->SetBoundsWidth(width);
    node->SetBoundsHeight(height);
}

/**
 * @tc.name: LifeCycle005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle005, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    const Vector4f& frame = {0.1};
    float positionX = 0.1;
    float positionY = 0.1;
    float width = 0.1;
    float height = 0.1;
    node->SetFrame(frame);
    node->SetFrame(positionX, positionY, width, height);
    node->SetFramePositionX(positionX);
    node->SetFramePositionY(positionY);
}

/**
 * @tc.name: LifeCycle007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSProxyNodeTest, LifeCycle007, TestSize.Level1)
{
	// return shared_ptr
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[1];
    auto node = RSProxyNode::Create(nodeId);
    ASSERT_TRUE(node != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSProxyNode::SharedPtr child2 = RSProxyNode::Create(child1->GetId());
    ASSERT_FALSE(child2 != nullptr);

    node->proxyNodeId_ = 0;
    NodeId res = node->GetHierarchyCommandNodeId();
    ASSERT_FALSE(res != 0);
}
}