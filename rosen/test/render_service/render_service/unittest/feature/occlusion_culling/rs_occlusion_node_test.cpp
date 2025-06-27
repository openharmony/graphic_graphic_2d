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
#include "feature/occlusion_culling/rs_occlusion_node.h"

#include "modifier/rs_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "modifier/rs_render_modifier.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSOcclusionNodeTest : public testing::Test {
public:
    static const NodeId nodeId;
    static const NodeId parentId;
    static const NodeId firstNodeId;
    static const NodeId secondNodeId;
    static const NodeId thirdNodeId;
    static const NodeId fouthNodeId;
    static const PropertyId propertyId;
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionNodeTest::SetUpTestCase() {}
void RSOcclusionNodeTest::TearDownTestCase() {}
void RSOcclusionNodeTest::SetUp() {}
void RSOcclusionNodeTest::TearDown() {}

const NodeId RSOcclusionNodeTest::parentId = 0;
const NodeId RSOcclusionNodeTest::nodeId = 1;
const NodeId RSOcclusionNodeTest::firstNodeId = 2;
const NodeId RSOcclusionNodeTest::secondNodeId = 3;
const NodeId RSOcclusionNodeTest::thirdNodeId = 4;
const NodeId RSOcclusionNodeTest::fouthNodeId = 5;
const PropertyId RSOcclusionNodeTest::propertyId = 0;

