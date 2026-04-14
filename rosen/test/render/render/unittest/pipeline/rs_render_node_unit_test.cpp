/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "dirty_region/rs_optimize_canvas_dirty_collector.h"
#include "drawable/rs_color_picker_drawable.h"
#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "drawable/rs_property_drawable_foreground.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "offscreen_render/rs_offscreen_render_thread.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_filter.h"
#include "skia_adapter/skia_canvas.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const std::string OUT_STR1 =
    "DISPLAY_NODERS_NODESURFACE_NODECANVAS_NODEROOT_NODEPROXY_NODECANVAS_DRAWING_NODEEFFECT_NODEUNKNOWN_NODE";
const std::string OUT_STR2 =
    "| RS_NODE[0], instanceRootNodeId[0], SharedTransitionParam: [0 -> 0], [nodeGroup1], uifirstRootNodeId_: 1, "
    "Properties: Bounds[-inf -inf -inf -inf] Frame[-inf -inf -inf -inf], NodeColorSpace: 4, "
    "RSUIContextToken: NO_RSUIContext, "
    "GetBootAnimation: true, isContainBootAnimation: true, isNodeDirty: 1, isPropertyDirty: true, "
    "isSubTreeDirty: true, IsPureContainer: true, Children list needs update, current count: 0 expected count: 0, "
    "disappearingChildren: 1(0 )\n  | RS_NODE[0], instanceRootNodeId[0], Properties: Bounds[-inf -inf -inf -inf] "
    "Frame[-inf -inf -inf -inf], NodeColorSpace: 4, RSUIContextToken: NO_RSUIContext, IsPureContainer: true\n";
const int DEFAULT_BOUNDS_SIZE = 10;
const int DEFAULT_NODE_ID = 1;
const NodeId TARGET_NODE_ID = 9999999999;
const NodeId INVALID_NODE_ID = 99999999999;
class RSRenderNodeDrawableAdapterBoy : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeDrawableAdapterBoy(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {
        renderParams_ = std::make_unique<RSRenderParams>(renderNode_.lock()->GetId());
        uifirstRenderParams_ = std::make_unique<RSRenderParams>(renderNode_.lock()->GetId());
    }
    ~RSRenderNodeDrawableAdapterBoy() override = default;

    void Draw(Drawing::Canvas& canvas) override
    {
        printf("Draw:GetRecordingState: %d \n", canvas.GetRecordingState());
    }
};

class RSRenderNodeUnitTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f,
        485.44f,
        -34.4f,
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::min(),
    };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

class DrawableTest : public RSDrawable {
public:
    bool OnUpdate(const RSRenderNode& content) override
    {
        return true;
    }
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override {}
    void OnSync() override
    {
        return;
    }
};

void RSRenderNodeUnitTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSRenderNodeUnitTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSRenderNodeUnitTest::SetUp() {}
void RSRenderNodeUnitTest::TearDown() {}

/**
 * @tc.name: GetNodeGroupTypeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GetNodeGroupTypeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retType = node->GetNodeGroupType();
    EXPECT_EQ(retType, RSRenderNode::NodeGroupType::NONE);
    node->nodeGroupType_ = RSRenderNode::NodeGroupType::GROUP_TYPE_BUTT;
    retType = node->GetNodeGroupType();
    EXPECT_EQ(retType, RSRenderNode::NodeGroupType::GROUP_TYPE_BUTT);
}

/**
 * @tc.name: InitRenderParamsTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, InitRenderParamsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->InitRenderParams();
    EXPECT_TRUE(node->renderDrawable_ == nullptr);
}

/**
 * @tc.name: UpdateRenderParamsTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateRenderParamsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);

    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->id_);
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetSandBox() == std::nullopt);

    node->GetMutableRenderProperties().sandbox_ = std::make_unique<Sandbox>();
    Vector2f vec2f(floatData[0], floatData[1]);
    node->GetMutableRenderProperties().sandbox_->position_ = vec2f;
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetSandBox() != std::nullopt);

    node->GetMutableRenderProperties().boundsGeo_ = nullptr;
    node->UpdateRenderParams();
    EXPECT_TRUE(node->GetRenderProperties().GetBoundsGeometry() == nullptr);
}

/**
 * @tc.name: UpdateCurCornerInfoTest
 * @tc.desc: Verify the boundary value handling when updating corner radius information,
 *           including max/min float values and empty radius scenarios.
 * @tc.type: FUNC
 * @tc.require: issueICVCOG
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateCurCornerInfoTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto maxFloatData = std::numeric_limits<float>::max();
    auto minFloatData = std::numeric_limits<float>::min();
    Vector4f curCornerRadius(floatData[0], floatData[1], floatData[2], minFloatData);
    Vector4f cornerRadius(floatData[0], floatData[1], floatData[2], maxFloatData);
    RectI curCornerRect;
    Vector4f emptyCornerRadius;
    node->UpdateCurCornerInfo(curCornerRadius, curCornerRect);
    EXPECT_TRUE(curCornerRadius[3] == minFloatData);
    node->GetMutableRenderProperties().SetCornerRadius(cornerRadius);
    node->UpdateCurCornerInfo(emptyCornerRadius, curCornerRect);
    EXPECT_TRUE(emptyCornerRadius[3] == maxFloatData);
    node->UpdateCurCornerInfo(curCornerRadius, curCornerRect);
    EXPECT_TRUE(curCornerRadius[3] == maxFloatData);
}

/**
 * @tc.name: UpdateSrcOrClipedAbsDrawRectChangeStateTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIAZV18
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateSrcOrClipedAbsDrawRectChangeStateTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto left = 0;
    auto top = 0;
    auto width = 1920;
    auto height = 1048;
    RectI rectI(left, top, width, height);

    node->srcOrClipedAbsDrawRectChangeFlag_ = true;
    node->UpdateSrcOrClipedAbsDrawRectChangeState(rectI);
    EXPECT_FALSE(node->geometryChangeNotPerceived_);

    EXPECT_EQ(RSSystemProperties::GetSkipGeometryNotChangeEnabled(), true);
    node->srcOrClipedAbsDrawRectChangeFlag_ = true;
    node->geometryChangeNotPerceived_ = true;
    node->UpdateSrcOrClipedAbsDrawRectChangeState(rectI);
    EXPECT_FALSE(node->srcOrClipedAbsDrawRectChangeFlag_);
}

/**
 * @tc.name: OnSyncTest1
 * @tc.desc: OnSync Test1
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeUnitTest, OnSyncTest1, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);
    node->renderDrawable_ = nullptr;
    node->OnSync();

    std::shared_ptr<RSRenderNode> child = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(child, nullptr);

    node->renderDrawable_ = std::make_shared<RSRenderNodeDrawableAdapterBoy>(child);
    EXPECT_NE(node->renderDrawable_, nullptr);

    node->drawCmdListNeedSync_ = true;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->stagingRenderParams_, nullptr);

    node->stagingRenderParams_->needSync_ = true;
    node->renderDrawable_->renderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->renderDrawable_->renderParams_, nullptr);

    node->uifirstNeedSync_ = true;
    node->renderDrawable_->uifirstRenderParams_ = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(node->renderDrawable_->uifirstRenderParams_, nullptr);

    node->uifirstSkipPartialSync_ = false;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    auto drawableFilter = std::make_shared<DrawableV2::RSForegroundFilterDrawable>();
    EXPECT_NE(drawableFilter, nullptr);

    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] = drawableFilter;
    node->drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    node->stagingRenderParams_->SetRSFreezeFlag(true);
    node->needClearSurface_ = true;
    std::function<void()> clearTask = []() { printf("ClearSurfaceTask CallBack\n"); };
    node->GetOpincCache().isOpincRootFlag_ = true;
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
    EXPECT_FALSE(node->drawCmdListNeedSync_);
    EXPECT_FALSE(node->uifirstNeedSync_);
    EXPECT_FALSE(node->needClearSurface_);
}

/**
 * @tc.name: OnSyncTest2
 * @tc.desc: OnSync Test2
 * @tc.type: FUNC
 * @tc.require: issueIA5Y41
 */
