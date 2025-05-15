/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "surface_buffer_impl.h"
#include "surface_type.h"
 
#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pixel_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"
 
using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;
 
namespace OHOS::Rosen {
class RSUniRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
 
private:
    static std::shared_ptr<Drawing::Image> CreateSkImage();
};
 
std::shared_ptr<Drawing::Image> CreateSkImage()
{
    const Drawing::ImageInfo info =
    Drawing::ImageInfo{200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    auto surface = Drawing::Surface::MakeRaster(info);
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush paint;
    paint.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(paint);
    canvas->DrawRect(Drawing::Rect(1, 1, 1, 1));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}
 
RSDisplayRenderNodeDrawable* GenerateDisplayDrawableById(NodeId id, RSDisplayNodeConfig config)
{
    std::shared_ptr<RSDisplayRenderNode> renderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    if (!renderNode) {
        return nullptr;
    }
    RSRenderNodeDrawableAdapter* displayAdapter = RSDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (!displayAdapter) {
        return nullptr;
    }
    return static_cast<RSDisplayRenderNodeDrawable*>(displayAdapter);
}

void RSUniRenderUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderUtilTest::TearDownTestCase() {}
void RSUniRenderUtilTest::SetUp() {}
void RSUniRenderUtilTest::TearDown() {}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect02
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when consumer is not nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect02, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    ASSERT_NE(surfaceNode->GetRSSurfaceHandler()->GetConsumer(), nullptr);

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnableBySrcRect03
 * @tc.desc: Test UpdateHwcNodeEnableBySrcRect when node is hardware disabled by src rect.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableBySrcRect03, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->isHardwareForcedDisabledBySrcRect_ = true;
    ASSERT_TRUE(surfaceNode->IsHardwareDisabledBySrcRect());

    rsUniRenderVisitor->UpdateHwcNodeEnableBySrcRect(*surfaceNode);
}

