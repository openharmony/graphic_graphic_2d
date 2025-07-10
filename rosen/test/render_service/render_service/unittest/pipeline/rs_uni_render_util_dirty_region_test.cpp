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

#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"
#include "screen_manager/rs_screen.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
static const int SIZE_ONE = 1;
static const int SIZE_TWO = 2;
static const int SIZE_THREE = 3;

class RSUniRenderUtilDirtyRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline Occlusion::Rect DEFAULT_RECT1 {0, 0, 500, 500};
    static inline Occlusion::Rect DEFAULT_RECT2 {500, 500, 1000, 1000};
    static inline Occlusion::Rect DEFAULT_RECT3 {1, 1, 501, 501};
    static inline OHOS::Rosen::RectI WHOLE_SCREEN_RECT {0, 0, 1000, 1000};
    static inline OHOS::Rosen::RectI SURFACE_DIRTY_RECT1 {101, 101, 100, 100};
    static inline OHOS::Rosen::RectI SURFACE_DIRTY_RECT2 {201, 201, 100, 100};
    static inline OHOS::Rosen::RectI DISPLAY_DIRTY_RECT1 {301, 301, 100, 100};
    static inline OHOS::Rosen::RectI NONE_DIRTY_RECT {0, 0, 0, 0};

private:
    void InitForMergeDirtyHistory(DrawableV2::RSScreenRenderNodeDrawable& displayDrawable,
        std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable, ScreenInfo& screenInfo,
        bool isSamplingOn = false, bool isAppWindow = false);
    int screenPhyWidth_ = 1000;
    int screenPhyHeight_ = 1000;
};

void RSUniRenderUtilDirtyRegionTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderUtilDirtyRegionTest::TearDownTestCase() {}
void RSUniRenderUtilDirtyRegionTest::SetUp() {}
void RSUniRenderUtilDirtyRegionTest::TearDown() {}

void RSUniRenderUtilDirtyRegionTest::InitForMergeDirtyHistory(
    DrawableV2::RSScreenRenderNodeDrawable& screenNodeDrawable,
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> drawable,
    ScreenInfo& screenInfo, bool isSamplingOn, bool isAppWindow)
{
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region visibleRegion(DEFAULT_RECT1);
    screenInfo.phyWidth = screenPhyWidth_;
    screenInfo.phyHeight = screenPhyHeight_;
    screenInfo.isSamplingOn = isSamplingOn;
    screenInfo.samplingScale = 1.f;
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(screenNodeDrawable.nodeId_);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);

    drawableParam->isAppWindow_ = isAppWindow;
    drawableParam->isLeashWindow_ = isAppWindow;
    drawableParam->isMainWindowType_ = isAppWindow;
    drawableParam->isLeashorMainWindow_ = isAppWindow;

    drawableParam->dstRect_ = WHOLE_SCREEN_RECT;
    drawableParam->SetVisibleRegion(visibleRegion);
    drawableParam->SetVisibleRegionInVirtual(visibleRegion);
    drawableParam->SetOldDirtyInSurface(WHOLE_SCREEN_RECT);

    drawable->renderParams_ = std::move(drawableParam);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(drawable);
    screenNodeDrawable.renderParams_ = std::move(displayNodeDrawableParam);
    ASSERT_NE(screenNodeDrawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual001
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while displayNode has no param
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    DrawableV2::RSScreenRenderNodeDrawable drawable(node);
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(drawable, 0);
    ASSERT_EQ(drawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual002
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while drawable is App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->isMainWindowType_ = true;
    drawableParam->isLeashWindow_ = true;
    drawableParam->isAppWindow_ = true;
    drawableParam->isLeashorMainWindow_ = true;
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
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
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
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest001
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = true;
    param->isAppWindow_ = true;
    param->isLeashorMainWindow_ = true;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest002
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest003
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable has no param
 * @tc.type: FUNC
 * @tc.require: issueIAE6P0
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam);
    ASSERT_EQ(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: AlignedDirtyRegion_001
 * @tc.desc: Verify function AlignedDirtyRegion while alignedBits equals zero.
 * @tc.type: FUNC
 * @tc.require: issueIAHPIM
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, AlignedDirtyRegion_001, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 0;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] == alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: AlignedDirtyRegion_002
 * @tc.desc: Verify function AlignedDirtyRegion while alignedBits non-zero.
 * @tc.type: FUNC
 * @tc.require: issueIAHPIM
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, AlignedDirtyRegion_002, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 2;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] != alignedRegion.GetRegionRects()[i]);
    }
}