HWTEST_F(RSRenderNodeUnitTest, OnSyncTest2, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(node, nullptr);

    node->uifirstSkipPartialSync_ = true;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_FILTER);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());

    node->uifirstSkipPartialSync_ = false;
    auto backgroundColorDrawable = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable, nullptr);
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_COLOR);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());

    node->uifirstSkipPartialSync_ = true;
    auto backgroundColorDrawable2 = std::make_shared<DrawableV2::RSBackgroundColorDrawable>();
    EXPECT_NE(backgroundColorDrawable2, nullptr);
    node->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_COLOR)]
        = backgroundColorDrawable2;
    node->dirtySlots_.emplace(RSDrawableSlot::BACKGROUND_COLOR);
    node->OnSync();
    EXPECT_TRUE(node->dirtySlots_.empty());
}

/**
 * @tc.name: ValidateLightResourcesTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, ValidateLightResourcesTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto& properties = node->GetMutableRenderProperties();
    properties.GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    properties.GetEffect().lightSourcePtr_->intensity_ = floatData[1];
    EXPECT_TRUE(properties.GetEffect().lightSourcePtr_->IsLightSourceValid());
    properties.GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.GetEffect().illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER;
    EXPECT_TRUE(properties.GetEffect().illuminatedPtr_->IsIlluminatedValid());
    node->ValidateLightResources();
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdatePointLightDirtySlotTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_TRUE(node.dirtySlots_.empty());
    node.UpdateDirtySlotsAndPendingNodes(RSDrawableSlot::MASK);
    EXPECT_FALSE(node.dirtySlots_.empty());
}

/**
 * @tc.name: UpdatePointLightDirtySlotTest2
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdatePointLightDirtySlotTest2, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.UpdatePointLightDirtySlot();
    EXPECT_FALSE(node.enableHdrEffect_);
}
/**
 * @tc.name: AddToPendingSyncListTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, AddToPendingSyncListTest, TestSize.Level1)
{
    auto sContext = std::make_shared<RSContext>();
    context = sContext;
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->addedToPendingSyncList_ = true;
    node->AddToPendingSyncList();
    EXPECT_TRUE(node->addedToPendingSyncList_);
    node->addedToPendingSyncList_ = false;
    node->AddToPendingSyncList();
    EXPECT_TRUE(node->addedToPendingSyncList_);
    node->context_.reset();
    node->addedToPendingSyncList_ = false;
    node->AddToPendingSyncList();
    EXPECT_FALSE(node->addedToPendingSyncList_);
}

/**
 * @tc.name: SetChildrenHasSharedTransitionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, SetChildrenHasSharedTransitionTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetChildrenHasSharedTransition(true);
    EXPECT_TRUE(node.childrenHasSharedTransition_);
}

/**
 * @tc.name: HasBlurFilterTest
 * @tc.desc: HasBlurFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeUnitTest, HasBlurFilterTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    EXPECT_FALSE(node.HasBlurFilter());
    node.renderProperties_.effect_ = std::make_unique<RSProperties::CommonEffectParams>();

    node.renderProperties_.effect_->materialFilter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());

    node.renderProperties_.filter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());

    node.renderProperties_.backgroundFilter_ = std::make_shared<RSFilter>();
    EXPECT_TRUE(node.HasBlurFilter());
    EXPECT_TRUE(node.renderProperties_.GetBackgroundFilter());
}

/**
 * @tc.name: GetPairedNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GetPairedNodeTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    auto ptr = sharedTransitionParam->GetPairedNode(id + 1);
    EXPECT_TRUE(ptr == outNode);
    ptr = sharedTransitionParam->GetPairedNode(id + 2);
    EXPECT_TRUE(ptr == inNode);
    ptr = sharedTransitionParam->GetPairedNode(id);
    EXPECT_FALSE(ptr);
}

/**
 @tc.name: UpdateAbsDrawRect
 @tc.desc: update node absrect.
 @tc.type: FUNC
 @tc.require: issueIAL4RE
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateAbsDrawRect, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);
    RectI absRect = {10, 10, 10, 10};
    nodeTest->stagingRenderParams_->SetAbsDrawRect(absRect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateHierarchyTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateHierarchyTest, TestSize.Level1)
{
    auto inNodeId = id + 1;
    auto outNodeId = id + 2;
    auto otherNodeId = id + 3;
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(inNodeId, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(outNodeId, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE;
    auto ret = sharedTransitionParam->IsLower(inNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE;
    ret = sharedTransitionParam->IsLower(outNodeId);
    EXPECT_TRUE(ret);
    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    ret = sharedTransitionParam->IsLower(otherNodeId);
    EXPECT_FALSE(ret);

    sharedTransitionParam->UpdateHierarchy(outNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_ABOVE_OUT_NODE);

    sharedTransitionParam->relation_ = SharedTransitionParam::NodeHierarchyRelation::UNKNOWN;
    sharedTransitionParam->UpdateHierarchy(inNodeId);
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::IN_NODE_BELOW_OUT_NODE);
}

/**
 * @tc.name: HasRelation
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, HasRelationTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_FALSE(sharedTransitionParam->HasRelation());

    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_TRUE(sharedTransitionParam->HasRelation());
}

/**
 * @tc.name: SetNeedGenerateDrawable
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, SetNeedGenerateDrawableTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);

    sharedTransitionParam->SetNeedGenerateDrawable(true);
    EXPECT_TRUE(sharedTransitionParam->needGenerateDrawable_);

    sharedTransitionParam->SetNeedGenerateDrawable(false);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);
}

/**
 * @tc.name: GenerateDrawable
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GenerateDrawableTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    parent->stagingRenderParams_ = std::make_unique<RSRenderParams>(0);
    parent->childHasSharedTransition_ = true;
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    inNode->parent_ = parent;
    outNode->parent_ = parent;

    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    inNode->SetSharedTransitionParam(sharedTransitionParam);
    outNode->SetSharedTransitionParam(sharedTransitionParam);

    sharedTransitionParam->GenerateDrawable(*outNode);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);

    sharedTransitionParam->needGenerateDrawable_ = true;
    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_TRUE(sharedTransitionParam->IsLower(inNode->GetId()));
    sharedTransitionParam->GenerateDrawable(*inNode);
    EXPECT_TRUE(sharedTransitionParam->needGenerateDrawable_);

    EXPECT_FALSE(sharedTransitionParam->IsLower(outNode->GetId()));
    sharedTransitionParam->GenerateDrawable(*outNode);
    EXPECT_FALSE(sharedTransitionParam->needGenerateDrawable_);
}

/**
 * @tc.name: UpdateUnpairedSharedTransitionMap
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateUnpairedSharedTransitionMapTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_TRUE(SharedTransitionParam::unpairedShareTransitions_.empty());

    SharedTransitionParam::UpdateUnpairedSharedTransitionMap(sharedTransitionParam);
    EXPECT_FALSE(SharedTransitionParam::unpairedShareTransitions_.empty());

    SharedTransitionParam::UpdateUnpairedSharedTransitionMap(sharedTransitionParam);
    EXPECT_TRUE(SharedTransitionParam::unpairedShareTransitions_.empty());
}

/**
 * @tc.name: ResetRelation
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, ResetRelationTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);

    sharedTransitionParam->UpdateHierarchy(inNode->GetId());
    EXPECT_NE(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);

    sharedTransitionParam->ResetRelation();
    EXPECT_EQ(sharedTransitionParam->relation_, SharedTransitionParam::NodeHierarchyRelation::UNKNOWN);
}

/**
 * @tc.name: DumpTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, DumpTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    auto dumpStr = sharedTransitionParam->Dump();

    EXPECT_FALSE(dumpStr.empty());
}

/**
 * @tc.name: InternalUnregisterSelfTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, InternalUnregisterSelfTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    sharedTransitionParam->InternalUnregisterSelf();

    ASSERT_FALSE(inNode->sharedTransitionParam_);
    ASSERT_FALSE(outNode->sharedTransitionParam_);
}

/**
 * @tc.name: RSRenderNodeDirtyTest001
 * @tc.desc: SetDirty OnRegisterTest SetParentSubTreeDirty and InitRenderParams test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDirtyTest001, TestSize.Level1)
{
    std::shared_ptr<RSContext> contextTest1 = nullptr;
    RSRenderNode nodetest1(0, contextTest1);
    // SetDirty test
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    parent->isSubTreeDirty_ = false;
    nodetest1.parent_ = parent;
    nodetest1.dirtyStatus_ = RSRenderNode::NodeDirty::CLEAN;
    // dirtyStatus_ is NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_EQ(nodetest1.dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
    // dirtyStatus_ is not NodeDirty::CLEAN
    nodetest1.SetDirty(true);
    EXPECT_TRUE(parent->isSubTreeDirty_);

    // OnRegister and InitRenderParams test
    std::shared_ptr<RSContext> contextTest2 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest2, nullptr);
    std::shared_ptr<RSRenderNode> nodeTest2 = std::make_shared<RSRenderNode>(0, contextTest2);
    EXPECT_NE(nodeTest2, nullptr);
    nodeTest2->renderDrawable_ = nullptr;
    std::shared_ptr<RSContext> contextTest3 = std::make_shared<RSContext>();
    EXPECT_NE(contextTest3, nullptr);
    nodeTest2->OnRegister(contextTest3);
    EXPECT_EQ(nodeTest2->dirtyStatus_, RSRenderNode::NodeDirty::DIRTY);
}

/**
 * @tc.name: RSRenderNodeDirtyTest002
 * @tc.desc: SetTreeStateChangeDirty and IsTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDirtyTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodetest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodetest, nullptr);
    nodetest->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(nodetest->IsTreeStateChangeDirty());
    nodetest->SetTreeStateChangeDirty(false);
    EXPECT_FALSE(nodetest->IsTreeStateChangeDirty());
}

/**
 * @tc.name: RSRenderNodeDirtyTest003
 * @tc.desc: SetParentTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDirtyTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(child1, nullptr);
    child1->parent_ = parent;
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    EXPECT_NE(child2, nullptr);
    child2->parent_ = child1;

    child2->SetParentTreeStateChangeDirty(true);
    EXPECT_TRUE(child1->IsTreeStateChangeDirty());
    EXPECT_TRUE(parent->IsTreeStateChangeDirty());
}

/**
 * @tc.name: SetChildrenTreeStateChangeDirty
 * @tc.desc: SetChildrenTreeStateChangeDirty test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, SetChildrenTreeStateChangeDirtyTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::shared_ptr<RSRenderNode> child1 = std::make_shared<RSRenderNode>(1);
    EXPECT_NE(child1, nullptr);
    parent->AddChild(child1, -1);
    child1->parent_ = parent;
    std::shared_ptr<RSRenderNode> child2 = std::make_shared<RSRenderNode>(2);
    EXPECT_NE(child2, nullptr);
    child2->SetTreeStateChangeDirty(true);
    child2->parent_ = child1;
    parent->AddChild(child1, -1);

    parent->SetChildrenTreeStateChangeDirty();
    EXPECT_TRUE(child2->IsTreeStateChangeDirty());
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest001
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, IsSubTreeNeedPrepareTest001, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);

    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "0");
    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::DISABLED);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, true));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, false));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, true));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, false));

    // restore SubTreePrepareCheckType to default
    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "2");
    checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest002
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, IsSubTreeNeedPrepareTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);

    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "1");
    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::DISABLE_SUBTREE_DIRTY_CHECK);
    parent->shouldPaint_ = false;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, true));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, false));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, true));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, false));

    parent->shouldPaint_ = true;
    bool isOccluded = false;
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    isOccluded = true;
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(true);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->SetFirstLevelCrossNode(false);
    parent->SetTreeStateChangeDirty(false);
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    // restore SubTreePrepareCheckType to default
    system::SetParameter("persist.sys.graphic.SubTreePrepareCheckType.type", "2");
    checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest003
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, IsSubTreeNeedPrepareTest003, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    parent->stagingRenderParams_ = std::move(stagingRenderParams);

    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
    parent->shouldPaint_ = true;
    bool isOccluded = false;

    parent->SetSubTreeDirty(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    parent->SetSubTreeDirty(false);

    parent->SetChildHasVisibleFilter(false);
    parent->childHasSharedTransition_ = false;
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));

    parent->childHasSharedTransition_ = true;
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 0; // false
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = false;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
    parent->isAccumulatedClipFlagChanged_ = true;
    parent->subSurfaceCnt_ = 1; // true
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(true, isOccluded));
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(false, isOccluded));
}

/**
 * @tc.name: IsSubTreeNeedPrepareTest004
 * @tc.desc: IsSubTreeNeedPrepare test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, IsSubTreeNeedPrepareTest004, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(parent, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    parent->stagingRenderParams_ = std::move(stagingRenderParams);

    auto checkType = RSSystemProperties::GetSubTreePrepareCheckType();
    EXPECT_EQ(checkType, SubTreePrepareCheckType::ENABLED);
    parent->shouldPaint_ = true;
    bool isOccluded = false;
    bool filterInGlobal = false;
    parent->SetSubTreeDirty(false);
    parent->childHasSharedTransition_ = false;

    parent->SetRenderGroupExcludedStateChanged(true);
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    EXPECT_FALSE(parent->IsRenderGroupExcludedStateChanged());

    parent->SetChildHasVisibleFilter(false);
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    filterInGlobal = true;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));

    parent->SetChildHasVisibleFilter(true);
    filterInGlobal = false;
    EXPECT_FALSE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
    filterInGlobal = true;
    EXPECT_TRUE(parent->IsSubTreeNeedPrepare(filterInGlobal, isOccluded));
}

/**
 * @tc.name: RSRenderNodeDumpTest002
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDumpTest002, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    nodeTest->InitRenderParams();

    std::string outTest1 = "";
    nodeTest->DumpNodeType(RSRenderNodeType::SCREEN_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::RS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::SURFACE_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::ROOT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::PROXY_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::CANVAS_DRAWING_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::EFFECT_NODE, outTest1);
    nodeTest->DumpNodeType(RSRenderNodeType::UNKNOW, outTest1);
    EXPECT_EQ(outTest1, OUT_STR1);

    std::string outTest2 = "";
    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(inNode, nullptr);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(outNode, nullptr);
    nodeTest->sharedTransitionParam_ = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    EXPECT_NE(nodeTest->sharedTransitionParam_, nullptr);
    nodeTest->nodeGroupType_ = RSRenderNode::GROUPED_BY_ANIM;
    nodeTest->uifirstRootNodeId_ = 1;
    nodeTest->SetBootAnimation(true);
    nodeTest->SetContainBootAnimation(true);
    nodeTest->dirtyStatus_ = RSRenderNode::NodeDirty::DIRTY;
    nodeTest->renderProperties_.isDirty_ = true;
    nodeTest->isSubTreeDirty_ = true;
    nodeTest->renderProperties_.isDrawn_ = false;
    nodeTest->renderProperties_.alphaNeedApply_ = false;
    nodeTest->isFullChildrenListValid_ = false;
    auto childNode = std::make_shared<RSRenderNode>(0);
    childNode->InitRenderParams();
    nodeTest->disappearingChildren_.emplace_back(childNode, 0);
    nodeTest->DumpTree(0, outTest2);
    EXPECT_EQ(outTest2, OUT_STR2);

    std::string outTest3 = "";
    nodeTest->DumpSubClassNode(outTest3);
    EXPECT_EQ(outTest3, "");
}

/**
 * @tc.name: RSRenderNodeDumpTest003
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueICCYNK
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDumpTest003, TestSize.Level1)
{
    std::string outTest = "";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    auto consumerSurfacePtr = IConsumerSurface::Create();
    auto buffer = SurfaceBuffer::Create();
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler() != nullptr);
    surfaceNode->GetRSSurfaceHandler()->buffer_.buffer = buffer;
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    ASSERT_FALSE(surfaceNode->GetRSSurfaceHandler()->GetConsumer() != nullptr);
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("ScalingMode") != string::npos);
    ASSERT_FALSE(outTest.find("TransformType") != string::npos);

    outTest = "";
    surfaceNode->GetRSSurfaceHandler()->SetConsumer(consumerSurfacePtr);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetBuffer() != nullptr);
    ASSERT_TRUE(surfaceNode->GetRSSurfaceHandler()->GetConsumer() != nullptr);
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("ScalingMode") != string::npos);
    ASSERT_TRUE(outTest.find("TransformType") != string::npos);

    outTest = "";
    surfaceNode->isRepaintBoundary_ = true;
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("RB: true") != string::npos);
}

/**
 * @tc.name: RSRenderNodeDumpTest004
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test, node is subSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeUnitTest, RSRenderNodeDumpTest004, TestSize.Level1)
{
    std::string outTest = "";
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(0);
    surfaceNode->InitRenderParams();
    surfaceNode->isSubSurfaceNode_ = true;
    surfaceNode->DumpTree(0, outTest);
    ASSERT_TRUE(outTest.find("isSubSurfaceId") != string::npos);
}

/**
 * @tc.name: RSSurfaceRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeUnitTest, RSSurfaceRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", Parent [null], Name [SurfaceNode], hasConsumer: 0, Alpha: 1.000000, Visible: 1, "
        "VisibleRegion [Empty], OpaqueRegion [Empty], OcclusionBg: 0, IsContainerTransparent: 0, SpecialLayer: 0, "
        "surfaceType: 0, ContainerConfig: [outR: 0 inR: 0 x: 0 y: 0 w: 0 h: 0], colorSpace: 4, uifirstColorGamut: 4"
        ", isSurfaceBufferOpaque: 0");
}

/**
 * @tc.name: RSScreenRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeUnitTest, RSScreenRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto context = std::make_shared<RSContext>();
    auto renderNode = std::make_shared<RSScreenRenderNode>(0, 0, context);
    renderNode->InitRenderParams();
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", colorSpace: 4");

    renderNode->DumpTree(0, outTest);
}

/**
 * @tc.name: RSRootRenderNodeDumpTest
 * @tc.desc: DumpNodeType DumpTree and DumpSubClassNode test
 * @tc.type: FUNC
 * @tc.require: issueIAJ6BA
 */
