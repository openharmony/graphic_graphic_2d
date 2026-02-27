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
#include <parameters.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/dirty/rs_uni_dirty_occlusion_util.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniDirtyOcclusionUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniDirtyOcclusionUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniDirtyOcclusionUtilTest::TearDownTestCase() {}
void RSUniDirtyOcclusionUtilTest::SetUp() {}
void RSUniDirtyOcclusionUtilTest::TearDown() {}

/**
 * @tc.name: CheckResetAccumulatedOcclusionRegion001
 * @tc.desc: Test CheckResetAccumulatedOcclusionRegion with needReset
 * @tc.type: FUNC
 * @tc.require: issue21091
 */
HWTEST_F(RSUniDirtyOcclusionUtilTest, CheckResetAccumulatedOcclusionRegion001, TestSize.Level2)
{
    RSUniDirtyOcclusionUtil::GetIsAnimationOcclusionRef().first = true;

    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 0;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    rsSurfaceRenderNode->InitRenderParams();

    // set surface node trasnparent.
    rsSurfaceRenderNode->globalAlpha_ = 0.f;
    rsSurfaceRenderNode->abilityBgAlpha_ = 0;
    RectI screenRect = { 0, 0, 1000, 1000};

    // add filter node to parent surface.
    NodeId id = 1;
    auto filterNode = std::make_shared<RSCanvasRenderNode>(++id);
    filterNode->oldDirtyInSurface_ = screenRect;
    auto& nodeMap = rsContext->GetMutableNodeMap();
    nodeMap.renderNodeMap_.clear();
    nodeMap.RegisterRenderNode(filterNode);
    rsSurfaceRenderNode->visibleFilterChild_.push_back(filterNode->GetId());

    RectI rect = RectI(0, 0, 100, 100);
    Occlusion::Region region(rect);
    auto& occlusionRegion = region;
    RSUniDirtyOcclusionUtil::CheckResetAccumulatedOcclusionRegion(*rsSurfaceRenderNode, screenRect, occlusionRegion);
    ASSERT_TRUE(occlusionRegion.IsEmpty());

    occlusionRegion = region;
    RSUniDirtyOcclusionUtil::GetIsAnimationOcclusionRef().first = false;
    RSUniDirtyOcclusionUtil::CheckResetAccumulatedOcclusionRegion(*rsSurfaceRenderNode, screenRect, occlusionRegion);
    ASSERT_TRUE(occlusionRegion.IsEmpty());
}

} // namespace OHOS::Rosen
