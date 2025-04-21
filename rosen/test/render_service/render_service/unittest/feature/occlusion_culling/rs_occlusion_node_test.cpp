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
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionNodeTest::SetUpTestCase() {}
void RSOcclusionNodeTest::TearDownTestCase() {}
void RSOcclusionNodeTest::SetUp() {}
void RSOcclusionNodeTest::TearDown() {}

/*
 * @tc.name: OcclusionNode_001
 * @tc.desc: Test OcclusionNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, OcclusionNode_001, TestSize.Level1)
{
    NodeId nodeId(0);
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::ROOT_NODE);
    EXPECT_EQ(node.id_, nodeId);
    EXPECT_EQ(node.type_, RSRenderNodeType::ROOT_NODE);
}

/*
 * @tc.name: GetId_001
 * @tc.desc: Test GetId
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, GetId_001, TestSize.Level1)
{
    NodeId nodeId(0);
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::ROOT_NODE);
    EXPECT_EQ(node.GetId(), nodeId);
}

/*
 * @tc.name: GetParentOcNode_001
 * @tc.desc: Test GetParentOcNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, GetParentOcNode_001, TestSize.Level1)
{
    NodeId nodeId(1);
    OcclusionNode node = OcclusionNode(nodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    node.parentOcNode_ = parentOcNode;
    EXPECT_EQ(node.GetParentOcNode().lock()->GetId(), parentId);
}

/*
 * @tc.name: UpdateChildrenOutOfRectInfo_001
 * @tc.desc: Test UpdateChildrenOutOfRectInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, UpdateChildrenOutOfRectInfo_001, TestSize.Level1)
{
    NodeId nodeId(0);
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->UpdateChildrenOutOfRectInfo(true);
    EXPECT_TRUE(node->hasChildrenOutOfRect_);
    node->UpdateChildrenOutOfRectInfo(false);
    EXPECT_FALSE(node->hasChildrenOutOfRect_);
}

/*
 * @tc.name: MarkAsRootOcclusionNode_001
 * @tc.desc: Test MarkAsRootOcclusionNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, MarkAsRootOcclusionNode_001, TestSize.Level1)
{
    NodeId nodeId(0);
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->MarkAsRootOcclusionNode();
    EXPECT_TRUE(node->isValidInCurrentFrame_);
    EXPECT_EQ(node->rootOcclusionNode_.lock()->GetId(), nodeId);
}

/*
 * @tc.name: IsSubTreeIgnored_001
 * @tc.desc: Test IsSubTreeIgnored
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, IsSubTreeIgnored_001, TestSize.Level1)
{
    NodeId nodeId(0);
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->isSubTreeIgnored_ = true;
    EXPECT_TRUE(node->IsSubTreeIgnored());
    node->isSubTreeIgnored_ = false;
    EXPECT_FALSE(node->IsSubTreeIgnored());
}

/*
 * @tc.name: ForwardOrderInsert_001
 * @tc.desc: Test ForwardOrderInsert
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_001, TestSize.Level1)
{
    NodeId nodeId(0);
    std::shared_ptr<OcclusionNode> node = std::make_shared<OcclusionNode>(nodeId, RSRenderNodeType::ROOT_NODE);
    node->ForwardOrderInsert(nullptr);
    EXPECT_EQ(node->lastChild_, nullptr);
}

/*
 * @tc.name: ForwardOrderInsert_002
 * @tc.desc: Test ForwardOrderInsert
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_002, TestSize.Level1)
{
    NodeId firstNodeId(0);
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    NodeId secondNodeId(1);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    EXPECT_EQ(second_node->parentOcNode_.lock()->id_, fisrt_node->id_);
    EXPECT_EQ(fisrt_node->firstChild_->id_, second_node->id_);
    EXPECT_EQ(fisrt_node->lastChild_->id_, second_node->id_);
    EXPECT_TRUE(second_node->isValidInCurrentFrame_);
}

/*
 * @tc.name: ForwardOrderInsert_003
 * @tc.desc: Test ForwardOrderInsert
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, ForwardOrderInsert_003, TestSize.Level1)
{
    NodeId firstNodeId(0);
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    NodeId secondNodeId(1);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    NodeId thirdNodeId(2);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(third_node);
    EXPECT_EQ(second_node->rightSibling_.lock()->id_, third_node->id_);
    EXPECT_EQ(third_node->leftSibling_.lock()->id_, second_node->id_);
    EXPECT_EQ(fisrt_node->lastChild_->id_, third_node->id_);
}

/*
 * @tc.name: RemoveChild_001
 * @tc.desc: Test RemoveChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, RemoveChild_001, TestSize.Level1)
{
    NodeId firstNodeId(0);
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    bool result = fisrt_node->RemoveChild(nullptr);
    EXPECT_FALSE(result);
    NodeId secondNodeId(1);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_FALSE(result);
    NodeId thirdNodeId(2);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::ROOT_NODE);
    second_node->parentOcNode_ = third_node;
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: RemoveChild_002
 * @tc.desc: Test RemoveChild
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, RemoveChild_002, TestSize.Level1)
{
    NodeId firstNodeId(0);
    std::shared_ptr<OcclusionNode> fisrt_node =
        std::make_shared<OcclusionNode>(firstNodeId, RSRenderNodeType::ROOT_NODE);
    NodeId secondNodeId(1);
    std::shared_ptr<OcclusionNode> second_node =
        std::make_shared<OcclusionNode>(secondNodeId, RSRenderNodeType::CANVAS_NODE);
    fisrt_node->ForwardOrderInsert(second_node);
    NodeId thirdNodeId(2);
    std::shared_ptr<OcclusionNode> third_node =
        std::make_shared<OcclusionNode>(thirdNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(third_node);
    NodeId fouthNodeId(3);
    std::shared_ptr<OcclusionNode> fouth_node =
        std::make_shared<OcclusionNode>(fouthNodeId, RSRenderNodeType::CANVAS_NODE);
    second_node->ForwardOrderInsert(fouth_node);
    bool result = second_node->RemoveChild(fouth_node);
    EXPECT_TRUE(result);
    EXPECT_TRUE(third_node->rightSibling_.expired());
    EXPECT_TRUE(fouth_node->parentOcNode_.expired());
    EXPECT_TRUE(fouth_node->leftSibling_.expired());
    result = fisrt_node->RemoveChild(second_node);
    EXPECT_TRUE(result);
    EXPECT_EQ(fisrt_node->lastChild_, nullptr);
}

/*
 * @tc.name: CollectNodeProperties_001
 * @tc.desc: Test CollectNodeProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_001, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode = std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
    RSRenderNode renderNode(rootNodeId);

    rootNode->CollectNodeProperties(renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);

    renderNode.nodeGroupType_ = RSRenderNode::NodeGroupType::GROUPED_BY_ANIM;
    NodeId firstNodeId(0);
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
 * @tc.name: CollectNodeProperties_002
 * @tc.desc: Test CollectNodeProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_002, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    float stagingValue(1.0f);
    PropertyId propertyId(1);
    auto renderProperty = std::make_shared<RSRenderProperty<float>>(stagingValue, propertyId);
    auto renderModifier = std::make_shared<RSShadowColorRenderModifier>(renderProperty);
    renderNode->modifiers_.emplace(renderModifier->GetPropertyId(), renderModifier);
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CollectNodeProperties_003
 * @tc.desc: Test CollectNodeProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_003, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    Vector4f property(100.f, 100.f, 200.f, 300.f);
    PropertyId propertyId(0);
    auto boundsProperty = std::make_shared<RSRenderPropertyBase>(propertyId);
    boundsProperty->modifierType_ = RSModifierType::BOUNDS;
    auto boundsModifier = std::make_shared<RSBoundsRenderModifier>(boundsProperty);
    renderNode->modifiers_.emplace(boundsModifier->GetPropertyId(), boundsModifier);
    renderNode->renderContent_->renderProperties_.clipToBounds_ = true;
    rootNode->CollectNodeProperties(*renderNode);
    EXPECT_TRUE(rootNode->isSubTreeIgnored_);
}

/*
 * @tc.name: CollectNodeProperties_004
 * @tc.desc: Test CollectNodeProperties
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CollectNodeProperties_004, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(parentId);
    PropertyId propertyId(0);
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
 * @tc.name: CalculateNodeAllBounds_001
 * @tc.desc: Test CalculateNodeAllBounds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CalculateNodeAllBounds_001, TestSize.Level1)
{
    RectI16 result;
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    rootNode->CalculateNodeAllBounds();
    EXPECT_EQ(rootNode->innerRect_, result);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    rootNode->parentOcNode_ = parentOcNode;
    rootNode->isSubTreeIgnored_ = true;
    rootNode->CalculateNodeAllBounds();
    EXPECT_EQ(rootNode->innerRect_, result);
}

/*
 * @tc.name: CalculateNodeAllBounds_002
 * @tc.desc: Test CalculateNodeAllBounds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, CalculateNodeAllBounds_002, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
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
    // rootNode
    rootNode->CalculateNodeAllBounds();
    EXPECT_EQ(rootNode->innerRect_.left_, 80);
    EXPECT_EQ(rootNode->innerRect_.top_, 135);
    EXPECT_EQ(rootNode->innerRect_.width_, 150);
    EXPECT_EQ(rootNode->innerRect_.height_, 140);
    EXPECT_EQ(rootNode->outerRect_.left_, 80);
    EXPECT_EQ(rootNode->outerRect_.top_, 130);
    EXPECT_EQ(rootNode->outerRect_.width_, 150);
    EXPECT_EQ(rootNode->outerRect_.height_, 150);
}

/*
 * @tc.name: UpdateClipRect_001
 * @tc.desc: Test UpdateClipRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, UpdateClipRect_001, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId renderNodeId(0);
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(renderNodeId);
    renderNode->renderContent_->renderProperties_.boundsGeo_->width_ = 100.0f;
    renderNode->renderContent_->renderProperties_.boundsGeo_->height_ = 200.0f;
    rootNode->UpdateClipRect(*renderNode);
    EXPECT_EQ(rootNode->clipOuterRect_.left_, 0);
    EXPECT_EQ(rootNode->clipOuterRect_.top_, 0);
    EXPECT_EQ(rootNode->clipOuterRect_.width_, 100);
    EXPECT_EQ(rootNode->clipOuterRect_.height_, 200);
    EXPECT_EQ(rootNode->clipInnerRect_.left_, 0);
    EXPECT_EQ(rootNode->clipInnerRect_.top_, 0);
    EXPECT_EQ(rootNode->clipInnerRect_.width_, 100);
    EXPECT_EQ(rootNode->clipInnerRect_.height_, 200);
}

/*
 * @tc.name: UpdateSubTreeProp_001
 * @tc.desc: Test UpdateSubTreeProp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, UpdateSubTreeProp_001, TestSize.Level1)
{
    NodeId rootNodeId(1);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::CANVAS_NODE);
    NodeId parentId(0);
    std::shared_ptr<OcclusionNode> parentOcNode =
        std::make_shared<OcclusionNode>(parentId, RSRenderNodeType::ROOT_NODE);
    NodeId childId(2);
    std::shared_ptr<OcclusionNode> children =
        std::make_shared<OcclusionNode>(childId, RSRenderNodeType::CANVAS_NODE);
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
    rootNode->UpdateSubTreeProp();
    EXPECT_EQ(rootNode->innerRect_.left_, 80);
    EXPECT_EQ(rootNode->innerRect_.top_, 135);
    EXPECT_EQ(rootNode->innerRect_.width_, 150);
    EXPECT_EQ(rootNode->innerRect_.height_, 140);

    EXPECT_EQ(rootNode->outerRect_.left_, 80);
    EXPECT_EQ(rootNode->outerRect_.top_, 130);
    EXPECT_EQ(rootNode->outerRect_.width_, 150);
    EXPECT_EQ(rootNode->outerRect_.height_, 150);

    EXPECT_EQ(children->innerRect_.left_, 100);
    EXPECT_EQ(children->innerRect_.top_, 175);
    EXPECT_EQ(children->innerRect_.width_, 200);
    EXPECT_EQ(children->innerRect_.height_, 185);

    EXPECT_EQ(children->outerRect_.left_, 100);
    EXPECT_EQ(children->outerRect_.top_, 175);
    EXPECT_EQ(children->outerRect_.width_, 200);
    EXPECT_EQ(children->outerRect_.height_, 185);
}

/*
 * @tc.name: DetectOcclusion_001
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_001, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode = std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
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
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 1;
    EXPECT_EQ(offTreeNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_002
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_002, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::CANVAS_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 3;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_003
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_003, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::SURFACE_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 2;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_004
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_004, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOutOfRootRect_ = true;
    NodeId firstChildId(1);
    std::shared_ptr<OcclusionNode> firstChild =
        std::make_shared<OcclusionNode>(firstChildId, RSRenderNodeType::CANVAS_NODE);
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOutOfRootRect_ = true;
    firstChild->isNeedClip_ = true;
    NodeId secondChildId(2);
    std::shared_ptr<OcclusionNode> secondChild =
        std::make_shared<OcclusionNode>(secondChildId, RSRenderNodeType::CANVAS_NODE);
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOutOfRootRect_ = true;
    NodeId thirdChildId(3);
    std::shared_ptr<OcclusionNode> thirdChild =
        std::make_shared<OcclusionNode>(thirdChildId, RSRenderNodeType::CANVAS_NODE);
    thirdChild->isValidInCurrentFrame_ = true;
    thirdChild->isOutOfRootRect_ = true;
    rootNode->lastChild_ = firstChild;
    firstChild->lastChild_ = thirdChild;
    firstChild->firstChild_ = secondChild;
    thirdChild->leftSibling_ = secondChild;
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 2;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_005
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_005, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
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
    thirdChild->isOpaque_ = true;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOpaque_ = true;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOpaque_ = true;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOpaque_ = true;
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
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    // Do not consider multiple nodes occluding one node
    EXPECT_EQ(culledNodes.size(), 0);
}

/*
 * @tc.name: DetectOcclusion_006
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_006, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
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
    thirdChild->isOpaque_ = true;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOpaque_ = true;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOpaque_ = true;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOpaque_ = true;
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
    // One pixel difference on the left
    thirdChild->innerRect_ = { 11, 10, 880, 1360 };
    thirdChild->outerRect_ = { 11, 10, 880, 1360 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 0;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_007
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_007, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
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
    thirdChild->isOpaque_ = true;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOpaque_ = true;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOpaque_ = true;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOpaque_ = true;
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
    // Inner and outer
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 0;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

/*
 * @tc.name: DetectOcclusion_008
 * @tc.desc: Test DetectOcclusion
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOcclusionNodeTest, DetectOcclusion_008, TestSize.Level1)
{
    NodeId rootNodeId(0);
    std::shared_ptr<OcclusionNode> rootNode =
        std::make_shared<OcclusionNode>(rootNodeId, RSRenderNodeType::ROOT_NODE);
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
    thirdChild->isOpaque_ = true;
    secondChild->isValidInCurrentFrame_ = true;
    secondChild->isOpaque_ = true;
    firstChild->isValidInCurrentFrame_ = true;
    firstChild->isOpaque_ = true;
    rootNode->isValidInCurrentFrame_ = true;
    rootNode->isOpaque_ = true;
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
    thirdChild->innerRect_ = { 9, 9, 641, 781 };
    thirdChild->outerRect_ = { 9, 9, 641, 781 };
    std::unordered_set<NodeId> culledNodes;
    std::unordered_set<NodeId> offTreeNodes;
    // normal
    rootNode->DetectOcclusion(culledNodes, offTreeNodes);
    int expectSize = 2;
    EXPECT_EQ(culledNodes.size(), expectSize);
}

} // namespace OHOS::Rosen