HWTEST_F(RSRenderNodeUnitTest, RSRootRenderNodeDumpTest, TestSize.Level1)
{
    std::string outTest = "";
    auto renderNode = std::make_shared<RSRootRenderNode>(0);
    renderNode->DumpSubClassNode(outTest);
    EXPECT_EQ(outTest, ", Visible: 1, Size: [-inf, -inf], EnableRender: 1");
}

/**
 * @tc.name: UpdateRenderStatus001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateRenderStatus001, TestSize.Level1)
{
    RSSurfaceRenderNode node(id, context);
    ASSERT_TRUE(node.IsNodeDirty());
    RectI dirtyRegion;
    bool isPartialRenderEnabled = false;
    node.UpdateRenderStatus(dirtyRegion, isPartialRenderEnabled);
}

/**
 * @tc.name: UpdateParentChildrenRectTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateParentChildrenRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSBaseRenderNode> parentNode;
    node.UpdateParentChildrenRect(parentNode);
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_FALSE(rsBaseRenderNode.HasChildrenOutOfRect());
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, ProcessTransitionBeforeChildrenTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionBeforeChildren(*canvas_);
    ASSERT_TRUE(canvas_->GetRecordingCanvas() == nullptr);
}

/**
 * @tc.name: AddModifierTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, AddModifierTest001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    ASSERT_FALSE(node.IsDirty());
}

/**
 * @tc.name: AddModifierTest002
 * @tc.desc: test AddModifierTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, AddModifierTest002, TestSize.Level1)
{
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr;
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    ModifierId id = 1;
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property, id, ModifierNG::RSPropertyType::BOUNDS);
    auto modifier1 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::FRAME, property, ++id, ModifierNG::RSPropertyType::FRAME);
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    node.AddModifier(modifier1);
    ASSERT_TRUE(node.IsDirty());
}

/**
 * @tc.name: GetPropertyTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeUnitTest, GetPropertyTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto rsRenderPropertyBase = node.GetProperty(id);
    EXPECT_EQ(rsRenderPropertyBase, nullptr);
}

/**
 * @tc.name: SetDrawingCacheTypeTest
 * @tc.desc: test SetDrawingCacheType for all drawing cache types
 * @tc.type: FUNC
 * @tc.require: issueI84LBZ
 */
