/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#ifdef NEW_SKIA
#include "include/gpu/GrDirectContext.h"
#else
#include "third_party/flutter/skia/include/gpu/GrContext.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline SkCanvas skCanvas_;
};

void RSRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&skCanvas_);
}
void RSRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
}
void RSRenderNodeTest::SetUp() {}
void RSRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRenderStatus001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, UpdateRenderStatus001, TestSize.Level1)
{
    RSSurfaceRenderNode node(id, context);
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
HWTEST_F(RSRenderNodeTest, UpdateParentChildrenRectTest, TestSize.Level1)
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
HWTEST_F(RSRenderNodeTest, ProcessTransitionBeforeChildrenTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    node.ProcessTransitionBeforeChildren(*canvas_);
}

/**
 * @tc.name: ProcessTransitionBeforeChildrenTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, AddModifierTest, TestSize.Level1)
{
    std::shared_ptr<RSRenderModifier> modifier = nullptr;
    RSRenderNode node(id, context);
    node.AddModifier(modifier);
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_TRUE(rsBaseRenderNode.IsDirty());

}

/**
 * @tc.name: SetSharedTransitionParamTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, SetSharedTransitionParamTest, TestSize.Level1)
{
    using SharedTransitionParam = std::pair<NodeId, std::weak_ptr<RSRenderNode>>;
    const std::optional<SharedTransitionParam> sharedTransitionParam;
    RSRenderNode node(id, context);
    node.SetSharedTransitionParam(std::move(sharedTransitionParam));
    RSBaseRenderNode rsBaseRenderNode(id, context);
    ASSERT_TRUE(rsBaseRenderNode.IsDirty());
}

/**
 * @tc.name: InitCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, InitCacheSurfaceTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
#ifdef NEW_SKIA
    node.InitCacheSurface(canvas_->recordingContext());
#else
    node.InitCacheSurface(canvas_->getGrContext());
#endif
}

/**
 * @tc.name: DrawCacheSurfaceTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest001, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    if (node.GetCompletedCacheSurface() == nullptr){
        ASSERT_EQ(NULL, node.GetCacheSurface());
    }
}

/**
 * @tc.name: DrawCacheSurfaceTest002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, DrawCacheSurfaceTest002, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isSubThreadNode = false;
    CacheType type = CacheType::ANIMATE_PROPERTY;
    node.SetCacheType(type);
    node.DrawCacheSurface(*canvas_, isSubThreadNode);
    ASSERT_EQ(node.GetCompletedCacheSurface(), NULL);
}

/**
 * @tc.name: MarkNodeGroupTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderNodeTest, MarkNodeGroupTest, TestSize.Level1)
{
    RSRenderNode node(id, context);
    bool isNodeGruop = true;
    bool isNodeGruop_ = false;

    auto nodeGruopType = node.GetNodeGroupType();
    RSRenderNode::NodeGroupType type = RSRenderNode::NodeGroupType::GROUPED_BY_UI;
    if (type >= nodeGruopType) {
        node.MarkNodeGroup(type, isNodeGruop);
        ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::TARGETED_CACHE);
        node.MarkNodeGroup(type, isNodeGruop_);
        ASSERT_EQ(node.GetDrawingCacheType(), RSDrawingCacheType::DISABLED_CACHE);
    }
}
} // namespace Rosen
} // namespace OHOS
