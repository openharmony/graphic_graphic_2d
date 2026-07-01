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
#include "drawable/rs_overlay_ng_shader_drawable.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_image_cache_provider.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"

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
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
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
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
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
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
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
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
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
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
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
 * @tc.name: OnSyncTest004
 * @tc.desc: OnSync with needSync true but stagingShader null
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingShader_ = nullptr;  // Key: shader is null
    overlayDrawable->stagingCornerRadius_ = 5.0f;
    overlayDrawable->stagingNodeId_ = 123;
    
    overlayDrawable->OnSync();
    
    // Should NOT create visualEffectContainer when shader is null
    EXPECT_EQ(overlayDrawable->visualEffectContainer_, nullptr);
    // But still updates cornerRadius and nodeId
    EXPECT_EQ(overlayDrawable->cornerRadius_, 5.0f);
    EXPECT_EQ(overlayDrawable->nodeId_, 123);
}

/**
 * @tc.name: OnSyncTest005
 * @tc.desc: OnSync with needSync false and stagingShader valid
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnSyncTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = false;
    overlayDrawable->stagingCornerRadius_ = 8.0f;
    
    overlayDrawable->OnSync();
    
    // Should NOT create visualEffectContainer when needSync is false
    EXPECT_EQ(overlayDrawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(overlayDrawable->cornerRadius_, 8.0f);
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
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    overlayDrawable->visualEffectContainer_ = nullptr;
    overlayDrawable->OnDraw(canvas.get(), rect.get());
    // Should return early without errors
}

/**
 * @tc.name: OnDrawTest004
 * @tc.desc: OnDraw with valid visualEffectContainer
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest004, TestSize.Level1)
{
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(0, 0, 100, 100);
    overlayDrawable->stagingCornerRadius_ = 5.0f;
    overlayDrawable->OnSync();
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    EXPECT_NE(overlayDrawable->visualEffectContainer_, nullptr);
    overlayDrawable->OnDraw(canvas.get(), rect.get());
    // Should execute normal rendering path
}

/**
 * @tc.name: OnDrawTest005
 * @tc.desc: OnDraw with empty drawRect
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest005, TestSize.Level1)
{
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF();  // Empty rect
    overlayDrawable->OnSync();
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    EXPECT_TRUE(overlayDrawable->drawRect_.IsEmpty());
    overlayDrawable->OnDraw(canvas.get(), rect.get());
    // Should use input rect when drawRect is empty
}

/**
 * @tc.name: OnDrawTest006
 * @tc.desc: OnDraw with non-empty drawRect
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest006, TestSize.Level1)
{
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(10, 10, 50, 50);
    overlayDrawable->OnSync();
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    EXPECT_FALSE(overlayDrawable->drawRect_.IsEmpty());
    overlayDrawable->OnDraw(canvas.get(), rect.get());
    // Should use drawRect when it's not empty
}

/**
 * @tc.name: OnDrawTest007
 * @tc.desc: OnDraw with RSPaintFilterCanvas and cached effect data
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest007, TestSize.Level1)
{
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(0, 0, 100, 100);
    overlayDrawable->stagingCornerRadius_ = 10.0f;
    overlayDrawable->OnSync();
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    // Set cached effect data
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    effectData->cachedRect_ = Drawing::RectI(0, 0, 100, 100);
    filterCanvas->SetEffectData(effectData);
    
    overlayDrawable->OnDraw(filterCanvas.get(), rect.get());
    // Should execute cached blur image path
}

/**
 * @tc.name: OnDrawTest008
 * @tc.desc: OnDraw with DRM intersect enabled
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest008, TestSize.Level1)
{
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingDrawRect_ = RectF(0, 0, 100, 100);
    overlayDrawable->OnSync();
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    // Set cached effect data and DRM intersect
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    filterCanvas->SetEffectData(effectData);
    filterCanvas->SetEffectIntersectWithDRM(true);
    
    overlayDrawable->OnDraw(filterCanvas.get(), rect.get());
    // Should execute DRM intersect path
}

/**
 * @tc.name: OnDrawTest009
 * @tc.desc: Full flow test: OnGenerate, OnSync, OnDraw
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSPropertyDrawableOverlayNGShaderTest, OnDrawTest009, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    renderNode.renderProperties_.SetOverlayNGShader(shader);
    renderNode.renderProperties_.SetCornerRadius(Vector4f(15.0f, 15.0f, 15.0f, 15.0f));
    
    auto overlayDrawable = std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_TRUE(overlayDrawable->OnUpdate(renderNode));
    EXPECT_TRUE(overlayDrawable->needSync_);
    
    overlayDrawable->OnSync();
    EXPECT_FALSE(overlayDrawable->needSync_);
    EXPECT_NE(overlayDrawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(overlayDrawable->cornerRadius_, 15.0f);
    
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 200, 200);
    
    overlayDrawable->OnDraw(canvas.get(), rect.get());
    // Should complete full rendering flow
}
} // namespace OHOS::Rosen