/*
 * @tc.name: UpdateHwcNodeEnable01
 * @tc.desc: Test UpdateHwcNodeEnable when surfaceNode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable01, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(nullptr);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnable02
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable02, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->ResetChildHardwareEnabledNodes();
    ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnable03
 * @tc.desc: Test UpdateHwcNodeEnable when hwcNodePtr is not on the tree.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnable03, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    NodeId childId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childId);
    childNode->SetIsOnTheTree(false);
    ASSERT_FALSE(childNode->IsOnTheTree());
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    displayNode->curMainAndLeashSurfaceNodes_.push_back(surfaceNode);
    rsUniRenderVisitor->curDisplayNode_ = displayNode;

    rsUniRenderVisitor->UpdateHwcNodeEnable();
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf01
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode is hardware forced disabled.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf01, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_TRUE(surfaceNode->IsHardwareForcedDisabled());

    std::vector<RectI> hwcRects;
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 0);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf02
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode has corner radius and anco force do direct.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf02, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(true);
    surfaceNode->SetAncoFlags(static_cast<uint32_t>(0x0001));
    ASSERT_TRUE(surfaceNode->GetAncoForceDoDirect());

    std::vector<RectI> hwcRects;
    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, false);
    EXPECT_EQ(hwcRects.size(), 1);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf03
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode intersects with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf03, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI(0, 0, 100, 100));
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_TRUE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeEnableByHwcNodeBelowSelf04
 * @tc.desc: Test UpdateHwcNodeEnableByHwcNodeBelowSelf when hwcNode does not intersect with hwcRects.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeEnableByHwcNodeBelowSelf04, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);

    surfaceNode->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
    surfaceNode->isOnTheTree_ = true;
    ASSERT_FALSE(surfaceNode->IsHardwareForcedDisabled());
    surfaceNode->SetAncoForceDoDirect(false);
    ASSERT_FALSE(surfaceNode->GetAncoForceDoDirect());

    surfaceNode->SetDstRect(RectI());
    std::vector<RectI> hwcRects;
    hwcRects.emplace_back(RectI(50, 50, 100, 100));
    ASSERT_FALSE(surfaceNode->GetDstRect().Intersect(RectI(50, 50, 100, 100)));

    NodeId displayNodeId = 1;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeEnableByHwcNodeBelowSelf(hwcRects, surfaceNode, true);
    EXPECT_EQ(hwcRects.size(), 2);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree01
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           curSurfaceNode_ is null or hwcNodes is empty.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree01, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        rsUniRenderVisitor->curSurfaceNode_ = nullptr;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
        ASSERT_NE(surfaceNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        ASSERT_EQ(surfaceNode->GetChildHardwareEnabledNodes().size(), 0);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree02
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           hwcNodePtr is nullptr or hwcNodePtr is not on the tree or GetCalcRectInPrepare is true.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree02, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        childNode = nullptr;
        ASSERT_EQ(childNode, nullptr);
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(false);
        ASSERT_FALSE(childNode->IsOnTheTree());
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }

    {
        NodeId childNodeId = 2;
        auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
        ASSERT_NE(childNode, nullptr);
        childNode->SetIsOnTheTree(true);
        ASSERT_TRUE(childNode->IsOnTheTree());
        childNode->SetCalcRectInPrepare(true);
        ASSERT_TRUE(childNode->GetCalcRectInPrepare());
        surfaceNode->ResetChildHardwareEnabledNodes();
        surfaceNode->AddChildHardwareEnabledNode(childNode);
        rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
        rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
    }
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree03
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and parent is null.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree03, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());
    ASSERT_EQ(childNode->GetParent().lock(), nullptr);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree04
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is false.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree04, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    NodeId fakeParentNodeId = 3;
    auto fakeParentNode = std::make_shared<RSSurfaceRenderNode>(fakeParentNodeId);
    ASSERT_NE(fakeParentNode, nullptr);
    fakeParentNode->AddChild(childNode);

    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_NE(parent->GetId(), parentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree05
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false,
 *           parent is not null and findInRoot is true.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree05, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: UpdateHwcNodeRectInSkippedSubTree06
 * @tc.desc: Test UpdateHwcNodeRectInSkippedSubTree when RS_PROFILER_SHOULD_BLOCK_HWCNODE() is false and
 *           parent's parent is not null.
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, UpdateHwcNodeRectInSkippedSubTree06, TestSize.Level2)
{
    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    NodeId parentNodeId = 1;
    auto parentNode = std::make_shared<RSRenderNode>(parentNodeId);
    ASSERT_NE(parentNode, nullptr);
    parentNode->InitRenderParams();

    NodeId childNodeId = 2;
    auto childNode = std::make_shared<RSSurfaceRenderNode>(childNodeId);
    ASSERT_NE(childNode, nullptr);
    childNode->InitRenderParams();
    childNode->SetIsOnTheTree(true);
    ASSERT_TRUE(childNode->IsOnTheTree());
    childNode->SetCalcRectInPrepare(false);
    ASSERT_FALSE(childNode->GetCalcRectInPrepare());

    parentNode->AddChild(childNode);
    auto parent = childNode->GetParent().lock();
    ASSERT_NE(parent, nullptr);
    ASSERT_EQ(parent->GetId(), parentNodeId);

    NodeId grandparentNodeId = 3;
    auto grandparentNode = std::make_shared<RSSurfaceRenderNode>(grandparentNodeId);
    ASSERT_NE(grandparentNode, nullptr);
    grandparentNode->InitRenderParams();
    grandparentNode->AddChild(parentNode);

    EXPECT_NE(parent->GetType(), RSRenderNodeType::DISPLAY_NODE);
    auto grandparent = parent->GetParent().lock();
    ASSERT_NE(grandparent, nullptr);
    ASSERT_EQ(grandparent->GetId(), grandparentNodeId);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->ResetChildHardwareEnabledNodes();
    surfaceNode->AddChildHardwareEnabledNode(childNode);
    rsUniRenderVisitor->curSurfaceNode_ = surfaceNode;
    NodeId displayNodeId = 4;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    rsUniRenderVisitor->UpdateHwcNodeRectInSkippedSubTree(*parentNode);
}

/*
 * @tc.name: CheckMergeGlobalFilterForDisplay01
 * @tc.desc: Test RSUniRenderVisitorTest.CheckMergeGlobalFilterForDisplay
 * @tc.type: FUNC
 * @tc.require: issueIC4XG4
 */
HWTEST_F(RSUniRenderVisitorTest, CheckMergeGlobalFilterForDisplay01, TestSize.Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    NodeId id0 = 0;
    pid_t pid0 = ExtractPid(id0);
    nodeMap.renderNodeMap_[pid0][id0] = nullptr;
    NodeId id1 = 1;
    pid_t pid1 = ExtractPid(id1);
    auto node1 = std::make_shared<RSRenderNode>(id1);
    nodeMap.renderNodeMap_[pid1][id1] = node1;
    NodeId id2 = 2;
    pid_t pid2 = ExtractPid(id2);
    auto node2 = std::make_shared<RSRenderNode>(id2);
    nodeMap.renderNodeMap_[pid2][id2] = node2;

    auto rsUniRenderVisitor = std::make_shared<RSUniRenderVisitor>();
    ASSERT_NE(rsUniRenderVisitor, nullptr);
    uint32_t left = 0;
    uint32_t top = 0;
    uint32_t width = 300;
    uint32_t height = 300;
    RectI rect{left, top, width, height};
    rsUniRenderVisitor->containerFilter_.insert({node2->GetId(), rect});
    NodeId displayNodeId = 3;
    RSDisplayNodeConfig config;
    rsUniRenderVisitor->curDisplayNode_ = std::make_shared<RSDisplayRenderNode>(displayNodeId, config);
    auto dirtyRegion = Occlusion::Region{ Occlusion::Rect{ rect } };
    rsUniRenderVisitor->CheckMergeGlobalFilterForDisplay(dirtyRegion);
}
} // namespace OHOS::Rosen