/**
 * @tc.name: MergeVisibleDirtyRegionTest
 * @tc.desc: Verify function MergeVisibleDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionTest, Function | SmallTest | Level2)
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
 * @tc.name: MergeVisibleDirtyRegionTest001
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node is App Window
 * @tc.type: FUNC
 * @tc.require: issuesICHR4V
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionTest001, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;

    NodeId id = 1;
    int32_t bufferAge = 1;
    Occlusion::Region visibleRegion(DEFAULT_RECT1);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = true;
    param->isAppWindow_ = true;
    param->isLeashorMainWindow_ = true;
    param->dstRect_ = WHOLE_SCREEN_RECT;
    param->SetVisibleRegion(visibleRegion);
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    surfaceDrawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT1);
    surfaceDrawable->GetSyncDirtyManager()->SetBufferAge(bufferAge);
    surfaceDrawable->GetSyncDirtyManager()->UpdateDirty(false);
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, true);
    drawable->renderParams_->isFirstLevelCrossNode_ = true;
    auto& rtThread = RSUniRenderThread::Instance();
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->SetIsFirstVisitCrossNodeDisplay(true);
    rtThread.Sync(std::move(rsRenderThreadParams));
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec, true);
    ASSERT_TRUE(drawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleDirtyRegionTest002
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node has no param
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionTest002, Function | SmallTest | Level2)
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
 * @tc.name: MergeVisibleDirtyRegionTest003
 * @tc.desc: Verify function MergeVisibleDirtyRegion while node is not App Window
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionTest003, Function | SmallTest | Level2)
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
 * @tc.name: MergeVisibleAdvancedDirtyRegionTest004
 * @tc.desc: Verify function MergeVisibleAdvancedDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleAdvancedDirtyRegionTest004, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceDrawawble;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isLeashorMainWindow_ = true;
    drawable->renderParams_ = std::move(param);
    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    allSurfaceDrawawble.emplace_back(nullptr);
    allSurfaceDrawawble.emplace_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(allSurfaceDrawawble, hasVisibleDirtyRegionSurfaceVec);
    EXPECT_TRUE(surfaceDrawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleAdvancedDirtyRegionTest001
 * @tc.desc: Verify function MergeVisibleAdvancedDirtyRegion while node is App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleAdvancedDirtyRegionTest001, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;

    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = true;
    param->isAppWindow_ = true;
    param->isLeashorMainWindow_ = true;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec);
    ASSERT_TRUE(drawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleAdvancedDirtyRegionTest002
 * @tc.desc: Verify function MergeVisibleAdvancedDirtyRegion while node has no param
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleAdvancedDirtyRegionTest002, Function | SmallTest | Level2)
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
    RSUniRenderUtil::MergeVisibleAdvancedDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec);
    ASSERT_FALSE(drawable->renderParams_);
}

/**
 * @tc.name: MergeVisibleAdvancedDirtyRegionTest003
 * @tc.desc: Verify function MergeVisibleAdvancedDirtyRegion while node is not App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleAdvancedDirtyRegionTest003, Function | SmallTest | Level2)
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

    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodeDrawables, hasVisibleDirtyRegionSurfaceVec);
    ASSERT_TRUE(drawable->renderParams_);
}

/**
 * @tc.name: SetDrawRegionForQuickRejectTest001
 * @tc.desc: Verify function SetDrawRegionForQuickReject when not main window type
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, SetDrawRegionForQuickRejectTest001, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region testRegion(DEFAULT_RECT1);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = false;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::SetDrawRegionForQuickReject(allSurfaceNodeDrawables, testRegion);
    ASSERT_EQ(surfaceDrawable->syncDirtyManager_->GetDirtyRegionForQuickReject().size(), 0);
}

/**
 * @tc.name: SetDrawRegionForQuickRejectTest002
 * @tc.desc: Verify function SetDrawRegionForQuickReject when main window type
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, SetDrawRegionForQuickRejectTest002, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region testRegion(DEFAULT_RECT1);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = true;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::SetDrawRegionForQuickReject(allSurfaceNodeDrawables, testRegion);
    ASSERT_NE(surfaceDrawable->syncDirtyManager_->GetDirtyRegionForQuickReject().size(), 0);
}

/**
 * @tc.name: SetDrawRegionForQuickRejectTest003
 * @tc.desc: Verify function SetDrawRegionForQuickReject when param is null
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, SetDrawRegionForQuickRejectTest003, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region testRegion(DEFAULT_RECT1);
    drawable->renderParams_ = nullptr;

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::SetDrawRegionForQuickReject(allSurfaceNodeDrawables, testRegion);
    ASSERT_EQ(surfaceDrawable->syncDirtyManager_->GetDirtyRegionForQuickReject().size(), 0);
}

/**
 * @tc.name: SetDrawRegionForQuickRejectTest004
 * @tc.desc: Verify function SetDrawRegionForQuickReject when dirtyManager is null
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, SetDrawRegionForQuickRejectTest004, Function | SmallTest | Level2)
{
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    NodeId id = 1;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto drawable = RSSurfaceRenderNodeDrawable::OnGenerate(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    Occlusion::Region testRegion(DEFAULT_RECT1);
    auto param = std::make_unique<RSSurfaceRenderParams>(id);
    ASSERT_NE(param, nullptr);
    param->isMainWindowType_ = true;
    drawable->renderParams_ = std::move(param);

    auto surfaceDrawable =
        std::shared_ptr<RSSurfaceRenderNodeDrawable>(static_cast<RSSurfaceRenderNodeDrawable*>(drawable));
    ASSERT_NE(surfaceDrawable, nullptr);
    surfaceDrawable->syncDirtyManager_ = nullptr;
    allSurfaceNodeDrawables.push_back(surfaceDrawable);
    RSUniRenderUtil::SetDrawRegionForQuickReject(allSurfaceNodeDrawables, testRegion);
    ASSERT_TRUE(drawable->renderParams_);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest004
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest004, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = true;
    param->isAppWindow_ = true;
    param->isLeashorMainWindow_ = true;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(nullptr);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam, true);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest005
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest005, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    param->isMainWindowType_ = false;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam, true);
    ASSERT_NE(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeVisibleDirtyRegionInVirtualTest006
 * @tc.desc: Verify function MergeVisibleDirtyRegionInVirtual while drawable has no param
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeVisibleDirtyRegionInVirtualTest006, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    std::vector<DrawableV2::RSRenderNodeDrawableAdapter::SharedPtr> allSurfaceNodeDrawables;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    allSurfaceNodeDrawables.push_back(drawable);
    RSUniRenderUtil::MergeVisibleDirtyRegionInVirtual(allSurfaceNodeDrawables, *displayNodeDrawableParam, true);
    ASSERT_EQ(drawable->renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual004
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while displayNode has no param
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual004, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    DrawableV2::RSScreenRenderNodeDrawable drawable(node);
    ScreenInfo screenInfo;
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(drawable, 0, screenInfo);
    ASSERT_EQ(drawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual005
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while drawable is App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual005, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->isMainWindowType_ = true;
    drawableParam->isLeashWindow_ = true;
    drawableParam->isAppWindow_ = true;
    drawableParam->isLeashorMainWindow_ = true;
    drawable->renderParams_ = std::move(drawableParam);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(nullptr);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(drawable);
    displayNodeDrawable.renderParams_ = std::move(displayNodeDrawableParam);
    ScreenInfo screenInfo;
    screenInfo.isSamplingOn = true;
    screenInfo.samplingScale = 1.f;
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayNodeDrawable, 0, screenInfo);
    ASSERT_NE(displayNodeDrawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual006
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while drawable is not App Window
 * @tc.type: FUNC
 * @tc.require: issuesIC3ZOJ
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual006, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    DrawableV2::RSScreenRenderNodeDrawable displayNodeDrawable(topNode);
    auto displayNodeDrawableParam = std::make_unique<RSScreenRenderParams>(displayNodeDrawable.nodeId_);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto drawableParam = std::make_unique<RSSurfaceRenderParams>(drawable->nodeId_);
    drawableParam->isMainWindowType_ = false;
    drawableParam->isLeashWindow_ = false;
    drawableParam->isAppWindow_ = false;
    drawable->renderParams_ = std::move(drawableParam);
    displayNodeDrawableParam->allMainAndLeashSurfaceDrawables_.push_back(drawable);
    displayNodeDrawable.renderParams_ = std::move(displayNodeDrawableParam);
    ScreenInfo screenInfo;
    RSUniRenderUtil::MergeDirtyHistoryInVirtual(displayNodeDrawable, 0, screenInfo);
    ASSERT_NE(displayNodeDrawable.renderParams_, nullptr);
}

/*
 * @tc.name: MergeDirtyHistoryInVirtual007
 * @tc.desc: Verify function MergeDirtyHistoryInVirtual while screenInfo.isSamplingOn is on and
 *           MergeDirtyRect, surfaceParams has SKIP, surfaceParams is black
 * @tc.type: FUNC
 * @tc.require: issuesICHR4V
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistoryInVirtual007, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    NodeId screenParamsId = 10000;
    ScreenInfo screenInfo;
    int32_t bufferAge = 1;
    ScreenId mainId = 1000;
    ScreenId otherId = 1001;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    DrawableV2::RSScreenRenderNodeDrawable screenNodeDrawable(topNode);

    InitForMergeDirtyHistory(screenNodeDrawable, drawable, screenInfo, true, true);
    drawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT1);
    screenNodeDrawable.GetSyncDirtyManager()->MergeDirtyRect(DISPLAY_DIRTY_RECT1);
    std::vector<RectI> damageRegionRects;

    damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(screenNodeDrawable, bufferAge, screenInfo);
    ASSERT_TRUE(damageRegionRects.size() == SIZE_TWO);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->renderParams_.get());
    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, true);
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(screenNodeDrawable, bufferAge, screenInfo);
    ASSERT_TRUE(damageRegionRects.size() == SIZE_ONE);

    auto screenParams = static_cast<RSScreenRenderParams*>(screenNodeDrawable.renderParams_.get());
    std::vector<uint64_t> blockList = {};
    surfaceParams->id_ = screenParamsId;
    blockList.push_back(surfaceParams->GetId());
    screenParams->screenInfo_.id = otherId;
    auto screenId = screenParams->GetScreenId();

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
    screenManagerImpl.SetDefaultScreenId(mainId);
    screenManagerImpl.screens_[screenId] = std::make_shared<impl::RSScreen>(screenId, true, nullptr, nullptr);
    screenManagerImpl.screens_[mainId] = std::make_shared<impl::RSScreen>(mainId, false, nullptr, nullptr);
    int32_t result = screenManager->AddVirtualScreenBlackList(screenId, blockList);
    ASSERT_EQ(result, SUCCESS);

    surfaceParams->GetMultableSpecialLayerMgr().Set(SpecialLayerType::SKIP, false);
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(screenNodeDrawable, bufferAge, screenInfo);
    ASSERT_TRUE(damageRegionRects.size() == SIZE_ONE);
}

/*
 * @tc.name: MergeDirtyHistory001
 * @tc.desc: Verify function MergeDirtyHistory while screenInfo.isSamplingOn is off and
 *           advancedDirtyType_ is disabled or default
 * @tc.type: FUNC
 * @tc.require: issuesICHR4V
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistory001, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    int32_t bufferAge = 1;
    int32_t oneDefaultTypeValue = -1;
    ScreenInfo screenInfo;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    DrawableV2::RSScreenRenderNodeDrawable screenNodeDrawable(topNode);

    InitForMergeDirtyHistory(screenNodeDrawable, drawable, screenInfo, false, true);

    auto& rtThread = RSUniRenderThread::Instance();
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::make_unique<RSRenderThreadParams>());
    }
    auto params = static_cast<RSScreenRenderParams*>(screenNodeDrawable.renderParams_.get());
    OHOS::Rosen::RSDirtyRectsDfx rsDirtyRectsDfx(screenNodeDrawable);
    std::vector<RectI> damageRegionRects;

    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
        rsDirtyRectsDfx, *params);
    ASSERT_EQ(damageRegionRects.size(), SIZE_ONE);
    ASSERT_TRUE(NONE_DIRTY_RECT == damageRegionRects[0]);

    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->advancedDirtyType_ = static_cast<AdvancedDirtyRegionType>(oneDefaultTypeValue);
    rtThread.Sync(std::move(rsRenderThreadParams));
    drawable->GetSyncDirtyManager()->Clear();
    drawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT1);
    drawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT2);
    screenNodeDrawable.GetSyncDirtyManager()->MergeDirtyRect(DISPLAY_DIRTY_RECT1);

    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
         rsDirtyRectsDfx, *params);
    ASSERT_EQ(damageRegionRects.size(), SIZE_TWO);
}

/*
 * @tc.name: MergeDirtyHistory002
 * @tc.desc: Verify function MergeDirtyHistory while screenInfo.isSamplingOn is on and advancedDirtyType_ is
 *           SET_ADVANCED_DISPLAY or SET_ADVANCED_SURFACE_AND_DISPLAY
 * @tc.type: FUNC
 * @tc.require: issuesICHR4V
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistory002, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    int32_t bufferAge = 1;
    ScreenInfo screenInfo;

    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    DrawableV2::RSScreenRenderNodeDrawable screenNodeDrawable(topNode);

    InitForMergeDirtyHistory(screenNodeDrawable, drawable, screenInfo, true, true);

    auto& rtThread = RSUniRenderThread::Instance();
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->advancedDirtyType_ = AdvancedDirtyRegionType::SET_ADVANCED_DISPLAY;
    if (!rtThread.GetRSRenderThreadParams()) {
        rtThread.Sync(std::move(rsRenderThreadParams));
    }
    auto params = static_cast<RSScreenRenderParams*>(screenNodeDrawable.renderParams_.get());
    OHOS::Rosen::RSDirtyRectsDfx rsDirtyRectsDfx(screenNodeDrawable);
    std::vector<RectI> damageRegionRects;
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
         rsDirtyRectsDfx, *params);
    ASSERT_EQ(damageRegionRects.size(), SIZE_ONE);

    rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->advancedDirtyType_ = AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY;
    rtThread.Sync(std::move(rsRenderThreadParams));
    drawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT1);
    drawable->GetSyncDirtyManager()->MergeDirtyRect(SURFACE_DIRTY_RECT2);
    screenNodeDrawable.GetSyncDirtyManager()->MergeDirtyRect(DISPLAY_DIRTY_RECT1);
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
         rsDirtyRectsDfx, *params);
    ASSERT_EQ(damageRegionRects.size(), SIZE_THREE);
}

/*
 * @tc.name: MergeDirtyHistory003
 * @tc.desc: Verify function MergeDirtyHistory while syncDirtyManager_ is nullptr or not FirstLevelCrossNode
 * @tc.type: FUNC
 * @tc.require: issuesICHR4V
*/
HWTEST_F(RSUniRenderUtilDirtyRegionTest, MergeDirtyHistory003, Function | SmallTest | Level2)
{
    NodeId nodeId = 1;
    int32_t bufferAge = -1;
    ScreenInfo screenInfo;
    auto topNode = std::make_shared<RSRenderNode>(nodeId++);
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    DrawableV2::RSScreenRenderNodeDrawable screenNodeDrawable(topNode);

    InitForMergeDirtyHistory(screenNodeDrawable, drawable, screenInfo, true, true);

    auto& rtThread = RSUniRenderThread::Instance();
    auto rsRenderThreadParams = std::make_unique<RSRenderThreadParams>();
    rsRenderThreadParams->advancedDirtyType_ = AdvancedDirtyRegionType::SET_ADVANCED_SURFACE_AND_DISPLAY;
    rtThread.Sync(std::move(rsRenderThreadParams));
    auto params = static_cast<RSScreenRenderParams*>(screenNodeDrawable.renderParams_.get());
    OHOS::Rosen::RSDirtyRectsDfx rsDirtyRectsDfx(screenNodeDrawable);
    std::vector<RectI> damageRegionRects;

    drawable->renderParams_->SetFirstLevelCrossNode(true);
    params->isFirstVisitCrossNodeDisplay_ = true;
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
        rsDirtyRectsDfx, *params);
    drawable->renderParams_->SetFirstLevelCrossNode(false);
    drawable->syncDirtyManager_ = nullptr;
    bufferAge = 1;
    damageRegionRects = RSUniRenderUtil::MergeDirtyHistory(screenNodeDrawable, bufferAge, screenInfo,
        rsDirtyRectsDfx, *params);
    ASSERT_EQ(damageRegionRects.size(), SIZE_ONE);
}

