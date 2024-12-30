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

} // namespace Rosen
} // namespace OHOS