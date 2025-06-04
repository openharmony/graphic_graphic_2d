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
    rsEffectRenderNode.childHasVisibleEffect_ = true;
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
    rsEffectRenderNode.renderContent_->renderProperties_.SetHaveEffectRegion(true);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    auto backgroundFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    rsEffectRenderNode.renderContent_->renderProperties_.backgroundFilter_ = backgroundFilter;
    paintFilterCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::OFFSCREEN);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    paintFilterCanvas.SetCacheType(RSPaintFilterCanvas::CacheType::UNDEFINED);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    auto image = std::make_shared<RSImage>();
    rsEffectRenderNode.renderContent_->renderProperties_.SetBgImage(image);
    rsEffectRenderNode.ProcessRenderBeforeChildren(paintFilterCanvas);
    EXPECT_NE(rsEffectRenderNode.renderContent_->renderProperties_.GetBgImage(), nullptr);
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
    rsEffectRenderNode.renderContent_->renderProperties_.boundsGeo_->absRect_.data_[0] = 1.f;
    rsEffectRenderNode.renderContent_->renderProperties_.boundsGeo_->absRect_.data_[1] = 1.f;
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
    rsEffectRenderNode.renderContent_->renderProperties_.backgroundFilter_ = backgroundFilter;
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, false);
    rsEffectRenderNode.CheckBlurFilterCacheNeedForceClearOrSave(false, true);
    EXPECT_EQ(rsEffectRenderNode.GetFilterDrawable(false), nullptr);
}

/**
 * @tc.name: UpdateFilterCacheWithSelfDirtyTest
 * @tc.desc: test results of UpdateFilterCacheWithSelfDirty
 * @tc.type:FUNC
 * @tc.require: issueI9VAI2
 */
HWTEST_F(RSEffectRenderNodeTest, UpdateFilterCacheWithSelfDirtyTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    RSEffectRenderNode rsEffectRenderNode(nodeId);
    rsEffectRenderNode.UpdateFilterCacheWithSelfDirty();
    EXPECT_EQ(rsEffectRenderNode.GetFilterDrawable(false), nullptr);
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
    rsEffectRenderNode.renderContent_->renderProperties_.backgroundFilter_ = backgroundFilter;
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
    rsEffectRenderNode.OnFilterCacheStateChanged();
    rsEffectRenderNode.stagingRenderParams_ = std::make_unique<RSEffectRenderParams>(rsEffectRenderNode.GetId());
    rsEffectRenderNode.OnFilterCacheStateChanged();
    EXPECT_NE(rsEffectRenderNode.stagingRenderParams_, nullptr);
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
    EXPECT_EQ(rsEffectRenderNode.GetFilterDrawable(false), nullptr);
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
    bool isForeground = true;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(dirtyManager, isForeground);

    rsEffectRenderNode.isRotationChanged_ = true;
    rsEffectRenderNode.preRotationStatus_ = true;
    rsEffectRenderNode.UpdateFilterCacheWithBelowDirty(dirtyManager, isForeground);
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
} // namespace OHOS::Rosen