HWTEST_F(RSRenderNodeUnitTest, SetDrawingCacheTypeTest, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetDrawingCacheType(RSDrawingCacheType::DISABLED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::FORCED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::FORCED_CACHE);
    node.SetDrawingCacheType(RSDrawingCacheType::TARGETED_CACHE);
    ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
}

/**
 * @tc.name: SetDrawNodeType
 * @tc.desc: test results of SetDrawNodeType
 * @tc.type: FUNC
 * @tc.require: IC8BLE
 */
HWTEST_F(RSRenderNodeUnitTest, SetDrawNodeType001, TestSize.Level1)
{
    RSRenderNode rsNode(id, context);
    rsNode.SetDrawNodeType(DrawNodeType::PureContainerType);
    ASSERT_EQ(rsNode.drawNodeType_, DrawNodeType::PureContainerType);
}

/**
 * @tc.name: GetDrawNodeType
 * @tc.desc: test results of GetDrawNodeType
 * @tc.type: FUNC
 * @tc.require: IC8BLE
 */
HWTEST_F(RSRenderNodeUnitTest, SetDrawNodeType002, TestSize.Level1)
{
    RSRenderNode rsNode(id, context);
    rsNode.SetDrawNodeType(DrawNodeType::GeometryPropertyType);
    ASSERT_EQ(rsNode.GetDrawNodeType(), DrawNodeType::GeometryPropertyType);
}

