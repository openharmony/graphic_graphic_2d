/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "common/rs_obj_abs_geometry.h"
#include "drawable/rs_drawable.h"
#include "drawable/rs_property_drawable_background.h"
#include "params/rs_effect_render_params.h"
#include "pipeline/rs_effect_render_node.h"
#include "render_thread/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSEffectRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline Drawing::Canvas canvas;
};

void RSEffectRenderNodeTest::SetUpTestCase() {}
void RSEffectRenderNodeTest::TearDownTestCase() {}
void RSEffectRenderNodeTest::SetUp() {}
void RSEffectRenderNodeTest::TearDown() {}

/**
 * @tc.name: Prepare
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, Prepare, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    rsEffectRenderNode.Prepare(visitor);

    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsEffectRenderNode.Prepare(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, Process, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    rsEffectRenderNode.Process(visitor);
    rsEffectRenderNode.Prepare(visitor);
    std::shared_ptr<RSNodeVisitor> visitorTwo = std::make_shared<RSRenderThreadVisitor>();
    rsEffectRenderNode.Prepare(visitorTwo);
    rsEffectRenderNode.Process(visitorTwo);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetFilterRect
 * @tc.desc: test results of GetFilterRect
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, GetFilterRect, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::RectI path;
    rsEffectRenderNode.SetEffectRegion(path);
    rsEffectRenderNode.GetFilterRect();
    rsEffectRenderNode.hasEffectChildrenWithoutEmptyRect_ = true;
    rsEffectRenderNode.GetFilterRect();
    ASSERT_FALSE(rsEffectRenderNode.GetMutableRenderProperties().GetHaveEffectRegion());
}

/**
 * @tc.name: ProcessRenderBeforeChildren
 * @tc.desc: test results of ProcessRenderBeforeChildren
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, ProcessRenderBeforeChildren, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::Canvas canvas(1, 1);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    rsEffectRenderNode.renderProperties_.SetHaveEffectRegion(true);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    auto backgroundFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    rsEffectRenderNode.renderProperties_.backgroundFilter_ = backgroundFilter;
    paintFilterCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    paintFilterCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::UNDEFINED);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    auto image = std::make_shared<RSImage>();
    rsEffectRenderNode.renderProperties_.SetBgImage(image);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    EXPECT_NE(rsEffectRenderNode.renderProperties_.GetBgImage(), nullptr);
}

/**
 * @tc.name: SetEffectRegion
 * @tc.desc: test results of SetEffectRegion
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, SetEffectRegion, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    Drawing::RectI path(1, 1, 1, 1);
    rsEffectRenderNode.SetEffectRegion(path);
    rsEffectRenderNode.SetEffectRegion(std::nullopt);
    Drawing::RectI rectI(0, 0, 1, 1);
    rsEffectRenderNode.SetEffectRegion(rectI);
    rsEffectRenderNode.renderProperties_.boundsGeo_->absRect_.data_[0] = 1.f;
    rsEffectRenderNode.renderProperties_.boundsGeo_->absRect_.data_[1] = 1.f;
    rsEffectRenderNode.SetEffectRegion(rectI);
    ASSERT_FALSE(rsEffectRenderNode.GetMutableRenderProperties().GetHaveEffectRegion());
}

/**
 * @tc.name: CheckBlurFilterCacheNeedForceClearOrSaveTest
 * @tc.desc: test results of CheckBlurFilterCacheNeedForceClearOrSave
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, CheckBlurFilterCacheNeedForceClearOrSaveTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, false);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, true);
    auto backgroundFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    rsEffectRenderNode.renderProperties_.backgroundFilter_ = backgroundFilter;
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, false);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, true);
    EXPECT_EQ(rsEffectRenderNode.GetFilterDrawable(false), nullptr);
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    auto image = std::make_shared<RSImage>();
    rsEffectRenderNode.renderProperties_.SetBgImage(image);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, false);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, true);
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = false;
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    rsEffectRenderNode.foldStatusChanged_ = true;
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, false);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, true);
    EXPECT_TRUE(filterDrawable->stagingCacheManager_->stagingForceClearCache_ ||
        filterDrawable->stagingCacheManager_->stagingForceUseCache_);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirtyTest001
 * @tc.desc: test results of UpdateFilterCacheWithSelfDirty
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateFilterCacheWithSelfDirtyTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    EXPECT_EQ(rsEffectRenderNode.GetFilterDrawable(false), nullptr);
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    EXPECT_FALSE(filterDrawable->stagingCacheManager_->stagingFilterRegionChanged_ ||
        filterDrawable->stagingCacheManager_->stagingFilterInteractWithDirty_);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirtyTest002
 * @tc.desc: test results of UpdateFilterCacheWithSelfDirty
 * @tc.type:FUNC
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateFilterCacheWithSelfDirtyTest002, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);

    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        backgroundFilterDrawable;
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    EXPECT_FALSE(rsEffectRenderNode.backgroundFilterRegionChanged_);
    backgroundFilterDrawable->stagingVisibleRectInfo_ =
        std::make_unique<DrawableV2::RSFilterDrawable::FilterVisibleRectInfo>();
    backgroundFilterDrawable->stagingVisibleRectInfo_->snapshotRect_ = RectI(0, 0, 10, 10);
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    EXPECT_FALSE(rsEffectRenderNode.backgroundFilterRegionChanged_);
}

/**
 * @tc.name: MarkFilterCacheFlagsTest
 * @tc.desc: test results of MarkFilterCacheFlags
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, MarkFilterCacheFlagsTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    auto filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    RSDirtyRegionManager dirtyManager;
    rsEffectRenderNode.MarkFilterCacheFlags(filterDrawable, dirtyManager, false);
    rsEffectRenderNode.isRotationChanged_ = true;
    rsEffectRenderNode.MarkFilterCacheFlags(filterDrawable, dirtyManager, false);
    filterDrawable->MarkFilterForceUseCache(true);
    filterDrawable->MarkFilterForceClearCache();
    rsEffectRenderNode.MarkFilterCacheFlags(filterDrawable, dirtyManager, false);
    EXPECT_TRUE(rsEffectRenderNode.isRotationChanged_);
}

/**
 * @tc.name: MarkFilterCacheFlagsWithForceClearFilterCache
 * @tc.desc: test for force clear filter cache
 * @tc.type: FUNC
 * @tc.require: issueICVQH7
 */
