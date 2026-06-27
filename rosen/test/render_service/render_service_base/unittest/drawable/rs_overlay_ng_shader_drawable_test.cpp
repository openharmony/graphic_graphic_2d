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

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "effect/rs_render_shader_base.h"
#include "ge_visual_effect_container.h"
#include "drawable/rs_overlay_ng_shader_drawable.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_properties_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableOverlayNGShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertyDrawableOverlayNGShaderTest::SetUpTestCase() {}
void RSPropertyDrawableOverlayNGShaderTest::TearDownTestCase() {}
void RSPropertyDrawableOverlayNGShaderTest::SetUp() {}
void RSPropertyDrawableOverlayNGShaderTest::TearDown() {}

/**
 * @tc.name: OnGenerateAndOnUpdateTest001
 * @tc.desc: OnGenerate and OnUpdate test with null shader
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateAndOnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    // Test OnGenerate with null shader - should return nullptr
    EXPECT_EQ(overlayDrawable->OnGenerate(renderNode), nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest002
 * @tc.desc: OnGenerate with valid overlay shader
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnGenerateAndOnUpdateTest002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto shader = std::make_shared<RSNGRenderShaderBase>();
    renderNode.renderProperties_.SetOverlayNGShader(shader);
    
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    // Test OnGenerate with valid shader - should return valid pointer
    auto result = overlayDrawable->OnGenerate(renderNode);
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: OnUpdateTest001
 * @tc.desc: OnUpdate extracts shader and sets needSync
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto shader = std::make_shared<RSNGRenderShaderBase>();
    renderNode.renderProperties_.SetOverlayNGShader(shader);
    
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    bool result = overlayDrawable->OnUpdate(renderNode);
    EXPECT_TRUE(result);
    EXPECT_TRUE(overlayDrawable->needSync_);
}

/**
 * @tc.name: OnUpdateTest002
 * @tc.desc: OnUpdate extracts corner radius
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto shader = std::make_shared<RSNGRenderShaderBase>();
    renderNode.renderProperties_.SetOverlayNGShader(shader);
    renderNode.renderProperties_.SetCornerRadius(Vector4f(5.0f, 5.0f, 5.0f, 5.0f));
    
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    overlayDrawable->OnUpdate(renderNode);
    EXPECT_EQ(overlayDrawable->stagingCornerRadius_, 5.0f);
}

/**
 * @tc.name: OnUpdateTest003
 * @tc.desc: OnUpdate with null shader returns false
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnUpdateTest003, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    bool result = overlayDrawable->OnUpdate(renderNode);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: OnSyncTest001
 * @tc.desc: OnSync creates visual effect container
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = std::make_shared<RSNGRenderShaderBase>();
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(0, 0, 100, 100);
    
    overlayDrawable->OnSync();
    
    EXPECT_NE(overlayDrawable->visualEffectContainer_, nullptr);
    EXPECT_FALSE(overlayDrawable->needSync_);
}

/**
 * @tc.name: OnSyncTest002
 * @tc.desc: OnSync updates draw rect from staging
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = std::make_shared<RSNGRenderShaderBase>();
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(10, 10, 50, 50);
    
    overlayDrawable->OnSync();
    
    EXPECT_EQ(overlayDrawable->drawRect_, RectF(10, 10, 50, 50));
}

/**
 * @tc.name: OnSyncTest003
 * @tc.desc: OnSync without sync flag still updates corner radius
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    overlayDrawable->stagingCornerRadius_ = 10.0f;
    overlayDrawable->stagingNodeId_ = 12345;
    overlayDrawable->needSync_ = false;
    
    overlayDrawable->OnSync();
    
    EXPECT_EQ(overlayDrawable->cornerRadius_, 10.0f);
    EXPECT_EQ(overlayDrawable->nodeId_, 12345);
}

/**
 * @tc.name: OnDrawTest001
 * @tc.desc: OnDraw with null canvas returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    Drawing::Rect rect(0, 0, 100, 100);
    overlayDrawable->OnDraw(nullptr, &rect);
    // Should return early without errors
}

/**
 * @tc.name: OnDrawTest002
 * @tc.desc: OnDraw with null rect returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    Drawing::Canvas canvas;
    overlayDrawable->OnDraw(&canvas, nullptr);
    // Should return early without errors
}

/**
 * @tc.name: OnDrawTest003
 * @tc.desc: OnDraw with null visualEffectContainer returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    Drawing::Canvas canvas;
    Drawing::Rect rect(0, 0, 100, 100);
    overlayDrawable->visualEffectContainer_ = nullptr;
    overlayDrawable->OnDraw(&canvas, &rect);
    // Should return early without errors
}
} // namespace OHOS::Rosen