/**
 * @tc.name: ManageDrawingCacheTest001
 * @tc.desc: SetDrawingCacheChanged test
 * @tc.type: FUNC
 * @tc.require: issueI9US6V
 */
HWTEST_F(RSRenderNodeUnitTest, ManageDrawingCacheTest001, TestSize.Level2)
{
    // SetDrawingCacheChanged test
    std::shared_ptr<RSRenderNode> nodeTest = std::make_shared<RSRenderNode>(0);
    EXPECT_NE(nodeTest, nullptr);
    std::unique_ptr<RSRenderParams> stagingRenderParams = std::make_unique<RSRenderParams>(0);
    EXPECT_NE(stagingRenderParams, nullptr);
    nodeTest->stagingRenderParams_ = std::move(stagingRenderParams);

    nodeTest->lastFrameSynced_ = true;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->stagingRenderParams_->isDrawingCacheChanged_ = true;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);

    nodeTest->SetDrawingCacheChanged(false);
    EXPECT_FALSE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    nodeTest->lastFrameSynced_ = false;
    nodeTest->stagingRenderParams_->needSync_ = false;
    nodeTest->SetDrawingCacheChanged(true);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->needSync_);
    EXPECT_TRUE(nodeTest->stagingRenderParams_->isDrawingCacheChanged_);

    // GetDrawingCacheChanged test
    EXPECT_TRUE(nodeTest->GetDrawingCacheChanged());
}

