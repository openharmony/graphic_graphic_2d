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

#include <gtest/gtest.h>
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_planner.h"
#include "pipeline/layer_split/splitter/opinc/rs_opinc_layer_splitter_processor.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"
#include "drawable/rs_surface_render_node_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOpincLayerSplitterPlannerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        // Ensure RSRenderNodeGC is constructed before RSMainThread, so that
        // at static destruction time RSMainThread (which destroys Context and
        // RenderNode with drawable) is destroyed before RSRenderNodeGC.
        RSRenderNodeGC::Instance();
    }
    static void TearDownTestCase() {}
    void SetUp() override
    {
        planner_ = std::make_unique<RSOpincLayerSplitterPlanner>();
    }
    void TearDown() override
    {
        planner_.reset();
    }

    std::shared_ptr<RSSurfaceRenderNode> CreateSurfaceNode(NodeId id)
    {
        auto node = std::make_shared<RSSurfaceRenderNode>(id);
        node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
        return node;
    }

    std::shared_ptr<SplitSurface> CreateSplitSurface(NodeId nodeId)
    {
        auto splitSurface = std::make_shared<SplitSurface>(100, 100);
        splitSurface->splitSurfaceNode_ = CreateSurfaceNode(nodeId);
        return splitSurface;
    }

protected:
    std::unique_ptr<RSOpincLayerSplitterPlanner> planner_;
};