/*
 * @tc.name: OcclusionNode
 * @tc.desc: Test OcclusionNode
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, OcclusionNode, TestSize.Level1)
{
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::ROOT_NODE);
    EXPECT_EQ(node.id_, nodeId);
    EXPECT_EQ(node.type_, RSRenderNodeType::ROOT_NODE);
}

/*
 * @tc.name: GetId
 * @tc.desc: Test GetId
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, GetId, TestSize.Level1)
{
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::ROOT_NODE);
    EXPECT_EQ(node.GetId(), nodeId);
}

/*
 * @tc.name: GetParentOcNode
 * @tc.desc: Test GetParentOcNode
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, GetParentOcNode, TestSize.Level1)
{
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    node.parentOcNode_ = parentOcNode;
    ASSERT_FALSE(node.GetParentOcNode().expired());
    EXPECT_EQ(node.GetParentOcNode().lock()->GetId(), parentId);
}

/*
 * @tc.name: UpdateChildrenOutOfRectInfo
 * @tc.desc: Test UpdateChildrenOutOfRectInfo
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, UpdateChildrenOutOfRectInfo, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->UpdateChildrenOutOfRectInfo(true);
    EXPECT_TRUE(node->hasChildrenOutOfRect_);
    node->UpdateChildrenOutOfRectInfo(false);
    EXPECT_FALSE(node->hasChildrenOutOfRect_);
}

/*
 * @tc.name: MarkAsRootOcclusionNode
 * @tc.desc: Test MarkAsRootOcclusionNode
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, MarkAsRootOcclusionNode, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->MarkAsRootOcclusionNode();
    EXPECT_TRUE(node->isValidInCurrentFrame_);
    ASSERT_FALSE(node->rootOcclusionNode_.expired());
    EXPECT_EQ(node->rootOcclusionNode_.lock()->GetId(), nodeId);
}

/*
 * @tc.name: IsSubTreeIgnored
 * @tc.desc: Test IsSubTreeIgnored
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, IsSubTreeIgnored, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->isSubTreeIgnored_ = true;
    EXPECT_TRUE(node->IsSubTreeIgnored());
    node->isSubTreeIgnored_ = false;
    EXPECT_FALSE(node->IsSubTreeIgnored());
}

/*
 * @tc.name: ForwardOrderInsert_WithNode_EqualsNullptr
 * @tc.desc: Test ForwardOrderInsert when node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_WithNode_EqualsNullptr, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->ForwardOrderInsert(nullptr);
    EXPECT_EQ(node->lastChild_, nullptr);
}

/*
 * @tc.name: ForwardOrderInsert_WithNodHasOnlyOneChild
 * @tc.desc: Test ForwardOrderInsert when node has only one child
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_WithNodHasOnlyOneChild, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    ASSERT_FALSE(second_node->parentOcNode_.expired());
    EXPECT_EQ(second_node->parentOcNode_.lock()->id_, fisrt_node->id_);
    ASSERT_NE(fisrt_node->firstChild_, nullptr);
    EXPECT_EQ(fisrt_node->firstChild_->id_, second_node->id_);
    ASSERT_NE(fisrt_node->lastChild_, nullptr);
    EXPECT_EQ(fisrt_node->lastChild_->id_, second_node->id_);
    EXPECT_TRUE(second_node->isValidInCurrentFrame_);
}

/*
 * @tc.name: ForwardOrderInsert_WithNodeHasMoreThanOneChild
 * @tc.desc: Test ForwardOrderInsert when node has more than one child
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_WithNodeHasMoreThanOneChild, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(third_node);
    ASSERT_FALSE(second_node->rightSibling_.expired());
    EXPECT_EQ(second_node->rightSibling_.lock()->id_, third_node->id_);
    ASSERT_FALSE(third_node->leftSibling_.expired());
    EXPECT_EQ(third_node->leftSibling_.lock()->id_, second_node->id_);
    ASSERT_NE(fisrt_node->lastChild_, nullptr);
    EXPECT_EQ(fisrt_node->lastChild_->id_, third_node->id_);
}

/*
 * @tc.name: RemoveChild_WithInAnomalousSituations
 * @tc.desc: Test RemoveChild in anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, RemoveChild_WithInAnomalousSituations, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    bool result = fisrt_node->RemoveChild(nullptr);
    EXPECT_FALSE(result);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_FALSE(result);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::ROOT_NODE);
    second_node->parentOcNode_ = third_node;
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: RemoveChild_WithInNonAnomalousSituations
 * @tc.desc: Test RemoveChild in non-anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, RemoveChild_WithInNonAnomalousSituations, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(third_node);
    std::shared_ptr<OcclusionNode> fouth_node =
        std::make_shared<OcclusionNode>(fouthNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(fouth_node);
    NodeId fifthNodeId = 5;
    std::shared_ptr<OcclusionNode> fifth_node =
    std::make_shared<OcclusionNode>(fifthNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(fifth_node);
    bool result = second_node->RemoveChild(fouth_node);
    EXPECT_TRUE(result);
    EXPECT_EQ(fifth_node->leftSibling_.lock()->id_, third_node->id_);
    result = second_node->RemoveChild(fifth_node);
    EXPECT_TRUE(result);
    EXPECT_TRUE(third_node->rightSibling_.expired());
    EXPECT_TRUE(fifth_node->parentOcNode_.expired());
    EXPECT_TRUE(fifth_node->leftSibling_.expired());
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_TRUE(result);
    EXPECT_EQ(fisrt_node->lastChild_, nullptr);
}

/*
 * @tc.name: RemoveSubTree
 * @tc.desc: Test RemoveSubTree
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, RemoveSubTree, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(third_node);
    std::shared_ptr<OcclusionNode> fouth_node =
        std::make_shared<OcclusionNode>(fouthNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(fouth_node);
    std::unordered_map<NodeId, std::shared_ptr<OcclusionNode>> occlusionNodes;
    occlusionNodes[firstNodeId] = fisrt_node;
    occlusionNodes[secondNodeId] = second_node;
    occlusionNodes[thirdNodeId] = third_node;
    occlusionNodes[fouthNodeId] = fouth_node;
    second_node->RemoveSubTree(occlusionNodes);
    int expectSize = 1;
    EXPECT_EQ(occlusionNodes.size(), expectSize);
    EXPECT_NE(occlusionNodes.find(firstNodeId), occlusionNodes.end());
}

/*
 * @tc.name: CollectNodeProperties_WithInAnomalousSituations
 * @tc.desc: Test CollectNodeProperties in anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithInAnomalousSituations, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    RSRenderNode renderNode(nodeId);
    rootNode->CollectNodeProperties(renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
    renderNode.nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_ANIM;
    std::shared_ptr<OcclusionNode> firstNode =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    firstNode->parentOcNode_ = rootNode;
    firstNode->CollectNodeProperties(renderNode);
    EXPECT_TRUE(firstNode->isSubTreeIgnored_);

    renderNode.nodeGroupType_ = RSRenderNode::NodeGroupType::NONE;
    renderNode.isTextureExportNode_ = true;
    firstNode->CollectNodeProperties(renderNode);
    EXPECT_TRUE(firstNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CollectNodeProperties_WithNodeNotInModifierWhiteList
 * @tc.desc: Test CollectNodeProperties with modifier white list
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithNodeNotInModifierWhiteList, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    float stagingValue(1.0f);
    auto renderProperty = std::make_shared<RSRenderProperty<float>>(stagingValue, propertyId);
    auto renderModifier = std::make_shared<RSShadowColorRenderModifier>(renderProperty);
    renderNode->modifiers_.emplace(renderModifier->GetPropertyId(), renderModifier);
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CollectNodeProperties_WithClipToBounds_EqualsTrue
 * @tc.desc: Test CollectNodeProperties when clipToBounds_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithClipToBounds_EqualsTrue, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    Vector4f property(100.f, 100.f, 200.f, 300.f);
    auto boundsProperty = std::make_shared<RSRenderPropertyBase>(propertyId);
    boundsProperty->modifierType_ = RSModifierType::BOUNDS;
    auto boundsModifier = std::make_shared<RSBoundsRenderModifier>(boundsProperty);
    renderNode->modifiers_.emplace(boundsModifier->GetPropertyId(), boundsModifier);
    renderNode->renderContent_->renderProperties_.clipToBounds_ = true;
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CollectNodeProperties_WithInNonAnomalous
 * @tc.desc: Test CollectNodeProperties in non-anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithInNonAnomalous, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    auto boundsProperty = std::make_shared<RSRenderPropertyBase>(propertyId);
    boundsProperty->modifierType_ = RSModifierType::BOUNDS;
    auto boundsModifier = std::make_shared<RSBoundsRenderModifier>(boundsProperty);
    renderNode->modifiers_.emplace(boundsModifier->GetPropertyId(), boundsModifier);
    Vector4f cornerRadius(0.f, 0.f, 0.f, 0.f);
    renderNode->renderContent_->renderProperties_.cornerRadius_ = cornerRadius;
    renderNode->renderContent_->renderProperties_.boundsGeo_->x_ = 0;
    renderNode->renderContent_->renderProperties_.boundsGeo_->y_ = 0;
    renderNode->renderContent_->renderProperties_.boundsGeo_->width_ = 120.f;
    renderNode->renderContent_->renderProperties_.boundsGeo_->height_ = 120.f;
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_FALSE(rootNode->isSubTreeIgnored_);
    EXPECT_FLOAT_EQ(rootNode->drawRect_.left_, renderNode->renderContent_->renderProperties_.boundsGeo_->x_);
    EXPECT_FLOAT_EQ(rootNode->drawRect_.top_, renderNode->renderContent_->renderProperties_.boundsGeo_->y_);
    EXPECT_FLOAT_EQ(rootNode->drawRect_.width_, renderNode->renderContent_->renderProperties_.boundsGeo_->width_);
    EXPECT_FLOAT_EQ(rootNode->drawRect_.height_, renderNode->renderContent_->renderProperties_.boundsGeo_->height_);
}

/*
 * @tc.name: CollectNodeProperties_WithIsBgOpaque_EqualTrue
 * @tc.desc: Test CollectNodeProperties and then isBgOpaque_ will be true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithIsBgOpaque_EqualTrue, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    renderNode->renderContent_->renderProperties_.SetBackgroundColor(RgbPalette::White());
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_TRUE(rootNode->isBgOpaque_);
}

/*
 * @tc.name: CollectNodeProperties_WithisNeedClip_EqualTrue
 * @tc.desc: Test CollectNodeProperties when isNeedClip_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_WithisNeedClip__EqualTrue, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    renderNode->renderContent_->renderProperties_.GetClipRRect().rect_ = {10, 10, 10, 10};
    rootNode->isNeedClip_ = true;
    Vector4f bounds = {10, 10, 10, 10};
    renderNode->renderContent_->renderProperties_.SetBounds(bounds);
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_EQ(rootNode->drawRect_, bounds);
}

/*
 * @tc.name: IsSubTreeShouldIgnored
 * @tc.desc: Test IsSubTreeShouldIgnored with this subtree make bounds non-rectangular.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, IsSubTreeShouldIgnored, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    RSProperties renderProperties;
    renderProperties.clipPath_ = std::make_shared<RSPath>();
    bool result = rootNode->IsSubTreeShouldIgnored(*renderNode, renderProperties);
    EXPECT_TRUE(result);
    renderProperties.clipPath_ = nullptr;
    #undef RS_ENABLE_UNI_RENDER
    result = rootNode->IsSubTreeShouldIgnored(*renderNode, renderProperties);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: CalculateDrawRect
 * @tc.desc: Test CalculateDrawRect with invalid properties.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CalculateDrawRect, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(nodeId);
    renderNode->renderContent_->renderProperties_.SetBounds(Vector4f{0, 0, 0, 0});
    rootNode->CalculateDrawRect(*renderNode, renderNode->renderContent_->renderProperties_);
    EXPECT_FALSE(rootNode->isSubTreeIgnored_);
    renderNode->renderContent_->renderProperties_.boundsGeo_->x_ = std::nan("");
    rootNode->CalculateDrawRect(*renderNode, renderNode->renderContent_->renderProperties_);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CalculateNodeAllBounds_WithIsSubTreeIgnored_EqualsTrue
 * @tc.desc: Test CalculateNodeAllBounds when isSubTreeIgnored_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CalculateNodeAllBounds_WithIsSubTreeIgnored_EqualsTrue, TestSize.Level1)
{
    RectI16 result;
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    rootNode->CalculateNodeAllBounds();
    EXPECT_EQ(rootNode->innerRect_, result);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    rootNode->isSubTreeIgnored_ = true;
    rootNode->CalculateNodeAllBounds();
    EXPECT_EQ(rootNode->innerRect_, result);
}

/*
 * @tc.name: CalculateNodeAllBounds_WithInNonAnomalous
 * @tc.desc: Test CalculateNodeAllBounds in non-anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CalculateNodeAllBounds_WithInNonAnomalous, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    parentOcNode->localScale_ = { 0.5f, 0.5f };
    parentOcNode->absPositions_ = { 50.0f, 100.0f };
    parentOcNode->clipOuterRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    parentOcNode->clipInnerRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    rootNode->parentOcNode_ = parentOcNode;
    rootNode->localPosition_ = { 60.0f, 110.0f };
    rootNode->drawRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    rootNode->cornerRadius_ = { 5.0f, 5.0f, 5.0f, 5.0f };
    rootNode->CalculateNodeAllBounds();
    // calculate the rootNode's scaled rect, offset it by the parent's absolute coordinates,
    // and then take the intersection with the parent's clipping rect
    RectI16 expectInnerRect = { 80, 135, 150, 140};
    // the outer rect will be added with cornerRadius
    RectI16 expectOuterRect = { 80, 130, 150, 150};
    EXPECT_EQ(rootNode->innerRect_.left_, expectInnerRect.left_);
    EXPECT_EQ(rootNode->innerRect_.top_, expectInnerRect.top_);
    EXPECT_EQ(rootNode->innerRect_.width_, expectInnerRect.width_);
    EXPECT_EQ(rootNode->innerRect_.height_, expectInnerRect.height_);
    EXPECT_EQ(rootNode->outerRect_.left_, expectOuterRect.left_);
    EXPECT_EQ(rootNode->outerRect_.top_, expectOuterRect.top_);
    EXPECT_EQ(rootNode->outerRect_.width_, expectOuterRect.width_);
    EXPECT_EQ(rootNode->outerRect_.height_, expectOuterRect.height_);
}

/*
 * @tc.name: UpdateClipRect
 * @tc.desc: Test UpdateClipRect
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, UpdateClipRect, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(nodeId);
    renderNode->renderContent_->renderProperties_.boundsGeo_->width_ = 100.0f;
    renderNode->renderContent_->renderProperties_.boundsGeo_->height_ = 200.0f;
    rootNode->UpdateClipRect(*renderNode);
    int initLeft = 0 ;
    int initTop = 0;
    EXPECT_EQ(rootNode->clipOuterRect_.left_, initLeft);
    EXPECT_EQ(rootNode->clipOuterRect_.top_, initTop);
    EXPECT_EQ(rootNode->clipOuterRect_.width_, renderNode->renderContent_->renderProperties_.boundsGeo_->width_);
    EXPECT_EQ(rootNode->clipOuterRect_.height_, renderNode->renderContent_->renderProperties_.boundsGeo_->height_);
    EXPECT_EQ(rootNode->clipInnerRect_.left_, initLeft);
    EXPECT_EQ(rootNode->clipInnerRect_.top_, initTop);
    EXPECT_EQ(rootNode->clipInnerRect_.width_, renderNode->renderContent_->renderProperties_.boundsGeo_->width_);
    EXPECT_EQ(rootNode->clipInnerRect_.height_, renderNode->renderContent_->renderProperties_.boundsGeo_->height_);
}

/*
 * @tc.name: UpdateSubTreeProp
 * @tc.desc: Test UpdateSubTreeProp
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, UpdateSubTreeProp, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> children =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    parentOcNode->localScale_ = { 0.5f, 0.5f };
    parentOcNode->absPositions_ = { 50.0f, 100.0f };
    parentOcNode->clipOuterRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    parentOcNode->clipInnerRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    rootNode->parentOcNode_ = parentOcNode;
    rootNode->lastChild_ = children;
    rootNode->localPosition_ = { 60.0f, 110.0f };
    rootNode->drawRect_ = { 60.0f, 60.0f, 300.0f, 300.0f };
    children->drawRect_ = { 40.0f, 40.0f, 400.0f, 400.0f };
    children->parentOcNode_ = rootNode;
    rootNode->cornerRadius_ = { 5.0f, 5.0f, 5.0f, 5.0f };
    // It is actually just recursively calling the method CalculateNodeAllBounds
    rootNode->UpdateSubTreeProp();
    RectI16 expectRootInnerRect = { 80, 135, 150, 140};
    EXPECT_EQ(rootNode->innerRect_.left_, expectRootInnerRect.left_);
    EXPECT_EQ(rootNode->innerRect_.top_, expectRootInnerRect.top_);
    EXPECT_EQ(rootNode->innerRect_.width_, expectRootInnerRect.width_);
    EXPECT_EQ(rootNode->innerRect_.height_, expectRootInnerRect.height_);
    RectI16 expectRootOuterRect = { 80, 130, 150, 150};
    EXPECT_EQ(rootNode->outerRect_.left_, expectRootOuterRect.left_);
    EXPECT_EQ(rootNode->outerRect_.top_, expectRootOuterRect.top_);
    EXPECT_EQ(rootNode->outerRect_.width_, expectRootOuterRect.width_);
    EXPECT_EQ(rootNode->outerRect_.height_, expectRootOuterRect.height_);
    RectI16 expectChildrenInnerRect = { 100, 175, 200, 185};
    EXPECT_EQ(children->innerRect_.left_, expectChildrenInnerRect.left_);
    EXPECT_EQ(children->innerRect_.top_, expectChildrenInnerRect.top_);
    EXPECT_EQ(children->innerRect_.width_, expectChildrenInnerRect.width_);
    EXPECT_EQ(children->innerRect_.height_, expectChildrenInnerRect.height_);
    RectI16 expectChildrenOuterRect = { 100, 175, 200, 185};
    EXPECT_EQ(children->outerRect_.left_, expectChildrenOuterRect.left_);
    EXPECT_EQ(children->outerRect_.top_, expectChildrenOuterRect.top_);
    EXPECT_EQ(children->outerRect_.width_, expectChildrenOuterRect.width_);
    EXPECT_EQ(children->outerRect_.height_, expectChildrenOuterRect.height_);
}

/*
 * @tc.name: UpdateSubTreePropWithRing
 * @tc.desc: Test UpdateSubTreePropWithRing
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, UpdateSubTreePropWithRing, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> children =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    rootNode->lastChild_ = children;
    rootNode->parentOcNode_ = children;
    children->lastChild_ = rootNode;
    children->parentOcNode_ = rootNode;
    rootNode->UpdateSubTreeProp();
    // In this case, the parent node is also the child node, but each node only update once.
    EXPECT_EQ(rootNode->isValidInCurrentFrame_, true);
    EXPECT_EQ(children->isValidInCurrentFrame_, true);
}

/*
 * @tc.name: DetectOcclusion_WithIsValidInCurrentFrame_EqualsFalse
 * @tc.desc: Test DetectOcclusion when isValidInCurrentFrame_ is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithIsValidInCurrentFrame_EqualsFalse, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    // the first node with isValidInCurrentFrame_ is false of the subtree will be collected into offTreeNodes
    int expectSize = 1;
    EXPECT_EQ(offTreeNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithIsOutOfRootRect_EqualsTrue
 * @tc.desc: Test DetectOcclusion when isOutOfRootRect_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithIsOutOfRootRect_EqualsTrue, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    int expectSize = 3;
    // node width isOutOfRootRect_ is true will be occluded
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithNodeType
 * @tc.desc: Test DetectOcclusion with node type
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithNodeType, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::SURFACE_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    // only CANVAS_NODE can be occluded
    int expectSize = 2;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithIsNeedClip_False
 * @tc.desc: Test DetectOcclusion with isNeedClip_ is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithIsNeedClip_False, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    firstChild->isNeedClip_ = true;
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    // node with isOutOfRootRect_ is true and isNeedClip_ is false will be occluded directly
    int expectSize = 2;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithMultipleNodesOccludingOneNode
 * @tc.desc: Test DetectOcclusion with multiple nodes occluding one node
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithMultipleNodesOccludingOneNode, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isBgOpaque_ = true;
    thirdChild->isAlphaNeed_ = false;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isBgOpaque_ = true;
    secondChild->isAlphaNeed_ = false;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isBgOpaque_ = true;
    firstChild->isAlphaNeed_ = false;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isBgOpaque_ = true;
    rootNode->isAlphaNeed_ = false;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    rootNode->innerRect_ = { 0, 0, 1260, 2720 };
    rootNode->outerRect_ = { 0, 0, 1260, 2720 };
    firstChild->innerRect_ = { 10, 10, 640, 780 };
    firstChild->outerRect_ = { 10, 10, 640, 780 };
    secondChild->innerRect_ = { 10, 10, 640, 390 };
    secondChild->outerRect_ = { 10, 10, 640, 390 };
    thirdChild->innerRect_ = { 10, 400, 640, 390 };
    thirdChild->outerRect_ = { 10, 400, 640, 390 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    // nodes two and three combined can occlude the first node.
    // but we do not consider multiple nodes occluding one node
    int expectSize = 0;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithTheLeftSideOffByOnePixel
 * @tc.desc: Test DetectOcclusion with node will be not occluded as expected
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithTheLeftSideOffByOnePixel, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isBgOpaque_ = true;
    thirdChild->isAlphaNeed_ = false;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isBgOpaque_ = true;
    secondChild->isAlphaNeed_ = false;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isBgOpaque_ = true;
    firstChild->isAlphaNeed_ = false;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isBgOpaque_ = true;
    rootNode->isAlphaNeed_ = false;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    rootNode->innerRect_ = { 0, 0, 1260, 2720 };
    rootNode->outerRect_ = { 0, 0, 1260, 2720 };
    firstChild->innerRect_ = { 10, 10, 640, 780 };
    firstChild->outerRect_ = { 10, 10, 640, 780 };
    secondChild->innerRect_ = { 10, 10, 640, 390 };
    secondChild->outerRect_ = { 10, 10, 640, 390 };
    // the left side was off by 1 pixel and did not cover completely, so it was not occluded.
    thirdChild->innerRect_ = { 11, 10, 880, 1360 };
    thirdChild->outerRect_ = { 11, 10, 880, 1360 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    int expectSize = 0;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithInnerAndOuterRect
 * @tc.desc: Test DetectOcclusion with inner and outer rect
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithInnerAndOuterRect, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isBgOpaque_ = true;
    thirdChild->isAlphaNeed_ = false;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isBgOpaque_ = true;
    secondChild->isAlphaNeed_ = false;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isBgOpaque_ = true;
    firstChild->isAlphaNeed_ = false;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isBgOpaque_ = true;
    rootNode->isAlphaNeed_ = false;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    rootNode->innerRect_ = { 0, 0, 1260, 2720 };
    rootNode->outerRect_ = { 0, 0, 1260, 2720 };
    firstChild->innerRect_ = { 20, 20, 320, 390 };
    firstChild->outerRect_ = { 10, 10, 640, 780 };
    secondChild->innerRect_ = { 11, 11, 640, 780 };
    secondChild->outerRect_ = { 10, 10, 640, 780 };
    thirdChild->innerRect_ = { 10, 400, 640, 390 };
    thirdChild->outerRect_ = { 10, 400, 640, 390 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    // occlude others using inner rect
    // be occluded by others using outer rect
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    int expectSize = 0;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_WithInNonAnomalous
 * @tc.desc: Test DetectOcclusion in non-anomalous situations.
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_WithInNonAnomalous, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::CANVAS_NODE);
    firstChild->UpdateChildrenOutOfRectInfo(false);
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    secondChild->UpdateChildrenOutOfRectInfo(false);
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    std::shared_ptr<OcclusionNode> fouthChild =
        std::make_shared<OcclusionNode>(fouthNodeId, RSRenderNodeType::CANVAS_NODE);
    fouthChild->isValidInCurrentFrame_ = true;
    fouthChild->isBgOpaque_ = true;
    fouthChild->isAlphaNeed_ = false;
    fouthChild->isSubTreeIgnored_ = true;
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isBgOpaque_ = true;
    thirdChild->isAlphaNeed_ = false;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isBgOpaque_ = true;
    secondChild->isAlphaNeed_ = false;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isBgOpaque_ = true;
    firstChild->isAlphaNeed_ = false;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isBgOpaque_ = true;
    rootNode->isAlphaNeed_ = false;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = fouthChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    fouthChild->leftSibling_ = thirdChild;
    rootNode->innerRect_ = { 0, 0, 1260, 2720 };
    rootNode->outerRect_ = { 0, 0, 1260, 2720 };
    firstChild->innerRect_ = { 10, 10, 640, 780 };
    firstChild->outerRect_ = { 10, 10, 640, 780 };
    secondChild->innerRect_ = { 10, 10, 640, 390 };
    secondChild->outerRect_ = { 10, 10, 640, 390 };
    thirdChild->innerRect_ = { 9, 9, 641, 781 };
    thirdChild->outerRect_ = { 9, 9, 641, 781 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    // first node will be occludered by third node
    // second node will be entire subtree occludered by third node
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    int expectSize = 1;
    EXPECT_EQ(culledNodes.size(), expectSize);
    EXPECT_EQ(culledEntireSubtree.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_Ringed_OcclusionTree
 * @tc.desc: Test DetectOcclusion with ringed occlusion tree
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_Ringed_OcclusionTree, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::CANVAS_NODE);
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isBgOpaque_ = true;
    thirdChild->isAlphaNeed_ = false;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isBgOpaque_ = true;
    secondChild->isAlphaNeed_ = false;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isBgOpaque_ = true;
    firstChild->isAlphaNeed_ = false;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isBgOpaque_ = true;
    rootNode->isAlphaNeed_ = false;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    secondChild->lastChild_ = thirdChild;
    thirdChild->leftSibling_ = secondChild;
    rootNode->innerRect_ = { 0, 0, 1260, 2720 };
    rootNode->outerRect_ = { 0, 0, 1260, 2720 };
    firstChild->innerRect_ = { 10, 10, 640, 780 };
    firstChild->outerRect_ = { 10, 10, 640, 780 };
    secondChild->innerRect_ = { 10, 10, 640, 390 };
    secondChild->outerRect_ = { 10, 10, 640, 390 };
    thirdChild->innerRect_ = { 9, 9, 641, 781 };
    thirdChild->outerRect_ = { 9, 9, 641, 781 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    rootNode->DetectOcclusion(culledNodes, culledEntireSubtree, offTreeNodes);
    EXPECT_NE(culledNodes.find(firstChildId), culledNodes.end());
    EXPECT_NE(culledEntireSubtree.find(secondChildId), culledEntireSubtree.end());
}

/*
 * @tc.name: PreorderTraversal
 * @tc.desc: Test PreorderTraversal
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, PreorderTraversal, TestSize.Level1)
{
    std::vector<std::shared_ptr<OcclusionNode>> result;
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isSubTreeIgnored_ = true;
    rootNode->PreorderTraversal(result);
    int expectSize = 0;
    EXPECT_EQ(result.size(), expectSize);
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::CANVAS_NODE);
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    rootNode->lastChild_ = firstChild;
    rootNode->firstChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    rootNode->isSubTreeIgnored_ = false;
    rootNode->PreorderTraversal(result);
    expectSize = 3;
    EXPECT_EQ(result.size(), expectSize);
}

/*
 * @tc.name: CheckNodeOcclusion
 * @tc.desc: Test CheckNodeOcclusion with isNeedClip_ is false
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CheckNodeOcclusion, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    OcclusionCoverageInfo coverageInfo;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> culledEntireSubtree;
    firstChild->isNeedClip_ = false;
    firstChild->hasChildrenOutOfRect_ = true;
    firstChild->isOutOfRootRect_ = true;
    firstChild->CheckNodeOcclusion(coverageInfo, culledNodes, culledEntireSubtree);
    EXPECT_NE(culledNodes.find(firstNodeId), culledNodes.end());
}

/*
 * @tc.name: IsOutOfRootRect
 * @tc.desc: Test IsOutOfRootRect
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, IsOutOfRootRect, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    RectI16 rectI16 = {10, 10, 10, 10};
    firstChild->rootOcclusionNode_ = rootNode->weak_from_this();
    bool result = firstChild->IsOutOfRootRect(rectI16);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: CalculateNodeAllBounds
 * @tc.desc: Test CalculateNodeAllBounds with isNeedClip_ is true
 * @tc.type: FUNC
 * @tc.require: issueIC2H2
 */
HWTEST_F(RSOcclusionNodeTest, CalculateNodeAllBounds, TestSize.Level1)
{
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::CANVAS_NODE);
    firstChild->parentOcNode_ = rootNode->weak_from_this();
    firstChild->isNeedClip_ = true;
    firstChild->CalculateNodeAllBounds();
    EXPECT_EQ(firstChild->clipOuterRect_, firstChild->outerRect_);
}

} // namespace OHOS::Rosen