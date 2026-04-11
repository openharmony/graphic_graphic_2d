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

#include "gtest/gtest.h"

#include "drawable/rs_material_shader_drawable.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect_container.h"
#include "ge_image_cache_provider.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialShaderDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialShaderDrawableTest::SetUpTestCase() {}
void RSMaterialShaderDrawableTest::TearDownTestCase() {}
void RSMaterialShaderDrawableTest::SetUp() {}
void RSMaterialShaderDrawableTest::TearDown() {}

/**
 * @tc.name: RSMaterialShaderDrawable001
 * @tc.desc: Test OnGenerate without material shader
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSMaterialShaderDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
}

/**
 * @tc.name: RSMaterialShaderDrawable002
 * @tc.desc: Test OnGenerate with material shader
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    node.GetMutableRenderProperties().SetMaterialShader(shader);
    auto drawable = std::static_pointer_cast<DrawableV2::RSMaterialShaderDrawable>(
        DrawableV2::RSMaterialShaderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    EXPECT_TRUE(drawable->needSync_);
    EXPECT_EQ(drawable->stagingShader_, shader);
}

/**
 * @tc.name: RSMaterialShaderDrawable003
 * @tc.desc: Test OnUpdate
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    EXPECT_FALSE(drawable->OnUpdate(node));
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    node.GetMutableRenderProperties().SetMaterialShader(shader);
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_TRUE(drawable->needSync_);
    
    node.GetMutableRenderProperties().SetMaterialShader(nullptr);
    EXPECT_FALSE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSMaterialShaderDrawable004
 * @tc.desc: Test OnSync
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable004, TestSize.Level1)
{
    DrawableV2::RSMaterialShaderDrawable drawable;
    drawable.needSync_ = false;
    drawable.OnSync();
    EXPECT_FALSE(drawable.needSync_);
    
    drawable.needSync_ = true;
    drawable.OnSync();
    EXPECT_TRUE(drawable.needSync_);
    
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    drawable.stagingShader_ = shader;
    drawable.needSync_ = true;
    drawable.OnSync();
    EXPECT_FALSE(drawable.needSync_);
    EXPECT_NE(drawable.visualEffectContainer_, nullptr);
}

/**
 * @tc.name: RSMaterialShaderDrawable005
 * @tc.desc: Test OnDraw with null params
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable005, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    
    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);
    
    drawable->OnDraw(canvas.get(), nullptr);
    ASSERT_TRUE(true);
    
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawable006
 * @tc.desc: Test OnDraw with RSPaintFilterCanvas
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable006, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    filterCanvas->SetEffectData(effectData);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    
    filterCanvas->SetEffectIntersectWithDRM(true);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawable007
 * @tc.desc: Test OnDraw with drawRect
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable007, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->drawRect_ = RectF();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
    
    drawable->drawRect_ = RectF(10, 10, 50, 50);
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawable008
 * @tc.desc: Test OnDraw with geCacheProvider
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable008, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    effectData->cachedRect_ = Drawing::RectI(0, 0, 100, 100);
    effectData->refractOut_ = 0.5f;
    effectData->geCacheProvider_ = nullptr;
    filterCanvas->SetEffectData(effectData);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawable009
 * @tc.desc: Test full flow OnGenerate OnSync OnDraw
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable009, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    node.GetMutableRenderProperties().SetMaterialShader(shader);
    
    auto drawable = std::static_pointer_cast<DrawableV2::RSMaterialShaderDrawable>(
        DrawableV2::RSMaterialShaderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    
    drawable->OnSync();
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);
    
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawable010
 * @tc.desc: Test OnUpdate with SDFShape
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSMaterialShaderDrawableTest, RSMaterialShaderDrawable010, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::FROSTED_GLASS_EFFECT);
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);
    node.GetMutableRenderProperties().SetMaterialShader(shader);
    
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen