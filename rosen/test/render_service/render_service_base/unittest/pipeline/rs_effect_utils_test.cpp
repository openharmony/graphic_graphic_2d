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

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_effect_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSEffectUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

void RSEffectUtilsTest::SetUpTestCase()
{
}
void RSEffectUtilsTest::TearDownTestCase()
{
}
void RSEffectUtilsTest::SetUp() {}
void RSEffectUtilsTest::TearDown() {}

/**
 * @tc.name: AccumulateFilterRenderContext001
 * @tc.desc: test AccumulateFilterRenderContext with null geometry
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    RSRenderNode rootNode(id + 1, context);
    FilterRenderContext filterContext;
    auto result = RSEffectUtils::AccumulateFilterRenderContext(node, rootNode, filterContext);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: AccumulateFilterRenderContext002
 * @tc.desc: test AccumulateFilterRenderContext with valid tree structure
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext002, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto rootNode = std::make_shared<RSRenderNode>(id + 1, context);
    rootNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto parentNode = std::make_shared<RSRenderNode>(id + 2, context);
    parentNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();

    parentNode->AddChild(node);
    rootNode->AddChild(parentNode);

    Drawing::Matrix identityMatrix;
    auto& nodeMatrix = node->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& parentNodeMatrix = parentNode->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& rootNodeAbsMatrix = rootNode->GetMutableRenderProperties().boundsGeo_->absMatrix_;
    nodeMatrix = identityMatrix;
    parentNodeMatrix = identityMatrix;
    rootNodeAbsMatrix = identityMatrix;
    rootNodeAbsMatrix->SetScale(10.0f, 10.0f);

    FilterRenderContext filterContext;
    filterContext.absMatrix = *rootNodeAbsMatrix;

    auto result = RSEffectUtils::AccumulateFilterRenderContext(*node, *rootNode, filterContext);
    EXPECT_TRUE(result);
    EXPECT_NE(filterContext.absMatrix.Get(0), identityMatrix.Get(0));
}

/**
 * @tc.name: AccumulateFilterRenderContext003
 * @tc.desc: test AccumulateFilterRenderContext with canvas node as offscreen parent
 * @tc.type: FUNC
 * @tc.require: issue24382
 */
HWTEST_F(RSEffectUtilsTest, AccumulateFilterRenderContext003, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(id, context);
    node->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto rootNode = std::make_shared<RSRenderNode>(id + 1, context);
    rootNode->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    auto canvasParent = std::make_shared<RSCanvasRenderNode>(id + 2, context);
    canvasParent->GetMutableRenderProperties().boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    canvasParent->SetDrawingCacheType(RSDrawingCacheType::FOREGROUND_FILTER_CACHE);

    canvasParent->AddChild(node);
    rootNode->AddChild(canvasParent);

    Drawing::Matrix identityMatrix;
    auto& nodeMatrix = node->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& canvasParentMatrix = canvasParent->GetMutableRenderProperties().boundsGeo_->matrix_;
    auto& rootNodeAbsMatrix = rootNode->GetMutableRenderProperties().boundsGeo_->absMatrix_;
    nodeMatrix = identityMatrix;
    canvasParentMatrix = identityMatrix;
    rootNodeAbsMatrix = identityMatrix;
    rootNodeAbsMatrix->SetScale(10.0f, 10.0f);

    FilterRenderContext filterContext;
    filterContext.absMatrix = rootNodeAbsMatrix.value();

    auto result = RSEffectUtils::AccumulateFilterRenderContext(*node, *rootNode, filterContext);
    EXPECT_TRUE(result);
    EXPECT_EQ(filterContext.offscreenNodeId, canvasParent->GetId());
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirtyAndPendingPurge001
 * @tc.desc: test UpdateFilterCacheWithBelowDirtyAndPendingPurge with MaterialFilter
 * @tc.type: FUNC
 * @tc.require: issue25152
 */
HWTEST_F(RSEffectUtilsTest, UpdateFilterCacheWithBelowDirtyAndPendingPurge001, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().GetEffect().materialFilter_ = std::make_shared<RSFilter>();

    RSDirtyRegionManager dirtyManager;
    dirtyManager.MergeDirtyRect(RectI(0, 0, 100, 100));

    RSEffectUtils::UpdateFilterCacheWithBelowDirtyAndPendingPurge(*node, dirtyManager);
    EXPECT_FALSE(dirtyManager.GetCurrentFrameDirtyRegion().IsEmpty());
}

/**
 * @tc.name: UpdateFilterCacheWithBelowDirtyAndPendingPurge002
 * @tc.desc: test UpdateFilterCacheWithBelowDirtyAndPendingPurge with BackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issue25152
 */
HWTEST_F(RSEffectUtilsTest, UpdateFilterCacheWithBelowDirtyAndPendingPurge002, TestSize.Level2)
{
    auto node = std::make_shared<RSCanvasRenderNode>(id, context);
    node->GetMutableRenderProperties().backgroundFilter_ = std::make_shared<RSFilter>();

    RSDirtyRegionManager dirtyManager;
    dirtyManager.MergeDirtyRect(RectI(0, 0, 100, 100));

    RSEffectUtils::UpdateFilterCacheWithBelowDirtyAndPendingPurge(*node, dirtyManager);
    EXPECT_FALSE(dirtyManager.GetCurrentFrameDirtyRegion().IsEmpty());
}
} // namespace Rosen
} // namespace OHOS
