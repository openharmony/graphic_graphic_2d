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

#include "drawable/rs_render_node_drawable.h"
#include "drawable/rs_render_node_shadow_drawable.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class RSRenderNodeShadowDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderNodeShadowDrawableTest::SetUpTestCase() {}
void RSRenderNodeShadowDrawableTest::TearDownTestCase() {}
void RSRenderNodeShadowDrawableTest::SetUp() {}
void RSRenderNodeShadowDrawableTest::TearDown() {}

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

class TestRSDrawable : public RSDrawable {
public:
    void OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const override
    {
        ++drawCount_;
    }

    void OnSync() override {}

    mutable int32_t drawCount_ = 0;
};

/**
 * @tc.name: DrawTest
 * @tc.desc: Verify function Draw
 * @tc.type:FUNC
 * @tc.require: issueI9U0VZ
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter> nodeDrawable =
        std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);
    Drawing::Canvas canvas;
    rsRenderNodeShadowDrawable->Draw(canvas);
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdIndex_.shadowIndex_ = 0;
    rsRenderNodeShadowDrawable->Draw(canvas);
    rsRenderNodeShadowDrawable->nodeDrawable_->renderParams_ = std::make_unique<RSRenderParams>(1);
    rsRenderNodeShadowDrawable->Draw(canvas);
    EXPECT_FALSE(rsRenderNodeShadowDrawable->nodeDrawable_->GetRenderParams()->GetShouldPaint());
}

/**
 * @tc.name: DrawWithValidShadowIndexTest
 * @tc.desc: Verify Draw continues when shadow index is within draw command list bounds
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawWithValidShadowIndexTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdIndex_.shadowIndex_ = 0;
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdList_.resize(1);

    Drawing::Canvas canvas;
    rsRenderNodeShadowDrawable->Draw(canvas);

    EXPECT_EQ(rsRenderNodeShadowDrawable->nodeDrawable_->GetRenderParams(), nullptr);
}

/**
 * @tc.name: DrawWithNullDrawCommandTest
 * @tc.desc: Verify Draw skips null draw commands and continues drawing valid commands
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawWithNullDrawCommandTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdIndex_.shadowIndex_ = 1;
    rsRenderNodeShadowDrawable->nodeDrawable_->renderParams_ = std::make_unique<RSRenderParams>(1);
    rsRenderNodeShadowDrawable->nodeDrawable_->renderParams_->SetShouldPaint(true);
    auto validDrawable = std::make_shared<TestRSDrawable>();
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdList_.emplace_back(nullptr);
    rsRenderNodeShadowDrawable->nodeDrawable_->drawCmdList_.emplace_back(validDrawable);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    rsRenderNodeShadowDrawable->Draw(canvas);

    EXPECT_EQ(validDrawable->drawCount_, 1);
}

/**
 * @tc.name: DrawWithMaskAndShadowTest
 * @tc.desc: Verify Draw only draws MASK and SHADOW, skips other slots
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawWithMaskAndShadowTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    auto saveAll = std::make_shared<TestRSDrawable>();
    auto mask = std::make_shared<TestRSDrawable>();
    auto shadow = std::make_shared<TestRSDrawable>();
    nodeDrawable->drawCmdList_ = {saveAll, mask, shadow};
    nodeDrawable->drawCmdIndex_.maskIndex_ = 1;
    nodeDrawable->drawCmdIndex_.shadowIndex_ = 2;
    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    nodeDrawable->renderParams_->SetShouldPaint(true);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    rsRenderNodeShadowDrawable->Draw(pfCanvas);

    EXPECT_EQ(saveAll->drawCount_, 0);
    EXPECT_EQ(mask->drawCount_, 1);
    EXPECT_EQ(shadow->drawCount_, 1);
}

/**
 * @tc.name: DrawWithoutMaskTest
 * @tc.desc: Verify Draw only draws SHADOW when maskIndex is -1
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawWithoutMaskTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    auto saveAll = std::make_shared<TestRSDrawable>();
    auto shadow = std::make_shared<TestRSDrawable>();
    nodeDrawable->drawCmdList_ = {saveAll, shadow};
    nodeDrawable->drawCmdIndex_.maskIndex_ = -1;
    nodeDrawable->drawCmdIndex_.shadowIndex_ = 1;
    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    nodeDrawable->renderParams_->SetShouldPaint(true);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    rsRenderNodeShadowDrawable->Draw(pfCanvas);

    EXPECT_EQ(saveAll->drawCount_, 0);
    EXPECT_EQ(shadow->drawCount_, 1);
}

/**
 * @tc.name: DrawSkipsNonEssentialSlotsTest
 * @tc.desc: Verify Draw skips SAVE_ALL and TRANSITION, only draws MASK and SHADOW
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawSkipsNonEssentialSlotsTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    auto saveAll = std::make_shared<TestRSDrawable>();
    auto mask = std::make_shared<TestRSDrawable>();
    auto transition = std::make_shared<TestRSDrawable>();
    auto shadow = std::make_shared<TestRSDrawable>();
    nodeDrawable->drawCmdList_ = {saveAll, mask, transition, shadow};
    nodeDrawable->drawCmdIndex_.maskIndex_ = 1;
    nodeDrawable->drawCmdIndex_.transitionIndex_ = 2;
    nodeDrawable->drawCmdIndex_.shadowIndex_ = 3;
    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    nodeDrawable->renderParams_->SetShouldPaint(true);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    rsRenderNodeShadowDrawable->Draw(pfCanvas);

    EXPECT_EQ(saveAll->drawCount_, 0);
    EXPECT_EQ(mask->drawCount_, 1);
    EXPECT_EQ(transition->drawCount_, 0);
    EXPECT_EQ(shadow->drawCount_, 1);
}

/**
 * @tc.name: DrawShadowIndexOutOfRangeTest
 * @tc.desc: Verify Draw returns early when shadowIndex is out of range
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawShadowIndexOutOfRangeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    nodeDrawable->drawCmdIndex_.shadowIndex_ = 5;
    nodeDrawable->drawCmdList_.resize(1);
    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    nodeDrawable->renderParams_->SetShouldPaint(true);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    rsRenderNodeShadowDrawable->Draw(pfCanvas);

    EXPECT_TRUE(nodeDrawable->GetRenderParams()->GetShouldPaint());
}

/**
 * @tc.name: DrawParamsNullTest
 * @tc.desc: Verify Draw returns early when params is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawParamsNullTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    auto shadow = std::make_shared<TestRSDrawable>();
    nodeDrawable->drawCmdList_ = {shadow};
    nodeDrawable->drawCmdIndex_.shadowIndex_ = 0;
    nodeDrawable->renderParams_ = nullptr;

    Drawing::Canvas canvas;
    rsRenderNodeShadowDrawable->Draw(canvas);

    EXPECT_EQ(shadow->drawCount_, 0);
}

/**
 * @tc.name: DrawShouldPaintFalseTest
 * @tc.desc: Verify Draw returns early when ShouldPaint is false
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderNodeShadowDrawableTest, DrawShouldPaintFalseTest, TestSize.Level1)
{
    auto node = std::make_shared<RSRenderNode>(0);
    auto nodeDrawable = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(node);
    auto rsRenderNodeShadowDrawable = std::make_shared<DrawableV2::RSRenderNodeShadowDrawable>(node, nodeDrawable);

    auto shadow = std::make_shared<TestRSDrawable>();
    nodeDrawable->drawCmdList_ = {shadow};
    nodeDrawable->drawCmdIndex_.shadowIndex_ = 0;
    nodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(1);
    nodeDrawable->renderParams_->SetShouldPaint(false);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas pfCanvas(&canvas);
    rsRenderNodeShadowDrawable->Draw(pfCanvas);

    EXPECT_EQ(shadow->drawCount_, 0);
}

} // namespace Rosen
} // namespace OHOS