/**
 * @tc.name: SetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay once
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeUnitTest,  SetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    // test default value
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);

    node.SetGeoUpdateDelay(true);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: SetGeoUpdateDelay02
 * @tc.desc: test SetGeoUpdateDelay would not be covered by later setting
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeUnitTest,  SetGeoUpdateDelay02, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.SetGeoUpdateDelay(false);
    ASSERT_EQ(node.GetGeoUpdateDelay(), true);
}

/**
 * @tc.name: ResetGeoUpdateDelay01
 * @tc.desc: test SetGeoUpdateDelay would be reset
 * @tc.type: FUNC
 * @tc.require: issueI8JMN8
 */
HWTEST_F(RSRenderNodeUnitTest,  ResetGeoUpdateDelay01, TestSize.Level2)
{
    RSRenderNode node(id, context);
    node.SetGeoUpdateDelay(true);
    node.ResetGeoUpdateDelay();
    ASSERT_EQ(node.GetGeoUpdateDelay(), false);
}

/**
 * @tc.name: SetBootAnimationTest
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type:FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSRenderNodeUnitTest, SetBootAnimationTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetBootAnimation(true);
    ASSERT_EQ(node.GetBootAnimation(), true);
    node.SetBootAnimation(false);
    ASSERT_FALSE(node.GetBootAnimation());

    RSDisplayNodeConfig config;
    NodeId nodeId = 2;
    auto logicalDisplayNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    auto rsRenderNode = std::static_pointer_cast<RSRenderNode>(logicalDisplayNode);
    auto tempRenderNode = std::make_shared<RSRenderNode>(1);
    rsRenderNode->parent_ = tempRenderNode;
    rsRenderNode->SetBootAnimation(false);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest01
 * @tc.desc: Check node only contains BasicGeoTransfrom by default
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest02
 * @tc.desc: Check node contains more than BasicGeoTransfrom if content dirty
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsContentDirty(), true);
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
}

/**
 * @tc.name: OnlyBasicGeoTransfromTest03
 * @tc.desc: Check node resets BasicGeoTransfrom as true
 * @tc.type: FUNC
 * @tc.require: issueI8KEUU
 */
HWTEST_F(RSRenderNodeUnitTest, OnlyBasicGeoTransfromTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.SetContentDirty();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), false);
    node.ResetIsOnlyBasicGeoTransform();
    ASSERT_EQ(node.IsOnlyBasicGeoTransform(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest01
 * @tc.desc: check dirty region not join when not dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest01, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest02
 * @tc.desc: check dirty region not join when not geometry dirty
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest02, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.geometryChangeNotPerceived_ = false;
    bool geoDirty = false;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), true);
}