HWTEST_F(RSEffectRenderNodeTest, MarkFilterCacheFlagsWithForceClearFilterCache, TestSize.Level1)
{
    RSEffectRenderNode node(1);
    std::shared_ptr<RSDirtyRegionManager> rsDirtyManager = std::make_shared<RSDirtyRegionManager>();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->cacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->MarkFilterForceClearCache();
    node.GetFilterRegionInfo().filterRegion_ = RectI(0, 0, 10, 10);
    node.GetFilterRegionInfo().defaultFilterRegion_ = RectI(0, 0, 10, 10);
    node.MarkFilterCacheFlags(filterDrawable, *rsDirtyManager, false);
    EXPECT_EQ(rsDirtyManager->GetCurrentFrameDirtyRegion(), RectI(0, 0, 10, 10));
}

/**
 * @tc.name: MarkFilterHasEffectChildrenTest
 * @tc.desc: test results of MarkFilterHasEffectChildren
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, MarkFilterHasEffectChildrenTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.MarkFilterHasEffectChildren();
    auto backgroundFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    rsEffectRenderNode.renderProperties_.backgroundFilter_ = backgroundFilter;
    rsEffectRenderNode.MarkFilterHasEffectChildren();
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.MarkFilterHasEffectChildren();
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: OnFilterCacheStateChangedTest
 * @tc.desc: test results of OnFilterCacheStateChanged
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, OnFilterCacheStateChangedTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    auto filterDrawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    rsEffectRenderNode.OnFilterCacheStateChanged();
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.OnFilterCacheStateChanged();
    auto effectParams = static_cast<RSEffectRenderParams*>(rsEffectRenderNode.stagingRenderParams_.get());
    EXPECT_FALSE(effectParams->GetCacheValid());
}

/**
 * @tc.name: MarkClearFilterCacheIfEffectChildrenChangedTest
 * @tc.desc: test results of MarkClearFilterCacheIfEffectChildrenChanged
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, MarkClearFilterCacheIfEffectChildrenChangedTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable =
        std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::MATERIAL_FILTER)] = filterDrawable;
    filterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = true;
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = true;
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    auto& bgProperties = rsEffectRenderNode.GetMutableRenderProperties();
    bgProperties.GetEffect().materialFilter_ = std::make_shared<RSFilter>();
    bgProperties.backgroundFilter_ = std::make_shared<RSFilter>();
    filterDrawable->stagingCacheManager_->stagingForceUseCache_ = false;
    filterDrawable->stagingCacheManager_->stagingForceClearCache_ = false;
    rsEffectRenderNode.lastFrameHasVisibleEffectWithoutEmptyRect_ = true;
    rsEffectRenderNode.SetChildHasVisibleFilter(false);
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    EXPECT_EQ(rsEffectRenderNode.IsForceClearFilterCache(filterDrawable), true);
    rsEffectRenderNode.lastFrameHasVisibleEffectWithoutEmptyRect_ = false;
    rsEffectRenderNode.SetChildHasVisibleFilter(true);
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    EXPECT_EQ(rsEffectRenderNode.IsForceClearFilterCache(filterDrawable), true);
    rsEffectRenderNode.lastFrameHasVisibleEffectWithoutEmptyRect_ = false;
    rsEffectRenderNode.SetChildHasVisibleFilter(false);
    rsEffectRenderNode.MarkClearFilterCacheIfEffectChildrenChanged();
    EXPECT_EQ(rsEffectRenderNode.IsForceClearFilterCache(filterDrawable), true);
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirty
 * @tc.desc: test results of UpdateFilterCacheWithBelowDirty
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateFilterCacheWithBelowDirty, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSEffectRenderNode rsEffectRenderNode(nodeId, context);
    RSDirtyRegionManager dirtyManager;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(
        Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()), RSDrawableSlot::COMPOSITING_FILTER);

    rsEffectRenderNode.isRotationChanged_ = true;
    rsEffectRenderNode.preRotationStatus_ = true;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(
        Occlusion::Rect(dirtyManager.GetCurrentFrameDirtyRegion()), RSDrawableSlot::COMPOSITING_FILTER);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: QuickPrepareTest
 * @tc.desc: test results of QuickPrepare
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, QuickPrepareTest, TestSize.Level1)
{
    NodeId id = 1;
    RSEffectRenderNode rsEffectRenderNode(id);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    rsEffectRenderNode.QuickPrepare(visitor);
    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsEffectRenderNode.QuickPrepare(visitor);
    EXPECT_FALSE(rsEffectRenderNode.preStaticStatus_);
}

/**
 * @tc.name: SetEffectIntersectWithDRM001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, SetEffectIntersectWithDRM001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.SetEffectIntersectWithDRM(true);
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.SetEffectIntersectWithDRM(true);
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetDarkColorMode001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSEffectRenderNodeTest, SetDarkColorMode001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.SetDarkColorMode(true);
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.SetDarkColorMode(true);
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: NotForceClearBackgroundFilterCacheWhenDirty
 * @tc.desc: Test function CheckBlurFilterCacheNeedForceClearOrSave
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSEffectRenderNodeTest, ForceClearForegroundFilterCacheWhenDirty, TestSize.Level1)
{
    auto renderNode = std::make_shared<RSEffectRenderNode>(1);
    ASSERT_NE(renderNode, nullptr);
    auto& properties = renderNode->GetMutableRenderProperties();
    properties.backgroundFilter_ = std::make_shared<RSFilter>();

    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    EXPECT_NE(backgroundFilterDrawable, nullptr);
    backgroundFilterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    backgroundFilterDrawable->cacheManager_ = std::make_unique<RSFilterCacheManager>();
    renderNode->GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)]
        = backgroundFilterDrawable;
    renderNode->CheckBlurFilterCacheNeedForceClearOrSave(true);
    EXPECT_EQ(backgroundFilterDrawable->stagingCacheManager_->stagingForceClearCache_, false);
}

/**
 * @tc.name: SetEffectIntersectWithDRMTest001
 * @tc.desc: Test function SetEffectIntersectWithDRM
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSEffectRenderNodeTest, SetEffectIntersectWithDRMTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.SetEffectIntersectWithDRM(true);
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.SetEffectIntersectWithDRM(true);
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: SetDarkColorModeTest001
 * @tc.desc: Test function SetDarkColorMode
 * @tc.type: FUNC
 * @tc.require: issueICD8D6
 */