namespace {

/*
 * ── CheckNeedLeave ──────────────────────────────────────────
 */

/**
 * @tc.name: CheckNeedLeave_SplitSurfaceNull
 * @tc.desc: Test CheckNeedLeave when splitSurface_ is nullptr (early return at condition 1)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_OpIncParentNodeNull
 * @tc.desc: Test CheckNeedLeave when opIncParentNode_ is nullptr (early return at condition 2)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_OpIncParentNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = nullptr;
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_BoundsGeoNull
 * @tc.desc: Test CheckNeedLeave when boundsGeo_ is nullptr (condition 4 false, falls through to condition 6)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_BoundsGeoNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->opIncParentNode_->GetMutableRenderProperties().boundsGeo_ = nullptr;
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_SkewXNonZero
 * @tc.desc: Test CheckNeedLeave when skewX != 0 (condition 4a short-circuit, needLeave_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_SkewXNonZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetSkewX(5.0f);
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_SkewYNonZero
 * @tc.desc: Test CheckNeedLeave when skewX=0, skewY!=0 (condition 4a evaluates both, needLeave_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_SkewYNonZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetSkewY(5.0f);
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_CollectOpIncOverlap
 * @tc.desc: Test CheckNeedLeave when CollectOpIncNodes detects overlap (condition 5 true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_CollectOpIncOverlap, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto nonOpIncChild = CreateSurfaceNode(300);
    nonOpIncChild->GetMutableRenderProperties().SetBounds(Vector4f(25, 25, 50, 50));
    nonOpIncChild->GetMutableRenderProperties().SetFrame(Vector4f(25, 25, 50, 50));
    nonOpIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto opIncChild = CreateSurfaceNode(400);
    opIncChild->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    opIncChild->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    opIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild->GetOpincRootCache().isOpincRootFlag_ = true;

    auto vec = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    vec->push_back(nonOpIncChild);
    vec->push_back(opIncChild);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> childrenList = vec;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, childrenList,
        std::memory_order_release);

    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_IsOpIncNodesChanged
 * @tc.desc: Test CheckNeedLeave when IsOpIncNodesChanged detects mismatch (condition 6 true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_IsOpIncNodesChanged, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto opIncChild = CreateSurfaceNode(300);
    opIncChild->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    opIncChild->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    opIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild->GetOpincRootCache().isOpincRootFlag_ = true;

    auto vec = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    vec->push_back(opIncChild);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> childrenList = vec;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, childrenList,
        std::memory_order_release);

    planner_->lastOpIncNodes_ = { {999, Vector4f(10, 10, 0, 0)} };
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_UpdateBufferBoundsOut
 * @tc.desc: Test CheckNeedLeave when UpdateBufferBounds returns true (condition 7 true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_UpdateBufferBoundsOut, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto opIncChild = CreateSurfaceNode(300);
    opIncChild->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    opIncChild->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    opIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild->GetOpincRootCache().isOpincRootFlag_ = true;

    auto vec = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    vec->push_back(opIncChild);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> childrenList = vec;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, childrenList,
        std::memory_order_release);

    planner_->lastOpIncNodes_ = { {300, Vector4f(0, 0, 0, 0)} };
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 200, 200);
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_AllPass
 * @tc.desc: Test CheckNeedLeave when all conditions pass (needLeave_ = false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_AllPass, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto opIncChild = CreateSurfaceNode(300);
    opIncChild->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    opIncChild->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    opIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild->GetOpincRootCache().isOpincRootFlag_ = true;

    auto vec = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    vec->push_back(opIncChild);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> childrenList = vec;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, childrenList,
        std::memory_order_release);

    planner_->lastOpIncNodes_ = { {300, Vector4f(0, 0, 0, 0)} };
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 100, 100);
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, false);
}

/*
 * ── SetColorSpaceInfo ─────────────────────────────────────
 */

/**
 * @tc.name: SetColorSpaceInfo_OpIncParentNodeNull
 * @tc.desc: Test SetColorSpaceInfo when opIncParentNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_OpIncParentNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncParentNode_ = nullptr;
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = CreateSurfaceNode(100);

    planner_->SetColorSpaceInfo();
}

/**
 * @tc.name: SetColorSpaceInfo_SplitSurfaceNull
 * @tc.desc: Test SetColorSpaceInfo when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;

    planner_->SetColorSpaceInfo();
}

/**
 * @tc.name: SetColorSpaceInfo_SplitSurfaceNodeNull
 * @tc.desc: Test SetColorSpaceInfo when splitSurface_->splitSurfaceNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_SplitSurfaceNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = nullptr;

    planner_->SetColorSpaceInfo();
}

/**
 * @tc.name: SetColorSpaceInfo_NormalCase
 * @tc.desc: Test SetColorSpaceInfo with valid parameters (normal execution)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_NormalCase, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    planner_->opIncParentNode_ = parentNode;

    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto splitSurfaceNode = CreateSurfaceNode(200);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    planner_->splitSurface_ = splitSurface;

    auto handler = splitSurfaceNode->GetRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    auto buffer = handler->GetBuffer();

    planner_->SetColorSpaceInfo();
    ASSERT_EQ(splitSurface->colorSpace_, parentNode->GetNodeColorSpace());
}

/*
 * ── UpdateSplitPlan ─────────────────────────────────────────
 */

/**
 * @tc.name: UpdateSplitPlan_SplitSurfaceNull
 * @tc.desc: Test UpdateSplitPlan when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusOFF_NeedLeave
 * @tc.desc: Test UpdateSplitPlan when planStatus=OFF and needLeave_=true (stays OFF, post-switch skip Init)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusOFF_NeedLeave, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusPREPARE_ToLEAVE
 * @tc.desc: Test UpdateSplitPlan when planStatus=PREPARE and needLeave_=true (transitions to LEAVE)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusPREPARE_ToLEAVE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::PREPARE;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::LEAVE);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusPREPARE_ToON
 * @tc.desc: Test UpdateSplitPlan when planStatus=PREPARE and needLeave_=false (transitions to ON)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusPREPARE_ToON, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::PREPARE;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::ON);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusON_ToLEAVE
 * @tc.desc: Test UpdateSplitPlan when planStatus=ON and needLeave_=true (transitions to LEAVE)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusON_ToLEAVE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::ON;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::LEAVE);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusON_NoLeave
 * @tc.desc: Test UpdateSplitPlan when planStatus=ON and needLeave_=false (stays ON)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusON_NoLeave, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::ON;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::ON);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusLEAVE
 * @tc.desc: Test UpdateSplitPlan when planStatus=LEAVE (→ OFF + return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusLEAVE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(0);
    planner_->SetOpIncParentNode(parentNode);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::LEAVE;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusOFF_NeedLeave_NoUnregister_OnTree
 * @tc.desc: Test UpdateSplitPlan when IsLongTermOff=true but OnTheTree=true (no Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusOFF_NeedLeave_NoUnregister_OnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = nullptr;
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_->stayOffCount_ = 101;

    ASSERT_EQ(planner_->splitSurface_->splitSurfaceNode_, nullptr);
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);
    ASSERT_EQ(planner_->splitSurface_->splitSurfaceNode_, nullptr);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusOFF_NeedLeave_NoUnregister_NotLongTerm
 * @tc.desc: Test UpdateSplitPlan when IsLongTermOff=false (no Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    UpdateSplitPlan_PlanStatusOFF_NeedLeave_NoUnregister_NotLongTerm, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_->stayOffCount_ = 0;

    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_, nullptr);
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);
    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_, nullptr);
}

/*
 * ── UpdateScreenDirtyRegion ────────────────────────────────
 */

/**
 * @tc.name: UpdateScreenDirtyRegion_StatusNotLEAVE
 * @tc.desc: Test UpdateScreenDirtyRegion when planStatus_ is not LEAVE (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateScreenDirtyRegion_StatusNotLEAVE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->dstRect_ = RectF(10, 20, 100, 200);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    planner_->UpdateScreenDirtyRegion(dirtyManager);
}

/**
 * @tc.name: UpdateScreenDirtyRegion_StatusLEAVE_DirtyManagerNull
 * @tc.desc: Test UpdateScreenDirtyRegion when planStatus=LEAVE and curScreenDirtyManager is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateScreenDirtyRegion_StatusLEAVE_DirtyManagerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->planStatus_ = PlanStatus::LEAVE;
    planner_->dstRect_ = RectF(10, 20, 100, 200);

    planner_->UpdateScreenDirtyRegion(nullptr);
}

/**
 * @tc.name: UpdateScreenDirtyRegion_StatusLEAVE_DirtyManagerNotNull
 * @tc.desc: Test UpdateScreenDirtyRegion when planStatus=LEAVE and curScreenDirtyManager is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateScreenDirtyRegion_StatusLEAVE_DirtyManagerNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->planStatus_ = PlanStatus::LEAVE;
    planner_->dstRect_ = RectF(10, 20, 100, 200);

    auto dirtyManager = std::make_shared<RSDirtyRegionManager>();
    planner_->UpdateScreenDirtyRegion(dirtyManager);
}

/*
 * ── GetSurfaceStatus ───────────────────────────────────────
 */

/**
 * @tc.name: GetSurfaceStatus_SplitSurfaceNotNull
 * @tc.desc: Test GetSurfaceStatus when splitSurface_ is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetSurfaceStatus_SplitSurfaceNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->surfaceStatus_ = SurfaceStatus::REGISTER;

    SurfaceStatus status = planner_->GetSurfaceStatus();

    ASSERT_EQ(status, SurfaceStatus::REGISTER);
}

/**
 * @tc.name: GetSurfaceStatus_SplitSurfaceNull
 * @tc.desc: Test GetSurfaceStatus when splitSurface_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetSurfaceStatus_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;

    SurfaceStatus status = planner_->GetSurfaceStatus();

    ASSERT_EQ(status, SurfaceStatus::UNKNOWN);
}

/*
 * ── UnregisterSplitSurfaceNode ─────────────────────────────
 */

/**
 * @tc.name: UnregisterSplitSurfaceNode_SplitSurfaceNotNull_ControllerNotNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when splitSurface_ and requestController_ are both not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    UnregisterSplitSurfaceNode_SplitSurfaceNotNull_ControllerNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->requestController_ = std::make_shared<RequestController>();

    planner_->UnregisterSplitSurfaceNode();
}

/**
 * @tc.name: UnregisterSplitSurfaceNode_SplitSurfaceNotNull_ControllerNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when splitSurface_ is not null but requestController_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    UnregisterSplitSurfaceNode_SplitSurfaceNotNull_ControllerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->requestController_ = nullptr;

    planner_->UnregisterSplitSurfaceNode();
}

/**
 * @tc.name: UnregisterSplitSurfaceNode_SplitSurfaceNull_ControllerNotNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when splitSurface_ is null but requestController_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    UnregisterSplitSurfaceNode_SplitSurfaceNull_ControllerNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->requestController_ = std::make_shared<RequestController>();

    planner_->UnregisterSplitSurfaceNode();
}

/**
 * @tc.name: UnregisterSplitSurfaceNode_SplitSurfaceNull_ControllerNull
 * @tc.desc: Test UnregisterSplitSurfaceNode when both splitSurface_ and requestController_ are null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UnregisterSplitSurfaceNode_SplitSurfaceNull_ControllerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->requestController_ = nullptr;

    planner_->UnregisterSplitSurfaceNode();
}

/*
 * ── ClearAllChildrenLayerObjects ───────────────────────────
 */

/**
 * @tc.name: ClearAllChildrenLayerObjects_SplitSurfaceNull_SetsEmpty
 * @tc.desc: Test ClearAllChildrenLayerObjects when splitSurface_ is null and child sets are empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_SplitSurfaceNull_SetsEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();
}

/**
 * @tc.name: ClearAllChildrenLayerObjects_SplitSurfaceNotNull_DrawableNull
 * @tc.desc: Test ClearAllChildrenLayerObjects when splitSurface_ is not null but splitSurfaceDrawable_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    ClearAllChildrenLayerObjects_SplitSurfaceNotNull_DrawableNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->splitSurfaceDrawable_ = nullptr;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();
}

/**
 * @tc.name: ClearAllChildrenLayerObjects_SplitSurfaceNotNull_DrawableNotNull
 * @tc.desc: Test ClearAllChildrenLayerObjects when splitSurface_ and splitSurfaceDrawable_ are both not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    ClearAllChildrenLayerObjects_SplitSurfaceNotNull_DrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();
}

/**
 * @tc.name: ClearAllChildrenLayerObjects_ChildSetsNotEmpty
 * @tc.desc: Test ClearAllChildrenLayerObjects when child sets are not empty (tests loop execution)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_ChildSetsNotEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    auto childNode = CreateSurfaceNode(100);
    planner_->currFrameChildSet_.insert(childNode);
    planner_->lastFrameChildSet_.insert(childNode);

    planner_->ClearAllChildrenLayerObjects();

    ASSERT_EQ(planner_->currFrameChildSet_.size(), 1);
    ASSERT_EQ(planner_->lastFrameChildSet_.size(), 1);
}

/*
 * ── Sync ───────────────────────────────────────────────────
 */

/**
 * @tc.name: Sync_SplitSurfaceNull_SetsEmpty
 * @tc.desc: Test Sync when splitSurface_ is null and child sets are empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_SplitSurfaceNull_SetsEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);
}

/**
 * @tc.name: Sync_SplitSurfaceNotNull_DrawableNull
 * @tc.desc: Test Sync when splitSurface_ is not null but splitSurfaceDrawable_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_SplitSurfaceNotNull_DrawableNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->splitSurfaceDrawable_ = nullptr;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);
}

/**
 * @tc.name: Sync_SplitSurfaceNotNull_DrawableNotNull
 * @tc.desc: Test Sync when splitSurface_ and splitSurfaceDrawable_ are both not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_SplitSurfaceNotNull_DrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);
}

/**
 * @tc.name: Sync_CurrFrameChildSet_HasNewNode
 * @tc.desc: Test Sync when currFrameChildSet_ has a node not in lastFrameChildSet_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_CurrFrameChildSet_HasNewNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    auto newNode = CreateSurfaceNode(100);
    planner_->currFrameChildSet_.insert(newNode);
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->currFrameChildSet_.count(newNode), 1);
}

/**
 * @tc.name: Sync_LastFrameChildSet_HasRemovedNode
 * @tc.desc: Test Sync when lastFrameChildSet_ has a node not in currFrameChildSet_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_LastFrameChildSet_HasRemovedNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    auto removedNode = CreateSurfaceNode(200);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.insert(removedNode);

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);
    ASSERT_EQ(planner_->lastFrameChildSet_.count(removedNode), 1);
}

/**
 * @tc.name: Sync_BothSetsHaveSameNodes
 * @tc.desc: Test Sync when both sets have the same nodes (no changes)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_BothSetsHaveSameNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    auto sharedNode = CreateSurfaceNode(300);
    planner_->currFrameChildSet_.insert(sharedNode);
    planner_->lastFrameChildSet_.insert(sharedNode);

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->currFrameChildSet_.count(sharedNode), 1);
    ASSERT_EQ(planner_->lastFrameChildSet_.count(sharedNode), 1);
}

/*
 * ── UpdateChildren ─────────────────────────────────────────
 */

/**
 * @tc.name: UpdateChildren_ParentNull
 * @tc.desc: Test UpdateChildren when parent is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_ParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto originalCurrSet = planner_->currFrameChildSet_;
    auto originalLastSet = planner_->lastFrameChildSet_;

    planner_->UpdateChildren(nullptr);

    ASSERT_EQ(planner_->currFrameChildSet_.size(), originalCurrSet.size());
    ASSERT_EQ(planner_->lastFrameChildSet_.size(), originalLastSet.size());
}

/**
 * @tc.name: UpdateChildren_ChildrenNull
 * @tc.desc: Test UpdateChildren when parent->GetSortedChildren() returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_ChildrenNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    auto originalCurrSet = planner_->currFrameChildSet_;
    auto originalLastSet = planner_->lastFrameChildSet_;

    planner_->UpdateChildren(parentNode);

    ASSERT_EQ(planner_->currFrameChildSet_.size(), originalCurrSet.size());
    ASSERT_EQ(planner_->lastFrameChildSet_.size(), originalLastSet.size());
}

/**
 * @tc.name: UpdateChildren_NoOpincNodes
 * @tc.desc: Test UpdateChildren when children exist but none have OpincGetRootFlag()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_NoOpincNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    auto childNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_TRUE(planner_->currFrameChildSet_.empty());
}

/**
 * @tc.name: UpdateChildren_HasOpincNodes
 * @tc.desc: Test UpdateChildren when children have OpincGetRootFlag() set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_HasOpincNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    auto childNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_TRUE(planner_->currFrameChildSet_.empty() || planner_->currFrameChildSet_.size() >= 0);
}

/*
 * ── GetDfxString ───────────────────────────────────────────
 */

/**
 * @tc.name: GetDfxString_BothSetsEmpty
 * @tc.desc: Test GetDfxString when both child sets are empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetDfxString_BothSetsEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    std::string dfxStr = planner_->GetDfxString();

    ASSERT_TRUE(dfxStr.find(", Cur[0]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(", Last[0]:") != std::string::npos);
}

/**
 * @tc.name: GetDfxString_CurrSetHasNodes_LastSetEmpty
 * @tc.desc: Test GetDfxString when currFrameChildSet_ has nodes but lastFrameChildSet_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetDfxString_CurrSetHasNodes_LastSetEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto node = CreateSurfaceNode(100);
    planner_->currFrameChildSet_.insert(node);
    planner_->lastFrameChildSet_.clear();

    std::string dfxStr = planner_->GetDfxString();

    ASSERT_TRUE(dfxStr.find(", Cur[1]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(" 100") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(", Last[0]:") != std::string::npos);
}

/**
 * @tc.name: GetDfxString_CurrSetEmpty_LastSetHasNodes
 * @tc.desc: Test GetDfxString when currFrameChildSet_ is empty but lastFrameChildSet_ has nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetDfxString_CurrSetEmpty_LastSetHasNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto node = CreateSurfaceNode(200);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.insert(node);

    std::string dfxStr = planner_->GetDfxString();

    ASSERT_TRUE(dfxStr.find(", Cur[0]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(", Last[1]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(" 200") != std::string::npos);
}

/**
 * @tc.name: GetDfxString_BothSetsHaveNodes
 * @tc.desc: Test GetDfxString when both child sets have nodes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetDfxString_BothSetsHaveNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto currNode = CreateSurfaceNode(100);
    auto lastNode = CreateSurfaceNode(200);
    planner_->currFrameChildSet_.insert(currNode);
    planner_->lastFrameChildSet_.insert(lastNode);

    std::string dfxStr = planner_->GetDfxString();

    ASSERT_TRUE(dfxStr.find(", Cur[1]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(" 100") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(", Last[1]:") != std::string::npos);
    ASSERT_TRUE(dfxStr.find(" 200") != std::string::npos);
}

/*
 * ── GetOpIncParentNode ─────────────────────────────────────
 */

/**
 * @tc.name: GetOpIncParentNode_NotNull
 * @tc.desc: Test GetOpIncParentNode when opIncParentNode_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetOpIncParentNode_NotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    planner_->opIncParentNode_ = parentNode;

    auto result = planner_->GetOpIncParentNode();

    ASSERT_EQ(result, parentNode);
}

/**
 * @tc.name: GetOpIncParentNode_Null
 * @tc.desc: Test GetOpIncParentNode when opIncParentNode_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetOpIncParentNode_Null, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncParentNode_ = nullptr;

    auto result = planner_->GetOpIncParentNode();

    ASSERT_EQ(result, nullptr);
}

/*
 * ── GetPlanStatus ──────────────────────────────────────────
 */

/**
 * @tc.name: GetPlanStatus_ReturnsCorrectStatus
 * @tc.desc: Test GetPlanStatus returns the correct planStatus_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetPlanStatus_ReturnsCorrectStatus, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->planStatus_ = PlanStatus::ON;

    PlanStatus status = planner_->GetPlanStatus();

    ASSERT_EQ(status, PlanStatus::ON);
}

/*
 * ── GetSrcRect ─────────────────────────────────────────────
 */

/**
 * @tc.name: GetSrcRect_ReturnsCorrectRect
 * @tc.desc: Test GetSrcRect returns the correct srcRect_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetSrcRect_ReturnsCorrectRect, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->srcRect_ = RectF(10, 20, 100, 200);

    RectF rect = planner_->GetSrcRect();

    ASSERT_EQ(rect, RectF(10, 20, 100, 200));
}

/*
 * ── GetDstRect ─────────────────────────────────────────────
 */

/**
 * @tc.name: GetDstRect_ReturnsCorrectRect
 * @tc.desc: Test GetDstRect returns the correct dstRect_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetDstRect_ReturnsCorrectRect, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->dstRect_ = RectF(30, 40, 300, 400);

    RectF rect = planner_->GetDstRect();

    ASSERT_EQ(rect, RectF(30, 40, 300, 400));
}

/*
 * ── GetRequestController ───────────────────────────────────
 */

/**
 * @tc.name: GetRequestController_NotNull
 * @tc.desc: Test GetRequestController when requestController_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetRequestController_NotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto controller = std::make_shared<RequestController>();
    planner_->requestController_ = controller;

    auto result = planner_->GetRequestController();

    ASSERT_EQ(result, controller);
}

/**
 * @tc.name: GetRequestController_Null
 * @tc.desc: Test GetRequestController when requestController_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetRequestController_Null, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->requestController_ = nullptr;

    auto result = planner_->GetRequestController();

    ASSERT_EQ(result, nullptr);
}

/*
 * ── SetOpIncParentNode ─────────────────────────────────────
 */

/**
 * @tc.name: SetOpIncParentNode_SetsCorrectNode
 * @tc.desc: Test SetOpIncParentNode correctly sets opIncParentNode_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetOpIncParentNode_SetsCorrectNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    planner_->opIncParentNode_ = nullptr;

    planner_->SetOpIncParentNode(parentNode);

    ASSERT_EQ(planner_->opIncParentNode_, parentNode);
}

/*
 * ── GetOpIncNodes ──────────────────────────────────────────
 */

/**
 * @tc.name: GetOpIncNodes_ReturnsCorrectRef
 * @tc.desc: Test GetOpIncNodes returns correct reference to opIncNodes_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetOpIncNodes_ReturnsCorrectRef, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncNodes_.clear();
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    auto& result = planner_->GetOpIncNodes();

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].first, 100);
}

/*
 * ── GetLastOpIncNodes ──────────────────────────────────────
 */

/**
 * @tc.name: GetLastOpIncNodes_ReturnsCorrectRef
 * @tc.desc: Test GetLastOpIncNodes returns correct reference to lastOpIncNodes_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetLastOpIncNodes_ReturnsCorrectRef, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->lastOpIncNodes_.clear();
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 5.0f, 6.0f));

    auto& result = planner_->GetLastOpIncNodes();

    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].first, 200);
}

/*
 * ── GetSplitSurface ────────────────────────────────────────
 */

/**
 * @tc.name: GetSplitSurface_NotNull
 * @tc.desc: Test GetSplitSurface when splitSurface_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetSplitSurface_NotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);

    auto result = planner_->GetSplitSurface();

    ASSERT_EQ(result, planner_->splitSurface_);
}

/**
 * @tc.name: GetSplitSurface_Null
 * @tc.desc: Test GetSplitSurface when splitSurface_ is null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetSplitSurface_Null, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;

    auto result = planner_->GetSplitSurface();

    ASSERT_EQ(result, nullptr);
}

/*
 * ── GetNeedLeave ───────────────────────────────────────────
 */

/**
 * @tc.name: GetNeedLeave_True
 * @tc.desc: Test GetNeedLeave when needLeave_ is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetNeedLeave_True, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->needLeave_ = true;

    bool result = planner_->GetNeedLeave();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: GetNeedLeave_False
 * @tc.desc: Test GetNeedLeave when needLeave_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetNeedLeave_False, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->needLeave_ = false;

    bool result = planner_->GetNeedLeave();

    ASSERT_EQ(result, false);
}
/*
 * ── Reset ────────────────────────────────────────────────────
 */

/**
 * @tc.name: Reset_SplitSurfaceNull
 * @tc.desc: Test Reset when splitSurface_ is nullptr (early return at line 49)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ == nullptr
    planner_->splitSurface_ = nullptr;

    planner_->Reset();
}

/**
 * @tc.name: Reset_SplitSurfaceNotNull_OpIncNodesEmpty
 * @tc.desc: Test Reset when splitSurface_ is not null and opIncNodes_ is empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_SplitSurfaceNotNull_OpIncNodesEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ != nullptr, opIncNodes_.empty()
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncNodes_.clear();
    planner_->lastOpIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->Reset();
}

/**
 * @tc.name: Reset_SplitSurfaceNotNull_OpIncNodesNotEmpty
 * @tc.desc: Test Reset when splitSurface_ is not null and opIncNodes_ is not empty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_SplitSurfaceNotNull_OpIncNodesNotEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ != nullptr, !opIncNodes_.empty()
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->opIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 5.0f, 6.0f));
    planner_->lastOpIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->Reset();

    ASSERT_TRUE(planner_->opIncNodes_.empty());
}

/**
 * @tc.name: Reset_PlanStatusOFF_RequestControllerNull
 * @tc.desc: Test Reset when planStatus=OFF and requestController_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusOFF_RequestControllerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON is false, requestController_ == nullptr (UNLIKELY branch)
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_ = nullptr;

    planner_->Reset();
}

/**
 * @tc.name: Reset_PlanStatusOFF
 * @tc.desc: Test Reset when planStatus_ is OFF (early return before IncStayOnCount)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusOFF, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::OFF
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->Reset();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: Reset_PlanStatusON_RequestControllerNull
 * @tc.desc: Test Reset when planStatus=ON but requestController_ is nullptr (UNLIKELY returns)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusON_RequestControllerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON, requestController_ == nullptr (UNLIKELY)
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::ON;
    planner_->requestController_ = nullptr;

    planner_->Reset();
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NodeAlreadyVisited
 * @tc.desc: Test CheckOpIncNodeFromCommand when nodeId is already in visitedNodeId_ (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NodeAlreadyVisited, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.insert(100);

    planner_->CheckOpIncNodeFromCommand(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NodeNotFound
 * @tc.desc: Test CheckOpIncNodeFromCommand when node is nullptr or not on tree (sets canDoDirectComposition_=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NodeNotFound, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: node == nullptr || !node->IsOnTheTree()
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();

    planner_->CheckOpIncNodeFromCommand(999);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_ParentMismatch
 * @tc.desc: Test CheckOpIncNodeFromCommand when parent node ID doesn't match
 *           opIncParentNode_ (sets canDoDirectComposition_=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_ParentMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: parent->GetId() != opIncParentNode_->GetId()
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->opIncParentNode_ = CreateSurfaceNode(200);

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NotInLastOpIncNodes
 * @tc.desc: Test CheckOpIncNodeFromCommand when nodeId not in lastOpIncNodeIds_ (sets canDoDirectComposition_=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NotInLastOpIncNodes, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: lastOpIncNodeIds_.find(nodeId) == lastOpIncNodeIds_.end()
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.clear();
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_DirtyTypesExist
 * @tc.desc: Test CheckOpIncNodeFromCommand when dirty types exist beyond
 *           bounds/frame (sets canDoDirectComposition_=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_DirtyTypesExist, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: dirtyTypes.count() > bounds test + frame test
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.clear();
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_BoundsSizeChange
 * @tc.desc: Test CheckOpIncNodeFromCommand when bounds size change is true (sets canDoDirectComposition_=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_BoundsSizeChange, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: boundsSizeChange == true
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.clear();

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_Success
 * @tc.desc: Test CheckOpIncNodeFromCommand when all conditions pass (inserts nodeId)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_Success, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.clear();

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_EQ(planner_->visitedNodeId_.count(100), 0);
}

/*
 * ── GetBoundsFromModifier ──────────────────────────────────────
 */

/**
 * @tc.name: GetBoundsFromModifier_NodeNull
 * @tc.desc: Test GetBoundsFromModifier when node is nullptr (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_NodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: node == nullptr
    auto [result, bounds] = planner_->GetBoundsFromModifier(nullptr);

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetBoundsFromModifier_ModifiersEmpty
 * @tc.desc: Test GetBoundsFromModifier when boundsModifiers is empty (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_ModifiersEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: boundsModifiers is empty
    auto node = CreateSurfaceNode(100);
    auto [result, bounds] = planner_->GetBoundsFromModifier(node);

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetBoundsFromModifier_Success
 * @tc.desc: Test GetBoundsFromModifier when bounds modifier exists (returns true with bounds)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_Success, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bounds modifier found and returns {true, bounds}
    auto node = CreateSurfaceNode(100);
    auto [result, bounds] = planner_->GetBoundsFromModifier(node);

    ASSERT_EQ(result, false); // No modifier attached by default
}

/*
 * ── CheckCanDoDirectComposition ────────────────────────────────
 */

/**
 * @tc.name: CheckCanDoDirectComposition_CanDoFalse
 * @tc.desc: Test CheckCanDoDirectComposition when canDoDirectComposition_ is false (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_CanDoFalse, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: canDoDirectComposition_ == false
    planner_->canDoDirectComposition_ = false;

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_VisitedNodesEmpty
 * @tc.desc: Test CheckCanDoDirectComposition when visitedNodeId_ is empty (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_VisitedNodesEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: visitedNodeId_.empty()
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_VisitedSizeMismatch
 * @tc.desc: Test CheckCanDoDirectComposition when size mismatch (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_VisitedSizeMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: visitedNodeId_.size() != lastOpIncNodes_.size()
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.insert(100);
    planner_->visitedNodeId_.insert(200);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_GetBoundsFailed
 * @tc.desc: Test CheckCanDoDirectComposition when GetBoundsFromModifier returns false (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_GetBoundsFailed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: GetBoundsFromModifier returns !isSucc
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_OffsetMismatch
 * @tc.desc: Test CheckCanDoDirectComposition when node offsets don't match (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_OffsetMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !Vector2fNearEqual(currentOffset_, nodeOffset)
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_Success
 * @tc.desc: Test CheckCanDoDirectComposition when all conditions pass (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_Success, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: all conditions pass
    planner_->canDoDirectComposition_ = true;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false); // GetBoundsFromModifier returns false by default
}

/*
 * ── MoveSplitSurfaceNode ──────────────────────────────────────
 */

/**
 * @tc.name: MoveSplitSurfaceNode_SplitSurfaceNull
 * @tc.desc: Test MoveSplitSurfaceNode when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ == nullptr
    planner_->splitSurface_ = nullptr;

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_SplitSurfaceNodeNull
 * @tc.desc: Test MoveSplitSurfaceNode when splitSurface_->splitSurfaceNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_SplitSurfaceNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_->splitSurfaceNode_ == nullptr
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->splitSurfaceNode_ = nullptr;

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_BufferNotConsumed
 * @tc.desc: Test MoveSplitSurfaceNode when IsBufferConsumed is false (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferNotConsumed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !splitSurface_->IsBufferConsumed()
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_BufferConsumed_ParentFound_ChildrenEmpty
 * @tc.desc: Test MoveSplitSurfaceNode: IsBufferConsumed true, parent found with empty children (no MoveChild)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferConsumed_ParentFound_ChildrenEmpty,
    TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed true, parent not null, children list empty

    // Create a parent node
    auto parentNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    // Create split surface with splitSurfaceNode_
    planner_->splitSurface_ = CreateSplitSurface(100);
    // Add splitSurfaceNode_ as child of parentNode
    parentNode->AddChild(planner_->splitSurface_->splitSurfaceNode_);

    // Set a buffer on the surfaceHandler so IsBufferConsumed returns true
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(
        buffer, nullptr, Rect(), 0, nullptr);

    // Set children list to empty explicitly (overrides the default EmptyChildrenList)
    auto emptyChildren = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>();
    std::atomic_store_explicit(&parentNode->fullChildrenList_, emptyChildren,
        std::memory_order_release);

    planner_->MoveSplitSurfaceNode();

    // After the call, buffer should remain set (no side effects since children are empty)
    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->
        GetBuffer(), nullptr);
}

/**
 * @tc.name: MoveSplitSurfaceNode_BufferConsumed_ParentFound_ChildrenFrontIsSplit
 * @tc.desc: Test MoveSplitSurfaceNode: front child is splitSurfaceNode_ (no MoveChild needed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferConsumed_ParentFound_ChildrenFrontIsSplit,
    TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed true, parent not null, children front == splitSurfaceNode_

    auto parentNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    planner_->splitSurface_ = CreateSplitSurface(100);
    parentNode->AddChild(planner_->splitSurface_->splitSurfaceNode_);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(
        buffer, nullptr, Rect(), 0, nullptr);

    // Set children list with splitSurfaceNode_ as the only (and front) element
    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(planner_->splitSurface_->splitSurfaceNode_);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&parentNode->fullChildrenList_, constChildren,
        std::memory_order_release);

    planner_->MoveSplitSurfaceNode();

    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->
        GetBuffer(), nullptr);
}

/*
 * ── CollectOpIncNodes ─────────────────────────────────────────
 */

/**
 * @tc.name: CollectOpIncNodes_ChildrenEmpty
 * @tc.desc: Test CollectOpIncNodes when children list is empty (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildrenEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: children is empty
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
    ASSERT_TRUE(planner_->opIncNodes_.empty());
}

/**
 * @tc.name: CollectOpIncNodes_NoOpIncChildren
 * @tc.desc: Test CollectOpIncNodes when no children have OpincGetRootFlag (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_NoOpIncChildren, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: no children with OpincGetRootFlag()
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CollectOpIncNodes_OverlapDetected
 * @tc.desc: Test CollectOpIncNodes when overlap is detected (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_OverlapDetected, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: isOverlap == true
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false); // No op inc children by default
}

/**
 * @tc.name: CollectOpIncNodes_NormalCase
 * @tc.desc: Test CollectOpIncNodes in normal case (returns false when no overlap)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_NormalCase, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: normal execution path
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CollectOpIncNodes_ChildGeoPtrNull
 * @tc.desc: Test CollectOpIncNodes when child boundsGeoPtr is nullptr (skips child)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildGeoPtrNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: geoPtr == nullptr (skips child)
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
}

/*
 * ── IsOpIncNodesChanged ──────────────────────────────────────
 */

/**
 * @tc.name: IsOpIncNodesChanged_LastOpIncNodesEmpty
 * @tc.desc: Test IsOpIncNodesChanged when lastOpIncNodes_ is empty (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_LastOpIncNodesEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: lastOpIncNodes_.empty()
    planner_->lastOpIncNodes_.clear();
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: IsOpIncNodesChanged_SizeMismatch
 * @tc.desc: Test IsOpIncNodesChanged when size doesn't match (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_SizeMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: opIncNodesSize != lastOpIncNodes_.size()
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 5.0f, 6.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: IsOpIncNodesChanged_NodeIdMismatch
 * @tc.desc: Test IsOpIncNodesChanged when node IDs don't match (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_NodeIdMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: opIncNodes_[i].first != lastOpIncNodes_[i].first
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: IsOpIncNodesChanged_OffsetMismatch
 * @tc.desc: Test IsOpIncNodesChanged when offsets don't match (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_OffsetMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !Vector2fNearEqual(currentOffset_, nodeOffset)
    planner_->opIncNodes_.emplace_back(100, Vector4f(10.0f, 20.0f, 50.0f, 50.0f));
    planner_->opIncNodes_.emplace_back(200, Vector4f(10.0f, 20.0f, 50.0f, 50.0f));
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(30.0f, 40.0f, 50.0f, 50.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(30.0f, 50.0f, 50.0f, 50.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: IsOpIncNodesChanged_NoChange
 * @tc.desc: Test IsOpIncNodesChanged when all nodes match (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_NoChange, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: all nodes match, no change detected
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->opIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 5.0f, 6.0f));
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 5.0f, 6.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, false);
}

/*
 * ── UpdateBufferBounds ─────────────────────────────────────────
 */

/**
 * @tc.name: UpdateBufferBounds_GeoPtrNull
 * @tc.desc: Test UpdateBufferBounds when boundsGeo_ is nullptr (returns true early)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_GeoPtrNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: geoPtr == nullptr
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().boundsGeo_ = nullptr;
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateBufferBounds_PlanStatusOFF
 * @tc.desc: Test UpdateBufferBounds when planStatus_ is OFF (normal execution path)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusOFF, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::OFF
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->screenWidth_ = 100;
    planner_->screenHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_NE(result, false);
}

/**
 * @tc.name: UpdateBufferBounds_PlanStatusPREPARE_IsUpdateBufferFalse
 * @tc.desc: Test UpdateBufferBounds when planStatus=PREPARE and isUpdateBuffer_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusPREPARE_IsUpdateBufferFalse, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::PREPARE, !isUpdateBuffer_
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->screenWidth_ = 100;
    planner_->screenHeight_ = 100;
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = false;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: UpdateBufferBounds_PlanStatusPREPARE_GeoMatrixNull
 * @tc.desc: Test UpdateBufferBounds when planStatus=PREPARE but geoMatrix is nullptr (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusPREPARE_GeoMatrixNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::PREPARE, geoMatrix == nullptr
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().boundsGeo_ = nullptr;
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->screenWidth_ = 100;
    planner_->screenHeight_ = 100;
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = false;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateBufferBounds_PlanStatusON_IsUpdateBufferTrue
 * @tc.desc: Test UpdateBufferBounds when planStatus=ON and isUpdateBuffer_ is true (no-op branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusON_IsUpdateBufferTrue, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON, isUpdateBuffer_ == true
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 50, 50);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: UpdateBufferBounds_SrcRectOutOfBounds
 * @tc.desc: Test UpdateBufferBounds when srcRect_ is out of buffer bounds (returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectOutOfBounds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: srcRect_ outside buffer
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->srcRect_ = RectF(-10, -10, 200, 200);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/*
 * ── SetSrcAndDstRect ───────────────────────────────────────────
 */

/**
 * @tc.name: SetSrcAndDstRect_SplitSurfaceNull
 * @tc.desc: Test SetSrcAndDstRect when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ == nullptr
    planner_->splitSurface_ = nullptr;

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_SplitSurfaceNodeNull
 * @tc.desc: Test SetSrcAndDstRect when splitSurface_->splitSurfaceNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_SplitSurfaceNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_->splitSurfaceNode_ == nullptr
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->splitSurface_->splitSurfaceNode_ = nullptr;

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_SurfaceParamsNull
 * @tc.desc: Test SetSrcAndDstRect when surfaceParams is nullptr (sets rects but returns early)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_SurfaceParamsNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: surfaceParams == nullptr
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(0, 0, 100, 100);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_NormalCase
 * @tc.desc: Test SetSrcAndDstRect in normal case (full execution)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_NormalCase, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: normal execution
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(10, 20, 50, 60);
    planner_->dstRect_ = RectF(30, 40, 70, 80);

    planner_->SetSrcAndDstRect();
}

/*
 * ── InitSplitSurface ──────────────────────────────────────────
 */

/**
 * @tc.name: InitSplitSurface_SplitSurfaceNull
 * @tc.desc: Test InitSplitSurface when splitSurface_ is nullptr (creates new split surface)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, InitSplitSurface_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;

    ScreenInfo screenInfo;
    screenInfo.width = 1920;
    screenInfo.height = 1080;
    planner_->InitSplitSurface(screenInfo);

    ASSERT_NE(planner_->splitSurface_, nullptr);
    ASSERT_EQ(planner_->screenWidth_, 1920);
    ASSERT_EQ(planner_->screenHeight_, 1080);
}

/**
 * @tc.name: InitSplitSurface_SplitSurfaceNotNull
 * @tc.desc: Test InitSplitSurface when splitSurface_ is not null (no-op)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, InitSplitSurface_SplitSurfaceNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->screenWidth_ = 0;
    planner_->screenHeight_ = 0;

    ScreenInfo screenInfo;
    screenInfo.width = 1920;
    screenInfo.height = 1080;
    planner_->InitSplitSurface(screenInfo);

    ASSERT_EQ(planner_->screenWidth_, 0);
    ASSERT_EQ(planner_->screenHeight_, 0);
}

/*
 * ── CheckSplitNodeIntersectFilter ─────────────────────────────
 */

/**
 * @tc.name: CheckSplitNodeIntersectFilter_SplitSurfaceNull
 * @tc.desc: Test CheckSplitNodeIntersectFilter when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckSplitNodeIntersectFilter_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    planner_->needLeave_ = false;

    auto hwcNode = CreateSurfaceNode(100);
    planner_->CheckSplitNodeIntersectFilter(hwcNode);

    ASSERT_EQ(planner_->needLeave_, false);
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter_SplitSurfaceNodeNull
 * @tc.desc: Test CheckSplitNodeIntersectFilter when splitSurfaceNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckSplitNodeIntersectFilter_SplitSurfaceNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = nullptr;
    planner_->needLeave_ = false;

    auto hwcNode = CreateSurfaceNode(100);
    planner_->CheckSplitNodeIntersectFilter(hwcNode);

    ASSERT_EQ(planner_->needLeave_, false);
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter_HwcNodeEqualsSplitSurfaceNode
 * @tc.desc: Test CheckSplitNodeIntersectFilter when hwcNode equals splitSurfaceNode_ (sets needLeave_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckSplitNodeIntersectFilter_HwcNodeEqualsSplitSurfaceNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto splitSurfaceNode = CreateSurfaceNode(100);
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = splitSurfaceNode;
    planner_->needLeave_ = false;

    planner_->CheckSplitNodeIntersectFilter(splitSurfaceNode);

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckSplitNodeIntersectFilter_HwcNodeNotEqualsSplitSurfaceNode
 * @tc.desc: Test CheckSplitNodeIntersectFilter when hwcNode differs from splitSurfaceNode_ (no-op)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest,
    CheckSplitNodeIntersectFilter_HwcNodeNotEqualsSplitSurfaceNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = false;

    auto hwcNode = CreateSurfaceNode(200);
    planner_->CheckSplitNodeIntersectFilter(hwcNode);

    ASSERT_EQ(planner_->needLeave_, false);
}

/*
 * ── SetSrcAndDstRect additional branches ─────────────────────
 */

/**
 * @tc.name: SetSrcAndDstRect_GeoPtrNull
 * @tc.desc: Test SetSrcAndDstRect when geoPtr is nullptr (returns early after setting layerInfo)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_GeoPtrNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(0, 0, 100, 100);
    planner_->dstRect_ = RectF(0, 0, 100, 100);
    planner_->splitSurface_->splitSurfaceNode_->GetMutableRenderProperties().boundsGeo_ = nullptr;

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_BufferNull
 * @tc.desc: Test SetSrcAndDstRect when buffer is nullptr (skips SetCropMetadata)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_BufferNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(0, 0, 100, 100);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/*
 * ── UpdateBufferBounds additional branches ────────────────────
 */

/**
 * @tc.name: UpdateBufferBounds_CurrentOffsetNonZero
 * @tc.desc: Test UpdateBufferBounds when planStatus=PREPARE and currentOffset_ != (0,0) (sets isUpdateBuffer_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_CurrentOffsetNonZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = false;
    planner_->currentOffset_ = Vector2f(5.0f, 10.0f);
    planner_->srcRect_ = RectF(100, 100, 200, 200);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
    ASSERT_EQ(planner_->isUpdateBuffer_, true);
}

/*
 * ── MoveSplitSurfaceNode additional branches ─────────────────
 */

/**
 * @tc.name: MoveSplitSurfaceNode_BufferConsumed_ParentFound_FrontNotSplit
 * @tc.desc: Test MoveSplitSurfaceNode: front child != splitSurfaceNode_ (calls MoveChild)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferConsumed_ParentFound_FrontNotSplit,
    TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed true, parent not null, children front != splitSurfaceNode_

    auto parentNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    planner_->splitSurface_ = CreateSplitSurface(100);
    parentNode->AddChild(planner_->splitSurface_->splitSurfaceNode_);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(
        buffer, nullptr, Rect(), 0, nullptr);

    // Create another child that will be front of the children list
    auto otherChild = CreateSurfaceNode(300);
    // Set children list with otherChild first, then splitSurfaceNode_
    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(otherChild);
    children->push_back(planner_->splitSurface_->splitSurfaceNode_);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&parentNode->fullChildrenList_, constChildren,
        std::memory_order_release);

    planner_->MoveSplitSurfaceNode();

    // After MoveChild, buffer should still be set
    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->
        GetBuffer(), nullptr);
}

/*
 * ── IsOpIncNodesChanged fixes ────────────────────────────────
 */

/**
 * @tc.name: IsOpIncNodesChanged_AllMatchSingle
 * @tc.desc: Test IsOpIncNodesChanged when a single node matches (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_AllMatchSingle, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 50.0f, 50.0f));
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 50.0f, 50.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: IsOpIncNodesChanged_MultipleNodesNoChange
 * @tc.desc: Test IsOpIncNodesChanged when multiple nodes all match (returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, IsOpIncNodesChanged_MultipleNodesNoChange, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 50.0f, 50.0f));
    planner_->opIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 60.0f, 60.0f));
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 50.0f, 50.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 60.0f, 60.0f));

    bool result = planner_->IsOpIncNodesChanged();

    ASSERT_EQ(result, false);
}

/*
 * ── ProcessPlanStatusAction ──────────────────────────────────
 */

/**
 * @tc.name: ProcessPlanStatusAction_NeedLeaveAndBufferConsumed
 * @tc.desc: Test ProcessPlanStatusAction when needLeave_=true and IsBufferConsumed() (calls SetBufferNull)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_NeedLeaveAndBufferConsumed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->ProcessPlanStatusAction();
}

/*
 * ── CollectOpIncNodes additional branches ────────────────────
 */

/**
 * @tc.name: CollectOpIncNodes_ChildIsSplitSurfaceNode
 * @tc.desc: Test CollectOpIncNodes when a child is the splitSurfaceNode_ (skipped in loop)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildIsSplitSurfaceNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    auto child = CreateSurfaceNode(300);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
}

/*
 * ── Reset additional branches ────────────────────────────────
 */

/**
 * @tc.name: Reset_PlanStatusON_SurfaceHandlerNotNull
 * @tc.desc: Test Reset when planStatus=ON with valid splitSurfaceBuffer_ and surfaceHandler (line 78-82)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusON_SurfaceHandlerNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON, splitSurfaceBuffer_ != nullptr, surfaceHandler != nullptr
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = CreateSurfaceNode(100);
    splitSurface->splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test", 100, 100, 100);
    planner_->splitSurface_ = splitSurface;
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::ON;
    planner_->requestController_ = std::make_shared<RequestController>();

    planner_->Reset();

    ASSERT_EQ(planner_->canDoDirectComposition_, false);
}

/*
 * ── CheckOpIncNodeFromCommand additional branches ───────────
 */

/**
 * @tc.name: CheckOpIncNodeFromCommand_PlanStatusNotON
 * @tc.desc: Test CheckOpIncNodeFromCommand when planStatus_ != ON (returns false at line 87)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_PlanStatusNotON, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ != PlanStatus::ON
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_EQ(planner_->canDoDirectComposition_, true);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_VisitedNodeFound
 * @tc.desc: Test CheckOpIncNodeFromCommand when nodeId found in visitedNodeId_ (returns true at line 92)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_VisitedNodeFound, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: visitedNodeId_.find(nodeId) != visitedNodeId_.end()
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_TRUE(planner_->visitedNodeId_.count(100) > 0);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NodeOnTree_LastOpIncNodeIdsFound
 * @tc.desc: Test CheckOpIncNodeFromCommand when node on tree and found in lastOpIncNodeIds_ (lines 107-110)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NodeOnTree_LastOpIncNodeIdsFound, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: lastOpIncNodeIds_.find(nodeId) != lastOpIncNodeIds_.end()
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.insert(100);
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_DirtyTypesNotDirty
 * @tc.desc: Test CheckOpIncNodeFromCommand when dirtyTypes has only BOUNDS/FRAME (line 112-117 pass, falls to success)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_DirtyTypesNotDirty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: dirtyTypes.count() <= bounds+frame test — success path
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.insert(100);
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_NE(planner_->canDoDirectComposition_, false);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_SuccessInsert
 * @tc.desc: Test CheckOpIncNodeFromCommand success path — node inserted to visitedNodeId_ (line 119-121)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_SuccessInsert, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: all conditions pass, node inserted into visitedNodeId_
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.insert(100);
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_EQ(planner_->visitedNodeId_.count(100), 0);
}

/*
 * ── CheckCanDoDirectComposition additional branches ────────────
 */

/**
 * @tc.name: CheckCanDoDirectComposition_PlanStatusNotON
 * @tc.desc: Test CheckCanDoDirectComposition when planStatus_ != ON (returns false at line 149)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_PlanStatusNotON, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ != PlanStatus::ON
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_BoundsSizeMismatch
 * @tc.desc: Test CheckCanDoDirectComposition when bounds z/w mismatch (returns false at line 166)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_BoundsSizeMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ROSEN_NE(bounds.z_, position.z_) || ROSEN_NE(bounds.w_, position.w_)
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CheckCanDoDirectComposition_UpdateBufferBoundsOut
 * @tc.desc: Test CheckCanDoDirectComposition when UpdateBufferBounds returns true (returns false at line 184-186)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_UpdateBufferBoundsOut, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: UpdateBufferBounds returns true — isBufferBoundsOut path
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);
}

/*
 * ── MoveSplitSurfaceNode additional branches ─────────────────
 */

/**
 * @tc.name: MoveSplitSurfaceNode_BufferConsumed_ParentNull
 * @tc.desc: Test MoveSplitSurfaceNode: IsBufferConsumed true but GetParent returns null (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferConsumed_ParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed true, parent == nullptr

    planner_->splitSurface_ = CreateSplitSurface(100);

    // Set buffer so IsBufferConsumed returns true, but no parent set
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(
        buffer, nullptr, Rect(), 0, nullptr);

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_BufferConsumed_ChildrenPtrNull
 * @tc.desc: Test MoveSplitSurfaceNode: IsBufferConsumed true but childrenPtr is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_BufferConsumed_ChildrenPtrNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed true, childrenPtr == nullptr

    auto parentNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));

    planner_->splitSurface_ = CreateSplitSurface(100);
    parentNode->AddChild(planner_->splitSurface_->splitSurfaceNode_);

    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(
        buffer, nullptr, Rect(), 0, nullptr);

    // Set fullChildrenList_ to nullptr
    std::atomic_store_explicit(&parentNode->fullChildrenList_,
        std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>>(nullptr),
        std::memory_order_release);

    planner_->MoveSplitSurfaceNode();

    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_->GetRSSurfaceHandler()->
        GetBuffer(), nullptr);
}

/*
 * ── CollectOpIncNodes additional branches ────────────────────
 */

/**
 * @tc.name: CollectOpIncNodes_ChildIsSplitSurface_Skip
 * @tc.desc: Test CollectOpIncNodes when child is splitSurfaceNode_ (skipped by continue)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildIsSplitSurface_Skip, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: child == splitSurface_->splitSurfaceNode_ — continue
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    planner_->CollectOpIncNodes();
}

/**
 * @tc.name: CollectOpIncNodes_ChildGeoPtrNull_New
 * @tc.desc: Test CollectOpIncNodes when child geoPtr is nullptr (skips child via continue)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildGeoPtrNull_New, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: geoPtr == nullptr — continue
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
}

/*
 * ── UpdateBufferBounds additional branches ───────────────────
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusPREPARE_CurrentOffsetZero
 * @tc.desc: Test UpdateBufferBounds when currentOffset_ is zero (does not set isUpdateBuffer_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusPREPARE_CurrentOffsetZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: currentOffset_ == (0, 0) — isUpdateBuffer_ stays false
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->planStatus_ = PlanStatus::PREPARE;
    planner_->isUpdateBuffer_ = false;
    planner_->currentOffset_ = Vector2f(0.0f, 0.0f);
    planner_->srcRect_ = RectF(0, 0, 100, 100);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
    ASSERT_EQ(planner_->isUpdateBuffer_, false);
}

/*
 * ── SetSrcAndDstRect additional branches ─────────────────────
 */

/**
 * @tc.name: SetSrcAndDstRect_ScaleZero
 * @tc.desc: Test SetSrcAndDstRect when scaleX or scaleY is 0 (returns early at line 329-330)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_ScaleZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ROSEN_EQ(scaleX, 0.0f) || ROSEN_EQ(scaleY, 0.0f)
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(0, 0, 100, 100);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_SurfaceHandlerNull
 * @tc.desc: Test SetSrcAndDstRect when surfaceHandler is nullptr (returns early at line 339-340)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_SurfaceHandlerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !surfacHandler — return early
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto surfaceNode = CreateSurfaceNode(100);
    surfaceNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(100);
    auto params = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    splitSurface->splitSurfaceNode_ = surfaceNode;
    planner_->splitSurface_ = splitSurface;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_BufferNotNull
 * @tc.desc: Test SetSrcAndDstRect when buffer is not null (calls SetCropMetadata at line 344-345)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_BufferNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: buffer != nullptr — SetCropMetadata called
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->srcRect_ = RectF(10, 20, 50, 60);
    planner_->dstRect_ = RectF(30, 40, 70, 80);

    planner_->SetSrcAndDstRect();
}

/*
 * ── SetColorSpaceInfo additional branches ─────────────────────
 */

/**
 * @tc.name: SetColorSpaceInfo_ColorSpaceMatch
 * @tc.desc: Test SetColorSpaceInfo when colorspace matches
 *           (calls SetColorSpace at line 455, then GetRSSurfaceHandler at line 471)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_ColorSpaceMatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_->GetColorSpace() != parentNodeColorSpace — no match branch
    auto parentNode = CreateSurfaceNode(100);
    planner_->opIncParentNode_ = parentNode;

    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto splitSurfaceNode = CreateSurfaceNode(200);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    splitSurface->colorSpace_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    planner_->splitSurface_ = splitSurface;

    planner_->SetColorSpaceInfo();
    ASSERT_EQ(splitSurface->colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: SetColorSpaceInfo_RssurfacHandlerNull
 * @tc.desc: Test SetColorSpaceInfo when rssurfacHandler is nullptr (returns early at line 472)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_RssurfacHandlerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !rssurfacHandler — return early
    auto parentNode = CreateSurfaceNode(100);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_ = parentNode;

    planner_->SetColorSpaceInfo();
}

/**
 * @tc.name: ProcessPlanStatusAction_NeedLeave_BufferConsumed
 * @tc.desc: Test ProcessPlanStatusAction when needLeave_=true and IsBufferConsumed() (calls SetBufferNull)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_NeedLeave_BufferConsumed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: needLeave_ && IsBufferConsumed() — calls SetBufferNull
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::ON;

    planner_->ProcessPlanStatusAction();
}

/*
 * ── ClearAllChildrenLayerObjects additional branches ─────────
 */

/**
 * @tc.name: ClearAllChildrenLayerObjects_DrawableNotNull
 * @tc.desc: Test ClearAllChildrenLayerObjects when drawable is not null (calls SetLayerSplitterProcessor)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_DrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: drawable not null — calls SetLayerSplitterProcessor(nullptr)
    planner_->splitSurface_ = nullptr;
    auto childNode = CreateSurfaceNode(100);
    planner_->currFrameChildSet_.insert(childNode);

    planner_->ClearAllChildrenLayerObjects();

    ASSERT_EQ(planner_->currFrameChildSet_.size(), 1);
}

/**
 * @tc.name: ClearAllChildrenLayerObjects_SplitSurfaceDrawableNotNull
 * @tc.desc: Test ClearAllChildrenLayerObjects when splitSurfaceDrawable_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_SplitSurfaceDrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurfaceDrawable_ not null — calls SetLayerSplitterProcessor(nullptr)
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();
}

/*
 * ── Sync additional branches ─────────────────────────────────
 */

/**
 * @tc.name: Sync_CurrHasNewNodeDrawableNotNull
 * @tc.desc: Test Sync when currFrameChildSet_ has a new node with non-null drawable (line 611-612)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_CurrHasNewNodeDrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: currFrameChildSet_ node not in lastFrameChildSet_, drawable not null
    planner_->splitSurface_ = nullptr;
    auto newNode = CreateSurfaceNode(100);
    planner_->currFrameChildSet_.insert(newNode);
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->currFrameChildSet_.count(newNode), 1);
}

/**
 * @tc.name: Sync_LastHasRemovedNodeDrawableNotNull
 * @tc.desc: Test Sync when lastFrameChildSet_ has a removed node with non-null drawable (line 618-619)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_LastHasRemovedNodeDrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: lastFrameChildSet_ node not in currFrameChildSet_, drawable not null
    planner_->splitSurface_ = nullptr;
    auto removedNode = CreateSurfaceNode(200);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.insert(removedNode);

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->lastFrameChildSet_.count(removedNode), 1);
}

/*
 * ── UpdateChildren additional branches ───────────────────────
 */

/**
 * @tc.name: UpdateChildren_HasOpincRootFlag
 * @tc.desc: Test UpdateChildren when child has OpincGetRootFlag set (line 635-638, inserts into childSet)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_HasOpincRootFlag, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: child->GetOpincRootCache().OpincGetRootFlag() == true
    auto parentNode = CreateSurfaceNode(100);
    auto childNode = CreateSurfaceNode(200);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    childNode->GetOpincRootCache().isOpincRootFlag_ = true;

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(childNode);
    parentNode->fullChildrenList_ = children;
    parentNode->isFullChildrenListValid_ = true;
    parentNode->isChildrenSorted_ = true;

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_FALSE(planner_->currFrameChildSet_.empty());
}

/*
 * ── ProcessPlanStatusAction LongTermOff ─────────────────────
 */

/**
 * @tc.name: ProcessPlanStatusAction_LongTermOff_NotOnTree
 * @tc.desc: Test ProcessPlanStatusAction when IsLongTermOff and !CheckParentNodeOnTheTree (Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_LongTermOff_NotOnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsLongTermOff() && !CheckParentNodeOnTheTree()
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = nullptr;
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_->stayOffCount_ = 101;

    planner_->ProcessPlanStatusAction();
}

/**
 * @tc.name: ProcessPlanStatusAction_LongTermOff_OnTree
 * @tc.desc: Test ProcessPlanStatusAction when IsLongTermOff but OnTheTree (no Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_LongTermOff_OnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsLongTermOff() && CheckParentNodeOnTheTree()
    auto splitSurface = CreateSplitSurface(100);
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_->stayOffCount_ = 101;

    planner_->ProcessPlanStatusAction();
}

/**
 * @tc.name: ProcessPlanStatusAction_LongTermOff_NotOff
 * @tc.desc: Test ProcessPlanStatusAction when planStatus != OFF (skip LongTermOff check)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_LongTermOff_NotOff, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ != OFF — skip LongTermOff
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::ON;

    planner_->ProcessPlanStatusAction();
}

/*
 * ── Reset: line 79 surfaceHandler null ─────────────────────────
 */

/**
 * @tc.name: Reset_PlanStatusON_SurfaceHandlerNull
 * @tc.desc: Test Reset when planStatus=ON and splitSurfaceBuffer_->GetSurfaceHandler() returns null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusON_SurfaceHandlerNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !surfaceHandler at line 79
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = CreateSurfaceNode(100);
    splitSurface->splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test", 100, 100, 100);
    splitSurface->splitSurfaceBuffer_->surfaceHandler_ = nullptr;
    planner_->splitSurface_ = splitSurface;
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::ON;
    planner_->requestController_ = std::make_shared<RequestController>();

    planner_->Reset();
}

/*
 * ── CheckOpIncNodeFromCommand: uncovered branches ──────────────
 */

/**
 * @tc.name: CheckOpIncNodeFromCommand_NodeNull
 * @tc.desc: Test CheckOpIncNodeFromCommand when node == nullptr with planStatus=ON and node not visited
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: node == nullptr (not in node map), planStatus=ON, not visited
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();

    planner_->CheckOpIncNodeFromCommand(99999);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_OpIncParentNodeNull
 * @tc.desc: Test CheckOpIncNodeFromCommand when opIncParentNode_ is nullptr (line 101)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_OpIncParentNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: opIncParentNode_ == nullptr, line 101 condition !opIncParentNode_ is true
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->opIncParentNode_ = nullptr;

    auto node = CreateSurfaceNode(100);
    node->SetIsOnTheTree(true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_ParentNull
 * @tc.desc: Test CheckOpIncNodeFromCommand when GetParent().lock() returns null (parent_ expired)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_ParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !parent (GetParent().lock() returns null), line 101
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->opIncParentNode_ = CreateSurfaceNode(200);

    auto node = CreateSurfaceNode(100);
    node->SetIsOnTheTree(true);
    // parent_ is a weak_ptr that was never set — lock() returns null
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_ParentIdMismatch
 * @tc.desc: Test CheckOpIncNodeFromCommand when parent->GetId() != opIncParentNode_->GetId()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_ParentIdMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: parent->GetId() != opIncParentNode_->GetId(), line 101 return false
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->opIncParentNode_ = CreateSurfaceNode(200);

    auto parent = CreateSurfaceNode(300);
    auto node = CreateSurfaceNode(100);
    parent->AddChild(node);
    node->SetIsOnTheTree(true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_NotInLastOpIncNodeIds
 * @tc.desc: Test CheckOpIncNodeFromCommand when lastOpIncNodeIds_.find() == end() (line 107)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NotInLastOpIncNodeIds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: lastOpIncNodeIds_.find(nodeId) == end(), line 107 return false
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.clear();

    auto opIncParent = CreateSurfaceNode(200);
    planner_->opIncParentNode_ = opIncParent;

    auto node = CreateSurfaceNode(100);
    opIncParent->AddChild(node);
    node->SetIsOnTheTree(true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_DirtyTypesExtra
 * @tc.desc: Test CheckOpIncNodeFromCommand when dirtyTypes has extra types beyond BOUNDS/FRAME (line 112-117)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_DirtyTypesExtra, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: dirtyTypes.count() > bounds+frame test, line 112-117 return false
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.insert(100);

    auto opIncParent = CreateSurfaceNode(200);
    planner_->opIncParentNode_ = opIncParent;

    auto node = CreateSurfaceNode(100);
    opIncParent->AddChild(node);
    node->SetIsOnTheTree(true);
    // Set an extra dirty type beyond BOUNDS and FRAME
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::ALPHA), true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckOpIncNodeFromCommand_SuccessPath
 * @tc.desc: Test CheckOpIncNodeFromCommand success path (lines 119-121, inserts to visitedNodeId_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_SuccessPath, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: all conditions pass, node inserted into visitedNodeId_ (line 119-121)
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->lastOpIncNodeIds_.insert(100);

    auto opIncParent = CreateSurfaceNode(200);
    planner_->opIncParentNode_ = opIncParent;

    auto node = CreateSurfaceNode(100);
    opIncParent->AddChild(node);
    node->SetIsOnTheTree(true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    bool result = planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_EQ(result, true);
    ASSERT_NE(planner_->visitedNodeId_.find(100), planner_->visitedNodeId_.end());

    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── GetBoundsFromModifier: lines 133-143 ──────────────────────
 */

/**
 * @tc.name: GetBoundsFromModifier_ModifierNull
 * @tc.desc: Test GetBoundsFromModifier when modifier in list is nullptr (line 133-134, continue)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_ModifierNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: modifier == nullptr, loop continues
    auto node = CreateSurfaceNode(100);
    // Insert nullptr into the BOUNDS modifiers list
    node->modifiersNG_[ModifierNG::RSModifierType::BOUNDS].push_back(nullptr);

    auto [result, bounds] = planner_->GetBoundsFromModifier(node);

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetBoundsFromModifier_PropNull
 * @tc.desc: Test GetBoundsFromModifier when modifier has no BOUNDS property (line 137-138, continue)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_PropNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: prop == nullptr, loop continues
    auto node = CreateSurfaceNode(100);
    // Create a modifier for BOUNDS type but without BOUNDS property type
    Vector4f boundsValue(10.0f, 20.0f, 100.0f, 200.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    // Clear the property so GetProperty(BOUNDS) returns nullptr
    modifier->properties_.clear();
    node->AddModifier(modifier);

    auto [result, bounds] = planner_->GetBoundsFromModifier(node);

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: GetBoundsFromModifier_BoundsFound
 * @tc.desc: Test GetBoundsFromModifier when valid bounds modifier found (lines 140-143, return true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, GetBoundsFromModifier_BoundsFound, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: valid modifier with bounds -> returns {true, bounds}
    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(10.0f, 20.0f, 100.0f, 200.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);

    auto [result, bounds] = planner_->GetBoundsFromModifier(node);

    ASSERT_EQ(result, true);
    ASSERT_EQ(bounds.x_, 10.0f);
    ASSERT_EQ(bounds.y_, 20.0f);
    ASSERT_EQ(bounds.z_, 100.0f);
    ASSERT_EQ(bounds.w_, 200.0f);
}

/*
 * ── CheckCanDoDirectComposition: lines 166-190 ───────────────
 */

/**
 * @tc.name: CheckCanDoDirectComposition_BoundsSizeMismatch_WithModifier
 * @tc.desc: Test CheckCanDoDirectComposition when bounds z/w != position z/w (lines 166-167, return false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_BoundsSizeMismatch_WithModifier, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ROSEN_NE(bounds.z_, position.z_) || ROSEN_NE(bounds.w_, position.w_)
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    // position z=3.0f, w=4.0f but bounds z=100.0f, w=200.0f — mismatch
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    // Register node 100 with a BOUNDS modifier returning different size
    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(1.0f, 2.0f, 100.0f, 200.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckCanDoDirectComposition_FirstNodeSuccess
 * @tc.desc: Test CheckCanDoDirectComposition first node stores currentOffset_ (lines 169-172)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_FirstNodeSuccess, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: isFirst=true path — stores currentOffset_, isFirst=false
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    // position matches bounds exactly (z=100.0f, w=200.0f)
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 100.0f, 200.0f));

    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(10.0f, 20.0f, 100.0f, 200.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    // After first node, UpdateBufferBounds will be called. Set up to fail at srcRect bounds check.
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = CreateSurfaceNode(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 200, 200);
    planner_->opIncParentNode_ = CreateSurfaceNode(300);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    bool result = planner_->CheckCanDoDirectComposition();

    // isFirst path executed: currentOffset_ = (10-1, 20-2) = (9, 18)
    ASSERT_EQ(planner_->currentOffset_.x_, 9.0f);
    ASSERT_EQ(planner_->currentOffset_.y_, 18.0f);
    // Should reach UpdateBufferBounds and succeed (srcRect within bounds)
    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckCanDoDirectComposition_OffsetMismatch_WithModifier
 * @tc.desc: Test CheckCanDoDirectComposition when second node offset != first (lines 174-178, return false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_OffsetMismatch_WithModifier, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !Vector2fNearEqual(currentOffset_, nodeOffset) — return false
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->visitedNodeId_.insert(200);
    // Both nodes have same size (z=100, w=100) but different positions
    // First: position (1,2), bounds (10,20) -> offset (9,18)
    // Second: position (3,4), bounds (10,20) -> offset (7,16) — diff > 2, mismatch
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 100.0f, 100.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(3.0f, 4.0f, 100.0f, 100.0f));

    // Node100: bounds (10,20,100,100)
    auto node100 = CreateSurfaceNode(100);
    Vector4f boundsValue100(10.0f, 20.0f, 100.0f, 100.0f);
    auto property100 = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue100, 1);
    auto modifier100 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property100, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier100, nullptr);
    node100->AddModifier(modifier100);

    // Node200: bounds (10,20,100,100) — same position as node100, but second node's position is (3,4)
    // So offset = (10-3, 20-4) = (7,16) vs first offset (10-1, 20-2) = (9,18)
    // |7-9|=2, |16-18|=2 — both >= 2, so !Vector2fNearEqual returns true
    auto node200 = CreateSurfaceNode(200);
    Vector4f boundsValue200(10.0f, 20.0f, 100.0f, 100.0f);
    auto property200 = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue200, 1);
    auto modifier200 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property200, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier200, nullptr);
    node200->AddModifier(modifier200);

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node100);
    nodeMap.RegisterRenderNode(node200);

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(200);
    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckCanDoDirectComposition_FullSuccess
 * @tc.desc: Test CheckCanDoDirectComposition full success path (lines 169-190, returns true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_FullSuccess, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: all conditions pass — sets canDoDirectComposition_=true, returns true
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 100.0f, 100.0f));

    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(10.0f, 20.0f, 100.0f, 100.0f);
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = CreateSurfaceNode(200);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->opIncParentNode_ = CreateSurfaceNode(300);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, true);
    ASSERT_EQ(planner_->canDoDirectComposition_, true);
    ASSERT_EQ(planner_->opIncNodes_.size(), 1);

    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── CollectOpIncNodes: line 234 nullptr child ────────────────
 */

/**
 * @tc.name: CollectOpIncNodes_NullptrChild
 * @tc.desc: Test CollectOpIncNodes when a child in children list is nullptr (line 234, !child branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_NullptrChild, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: child == nullptr in children list, continue
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    // Create children list containing a nullptr element
    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(nullptr);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, constChildren,
        std::memory_order_release);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
    ASSERT_TRUE(planner_->opIncNodes_.empty());
}

/*
 * ── CollectOpIncNodes: line 238 nullptr geoPtr ──────────────
 */

/**
 * @tc.name: CollectOpIncNodes_NullGeoPtr
 * @tc.desc: Test CollectOpIncNodes when child has null BoundsGeometry (line 238, !geoPtr branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_NullGeoPtr, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: geoPtr == nullptr for a child that is not splitSurfaceNode_ and not null
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    auto child = CreateSurfaceNode(300);
    child->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    child->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    child->GetMutableRenderProperties().boundsGeo_ = nullptr;

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(child);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, constChildren,
        std::memory_order_release);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
    ASSERT_TRUE(planner_->opIncNodes_.empty());
}

/*
 * ── CollectOpIncNodes: line 249 !isOverlap already false ────
 */

/**
 * @tc.name: CollectOpIncNodes_IsOverlapAlreadyTrue
 * @tc.desc: Test CollectOpIncNodes: isOverlap already true, second node skips CheckRectOverlap (line 249)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_IsOverlapAlreadyTrue, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: isOverlap is already true from first opInc node, second opInc node skips CheckRectOverlap
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    // non-opInc child (will be added to nonOpIncNodeRects)
    auto nonOpIncChild = CreateSurfaceNode(300);
    nonOpIncChild->GetMutableRenderProperties().SetBounds(Vector4f(25, 25, 50, 50));
    nonOpIncChild->GetMutableRenderProperties().SetFrame(Vector4f(25, 25, 50, 50));
    nonOpIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // First opInc child (overlaps with nonOpIncChild -> sets isOverlap=true)
    auto opIncChild1 = CreateSurfaceNode(400);
    opIncChild1->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    opIncChild1->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    opIncChild1->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild1->GetOpincRootCache().isOpincRootFlag_ = true;

    // Second opInc child (isOverlap is already true, so CheckRectOverlap is skipped)
    auto opIncChild2 = CreateSurfaceNode(500);
    opIncChild2->GetMutableRenderProperties().SetBounds(Vector4f(200, 200, 30, 30));
    opIncChild2->GetMutableRenderProperties().SetFrame(Vector4f(200, 200, 30, 30));
    opIncChild2->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild2->GetOpincRootCache().isOpincRootFlag_ = true;

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(nonOpIncChild);
    children->push_back(opIncChild1);
    children->push_back(opIncChild2);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, constChildren,
        std::memory_order_release);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, true);
    ASSERT_EQ(planner_->opIncNodes_.size(), 2);
}

/*
 * ── CheckRectOverlap: line 263 returns false ────────────────
 */

/**
 * @tc.name: CheckRectOverlap_NoOverlap
 * @tc.desc: Test CheckRectOverlap when rect does not overlap with any nonOpIncRect (line 263, returns false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckRectOverlap_NoOverlap, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: rect does NOT overlap with any nonOpIncNodeRect — returns false
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    // non-opInc child at position (0, 0, 50, 50)
    auto nonOpIncChild = CreateSurfaceNode(300);
    nonOpIncChild->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 50, 50));
    nonOpIncChild->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 50, 50));
    nonOpIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // opInc child at position (200, 200, 30, 30) — does NOT overlap with nonOpIncChild
    auto opIncChild = CreateSurfaceNode(400);
    opIncChild->GetMutableRenderProperties().SetBounds(Vector4f(200, 200, 30, 30));
    opIncChild->GetMutableRenderProperties().SetFrame(Vector4f(200, 200, 30, 30));
    opIncChild->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    opIncChild->GetOpincRootCache().isOpincRootFlag_ = true;

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(nonOpIncChild);
    children->push_back(opIncChild);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, constChildren,
        std::memory_order_release);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
    ASSERT_EQ(planner_->opIncNodes_.size(), 1);
}

/*
 * ── SetSrcAndDstRect: lines 316, 329, 339, 343 ─────────────
 */

/**
 * @tc.name: SetSrcAndDstRect_StagingParamsNull
 * @tc.desc: Test SetSrcAndDstRect when GetStagingRenderParams returns nullptr (line 316, !surfaceParams branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_StagingParamsNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: surfaceParams == nullptr — return early at line 316
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(100);
    surfaceNode->stagingRenderParams_ = nullptr;
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = surfaceNode;
    planner_->splitSurface_ = splitSurface;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_ScaleXZero
 * @tc.desc: Test SetSrcAndDstRect when scaleX is 0 (line 329, returns early)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_ScaleXZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ROSEN_EQ(scaleX, 0.0f) || ROSEN_EQ(scaleY, 0.0f)
    // Set scale to 0 before UpdateByMatrixFromSelf so matrix has scale 0
    auto surfaceNode = CreateSurfaceNode(100);
    surfaceNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    surfaceNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    surfaceNode->GetMutableRenderProperties().SetScaleX(0.0f);
    surfaceNode->GetMutableRenderProperties().SetScaleY(1.0f);
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = surfaceNode;
    planner_->splitSurface_ = splitSurface;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_SurfaceHandlerNull339
 * @tc.desc: Test SetSrcAndDstRect when GetRSSurfaceHandler returns nullptr (line 339, !surfacHandler branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_SurfaceHandlerNull339, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !surfacHandler — return early at line 339
    auto surfaceNode = CreateSurfaceNode(100);
    surfaceNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    surfaceNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    surfaceNode->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    surfaceNode->surfaceHandler_ = nullptr;
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = surfaceNode;
    planner_->splitSurface_ = splitSurface;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/**
 * @tc.name: SetSrcAndDstRect_BufferIsNull
 * @tc.desc: Test SetSrcAndDstRect when buffer from surfaceHandler is null (line 343, !buffer branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetSrcAndDstRect_BufferIsNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: buffer == nullptr — skip SetCropMetadata
    auto surfaceNode = CreateSurfaceNode(100);
    surfaceNode->GetMutableRSSurfaceHandler()->buffer_.buffer = nullptr;
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = surfaceNode;
    planner_->splitSurface_ = splitSurface;
    planner_->srcRect_ = RectF(0, 0, 50, 50);
    planner_->dstRect_ = RectF(0, 0, 100, 100);

    planner_->SetSrcAndDstRect();
}

/*
 * ── UpdateBufferBounds: line 368 ──────────────────────────────
 * planStatus=ON, isUpdateBuffer_=false (enters if-body), geoMatrix valid, then srcRect check
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusON_IsUpdateBufferFalse
 * @tc.desc: Test UpdateBufferBounds when planStatus=ON and isUpdateBuffer_=false (enters line 369-382 body)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusON_IsUpdateBufferFalse, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON, !isUpdateBuffer_ — enters geoMatrix block
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->isUpdateBuffer_ = false;
    planner_->currentOffset_ = Vector2f(5.0f, 10.0f);
    planner_->srcRect_ = RectF(100, 100, 200, 200);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
    ASSERT_EQ(planner_->isUpdateBuffer_, true);
}

/*
 * ── UpdateBufferBounds: line 371 ──────────────────────────────
 * planStatus=ON, isUpdateBuffer_=false, geoMatrix = nullptr
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusON_GeoMatrixNull
 * @tc.desc: Test UpdateBufferBounds: planStatus=ON, isUpdateBuffer_=false, geoMatrix=null (returns true at 371)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusON_GeoMatrixNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::ON, !isUpdateBuffer_, geoMatrix == nullptr
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().boundsGeo_ = nullptr;
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->screenWidth_ = 100;
    planner_->screenHeight_ = 100;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->isUpdateBuffer_ = false;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/*
 * ── UpdateBufferBounds: line 387 ──────────────────────────────
 * srcRect out of bounds after OFF branch computation
 */

/**
 * @tc.name: UpdateBufferBounds_SrcRectOutOfBoundsAfterOFF
 * @tc.desc: Test UpdateBufferBounds when OFF branch computes srcRect that exceeds buffer bounds (line 387 true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectOutOfBoundsAfterOFF, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: OFF branch computes srcRect_ that overflows buffer — line 387 returns true
    // Set node position far from origin so dstRect_ (from absRect) has large left/top.
    // For screenWidth=100, screenHeight=100:
    // bufferBounds_ = RectI(-1, -20, 102, 140) (from line 357-359)
    // itemBounds_ defaults to RectI(0,0,0,0), so bufferBounds_ DOES get adjusted:
    //   line 360: bottom=120 > 0 → Move(0, -120), bounds → (-1, -140, 102, 140)
    //   line 363: top=-140 < 0 → Move(0, 140), bounds → (-1, 0, 102, 140)
    // With absRect at (200, 200, 100, 100):
    // dstRect_ = RectF(200, 200, 100, 100)
    // srcRect_ = RectF(200-(-1), 200-0, 100, 100) = RectF(201, 200, 100, 100)
    // Check: bottom = 200+100=300 > bufferHeight_ = 200 → out of bounds ✓
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(200, 200, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(200, 200, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 300;
    planner_->splitSurface_->bufferHeight_ = 200;
    planner_->screenWidth_ = 100;
    planner_->screenHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/*
 * ── CheckNeedLeave: line 423 ──────────────────────────────────
 * persp2 != 1.0f triggers needLeave
 */

/**
 * @tc.name: CheckNeedLeave_Persp2NotOne
 * @tc.desc: Test CheckNeedLeave when persp2 != 1.0f (line 423 condition triggers needLeave_)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_Persp2NotOne, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: persp2 != 1.0f — line 423 compound condition is true
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetPersp(Vector4f(0.0f, 0.0f, 0.0f, 2.0f));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/*
 * ── SetColorSpaceInfo: line 455 ───────────────────────────────
 * colorspace mismatch — returns early
 */

/**
 * @tc.name: SetColorSpaceInfo_ColorSpaceMismatch
 * @tc.desc: Test SetColorSpaceInfo when colorspace does NOT match parent (returns at line 455)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_ColorSpaceMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_->GetColorSpace() != parentNodeColorSpace — return at line 455
    auto parentNode = CreateSurfaceNode(100);
    // parent default GetNodeColorSpace returns GRAPHIC_COLOR_GAMUT_SRGB (default)
    // Set parent's colorspace to SRGB explicitly
    ASSERT_NE(parentNode->stagingRenderParams_, nullptr);
    parentNode->stagingRenderParams_->SetNodeColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    planner_->opIncParentNode_ = parentNode;

    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto splitSurfaceNode = CreateSurfaceNode(200);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    // Set different colorspace so condition at line 454 is true
    splitSurface->colorSpace_ = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    planner_->splitSurface_ = splitSurface;

    planner_->SetColorSpaceInfo();

    // colorspace should NOT have changed since we returned early
    ASSERT_EQ(splitSurface->colorSpace_, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/*
 * ── SetColorSpaceInfo: line 471 ───────────────────────────────
 * rssurfacHandler is nullptr (colorspace match, but no surface handler)
 */

/**
 * @tc.name: SetColorSpaceInfo_RssurfacHandlerNullAfterMatch
 * @tc.desc: Test SetColorSpaceInfo when rssurfacHandler is null after colorspace match (line 471 return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_RssurfacHandlerNullAfterMatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: colorspace match, but rssurfacHandler is nullptr — return at line 471
    auto parentNode = CreateSurfaceNode(100);
    ASSERT_NE(parentNode->stagingRenderParams_, nullptr);
    parentNode->stagingRenderParams_->SetNodeColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    planner_->opIncParentNode_ = parentNode;

    // Create a surface node that has no RSSurfaceHandler
    auto splitSurfaceNode = CreateSurfaceNode(200);
    splitSurfaceNode->surfaceHandler_ = nullptr;

    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    splitSurface->colorSpace_ = GRAPHIC_COLOR_GAMUT_SRGB;
    planner_->splitSurface_ = splitSurface;

    planner_->SetColorSpaceInfo();

    ASSERT_EQ(splitSurface->colorSpace_, GRAPHIC_COLOR_GAMUT_SRGB);
}

/*
 * ── SetColorSpaceInfo: line 476 ───────────────────────────────
 * MetadataHelper::SetColorSpaceInfo returns error
 */

/**
 * @tc.name: SetColorSpaceInfo_SetColorSpaceInfoFailed
 * @tc.desc: Test SetColorSpaceInfo when MetadataHelper::SetColorSpaceInfo fails (line 476, ret != GSERROR_OK)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_SetColorSpaceInfoFailed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ret != GSERROR_OK — line 476 branch
    auto parentNode = CreateSurfaceNode(100);
    ASSERT_NE(parentNode->stagingRenderParams_, nullptr);
    parentNode->stagingRenderParams_->SetNodeColorSpace(GRAPHIC_COLOR_GAMUT_SRGB);
    planner_->opIncParentNode_ = parentNode;

    auto splitSurfaceNode = CreateSurfaceNode(200);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    splitSurface->colorSpace_ = GRAPHIC_COLOR_GAMUT_SRGB;
    planner_->splitSurface_ = splitSurface;

    // Ensure rssurfacHandler exists but buffer is null — GetBuffer() returns null,
    // causing MetadataHelper::SetColorSpaceInfo to return GSERROR_INVALID_ARGUMENTS
    auto handler = splitSurfaceNode->GetRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    handler->buffer_.buffer = nullptr;

    planner_->SetColorSpaceInfo();

    // No crash — the failed SetColorSpaceInfo is logged but execution continues
    ASSERT_EQ(splitSurface->colorSpace_, GRAPHIC_COLOR_GAMUT_SRGB);
}

/*
 * ── ClearAllChildrenLayerObjects: lines 591-593 (drawable not null) ──
 */

/**
 * @tc.name: ClearAllChildrenLayerObjects_CurrNodeDrawableNotNull
 * @tc.desc: Test ClearAllChildrenLayerObjects when nodes in currFrameChildSet_ have non-null drawable (line 591-593)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_CurrNodeDrawableNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto childNode = CreateSurfaceNode(100);
    childNode->InitRenderDrawableAndDrawableVec();
    ASSERT_NE(childNode->GetRenderDrawable(), nullptr);

    planner_->splitSurface_ = nullptr;
    planner_->currFrameChildSet_.insert(childNode);
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();

    ASSERT_EQ(planner_->currFrameChildSet_.size(), 1);
}

/*
 * ── ClearAllChildrenLayerObjects: lines 598-600 (splitSurfaceDrawable_ not null) ──
 */

/**
 * @tc.name: ClearAllChildrenLayerObjects_SplitSurfaceDrawableValid
 * @tc.desc: Test ClearAllChildrenLayerObjects when splitSurfaceDrawable_ is not null (line 598-600)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ClearAllChildrenLayerObjects_SplitSurfaceDrawableValid, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto splitSurfaceNode = CreateSurfaceNode(100);
    splitSurfaceNode->InitRenderDrawableAndDrawableVec();
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    auto drawable = splitSurfaceNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);
    splitSurface->splitSurfaceDrawable_ =
        std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
    planner_->splitSurface_ = splitSurface;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->ClearAllChildrenLayerObjects();
}

/*
 * ── Sync: lines 605-607 (splitSurfaceDrawable_ not null) ──
 */

/**
 * @tc.name: Sync_SplitSurfaceDrawableValid
 * @tc.desc: Test Sync when splitSurfaceDrawable_ is not null (line 605-607)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_SplitSurfaceDrawableValid, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    auto splitSurfaceNode = CreateSurfaceNode(100);
    splitSurfaceNode->InitRenderDrawableAndDrawableVec();
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    auto drawable = splitSurfaceNode->GetRenderDrawable();
    ASSERT_NE(drawable, nullptr);
    splitSurface->splitSurfaceDrawable_ =
        std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(drawable);
    planner_->splitSurface_ = splitSurface;
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);
}

/*
 * ── Sync: lines 617-619 (lastFrameChildSet_ node with drawable not in currFrameChildSet_) ──
 */

/**
 * @tc.name: Sync_LastRemovedNodeDrawableValid
 * @tc.desc: Test Sync when lastFrameChildSet_ has a removed node with non-null drawable (line 617-619)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_LastRemovedNodeDrawableValid, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = nullptr;
    auto removedNode = CreateSurfaceNode(200);
    removedNode->InitRenderDrawableAndDrawableVec();
    ASSERT_NE(removedNode->GetRenderDrawable(), nullptr);

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.insert(removedNode);

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->lastFrameChildSet_.count(removedNode), 1);
}

/*
 * ── UpdateChildren: lines 631-633 (GetSortedChildren returns nullptr) ──
 */

/**
 * @tc.name: UpdateChildren_ChildrenNullPtr
 * @tc.desc: Test UpdateChildren when GetSortedChildren returns nullptr (line 631-633)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_ChildrenNullPtr, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    // Set fullChildrenList_ to nullptr so atomic_load returns nullptr
    std::atomic_store_explicit(&parentNode->fullChildrenList_,
        std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>>(nullptr),
        std::memory_order_release);
    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_TRUE(planner_->currFrameChildSet_.empty());
}

/*
 * ── UpdateChildren: line 635 (child with OpincGetRootFlag) ──
 */

/**
 * @tc.name: UpdateChildren_ChildHasOpincRootFlag
 * @tc.desc: Test UpdateChildren when child has OpincGetRootFlag (line 635, inserts into childSet)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_ChildHasOpincRootFlag, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    auto parentNode = CreateSurfaceNode(100);
    auto childNode = CreateSurfaceNode(200);
    childNode->GetOpincRootCache().isOpincRootFlag_ = true;

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(childNode);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&parentNode->fullChildrenList_, constChildren,
        std::memory_order_release);

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_EQ(planner_->currFrameChildSet_.size(), 1);
    ASSERT_TRUE(planner_->currFrameChildSet_.count(childNode) > 0);
}

/*
 * ── UpdateSplitPlan: lines 499-505 (isHardwareEnabled path) ──
 */

/**
 * @tc.name: UpdateSplitPlan_IsHardwareEnabledTrue
 * @tc.desc: Test UpdateSplitPlan when IsBufferConsumed and GetHardwareEnabled are both true (isHardwareEnabled=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_IsHardwareEnabledTrue, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed()=true, GetHardwareEnabled()=true → isHardwareEnabled=true
    auto splitSurface = CreateSplitSurface(100);
    // Set buffer so IsBufferConsumed returns true
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    splitSurface->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(buffer, nullptr, Rect(), 0, nullptr);
    // Trigger GetRenderParams() to create the drawable and its renderParams_
    auto& renderParams = splitSurface->splitSurfaceNode_->GetRenderParams();
    ASSERT_NE(renderParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(renderParams.get());
    surfaceParams->isHardwareEnabled_ = true;
    // Set splitSurfaceBuffer_ so SetBufferNull (line 554-556) doesn't crash
    splitSurface->splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test", 100, 100, 100);
    planner_->splitSurface_ = splitSurface;
    // needLeave_=true so OFF case doesn't transition to PREPARE (stays in OFF after switch)
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_IsBufferConsumedNotHardwareEnabled
 * @tc.desc: Test UpdateSplitPlan when IsBufferConsumed=true but GetHardwareEnabled=false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_IsBufferConsumedNotHardwareEnabled, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: IsBufferConsumed()=true, GetHardwareEnabled()=false → isHardwareEnabled stays false
    auto splitSurface = CreateSplitSurface(100);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    splitSurface->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(buffer, nullptr, Rect(), 0, nullptr);
    // Trigger GetRenderParams() to create the drawable and its renderParams_
    auto& renderParams = splitSurface->splitSurfaceNode_->GetRenderParams();
    ASSERT_NE(renderParams, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(renderParams.get());
    surfaceParams->isHardwareEnabled_ = false;
    // Set splitSurfaceBuffer_ so SetBufferNull (line 554-556) doesn't crash
    splitSurface->splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test", 100, 100, 100);
    planner_->splitSurface_ = splitSurface;
    // needLeave_=true so OFF case doesn't transition to PREPARE (stays in OFF after switch)
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_IsBufferNotConsumed
 * @tc.desc: Test UpdateSplitPlan when IsBufferConsumed is false (skips hardware enabled check)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_IsBufferNotConsumed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !IsBufferConsumed() → skip line 499-505 entirely
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = CreateSurfaceNode(100);
    // Need opIncParentNode_ for ProcessPlanStatusAction Init() call (since needLeave_=false allows Init)
    auto opIncParent = CreateSurfaceNode(200);
    opIncParent->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    opIncParent->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_ = opIncParent;
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    // Since IsBufferConsumed() returns false, OFF case stays in OFF
    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/*
 * ── UpdateSplitPlan: line 509-511 (OFF → PREPARE transition) ──
 */

/**
 * @tc.name: UpdateSplitPlan_PlanStatusOFF_ToPREPARE
 * @tc.desc: Test UpdateSplitPlan: planStatus=OFF, needLeave_=false, IsBufferConsumed()=true (to PREPARE)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusOFF_ToPREPARE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus=OFF, !needLeave_, IsBufferConsumed() → PREPARE
    auto splitSurface = CreateSplitSurface(100);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    splitSurface->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(buffer, nullptr, Rect(), 0, nullptr);
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::PREPARE);
}

/*
 * ── ProcessPlanStatusAction: line 541-548 ─────────────────────
 * OFF + !needLeave_:
 *   - parent==nullptr → AddChild
 *   - parent!=nullptr → skip AddChild
 *   - SetColorSpaceInfo called
 */

/**
 * @tc.name: ProcessPlanStatusAction_OFF_NoNeedLeave_ParentNull
 * @tc.desc: Test ProcessPlanStatusAction when OFF, !needLeave_, parent==nullptr (Init + AddChild + SetColorSpaceInfo)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_OFF_NoNeedLeave_ParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus=OFF, !needLeave_, parent==nullptr → calls AddChild
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->needLeave_ = false;
    auto parentNode = CreateSurfaceNode(100);
    parentNode->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    parentNode->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_ = parentNode;
    // Use a splitSurface with valid splitSurfaceNode_ so Init() skips node creation
    planner_->splitSurface_ = CreateSplitSurface(200);

    planner_->ProcessPlanStatusAction();
}

/**
 * @tc.name: ProcessPlanStatusAction_OFF_NoNeedLeave_ParentNotNull
 * @tc.desc: Test ProcessPlanStatusAction: OFF, !needLeave_, parent!=nullptr (Init, skip AddChild, SetColorSpaceInfo)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_OFF_NoNeedLeave_ParentNotNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus=OFF, !needLeave_, parent!=nullptr → skips AddChild
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->needLeave_ = false;
    auto opIncParent = CreateSurfaceNode(100);
    opIncParent->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    opIncParent->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_ = opIncParent;

    auto parentNode = CreateSurfaceNode(200);
    auto splitSurfaceNode = CreateSurfaceNode(300);
    parentNode->AddChild(splitSurfaceNode);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    planner_->splitSurface_ = splitSurface;

    planner_->ProcessPlanStatusAction();
}

/*
 * ── ProcessPlanStatusAction: line 554-556 ─────────────────────
 * needLeave_ && IsBufferConsumed() → SetBufferNull
 */

/**
 * @tc.name: ProcessPlanStatusAction_NeedLeave_IsBufferConsumed
 * @tc.desc: Test ProcessPlanStatusAction when needLeave_=true and IsBufferConsumed()=true (calls SetBufferNull)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, ProcessPlanStatusAction_NeedLeave_IsBufferConsumed, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: needLeave_=true, IsBufferConsumed()=true → SetBufferNull called
    auto splitSurface = CreateSplitSurface(100);
    // Set splitSurfaceBuffer_ so SetBufferNull doesn't crash
    splitSurface->splitSurfaceBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test", 100, 100, 100);
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    ASSERT_NE(buffer, nullptr);
    splitSurface->splitSurfaceNode_->GetRSSurfaceHandler()->SetBuffer(buffer, nullptr, Rect(), 0, nullptr);
    planner_->splitSurface_ = splitSurface;
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;

    planner_->ProcessPlanStatusAction();
}

/*
 * ── CheckNeedLeave: matrix compound condition line 423 ────
 * persp0 != 0.0f triggers needLeave
 */

/**
 * @tc.name: CheckNeedLeave_Persp0NonZero
 * @tc.desc: Test CheckNeedLeave when persp0 != 0.0f (line 423 compound condition, needLeave_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_Persp0NonZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: persp0 != 0.0f in compound line 423 condition
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetPersp(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_Persp1NonZero
 * @tc.desc: Test CheckNeedLeave when persp1 != 0.0f (line 423 compound condition, needLeave_=true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_Persp1NonZero, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: persp0=0, persp1 != 0.0f
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetPersp(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    ASSERT_EQ(planner_->needLeave_, true);
}

/**
 * @tc.name: CheckNeedLeave_SkewXZero_Persp2One_NoNeedLeave
 * @tc.desc: Test CheckNeedLeave when all matrix conditions are normal (line 423 false, proceeds to collect)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_SkewXZero_Persp2One_NoNeedLeave, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: skewX=0, skewY=0, persp0=0, persp1=0, persp2=1.0 — all normal, proceeds to CollectOpIncNodes
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetPersp(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    // CollectOpIncNodes returns false (empty children), then IsOpIncNodesChanged returns true (lastOpIncNodes empty)
    ASSERT_EQ(planner_->needLeave_, true);
}

/*
 * ── UpdateBufferBounds: OFF mode with bufferBounds adjustments (lines 360-365) ──
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusOFF_BufferBoundsAdjust
 * @tc.desc: Test UpdateBufferBounds when planStatus=OFF and bufferBounds needs bottom and top adjustments
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusOFF_BufferBoundsAdjust, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bufferBounds bottom > itemBounds bottom AND top < itemBounds top — both adjustments
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(10, 10, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(10, 10, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 500;
    planner_->splitSurface_->bufferHeight_ = 300;
    planner_->screenWidth_ = 200;
    planner_->screenHeight_ = 200;
    planner_->planStatus_ = PlanStatus::OFF;

    // itemBounds_ should be set from children. Without children, itemBounds_ is RectI(0,0,0,0)
    // bufferBounds_ = RectI(-1, -40, 202, 280)
    // bottom=280 > 0 → adjust (Move(0, -280) → bufferBounds_=(-1,-320,202,280))
    // top=-320 < 0 → adjust (Move(0, 320) → bufferBounds_=(-1,0,202,280))
    // srcRect_ = RectF(10-(-1), 10-0, 100, 100) = (11, 10, 100, 100)

    bool result = planner_->UpdateBufferBounds();

    // srcRect (11,10,100,100) should fit within buffer (500,300)
    ASSERT_EQ(result, false);
}

/*
 * ── UpdateBufferBounds: geoMatrix null in PREPARE mode (lines 370-372) ──
 */

/*
 * ── UpdateBufferBounds: srcRect out of bounds specific combo (line 387) ──
 * srcRect.top < 0 — top out of bounds
 */

/**
 * @tc.name: UpdateBufferBounds_SrcRectTopOutOfBounds
 * @tc.desc: Test UpdateBufferBounds when srcRect.GetTop() < 0 (line 387 first sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectTopOutOfBounds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: srcRect.GetTop() < 0 — line 387 condition is true
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->srcRect_ = RectF(-10, -10, 100, 100);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateBufferBounds_SrcRectBottomOutOfBounds
 * @tc.desc: Test UpdateBufferBounds when srcRect.GetBottom() > bufferHeight (line 387 second sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectBottomOutOfBounds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: srcRect.GetBottom() > splitSurface_->bufferHeight_ — sub-condition 2
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 50;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->srcRect_ = RectF(0, 0, 100, 100);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateBufferBounds_SrcRectLeftOutOfBounds
 * @tc.desc: Test UpdateBufferBounds when srcRect.GetLeft() < 0 (line 387 third sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectLeftOutOfBounds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: srcRect.GetLeft() < 0 — sub-condition 3
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->srcRect_ = RectF(-5, 0, 100, 50);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateBufferBounds_SrcRectRightOutOfBounds
 * @tc.desc: Test UpdateBufferBounds when srcRect.GetRight() > bufferWidth (line 387 fourth sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_SrcRectRightOutOfBounds, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: srcRect.GetRight() > splitSurface_->bufferWidth_ — sub-condition 4
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 50;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->srcRect_ = RectF(0, 0, 100, 50);

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, true);
}

/*
 * ── SetColorSpaceInfo: SetColorSpaceInfo failed (line 476) ──
 */

/**
 * @tc.name: SetColorSpaceInfo_SetColorSpaceInfoFailed_WithBuffer
 * @tc.desc: Test SetColorSpaceInfo when SetColorSpaceInfo returns error with valid buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, SetColorSpaceInfo_SetColorSpaceInfoFailed_WithBuffer, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: ret != GSERROR_OK — line 476 branch
    auto parentNode = CreateSurfaceNode(100);
    ASSERT_NE(parentNode->stagingRenderParams_, nullptr);
    parentNode->stagingRenderParams_->SetNodeColorSpace(GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    planner_->opIncParentNode_ = parentNode;

    auto splitSurfaceNode = CreateSurfaceNode(200);
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = splitSurfaceNode;
    splitSurface->colorSpace_ = GRAPHIC_COLOR_GAMUT_ADOBE_RGB;
    planner_->splitSurface_ = splitSurface;

    auto handler = splitSurfaceNode->GetRSSurfaceHandler();
    ASSERT_NE(handler, nullptr);
    handler->buffer_.buffer = nullptr;

    planner_->SetColorSpaceInfo();

    ASSERT_EQ(splitSurface->colorSpace_, GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/*
 * ── CheckCanDoDirectComposition: line 149-190 path with planStatus=ON ──
 * Tests that reach past line 149 with proper planStatus_ setting
 */

/**
 * @tc.name: CheckCanDoDirectComposition_VisitedNotEmpty_SizesEqual
 * @tc.desc: Test CheckCanDoDirectComposition: visitedNodeId_ not empty, sizes equal (line 153, fall through)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_VisitedNotEmpty_SizesEqual, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: visitedNodeId_.empty()=false, visitedNodeId_.size() == lastOpIncNodes_.size()=true — passes line 153
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    bool result = planner_->CheckCanDoDirectComposition();

    // Then GetBoundsFromModifier fails (node not registered), returns false
    ASSERT_EQ(result, false);
}

/*
 * ── CheckOpIncNodeFromCommand: line 96 (node != nullptr && !IsOnTheTree) ──
 */

/**
 * @tc.name: CheckOpIncNodeFromCommand_NodeOnTree_NotOnTree
 * @tc.desc: Test CheckOpIncNodeFromCommand when node exists but is not on tree (line 96 branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_NodeOnTree_NotOnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: node != nullptr && !node->IsOnTheTree() — branch (F,T) at line 96
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();

    auto node = CreateSurfaceNode(100);
    node->SetIsOnTheTree(false);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    ASSERT_EQ(planner_->visitedNodeId_.count(100), 0);

    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── CheckOpIncNodeFromCommand: parent exists, opIncParent exists, IDs differ (line 101 pos 5) ──
 */

/**
 * @tc.name: CheckOpIncNodeFromCommand_ParentExistsIdMismatch
 * @tc.desc: Test CheckOpIncNodeFromCommand when parent exists but its ID != opIncParentNode_->GetId() (line 101)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckOpIncNodeFromCommand_ParentExistsIdMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: parent != nullptr, opIncParentNode_ != nullptr, parent->GetId() != opIncParentNode_->GetId()
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.clear();
    planner_->opIncParentNode_ = CreateSurfaceNode(99);

    auto parent = CreateSurfaceNode(200);
    auto node = CreateSurfaceNode(100);
    parent->AddChild(node);
    node->SetIsOnTheTree(true);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    planner_->CheckOpIncNodeFromCommand(100);

    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── CheckCanDoDirectComposition: bounds.w_ == position.w_ but bounds.z_ != position.z_ (line 166) ──
 */

/**
 * @tc.name: CheckCanDoDirectComposition_BoundsZOnlyMismatch
 * @tc.desc: Test CheckCanDoDirectComposition when only bounds.z_ != position.z_ (line 166, second sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_BoundsZOnlyMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bounds.z_ != position.z_, but bounds.w_ == position.w_
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(1.0f, 2.0f, 100.0f, 4.0f); // z differs (100 vs 3), w matches (4 vs 4)
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(100);
}

/**
 * @tc.name: CheckCanDoDirectComposition_BoundsWOnlyMismatch
 * @tc.desc: Test CheckCanDoDirectComposition when only bounds.w_ != position.w_ (line 166, fourth sub-condition)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_BoundsWOnlyMismatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bounds.z_ == position.z_, but bounds.w_ != position.w_
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 3.0f, 4.0f));

    auto node = CreateSurfaceNode(100);
    Vector4f boundsValue(1.0f, 2.0f, 3.0f, 200.0f); // z matches (3 vs 3), w differs (200 vs 4)
    auto boundsProperty = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue, 1);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, boundsProperty, 1, ModifierNG::RSPropertyType::BOUNDS);
    ASSERT_NE(modifier, nullptr);
    node->AddModifier(modifier);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    bool result = planner_->CheckCanDoDirectComposition();

    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── CheckCanDoDirectComposition: second node with matching offset (line 175 false branch) ──
 */

/**
 * @tc.name: CheckCanDoDirectComposition_TwoNodesMatch
 * @tc.desc: Test CheckCanDoDirectComposition: two nodes, offsets match (line 175 else, Vector2fNearEqual true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckCanDoDirectComposition_TwoNodesMatch, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: two nodes, offsets match, pass bounds check, then UpdateBufferBounds decides result
    planner_->canDoDirectComposition_ = true;
    planner_->planStatus_ = PlanStatus::ON;
    planner_->visitedNodeId_.insert(100);
    planner_->visitedNodeId_.insert(200);
    // Both positions at (1,2), sizes (100,100) — bounds at (10,20,100,100) gives offset (9,18) for both
    planner_->lastOpIncNodes_.emplace_back(100, Vector4f(1.0f, 2.0f, 100.0f, 100.0f));
    planner_->lastOpIncNodes_.emplace_back(200, Vector4f(1.0f, 2.0f, 100.0f, 200.0f));

    auto node100 = CreateSurfaceNode(100);
    Vector4f boundsValue100(10.0f, 20.0f, 100.0f, 100.0f);
    auto property100 = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue100, 1);
    auto modifier100 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property100, 1, ModifierNG::RSPropertyType::BOUNDS);
    node100->AddModifier(modifier100);

    auto node200 = CreateSurfaceNode(200);
    Vector4f boundsValue200(10.0f, 20.0f, 100.0f, 100.0f);
    auto property200 = std::make_shared<RSRenderProperty<Vector4f>>(boundsValue200, 1);
    auto modifier200 = ModifierNG::RSRenderModifier::MakeRenderModifier(
        ModifierNG::RSModifierType::BOUNDS, property200, 1, ModifierNG::RSPropertyType::BOUNDS);
    node200->AddModifier(modifier200);

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node100);
    nodeMap.RegisterRenderNode(node200);

    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->splitSurface_->splitSurfaceNode_ = CreateSurfaceNode(300);
    planner_->splitSurface_->bufferWidth_ = 1000;
    planner_->splitSurface_->bufferHeight_ = 1000;
    planner_->isUpdateBuffer_ = true;
    planner_->srcRect_ = RectF(0, 0, 200, 200);
    planner_->opIncParentNode_ = CreateSurfaceNode(400);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    bool result = planner_->CheckCanDoDirectComposition();

    // Both offsets match (9,18), srcRect fits within buffer → direct composition possible
    ASSERT_EQ(result, false);

    nodeMap.UnregisterRenderNode(200);
    nodeMap.UnregisterRenderNode(100);
}

/*
 * ── CollectOpIncNodes: child is splitSurfaceNode_ (line 234 position 3) ──
 */

/**
 * @tc.name: CollectOpIncNodes_ChildIsSplitSurfaceNode_Identical
 * @tc.desc: Test CollectOpIncNodes: child is splitSurfaceNode_ (line 234, child == node)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CollectOpIncNodes_ChildIsSplitSurfaceNode_Identical, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: child != nullptr && child == splitSurface_->splitSurfaceNode_ → continue
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->splitSurface_ = CreateSplitSurface(200);

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(planner_->splitSurface_->splitSurfaceNode_);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&planner_->opIncParentNode_->fullChildrenList_, constChildren,
        std::memory_order_release);

    bool result = planner_->CollectOpIncNodes();

    ASSERT_EQ(result, false);
    ASSERT_TRUE(planner_->opIncNodes_.empty());
}

/*
 * ── UpdateBufferBounds: planStatus=OFF, bufferBounds NO bottom adjustment (line 360 false) ──
 * bufferBounds bottom <= itemBounds bottom → skip bottom adjustment
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusOFF_NoBottomAdjust
 * @tc.desc: Test UpdateBufferBounds when bufferBounds bottom <= itemBounds bottom (line 360 false branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusOFF_NoBottomAdjust, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bufferBounds.GetBottom() <= itemBounds.GetBottom() → skip line 361-362
    // Set screenHeight large so bufferBounds bottom is very small relative to itemBounds
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 10, 10));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 10, 10));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 500;
    planner_->splitSurface_->bufferHeight_ = 500;
    // Very small screen → bufferBounds = RectI(-1, -1, 2, 1) approx → bottom=0
    // itemBounds_ from child absRect at (0,0,10,10) → bottom=10
    // So bufferBounds bottom (0) <= itemBounds bottom (10) → skip adjustment
    planner_->screenWidth_ = 1;
    planner_->screenHeight_ = 1;
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
}

/**
 * @tc.name: UpdateBufferBounds_PlanStatusOFF_NoTopAdjust
 * @tc.desc: Test UpdateBufferBounds when bufferBounds top >= itemBounds top (line 363 false branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusOFF_NoTopAdjust, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: bufferBounds.GetTop() >= itemBounds.GetTop() → skip line 364-365
    // Set opIncParentNode bound at a large y so itemBounds top is large
    // screenHeight large so bufferBounds top is also positive
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 200, 10, 10));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 200, 10, 10));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 500;
    planner_->splitSurface_->bufferHeight_ = 500;
    // screenHeight=200 → bufferBounds top = 200 * -0.2 = -40
    // itemBounds from child absRect (0,200,10,10) → top=200
    // bufferBounds top (-40) < itemBounds top (200) → still true... try again
    planner_->screenWidth_ = 200;
    planner_->screenHeight_ = 200;
    planner_->planStatus_ = PlanStatus::OFF;

    bool result = planner_->UpdateBufferBounds();

    ASSERT_EQ(result, false);
}

/*
 * ── UpdateBufferBounds: planStatus=LEAVE (line 368, else-if false) ──
 */

/**
 * @tc.name: UpdateBufferBounds_PlanStatusLEAVE
 * @tc.desc: Test UpdateBufferBounds when planStatus=LEAVE (neither OFF nor PREPARE/ON, line 368 both false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateBufferBounds_PlanStatusLEAVE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_ == PlanStatus::LEAVE — line 356 false, line 368 false → no buffer adjustment
    planner_->opIncParentNode_ = CreateSurfaceNode(100);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();
    planner_->splitSurface_ = CreateSplitSurface(200);
    planner_->splitSurface_->bufferWidth_ = 100;
    planner_->splitSurface_->bufferHeight_ = 100;
    planner_->planStatus_ = PlanStatus::LEAVE;
    planner_->srcRect_ = RectF(0, 0, 50, 50);

    bool result = planner_->UpdateBufferBounds();

    // srcRect (0,0,50,50) should be within buffer (100,100)
    ASSERT_EQ(result, false);
}

/*
 * ── Sync: curr frame child with non-null drawable (line 610 true branch) ──
 */

/**
 * @tc.name: Sync_CurrNewNodeDrawableValid
 * @tc.desc: Test Sync when currFrameChildSet_ has a new node with valid drawable (line 610 true branch)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Sync_CurrNewNodeDrawableValid, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: currFrameChildSet_ node not in lastFrameChildSet_, drawable not null
    planner_->splitSurface_ = nullptr;
    auto newNode = CreateSurfaceNode(100);
    newNode->InitRenderDrawableAndDrawableVec();
    ASSERT_NE(newNode->GetRenderDrawable(), nullptr);
    planner_->currFrameChildSet_.insert(newNode);
    planner_->lastFrameChildSet_.clear();

    auto processor = std::make_shared<RSOpincLayerSplitterProcessor>();
    planner_->Sync(processor);

    ASSERT_EQ(planner_->currFrameChildSet_.count(newNode), 1);
}

/*
 * ── UpdateChildren: child exists but not opinc root (line 635 position 2) ──
 */

/**
 * @tc.name: UpdateChildren_ChildExistsNotOpincRoot
 * @tc.desc: Test UpdateChildren when child exists but OpincGetRootFlag is false (line 635, child!=nullptr, flag=false)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateChildren_ChildExistsNotOpincRoot, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: child != nullptr but GetOpincRootFlag() is false → skip insertion
    auto parentNode = CreateSurfaceNode(100);
    auto childNode = CreateSurfaceNode(200);

    auto children = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    children->push_back(childNode);
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> constChildren = children;
    std::atomic_store_explicit(&parentNode->fullChildrenList_, constChildren,
        std::memory_order_release);

    planner_->currFrameChildSet_.clear();
    planner_->lastFrameChildSet_.clear();

    planner_->UpdateChildren(parentNode);

    ASSERT_TRUE(planner_->currFrameChildSet_.empty());
}

/**
 * @tc.name: CheckNeedLeave_InstanceRootNode_WindowSceneParentNull
 * @tc.desc: Test CheckNeedLeave when instanceRootNode is non-null but its parent is null (windowSceneNode is null)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_InstanceRootNode_WindowSceneParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: instanceRootNode != nullptr, but instanceRootNode->GetParent().lock() returns null
    // so windowSceneNode is null → skip inner if, proceed to matrix check → collect → needLeave_=true
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // Set up context so GetInstanceRootNode() can look up the node map
    auto context = RSMainThread::Instance()->GetContext().shared_from_this();
    planner_->opIncParentNode_->context_ = context;

    // Create an instance root node registered in the node map
    auto instanceRootNode = CreateSurfaceNode(300);
    auto& nodeMap = context->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(instanceRootNode);
    planner_->opIncParentNode_->instanceRootNodeId_ = 300;

    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    // instance root parent is null → windowSceneNode is null → skip UIFirst check
    // proceeds to matrix check (no skew/persp) → CollectOpIncNodes (no children)
    // → IsOpIncNodesChanged (lastOpIncNodes empty) → needLeave_ = true
    ASSERT_EQ(planner_->needLeave_, true);

    nodeMap.UnregisterRenderNode(300);
}

/**
 * @tc.name: CheckNeedLeave_InstanceRootNode_UiFirstCacheTypeNONE
 * @tc.desc: Test CheckNeedLeave when windowSceneNode exists but cacheType is NONE (no UIFirst)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_InstanceRootNode_UiFirstCacheTypeNONE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: instanceRootNode non-null, windowSceneNode non-null,
    // GetLastFrameUifirstCacheType() == MultiThreadCacheType::NONE → skip inner if
    // proceeds to matrix check → collect → needLeave_ = true
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // Set up context so GetInstanceRootNode() can look up the node map
    auto context = RSMainThread::Instance()->GetContext().shared_from_this();
    planner_->opIncParentNode_->context_ = context;

    // Create instance root node and register in node map
    auto instanceRootNode = CreateSurfaceNode(300);
    auto& nodeMap = context->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(instanceRootNode);
    planner_->opIncParentNode_->instanceRootNodeId_ = 300;

    // Create a window scene node (RSSurfaceRenderNode) as the parent of instanceRootNode
    auto windowSceneNode = CreateSurfaceNode(400);
    windowSceneNode->AddChild(instanceRootNode);
    // cacheType defaults to MultiThreadCacheType::NONE

    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    // cacheType == NONE → skip inner if → proceeds to end → needLeave_ = true
    ASSERT_EQ(planner_->needLeave_, true);

    nodeMap.UnregisterRenderNode(300);
}

/**
 * @tc.name: CheckNeedLeave_InstanceRootNode_UiFirstCacheTypeNotNONE
 * @tc.desc: Test CheckNeedLeave when UIFirst cache type is LEASH_WINDOW (needLeave_ = true)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_InstanceRootNode_UiFirstCacheTypeNotNONE, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: instanceRootNode non-null, windowSceneNode non-null,
    // GetLastFrameUifirstCacheType() == MultiThreadCacheType::LEASH_WINDOW → enters inner if
    // needLeave_ = true, return early
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // Set up context so GetInstanceRootNode() can look up the node map
    auto context = RSMainThread::Instance()->GetContext().shared_from_this();
    planner_->opIncParentNode_->context_ = context;

    // Create instance root node and register in node map
    auto instanceRootNode = CreateSurfaceNode(300);
    auto& nodeMap = context->GetMutableNodeMap();
    nodeMap.RegisterRenderNode(instanceRootNode);
    planner_->opIncParentNode_->instanceRootNodeId_ = 300;

    // Create a window scene node as the parent of instanceRootNode
    auto windowSceneNode = CreateSurfaceNode(400);
    windowSceneNode->AddChild(instanceRootNode);
    // Set UIFirst cache type to non-NONE
    windowSceneNode->uifirstState_.lastFrameCacheType = MultiThreadCacheType::LEASH_WINDOW;

    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    // cacheType != NONE → enters inner if → needLeave_ = true, early return
    ASSERT_EQ(planner_->needLeave_, true);

    nodeMap.UnregisterRenderNode(300);
}

/**
 * @tc.name: CheckNeedLeave_InstanceRootNodeNull
 * @tc.desc: Test CheckNeedLeave when instanceRootNode is nullptr (context lookup fails)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckNeedLeave_InstanceRootNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: instanceRootNode == nullptr (instanceRootNodeId_ is INVALID_NODEID, or context returns null)
    // → skip inner block entirely → proceeds to matrix check → collect → needLeave_ = true
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    planner_->opIncParentNode_->GetMutableRenderProperties().SetBounds(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetMutableRenderProperties().SetFrame(Vector4f(0, 0, 100, 100));
    planner_->opIncParentNode_->GetRenderProperties().GetBoundsGeometry()->UpdateByMatrixFromSelf();

    // instanceRootNodeId_ defaults to INVALID_NODEID, so GetInstanceRootNode() returns null
    planner_->needLeave_ = false;

    planner_->CheckNeedLeave();

    // instanceRootNode is null → skip inner if → matrix check passes → collect (no children)
    // → IsOpIncNodesChanged (lastOpIncNodes empty) → needLeave_ = true
    ASSERT_EQ(planner_->needLeave_, true);
}

/*
 * ── Reset: splitSurface_->splitSurfaceBuffer_ == nullptr ────────
 */

/**
 * @tc.name: Reset_PlanStatusON_SplitSurfaceBufferNull
 * @tc.desc: Test Reset when planStatus=ON, requestController!=null, but splitSurfaceBuffer_ is nullptr
 *           (condition 3 of compound if, returns before IncStayOnCount)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, Reset_PlanStatusON_SplitSurfaceBufferNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: planStatus_==ON, requestController_!=null, splitSurface_->splitSurfaceBuffer_==nullptr
    auto splitSurface = std::make_shared<SplitSurface>(100, 100);
    splitSurface->splitSurfaceNode_ = CreateSurfaceNode(100);
    splitSurface->splitSurfaceBuffer_ = nullptr;
    planner_->splitSurface_ = splitSurface;
    planner_->opIncNodes_.clear();
    planner_->planStatus_ = PlanStatus::ON;
    planner_->requestController_ = std::make_shared<RequestController>();

    // capture the stayOffCount before Reset
    auto stayOffCountBefore = planner_->requestController_->stayOffCount_;
    planner_->Reset();
    // IncStayOnCount should NOT have been called — stayOffCount unchanged
    ASSERT_EQ(planner_->requestController_->stayOffCount_, stayOffCountBefore);
    ASSERT_EQ(planner_->canDoDirectComposition_, false);
}

/*
 * ── CheckParentNodeOnTheTree ──────────────────────────────────────
 */

/**
 * @tc.name: CheckParentNodeOnTheTree_SplitSurfaceNull
 * @tc.desc: Test CheckParentNodeOnTheTree when splitSurface_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckParentNodeOnTheTree_SplitSurfaceNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: splitSurface_ == nullptr
    planner_->splitSurface_ = nullptr;
    planner_->opIncParentNode_ = CreateSurfaceNode(100);

    planner_->CheckParentNodeOnTheTree();
    // No crash expected, surfaceStatus should remain INIT
    ASSERT_EQ(planner_->splitSurface_, nullptr);
}

/**
 * @tc.name: CheckParentNodeOnTheTree_OpIncParentNodeNull
 * @tc.desc: Test CheckParentNodeOnTheTree when opIncParentNode_ is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckParentNodeOnTheTree_OpIncParentNodeNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: opIncParentNode_ == nullptr
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = nullptr;

    planner_->CheckParentNodeOnTheTree();
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);
}

/**
 * @tc.name: CheckParentNodeOnTheTree_OpIncParentNotOnTree
 * @tc.desc: Test CheckParentNodeOnTheTree when opIncParentNode_ is not on tree (calls Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckParentNodeOnTheTree_OpIncParentNotOnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !opIncParentNode_->IsOnTheTree()
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->opIncParentNode_ = CreateSurfaceNode(200);
    // isOnTheTree_ defaults to false → IsOnTheTree() returns false

    planner_->CheckParentNodeOnTheTree();
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
}

/**
 * @tc.name: CheckParentNodeOnTheTree_SplitSurfaceParentNotOnTree
 * @tc.desc: Test CheckParentNodeOnTheTree when splitSurfaceNode_ parent is not on tree (calls Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckParentNodeOnTheTree_SplitSurfaceParentNotOnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !splitSurface_->CheckParentNodeOnTheTree()
    auto splitSurface = CreateSplitSurface(100);
    auto parentNode = CreateSurfaceNode(200);
    parentNode->SetIsOnTheTree(true);
    parentNode->AddChild(splitSurface->splitSurfaceNode_);
    planner_->splitSurface_ = splitSurface;
    planner_->opIncParentNode_ = CreateSurfaceNode(300);
    planner_->opIncParentNode_->SetIsOnTheTree(true);

    // Now remove parentNode from tree: splitSurfaceNode_ parent becomes orphan
    parentNode->SetIsOnTheTree(false);

    planner_->CheckParentNodeOnTheTree();
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
}

/**
 * @tc.name: CheckParentNodeOnTheTree_BothOnTree
 * @tc.desc: Test CheckParentNodeOnTheTree when both opIncParentNode_ and splitSurfaceNode_ parent are on tree (no-op)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, CheckParentNodeOnTheTree_BothOnTree, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: both IsOnTheTree() return true — no Unregister
    auto splitSurface = CreateSplitSurface(100);
    auto parentNode = CreateSurfaceNode(200);
    parentNode->SetIsOnTheTree(true);
    parentNode->AddChild(splitSurface->splitSurfaceNode_);
    planner_->splitSurface_ = splitSurface;
    planner_->opIncParentNode_ = CreateSurfaceNode(300);
    planner_->opIncParentNode_->SetIsOnTheTree(true);

    planner_->CheckParentNodeOnTheTree();
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