/**
 * @tc.name: ExpandDamageRegionToSingleRect001
 * @tc.desc: Verify function ExpandDamageRegionToSingleRect, if region is empty, nothing will happen.
 * @tc.type:FUNC
 * @tc.require:issuesIC0Y3E
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, ExpandDamageRegionToSingleRect001, Function | SmallTest | Level2)
{
    Occlusion::Region region;
    RSUniRenderUtil::ExpandDamageRegionToSingleRect(region);
    EXPECT_TRUE(region.IsEmpty());
}

/**
 * @tc.name: ExpandDamageRegionToSingleRect002
 * @tc.desc: Verify function ExpandDamageRegionToSingleRect, if region is single rect, nothing will happen.
 * @tc.type:FUNC
 * @tc.require:issuesIC0Y3E
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, ExpandDamageRegionToSingleRect002, Function | SmallTest | Level2)
{
    Occlusion::Region region = Occlusion::Region { DEFAULT_RECT1 };
    RSUniRenderUtil::ExpandDamageRegionToSingleRect(region);
    EXPECT_EQ(region.GetSize(), 1);
    EXPECT_EQ(region.Area(), DEFAULT_RECT1.Area());
}

/**
 * @tc.name: ExpandDamageRegionToSingleRect003
 * @tc.desc: Verify function ExpandDamageRegionToSingleRect, if expanding is too expensive, nothing will happen.
 * @tc.type:FUNC
 * @tc.require:issuesIC0Y3E
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, ExpandDamageRegionToSingleRect003, Function | SmallTest | Level2)
{
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    Occlusion::Region region1 = Occlusion::Region { DEFAULT_RECT1 };
    Occlusion::Region region2 = Occlusion::Region { DEFAULT_RECT2 };
    Occlusion::Region damageRegion;
    damageRegion = region1.Or(region2);
    RSUniRenderUtil::ExpandDamageRegionToSingleRect(damageRegion);
    EXPECT_FALSE(damageRegion.Area() > damageRegion.GetBound().Area() * clipRectThreshold);
    EXPECT_EQ(damageRegion.GetSize(), 2);
}

/**
 * @tc.name: ExpandDamageRegionToSingleRect004
 * @tc.desc: ExpandDamageRegionToSingleRect, multi-rects will be joined as one rect if expanding is at reasonable cost.
 * @tc.type:FUNC
 * @tc.require:issuesIC0Y3E
 */
HWTEST_F(RSUniRenderUtilDirtyRegionTest, ExpandDamageRegionToSingleRect004, Function | SmallTest | Level2)
{
    const auto clipRectThreshold = RSSystemProperties::GetClipRectThreshold();
    Occlusion::Region region1 = Occlusion::Region { DEFAULT_RECT1 };
    Occlusion::Region region2 = Occlusion::Region { DEFAULT_RECT3 };
    Occlusion::Region damageRegion;
    damageRegion = region1.Or(region2);
    RSUniRenderUtil::ExpandDamageRegionToSingleRect(damageRegion);
    EXPECT_TRUE(damageRegion.Area() > damageRegion.GetBound().Area() * clipRectThreshold);
    EXPECT_EQ(damageRegion.GetSize(), 1);
}
} // namespace OHOS::Rosen
