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

#include "pipeline/rs_base_render_node.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBaseRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSBaseRenderNodeTest::SetUpTestCase() {}
void RSBaseRenderNodeTest::TearDownTestCase() {}
void RSBaseRenderNodeTest::SetUp() {}
void RSBaseRenderNodeTest::TearDown() {}

/**
 * @tc.name: AddChild001
 * @tc.desc: test results of AddChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, AddChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto childtwo = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
    ASSERT_TRUE(childone->IsOnTheTree());

    index = 0;
    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
    ASSERT_TRUE(childtwo->IsOnTheTree());

    node->AddChild(childtwo, index);
    ASSERT_EQ(node->GetChildrenCount(), 2);
}

/**
 * @tc.name: RemoveChild001
 * @tc.desc: test results of RemoveChild
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. RemoveChild
     */
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: ClearChildren001
 * @tc.desc: test results of ClearChildren
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSBaseRenderNodeTest, ClearChildren001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: MoveChild001
 * @tc.desc: test results of MoveChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, MoveChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->MoveChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
}

/**
 * @tc.name: SetIsOnTheTree001
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetIsOnTheTree001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChild001
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->ClearChildren();
    ASSERT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: RemoveCrossParentChild001
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveCrossParentChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. AddChild
     */
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto newParent = std::make_shared<RSBaseRenderNode>(id + 2, context);
    int index = -1;
    node->AddCrossParentChild(childone, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);

    /**
     * @tc.steps: step2. ClearChildren
     */
    node->RemoveCrossParentChild(childone, newParent);
}

/**
 * @tc.name: SetIsOnTheTree002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, SetIsOnTheTree002, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    auto childone = nullptr;
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(childone, index);
    bool skipTransition = false;
    node->RemoveChild(childone, skipTransition);
    EXPECT_EQ(node->GetChildrenCount(), 0);
}

/**
 * @tc.name: AddCrossParentChildTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, AddCrossParentChildTest001, TestSize.Level1)
{
    int32_t index = 1;
    int32_t index_ = 0;
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::shared_ptr<RSBaseRenderNode> child_;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->AddCrossParentChild(child, index);
    node->AddCrossParentChild(child_, index_);
}

/**
 * @tc.name: RemoveCrossParentChildTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveCrossParentChildTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSBaseRenderNode> child = nullptr;
    std::weak_ptr<RSBaseRenderNode> newParent;
    node->RemoveCrossParentChild(child, newParent);
}

/**
 * @tc.name: RemoveFromTreeTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, RemoveFromTreeTest, TestSize.Level1)
{
    bool skipTransition = false;
    bool skipTransition_ = true;
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    node->RemoveFromTree(skipTransition);
    node->RemoveFromTree(skipTransition_);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBaseRenderNodeTest, PrepareTest, TestSize.Level1)
{
    auto node = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);
}

} // namespace OHOS::Rosen