HWTEST_F(RSEffectRenderNodeTest, SetDarkColorModeTest001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.SetDarkColorMode(true);
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.SetDarkColorMode(true);
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
}

/**
 * @tc.name: MarkFilterDebugEnabled001
 * @tc.desc: Test function MarkFilterDebugEnabled
 * @tc.type: FUNC
 * @tc.require: issue20057
 */
HWTEST_F(RSEffectRenderNodeTest, MarkFilterDebugEnabled001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.MarkFilterDebugEnabled();
    auto backgroundFilterDrawable = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    rsEffectRenderNode.GetDrawableVec(__func__)[static_cast<uint32_t>(RSDrawableSlot::BACKGROUND_FILTER)] =
        backgroundFilterDrawable;
    backgroundFilterDrawable->stagingCacheManager_ = nullptr;
    rsEffectRenderNode.MarkFilterDebugEnabled();
    backgroundFilterDrawable->stagingCacheManager_ = std::make_unique<RSFilterCacheManager>();
    rsEffectRenderNode.MarkFilterDebugEnabled();
    EXPECT_TRUE(backgroundFilterDrawable->stagingCacheManager_->debugEnabled_);
}

/**
 * @tc.name: UpdateChildHasVisibleEffectWithoutEmptyRect001
 * @tc.desc: Test function UpdateChildHasVisibleEffectWithoutEmptyRect
 * @tc.type: FUNC
 * @tc.require: issue21186
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateChildHasVisibleEffectWithoutEmptyRect001, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.hasEffectChildrenWithoutEmptyRect_ = true;
    rsEffectRenderNode.SetChildHasVisibleEffect(false);
    EXPECT_FALSE(rsEffectRenderNode.ChildHasVisibleEffect());
    rsEffectRenderNode.UpdateChildHasVisibleEffectWithoutEmptyRect();
    EXPECT_FALSE(rsEffectRenderNode.ChildHasVisibleEffectWithoutEmptyRect());

    rsEffectRenderNode.hasEffectChildrenWithoutEmptyRect_ = true;
    rsEffectRenderNode.SetChildHasVisibleEffect(true);
    EXPECT_TRUE(rsEffectRenderNode.ChildHasVisibleEffect());
    rsEffectRenderNode.UpdateChildHasVisibleEffectWithoutEmptyRect();
    EXPECT_FALSE(rsEffectRenderNode.ChildHasVisibleEffectWithoutEmptyRect());

    rsEffectRenderNode.hasEffectChildrenWithoutEmptyRect_ = true;
    auto context = std::make_shared<RSContext>();
    rsEffectRenderNode.context_ = context->weak_from_this();
    rsEffectRenderNode.UpdateChildHasVisibleEffectWithoutEmptyRect();
    EXPECT_FALSE(rsEffectRenderNode.ChildHasVisibleEffectWithoutEmptyRect());
}
} // namespace OHOS::Rosen