/**
 * @tc.name: UpdateDirtyRegionTest03
 * @tc.desc: check dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest03, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    bool isDirtyRectCorrect = (rsDirtyManager->GetCurrentFrameDirtyRegion() == absRect);
    ASSERT_EQ(isDirtyRectCorrect, true);
}

/**
 * @tc.name: UpdateDirtyRegionTest04
 * @tc.desc: check shadow dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest04, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetShadowOffsetX(10.0f);
    properties.SetShadowOffsetY(10.0f);
    properties.SetShadowRadius(10.0f);
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest05
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest05, TestSize.Level1)
{
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    RectI clipRect{0, 0, 1000, 1000};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    properties.SetOutlineWidth(10.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest06
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest06, TestSize.Level1)
{
    //test RSRenderNode
    RSRenderNode node(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(500, 500);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 500*500
    RectI clipRect{0, 0, 500, 500};
    node.SetDirty();
    node.shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 50, 50};
    auto& properties = node.GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 5
    properties.SetOutlineWidth(5.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    node.UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest07
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest07, TestSize.Level1)
{
    //test RSCanvasRenderNode
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(DEFAULT_NODE_ID, context);
    canvasNode->InitRenderParams();
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(800, 800);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 800*800
    RectI clipRect{0, 0, 800, 800};
    canvasNode->SetDirty();
    canvasNode->shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = canvasNode->GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 8
    properties.SetOutlineWidth(8.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    canvasNode->UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: UpdateDirtyRegionTest08
 * @tc.desc: check outline dirty region add successfully
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateDirtyRegionTest08, TestSize.Level1)
{
    //test RSSurfaceRenderNode
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    //set dirty surfacesize
    rsDirtyManager->SetSurfaceSize(1000, 1000);
    rsDirtyManager->MarkAsTargetForDfx();
    //create clip 1000*1000
    RectI clipRect{0, 0, 1000, 1000};
    surfaceNode->SetDirty();
    surfaceNode->shouldPaint_ = true;
    RectI absRect = RectI{0, 0, 100, 100};
    auto& properties = surfaceNode->GetMutableRenderProperties();
    properties.boundsGeo_->absRect_ = absRect;
    properties.clipToBounds_ = true;
    //set border width 10
    properties.SetOutlineWidth(10.0f);
    properties.SetOutlineStyle((uint32_t)BorderStyle::SOLID);
    properties.SetOutlineColor(RSColor(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX));
    bool geoDirty = true;
    surfaceNode->UpdateDirtyRegion(*rsDirtyManager, geoDirty, clipRect);
    ASSERT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion().IsEmpty(), false);
}

/**
 * @tc.name: GetFilterRectTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GetFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    auto rect = node.GetFilterRect();
    EXPECT_TRUE(rect.ToString().compare("[0, 0, 0, 0]") == 0);

    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    node.renderProperties_.SetClipBounds(rsPath);
    auto rect1 = node.GetFilterRect();
    EXPECT_TRUE(rect1.ToString().compare("[0, 0, 0, 0]") == 0);

    node.renderProperties_.boundsGeo_ = nullptr;
    auto rect2 = node.GetFilterRect();
    EXPECT_TRUE(rect2.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: CalVisibleFilterRect
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, CalVisibleFilterRectTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RectI prepareClipRect { 1, 1, 1, 1 };
    node.CalVisibleFilterRect(prepareClipRect, prepareClipRect);
    EXPECT_TRUE(node.GetFilterRegionInfo().filterRegion_.ToString().compare("[0, 0, 0, 0]") == 0);
}

/**
 * @tc.name: OnTreeStateChangedTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, OnTreeStateChangedTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id, context); // isOnTheTree_ false
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    node->renderProperties_.filter_ = filter;
    node->OnTreeStateChanged();
    EXPECT_FALSE(node->isOnTheTree_);
    EXPECT_TRUE(node->HasBlurFilter());

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id, context);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 1, context);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->SetSharedTransitionParam(sharedTransitionParam);
    node->OnTreeStateChanged();
    EXPECT_FALSE(node->sharedTransitionParam_->paired_);

    node->isOnTheTree_ = true;
    node->OnTreeStateChanged();
    EXPECT_TRUE(node->IsDirty());

    auto canvasDrawingNode = std::make_shared<RSCanvasDrawingRenderNode>(1);
    canvasDrawingNode->isNeverOnTree_ = false;
    canvasDrawingNode->OnTreeStateChanged();
    EXPECT_FALSE(canvasDrawingNode->isNeverOnTree_);
    canvasDrawingNode->isOnTheTree_ = true;
    canvasDrawingNode->OnTreeStateChanged();
    EXPECT_FALSE(canvasDrawingNode->isNeverOnTree_);
}

/**
 * @tc.name: HasDisappearingTransitionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, HasDisappearingTransitionTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->disappearingTransitionCount_ = 1;
    node->isOnTheTree_ = true;
    EXPECT_TRUE(node->HasDisappearingTransition(true));
    node->disappearingTransitionCount_ = 0;
    EXPECT_FALSE(node->HasDisappearingTransition(false));
    auto node1 = std::make_shared<RSRenderNode>(id + 1, context);
    auto parent = node1->weak_from_this();
    node->SetParent(parent);
    EXPECT_FALSE(node->HasDisappearingTransition(true));
}

/**
 * @tc.name: GenerateFullChildrenListTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GenerateFullChildrenListTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->isChildrenSorted_ && node->children_.empty() && node->disappearingChildren_.empty());

    // children_ isn't empty
    std::weak_ptr<RSRenderNode> wNode1;
    node->children_.emplace_back(wNode1); // wNode1 is nullptr
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->children_.empty());
    std::shared_ptr<RSRenderNode> sNode1 = std::make_shared<RSRenderNode>(id, context);
    sNode1->isBootAnimation_ = false;
    wNode1 = sNode1;
    node->children_.emplace_back(wNode1); // wNode1 isn't nullptr

    node->isContainBootAnimation_ = true;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(!node->children_.empty());

    node->isContainBootAnimation_ = false;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(!node->children_.empty());

    // disappearingChildren_ isn't empty
    node->disappearingChildren_.emplace_back(sNode1, id);
    node->GenerateFullChildrenList();
    EXPECT_FALSE(node->isContainBootAnimation_);

    node->isContainBootAnimation_ = true;
    node->GenerateFullChildrenList();
    EXPECT_TRUE(node->isContainBootAnimation_);
}

/**
 * @tc.name: ResortChildrenTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, ResortChildrenTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto node1 = std::make_shared<RSRenderNode>(id + 1, context);
    auto node2 = std::make_shared<RSRenderNode>(id + 2, context);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(std::move(node1));
    fullChildrenList->emplace_back(std::move(node2));
    node->fullChildrenList_ = fullChildrenList;
    node->isChildrenSorted_ = false;
    EXPECT_TRUE(!node->fullChildrenList_->empty());
    node->ResortChildren();
    EXPECT_TRUE(node->isChildrenSorted_);
}

/**
 * @tc.name: HasChildrenOutOfRectTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, HasChildrenOutOfRectTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->hasChildrenOutOfRect_ = true;
    EXPECT_TRUE(node->HasChildrenOutOfRect());
    node->renderProperties_.clipToBounds_ = true;
    EXPECT_TRUE(node->GetRenderProperties().GetClipToBounds());
    EXPECT_FALSE(node->HasChildrenOutOfRect());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleFilterChildTest_001, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetMutableRenderProperties().needFilter_ = true;
    EXPECT_TRUE(childNode->GetRenderProperties().NeedFilter());
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleFilterChildTest_002, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetHwcRecorder().SetBlendWithBackground(true);
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleFilterChildTest_003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleFilterChildTest_003, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetHwcRecorder().SetForegroundColorValid(true);
    node->UpdateVisibleFilterChild(*childNode);
    EXPECT_TRUE(!node->visibleFilterChild_.empty());
}

/**
 * @tc.name: UpdateVisibleEffectChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateVisibleEffectChildTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    childNode->GetMutableRenderProperties().GetEffect().useEffect_ = true;
    childNode->SetOldDirtyInSurface(RectI(0, 0, 10, 10));
    EXPECT_TRUE(childNode->GetRenderProperties().GetUseEffect());
    node->UpdateVisibleEffectChild(*childNode);
    EXPECT_TRUE(!node->visibleEffectChild_.empty());
}

/**
 * @tc.name: GetInstanceRootNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GetInstanceRootNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retNode = node->GetInstanceRootNode();
    EXPECT_TRUE(retNode == nullptr);
    auto sContext = std::make_shared<RSContext>();
    node->context_ = sContext;
    node->GetInstanceRootNode();
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
}

/**
 * @tc.name: AddUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeUnitTest, AddUIExtensionChildTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childNode = std::make_shared<RSRenderNode>(id + 1, context);
    node->AddUIExtensionChild(childNode);
    EXPECT_FALSE(childNode->GetParent().lock());

    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    node->AddUIExtensionChild(uiExtension);
    EXPECT_FALSE(uiExtension->GetParent().lock());

    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 3, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: MoveUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeUnitTest, MoveUIExtensionChildTest, TestSize.Level1)
{
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
    parent->MoveUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: RemoveUIExtensionChildTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeUnitTest, RemoveUIExtensionChildTest, TestSize.Level1)
{
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    uiExtension->SetSurfaceNodeType(RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE);
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 2, context);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parent->AddUIExtensionChild(uiExtension);
    EXPECT_TRUE(uiExtension->GetParent().lock());
    parent->RemoveUIExtensionChild(uiExtension);
    EXPECT_FALSE(uiExtension->GetParent().lock());
}

/**
 * @tc.name: NeedRoutedBasedOnUIExtensionTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueIBK74G
 */
