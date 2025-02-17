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
#include "rs_test_util.h"
#include "surface_buffer_impl.h"

#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderUtilDirtyRegionExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniRenderUtilDirtyRegionExtTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderUtilDirtyRegionExtTest::TearDownTestCase() {}
void RSUniRenderUtilDirtyRegionExtTest::SetUp() {}
void RSUniRenderUtilDirtyRegionExtTest::TearDown() {}

/*
 * @tc.name: MergeDirtyHistoryInVirtual001
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while displayNode has no param
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeDirtyHistoryInVirtual001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    DrawableV2::RSDisplayRenderNodeDrawable drawable(node);
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(drawable, 0);
    ASSERT_EQ(drawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual002
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while drawable is App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeDirtyHistoryInVirtual002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSDisplayRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSDisplayRenderParams>(displayNodeDrawable.nodeId_);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->isMainWindowType_ = false;
    drawableParam->isLeashWindow_ = false;
    drawableParam->isAppWindow_ = true;
    drawable->renderParams_ = std::move(drawableParam);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(nullptr);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(drawable);
    displayNodeDrawable.renderParams_ = std::move(displayNodeDrawableParam);
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayNodeDrawable, 0);
    ASSERT_NE(displayNodeDrawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual003
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeDirtyHistoryInVirtual003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSDisplayRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSDisplayRenderParams>(displayNodeDrawable.nodeId_);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->isMainWindowType_ = false;
    drawableParam->isLeashWindow_ = false;
    drawableParam->isAppWindow_ = false;
    drawable->renderParams_ = std::move(drawableParam);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(drawable);
    displayNodeDrawable.renderParams_ = std::move(displayNodeDrawableParam);
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayNodeDrawable, 0);
    ASSERT_NE(displayNodeDrawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualExtTest001
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionInVirtualExtTest001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = true;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualExtTest002
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionInVirtualExtTest002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualExtTest003
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable has no param
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionInVirtualExtTest003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables);
    ASSERT_EQ(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: SetAllSurfaceDrawableGlobalDirtyRegionExtTest001
 * @tc.desc: Verify function SetAllSurfaceDrawableGlobalDirtyRegion while drawable is not Main Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, SetAllSurfaceDrawableGlobalDirtyRegionExtTest001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceDrawables;
    Occlusion::Region globalDirtyRegion;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    allSurfaceDrawables.push_back(drawable);
    RSUniRenderUtil::SetAllSurfaceDrawableGlobalDityRegion(allSurfaceDrawables, globalDirtyRegion);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: SetAllSurfaceDrawableGlobalDirtyRegionExtTest002
 * @tc.desc: Verify function SetAllSurfaceDrawableGlobalDirtyRegion while drawable is Main Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, SetAllSurfaceDrawableGlobalDirtyRegionExtTest002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceDrawables;
    Occlusion::Region globalDirtyRegion;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    allSurfaceDrawables.push_back(nullptr);
    allSurfaceDrawables.push_back(drawable);
    RSUniRenderUtil::SetAllSurfaceDrawableGlobalDityRegion(allSurfaceDrawables, globalDirtyRegion);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: SetAllSurfaceDrawableGlobalDirtyRegionExtTest003
 * @tc.desc: Verify function SetAllSurfaceDrawableGlobalDirtyRegion while drawable has no param
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, SetAllSurfaceDrawableGlobalDirtyRegionExtTest003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceDrawables;
    Occlusion::Region globalDirtyRegion;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    allSurfaceDrawables.push_back(drawable);
    RSUniRenderUtil::SetAllSurfaceDrawableGlobalDityRegion(allSurfaceDrawables, globalDirtyRegion);
    ASSERT_EQ(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: AlignedDirtyRegionExtTest_001
 * @tc.desc: Verify function AlignedDirtyRegionExtTest while alignedBits equals zero.
 * @tc.type: FUNC
 * @tc.require: issueIAHPIM
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, AlignedDirtyRegionExtTest_001, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 0;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegionExtTest(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] == alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: AlignedDirtyRegionExtTest_002
 * @tc.desc: Verify function AlignedDirtyRegionExtTest while alignedBits non-zero.
 * @tc.type: FUNC
 * @tc.require: issueIAHPIM
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, AlignedDirtyRegionExtTest_002, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 2;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegionExtTest(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] != alignedRegion.GetRegionRects()[i]);
    }
}

/**
 * @tc.name: MergeVisibleDirtyRegionExtTest
 * @tc.desc: Verify function MergeVisibleDirtyRegion
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionExtTest, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceDrawawble;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    drawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    allSurfaceDrawawble.emplace_back(nullptr);
    allSurfaceDrawawble.emplace_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceDrawawble, hasVisibleDirtyRegionSurfaceVec, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceDrawawble, hasVisibleDirtyRegionSurfaceVec, true);
    EXPECT_TRUE(surfaceDrawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleDirtyRegionExtTest001
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node is App Window
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionExtTest001, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;

    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = true;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, true);
    ASSERT_TRUE(drawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleDirtyRegionExtTest002
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node has no param
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionExtTest002, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, true);
    ASSERT_FALSE(drawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleDirtyRegionExtTest003
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node is not App Window
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, MergeVisibleDirtyRegionExtTest003, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(surfaceDrawable);

    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, true);
    ASSERT_TRUE(drawable->renderParams_);
}

/**
 * @tc.name: ScreenIntersectDirtyRectsExtTest
 * @tc.desc: Verify function ScreenIntersectDirtyRects
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionExtTest, ScreenIntersectDirtyRectsExtTest, Function | SmallTest | Level2)
{
    Occlusion::Region region;
    ScreenInfo screenInfo;
    EXPECT_TRUE(RSUniRenderUtil::ScreenIntersectDirtyRects(region, screenInfo).empty());
}
} // namespace OHOS::Rosen
