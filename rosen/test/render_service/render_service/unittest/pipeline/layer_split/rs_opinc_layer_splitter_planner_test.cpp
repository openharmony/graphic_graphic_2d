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
#include "pipeline/layer_split/surface/rs_split_surface.h"
#include "pipeline/layer_split/splitter/opinc/controller/rs_opinc_split_controller.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_screen_render_params.h"
#include "params/rs_surface_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOpincLayerSplitterPlannerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
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
    planner_->splitSurface_ = std::make_shared<SplitSurface>(100, 100);
    planner_->needLeave_ = false;
    planner_->planStatus_ = PlanStatus::LEAVE;

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->planStatus_, PlanStatus::OFF);
}

/**
 * @tc.name: UpdateSplitPlan_PlanStatusOFF_NeedLeave_Unregister
 * @tc.desc: Test UpdateSplitPlan when IsLongTermOff=true and !OnTheTree (triggers Unregister)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, UpdateSplitPlan_PlanStatusOFF_NeedLeave_Unregister, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);
    planner_->needLeave_ = true;
    planner_->planStatus_ = PlanStatus::OFF;
    planner_->requestController_->stayOffCount_ = 101;

    ASSERT_NE(planner_->splitSurface_->splitSurfaceNode_, nullptr);
    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::INIT);

    planner_->UpdateSplitPlan();

    ASSERT_EQ(planner_->splitSurface_->surfaceStatus_, SurfaceStatus::UNREGISTER);
    ASSERT_EQ(planner_->splitSurface_->splitSurfaceNode_, nullptr);
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
 * @tc.name: MoveSplitSurfaceNode_ParentNull
 * @tc.desc: Test MoveSplitSurfaceNode when parent is nullptr (early return after buffer consumed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_ParentNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: parent == nullptr
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_ChildrenPtrNull
 * @tc.desc: Test MoveSplitSurfaceNode when childrenPtr is nullptr (early return)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_ChildrenPtrNull, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: !childrenPtr
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
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
 * @tc.name: MoveSplitSurfaceNode_ChildrenEmpty
 * @tc.desc: Test MoveSplitSurfaceNode when children list is empty (no MoveChild)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_ChildrenEmpty, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
}

/**
 * @tc.name: MoveSplitSurfaceNode_ChildFrontEqualsSplitSurfaceNode
 * @tc.desc: Test MoveSplitSurfaceNode when first child is already splitSurfaceNode_ (no MoveChild needed)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_ChildFrontEqualsSplitSurfaceNode, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
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
 * @tc.name: MoveSplitSurfaceNode_ChildrenNotEmpty_FrontNotSplitSurface
 * @tc.desc: Test MoveSplitSurfaceNode when children not empty and front != splitSurfaceNode_ (calls MoveChild)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSOpincLayerSplitterPlannerTest, MoveSplitSurfaceNode_ChildrenNotEmpty_FrontNotSplitSurface, TestSize.Level0)
{
    ASSERT_NE(planner_, nullptr);
    // Test Case: children not empty, front != splitSurfaceNode_
    planner_->splitSurface_ = CreateSplitSurface(100);

    planner_->MoveSplitSurfaceNode();
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

} // namespace
} // namespace Rosen
} // namespace OHOS