HWTEST_F(RSRenderNodeUnitTest, NeedRoutedBasedOnUIExtensionTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto uiExtension = std::make_shared<RSSurfaceRenderNode>(id + 1, context);
    uiExtension->SetUIExtensionUnobscured(true);
    EXPECT_FALSE(node->NeedRoutedBasedOnUIExtension(uiExtension));
}

/**
 * @tc.name: UpdateTreeUifirstRootNodeIdTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateTreeUifirstRootNodeIdTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto childId = id + 1;
    auto childNode = std::make_shared<RSRenderNode>(childId, context);
    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->emplace_back(std::move(childNode));
    node->fullChildrenList_ = fullChildrenList;
    node->UpdateTreeUifirstRootNodeId(childId);
    EXPECT_TRUE(node->uifirstRootNodeId_ == childId);
    for (auto& child : *node->GetChildren()) {
        EXPECT_TRUE(child != nullptr);
    }
}

/**
 * @tc.name: GetFirstLevelNodeTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, GetFirstLevelNodeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    auto retNode = node->GetFirstLevelNode();
    EXPECT_TRUE(retNode == nullptr);
    auto sContext = std::make_shared<RSContext>();
    node->context_ = sContext;
    node->GetFirstLevelNode();
    EXPECT_TRUE(node->GetContext().lock() != nullptr);
}

/**
 * @tc.name: SetGeoUpdateDelayTest
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI9T3XY
 */
HWTEST_F(RSRenderNodeUnitTest, SetGeoUpdateDelayTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    EXPECT_FALSE(node->geoUpdateDelay_);
    node->SetGeoUpdateDelay(true);
    EXPECT_TRUE(node->geoUpdateDelay_);
}

/**
 * @tc.name: UpdateSubSurfaceCntTest
 * @tc.desc: test addChild and removeChild
 * @tc.type: FUNC
 * @tc.require: issueBAYJG
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateSubSurfaceCntTest001, TestSize.Level1)
{
    const int cnt = 0;
    auto rootNode = std::make_shared<RSRenderNode>(DEFAULT_NODE_ID);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 1);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    auto selfDrawNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 2);
    selfDrawNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    leashNode->AddChild(selfDrawNode);
    leashNode->UpdateSubSurfaceCnt(selfDrawNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    rootNode->RemoveChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(-leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt);
}

/**
 * @tc.name: UpdateSubSurfaceCntTest
 * @tc.desc: test addChild and loop
 * @tc.type: FUNC
 * @tc.require: issueBAYJG
 */
HWTEST_F(RSRenderNodeUnitTest, UpdateSubSurfaceCntTest002, TestSize.Level1)
{
    const int cnt = 0;
    auto rootNode = std::make_shared<RSRenderNode>(DEFAULT_NODE_ID);

    auto leashNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 1);
    leashNode->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    rootNode->AddChild(leashNode);
    rootNode->UpdateSubSurfaceCnt(leashNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 1);

    auto appNode = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 2);
    appNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    leashNode->AddChild(appNode);
    leashNode->UpdateSubSurfaceCnt(appNode->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 2);

    auto antherApp = std::make_shared<RSSurfaceRenderNode>(DEFAULT_NODE_ID + 3);
    antherApp->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    appNode->AddChild(antherApp);
    appNode->UpdateSubSurfaceCnt(antherApp->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 3);

    // loop tree
    antherApp->AddChild(leashNode);
    antherApp->UpdateSubSurfaceCnt(antherApp->subSurfaceCnt_);
    EXPECT_EQ(rootNode->subSurfaceCnt_, cnt + 3);
}
} // namespace Rosen
} // namespace OHOS
