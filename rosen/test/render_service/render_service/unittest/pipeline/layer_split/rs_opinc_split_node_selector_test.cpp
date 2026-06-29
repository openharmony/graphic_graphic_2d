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

#include "pipeline/layer_split/selector/opinc/rs_opinc_split_node_selector.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {

std::shared_ptr<RSSurfaceRenderNode> CreateSurfaceNode(NodeId id)
{
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    return node;
}

inline std::shared_ptr<OpincSplitNodeSelector>& GetOpincSelector()
{
    std::shared_ptr<OpincSplitNodeSelector> instance = OpincSplitNodeSelector::GetInstance();
    return instance;
}

void SetupNodeGeometry(const std::shared_ptr<RSRenderNode>& node, float width, float height)
{
    node->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, width, height));
    node->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, width, height));
    node->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
}

class OpincSplitNodeSelectorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(OpincSplitNodeSelectorTest, GetInstance, TestSize.Level1)
{
    auto& instance1 = OpincSplitNodeSelector::GetInstance();
    auto& instance2 = OpincSplitNodeSelector::GetInstance();
    ASSERT_EQ(instance1.get(), instance2.get());
}

HWTEST_F(OpincSplitNodeSelectorTest, MakeProcessor001, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto processor = instance->MakeProcessor();
    ASSERT_NE(processor, nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, MakePlanner001, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto planner = instance->MakePlanner();
    ASSERT_NE(planner, nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, GetCurrParentNode001, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    ASSERT_EQ(instance->GetCurrParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, GetLastParentNode001, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    ASSERT_EQ(instance->GetLastParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, SetCurrParentNode, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto node1 = std::make_shared<RSRenderNode>(1);
    auto node2 = std::make_shared<RSRenderNode>(2);
    instance->SetCurrParentNode(node1);
    ASSERT_NE(instance->GetCurrParentNode(), nullptr);
    instance->SetCurrParentNode(node2);
    ASSERT_NE(instance->GetLastParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, SelectParentNode, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto childNode = CreateSurfaceNode(2);
    SetupNodeGeometry(childNode, 100, 100);
    childNode->GetOpincRootCache().isOpincRootFlag_ = true;
    ASSERT_NE(childNode->GetRenderProperties().GetBoundsGeometry(), nullptr);
    instance->RecordSplitNode(childNode);
    ASSERT_EQ(instance->SelectParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, SelectParentNode004, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto parentNode = CreateSurfaceNode(1);
    SetupNodeGeometry(parentNode, 100, 100);
    auto childNode = CreateSurfaceNode(2);
    SetupNodeGeometry(childNode, 100, 100);
    childNode->GetOpincRootCache().isOpincRootFlag_ = true;
    parentNode->AddChild(childNode);
    instance->RecordSplitNode(childNode);
    auto result = instance->SelectParentNode();
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result, parentNode);
}

HWTEST_F(OpincSplitNodeSelectorTest, SelectParentNode005, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto parentNode1 = CreateSurfaceNode(1);
    SetupNodeGeometry(parentNode1, 100, 100);
    auto parentNode2 = CreateSurfaceNode(3);
    SetupNodeGeometry(parentNode2, 200, 200);
    auto childNode1 = CreateSurfaceNode(2);
    SetupNodeGeometry(childNode1, 50, 50);
    childNode1->GetOpincRootCache().isOpincRootFlag_ = true;
    auto childNode2 = CreateSurfaceNode(4);
    SetupNodeGeometry(childNode2, 100, 100);
    childNode2->GetOpincRootCache().isOpincRootFlag_ = true;
    parentNode1->AddChild(childNode1);
    parentNode2->AddChild(childNode2);
    instance->RecordSplitNode(childNode1);
    instance->RecordSplitNode(childNode2);
    auto result = instance->SelectParentNode();
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result, parentNode2);
}

HWTEST_F(OpincSplitNodeSelectorTest, RecordSplitNode001, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto childNode = CreateSurfaceNode(1);
    childNode->GetOpincRootCache().isOpincRootFlag_ = false;
    instance->RecordSplitNode(childNode);
    ASSERT_EQ(instance->SelectParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, RecordSplitNode002, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto childNode = CreateSurfaceNode(1);
    childNode->GetOpincRootCache().isOpincRootFlag_ = true;
    SetupNodeGeometry(childNode, 100, 100);
    instance->RecordSplitNode(childNode);
    ASSERT_NE(instance->SelectParentNode(), nullptr);
}

HWTEST_F(OpincSplitNodeSelectorTest, RecordSplitNode003, TestSize.Level1)
{
    auto& instance = GetOpincSelector();
    auto childNode1 = CreateSurfaceNode(1);
    childNode1->GetOpincRootCache().isOpincRootFlag_ = true;
    SetupNodeGeometry(childNode1, 50, 50);
    auto childNode2 = CreateSurfaceNode(2);
    childNode2->GetOpincRootCache().isOpincRootFlag_ = true;
    SetupNodeGeometry(childNode2, 100, 100);
    instance->RecordSplitNode(childNode1);
    instance->RecordSplitNode(childNode2);
    ASSERT_NE(instance->SelectParentNode(), nullptr);
}

} // namespace
} // namespace OHOS::Rosen