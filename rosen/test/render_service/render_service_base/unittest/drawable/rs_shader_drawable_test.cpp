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
#include "drawable/rs_shader_drawable.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "render/rs_effect_luminance_manager.h"
#include "ge_image_cache_provider.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShaderDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderDrawableTest::SetUpTestCase() {}
void RSShaderDrawableTest::TearDownTestCase() {}
void RSShaderDrawableTest::SetUp() {}
void RSShaderDrawableTest::TearDown() {}

namespace {
// Testable base class that drives the RSShaderDrawable template method with a controllable
// slot shader, used to cover the base OnUpdate/OnSync/OnDraw paths directly.
class TestableShaderDrawable : public DrawableV2::RSShaderDrawable {
public:
    TestableShaderDrawable() = default;
    ~TestableShaderDrawable() override = default;

    void SetTestShader(const std::shared_ptr<RSNGRenderShaderBase>& shader)
    {
        testShader_ = shader;
    }

protected:
    std::shared_ptr<RSNGRenderShaderBase> GetShader(const RSProperties& properties) const override
    {
        return testShader_;
    }

    void DfxOnDraw() const override {}

private:
    std::shared_ptr<RSNGRenderShaderBase> testShader_;
};
} // namespace

/**
 * @tc.name: RSShaderDrawableBaseOnUpdate001
 * @tc.desc: Base OnUpdate returns false when the slot shader is absent
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnUpdate001, TestSize.Level1)
{
    RSRenderNode node(0);
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSShaderDrawableBaseOnUpdate002
 * @tc.desc: Base OnUpdate with a shader publishes the staging state
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnUpdate002, TestSize.Level1)
{
    RSRenderNode node(0);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    ASSERT_NE(shader, nullptr);
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->SetTestShader(shader);
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_TRUE(drawable->needSync_);
    EXPECT_EQ(drawable->stagingShader_, shader);
    EXPECT_EQ(drawable->stagingNodeId_, node.GetId());
}

/**
 * @tc.name: RSShaderDrawableBaseOnUpdate003
 * @tc.desc: Base OnUpdate with an HDR shader enables EDR in staging
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnUpdate003, TestSize.Level1)
{
    RSRenderNode node(0);
    auto shader = std::make_shared<RSNGRenderRoundedRectFlowlight>();
    ASSERT_NE(shader, nullptr);
    Vector4f color{0.5f, 1.5f, 0.5f, 1.0f};
    shader->Setter<RoundedRectFlowlightColorRenderTag>(color);
    EXPECT_TRUE(RSEffectLuminanceManager::GetInstance().GetEnableHdrEffect(shader));
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->SetTestShader(shader);
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_TRUE(drawable->stagingEnableEDREffect_);
    EXPECT_TRUE(drawable->GetEnableEDR());
}

/**
 * @tc.name: RSShaderDrawableBaseOnSync001
 * @tc.desc: Base OnSync with staging shader publishes RT members
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnSync001, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    drawable->stagingShader_ = shader;
    drawable->needSync_ = true;
    drawable->stagingDrawRect_ = RectF(10, 10, 50, 50);
    drawable->stagingCornerRadius_ = 5.0f;
    drawable->stagingNodeId_ = 7;
    drawable->stagingEnableEDREffect_ = true;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(drawable->drawRect_, RectF(10, 10, 50, 50));
    EXPECT_EQ(drawable->cornerRadius_, 5.0f);
    EXPECT_EQ(drawable->nodeId_, 7);
    EXPECT_TRUE(drawable->enableEDREffect_);
}

/**
 * @tc.name: RSShaderDrawableBaseOnSync002
 * @tc.desc: Base OnSync without shader skips publishing
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnSync002, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->needSync_ = true;
    drawable->stagingShader_ = nullptr;
    drawable->OnSync();
    EXPECT_TRUE(drawable->needSync_);
    EXPECT_EQ(drawable->visualEffectContainer_, nullptr);
}

/**
 * @tc.name: RSShaderDrawableBaseOnSync003
 * @tc.desc: Base OnSync without sync flag publishes nothing
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnSync003, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    drawable->stagingShader_ = shader;
    drawable->needSync_ = false;
    drawable->stagingCornerRadius_ = 8.0f;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    EXPECT_EQ(drawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(drawable->cornerRadius_, 0.0f);
}

/**
 * @tc.name: RSShaderDrawableBaseOnDraw001
 * @tc.desc: Base OnDraw returns early on null canvas
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnDraw001, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Rect rect(0, 0, 100, 100);
    drawable->OnDraw(nullptr, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSShaderDrawableBaseOnDraw002
 * @tc.desc: Base OnDraw returns early on null rect or container
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnDraw002, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    drawable->OnDraw(canvas.get(), nullptr);
    drawable->visualEffectContainer_ = nullptr;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSShaderDrawableBaseOnDraw003
 * @tc.desc: Base OnDraw walks the normal drawing path
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnDraw003, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->drawRect_ = RectF(0, 0, 100, 100);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSShaderDrawableBaseOnDraw004
 * @tc.desc: Base OnDraw with cached effect data and DRM intersect
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSShaderDrawableBaseOnDraw004, TestSize.Level1)
{
    auto drawable = std::make_shared<TestableShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->drawRect_ = RectF(0, 0, 100, 100);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    effectData->cachedRect_ = Drawing::RectI(0, 0, 100, 100);
    canvas->SetEffectData(effectData);
    drawable->OnDraw(canvas.get(), rect.get());
    canvas->SetEffectIntersectWithDRM(true);
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSForegroundShaderDrawableOnGenerate001
 * @tc.desc: Foreground OnGenerate returns nullptr without a foreground shader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnGenerate001, TestSize.Level1)
{
    RSRenderNode node(0);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSForegroundShaderDrawable::OnGenerate(node);
    EXPECT_EQ(drawable, nullptr);
}

/**
 * @tc.name: RSForegroundShaderDrawableOnGenerate002
 * @tc.desc: Foreground OnGenerate creates a drawable with a foreground shader
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnGenerate002, TestSize.Level1)
{
    RSRenderNode node(0);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    ASSERT_NE(shader, nullptr);
    node.GetMutableRenderProperties().SetForegroundShader(shader);
    auto drawable = std::static_pointer_cast<DrawableV2::RSForegroundShaderDrawable>(
        DrawableV2::RSForegroundShaderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    EXPECT_TRUE(drawable->needSync_);
    EXPECT_EQ(drawable->stagingShader_, shader);
}

/**
 * @tc.name: RSForegroundShaderDrawableOnUpdate001
 * @tc.desc: Foreground OnUpdate toggles with shader presence
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnUpdate001, TestSize.Level1)
{
    RSRenderNode node(0);
    auto drawable = std::make_shared<DrawableV2::RSForegroundShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    EXPECT_FALSE(drawable->OnUpdate(node));
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    node.GetMutableRenderProperties().SetForegroundShader(shader);
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_TRUE(drawable->needSync_);
    node.GetMutableRenderProperties().SetForegroundShader(nullptr);
    EXPECT_FALSE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSForegroundShaderDrawableOnSync001
 * @tc.desc: Foreground OnSync publishes the staging snapshot
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnSync001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->needSync_ = false;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    drawable->stagingShader_ = shader;
    drawable->needSync_ = true;
    drawable->stagingDrawRect_ = RectF(0, 0, 100, 100);
    drawable->stagingCornerRadius_ = 4.0f;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(drawable->drawRect_, RectF(0, 0, 100, 100));
}

/**
 * @tc.name: RSForegroundShaderDrawableOnSync002
 * @tc.desc: Foreground OnSync with null staging shader keeps needSync
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnSync002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->stagingShader_ = nullptr;
    drawable->needSync_ = true;
    drawable->OnSync();
    EXPECT_TRUE(drawable->needSync_);
    EXPECT_EQ(drawable->visualEffectContainer_, nullptr);
}

/**
 * @tc.name: RSForegroundShaderDrawableOnDraw001
 * @tc.desc: Foreground OnDraw covers null and normal paths
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnDraw001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    Drawing::Rect rect(0, 0, 100, 100);
    drawable->OnDraw(nullptr, &rect);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    drawable->OnDraw(canvas.get(), nullptr);
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->drawRect_ = RectF(0, 0, 100, 100);
    auto rectPtr = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    drawable->OnDraw(canvas.get(), rectPtr.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSForegroundShaderDrawableOnDraw002
 * @tc.desc: Foreground OnDraw with cached effect data and DRM intersect
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableOnDraw002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSForegroundShaderDrawable>();
    ASSERT_NE(drawable, nullptr);
    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->drawRect_ = RectF(0, 0, 100, 100);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rectPtr = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    effectData->cachedRect_ = Drawing::RectI(0, 0, 100, 100);
    canvas->SetEffectData(effectData);
    drawable->OnDraw(canvas.get(), rectPtr.get());
    canvas->SetEffectIntersectWithDRM(true);
    drawable->OnDraw(canvas.get(), rectPtr.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSForegroundShaderDrawableFullFlow001
 * @tc.desc: Foreground full flow OnGenerate OnSync OnDraw
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSForegroundShaderDrawableFullFlow001, TestSize.Level1)
{
    RSRenderNode node(0);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    node.GetMutableRenderProperties().SetForegroundShader(shader);
    node.GetMutableRenderProperties().SetCornerRadius(Vector4f(6.0f, 6.0f, 6.0f, 6.0f));
    auto drawable = std::static_pointer_cast<DrawableV2::RSForegroundShaderDrawable>(
        DrawableV2::RSForegroundShaderDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    drawable->OnSync();
    EXPECT_NE(drawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(drawable->cornerRadius_, 6.0f);
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 200, 200);
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawableOnGenerate001
 * @tc.desc: Test OnGenerate without material shader
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnGenerate001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSMaterialShaderDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
}

/**
 * @tc.name: RSMaterialShaderDrawableOnGenerate002
 * @tc.desc: Test OnGenerate with material shader
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnGenerate002, TestSize.Level1)
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
 * @tc.name: RSMaterialShaderDrawableOnUpdate001
 * @tc.desc: Test OnUpdate
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnUpdate001, TestSize.Level1)
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
 * @tc.name: RSMaterialShaderDrawableOnSync001
 * @tc.desc: Test OnSync
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnSync001, TestSize.Level1)
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
 * @tc.name: RSMaterialShaderDrawableOnDraw001
 * @tc.desc: Test OnDraw with null params
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnDraw001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
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
 * @tc.name: RSMaterialShaderDrawableOnDraw002
 * @tc.desc: Test OnDraw with RSPaintFilterCanvas
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnDraw002, TestSize.Level1)
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
 * @tc.name: RSMaterialShaderDrawableOnDraw003
 * @tc.desc: Test OnDraw with drawRect
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnDraw003, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialShaderDrawable>();
    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
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
 * @tc.name: RSMaterialShaderDrawableOnDraw004
 * @tc.desc: Test OnDraw with geCacheProvider
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnDraw004, TestSize.Level1)
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
 * @tc.name: RSMaterialShaderDrawableFullFlow001
 * @tc.desc: Test full flow OnGenerate OnSync OnDraw
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableFullFlow001, TestSize.Level1)
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

    auto baseCanvas = std::make_shared<Drawing::Canvas>();
    auto canvas = std::make_shared<RSPaintFilterCanvas>(baseCanvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialShaderDrawableOnUpdateSDFShape001
 * @tc.desc: Test OnUpdate with SDFShape
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSMaterialShaderDrawableOnUpdateSDFShape001, TestSize.Level1)
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

/**
 * @tc.name: RSOverlayNGShaderDrawableOnGenerate001
 * @tc.desc: OnGenerate and OnUpdate test with null shader
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnGenerate001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    // Test OnGenerate with null shader - should return nullptr
    EXPECT_EQ(overlayDrawable->OnGenerate(renderNode), nullptr);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnGenerate002
 * @tc.desc: OnGenerate with valid overlay shader
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnGenerate002, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnUpdate001
 * @tc.desc: OnUpdate extracts shader and sets needSync
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnUpdate001, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnUpdate002
 * @tc.desc: OnUpdate extracts corner radius
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnUpdate002, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnUpdate003
 * @tc.desc: OnUpdate with null shader returns false
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnUpdate003, TestSize.Level1)
{
    RSRenderNode renderNode(0);

    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    bool result = overlayDrawable->OnUpdate(renderNode);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnSync001
 * @tc.desc: OnSync creates visual effect container
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnSync001, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnSync002
 * @tc.desc: OnSync updates draw rect from staging
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnSync002, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnSync003
 * @tc.desc: OnSync without sync flag publishes nothing
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnSync003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::AURORA_NOISE);
    overlayDrawable->stagingShader_ = shader;
    overlayDrawable->stagingCornerRadius_ = 10.0f;
    overlayDrawable->stagingNodeId_ = 12345;
    overlayDrawable->needSync_ = false;

    overlayDrawable->OnSync();

    // No sync request: the whole staging snapshot stays unpublished.
    EXPECT_EQ(overlayDrawable->cornerRadius_, 0.0f);
    EXPECT_EQ(overlayDrawable->nodeId_, INVALID_NODEID);
    EXPECT_FALSE(overlayDrawable->needSync_);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnSync004
 * @tc.desc: OnSync with needSync true but stagingShader null
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnSync004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    overlayDrawable->needSync_ = true;
    overlayDrawable->stagingShader_ = nullptr;  // Key: shader is null
    overlayDrawable->stagingCornerRadius_ = 5.0f;
    overlayDrawable->stagingNodeId_ = 123;

    overlayDrawable->OnSync();

    // No staged shader means the base sync is skipped entirely: no container rebuild and no
    // member publishing, needSync_ is kept.
    EXPECT_EQ(overlayDrawable->visualEffectContainer_, nullptr);
    EXPECT_EQ(overlayDrawable->cornerRadius_, 0.0f);
    EXPECT_EQ(overlayDrawable->nodeId_, INVALID_NODEID);
    EXPECT_TRUE(overlayDrawable->needSync_);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnSync005
 * @tc.desc: OnSync with needSync false and stagingShader valid
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnSync005, TestSize.Level1)
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
    EXPECT_EQ(overlayDrawable->cornerRadius_, 0.0f);
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnDraw001
 * @tc.desc: OnDraw with null canvas returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    Drawing::Rect rect(0, 0, 100, 100);
    overlayDrawable->OnDraw(nullptr, &rect);
    // Should return early without errors
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnDraw002
 * @tc.desc: OnDraw with null rect returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw002, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSOverlayNGShaderDrawable> overlayDrawable =
        std::make_shared<DrawableV2::RSOverlayNGShaderDrawable>();
    EXPECT_NE(overlayDrawable, nullptr);

    Drawing::Canvas canvas;
    overlayDrawable->OnDraw(&canvas, nullptr);
    // Should return early without errors
}

/**
 * @tc.name: RSOverlayNGShaderDrawableOnDraw003
 * @tc.desc: OnDraw with null visualEffectContainer returns early
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw003, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnDraw004
 * @tc.desc: OnDraw with valid visualEffectContainer
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw004, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnDraw005
 * @tc.desc: OnDraw with empty drawRect
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw005, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnDraw006
 * @tc.desc: OnDraw with non-empty drawRect
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw006, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnDraw007
 * @tc.desc: OnDraw with RSPaintFilterCanvas and cached effect data
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw007, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableOnDraw008
 * @tc.desc: OnDraw with DRM intersect enabled
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableOnDraw008, TestSize.Level1)
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
 * @tc.name: RSOverlayNGShaderDrawableFullFlow001
 * @tc.desc: Full flow test: OnGenerate, OnSync, OnDraw
 * @tc.type: FUNC
 * @tc.require:issueI9SCBR
 */
HWTEST_F(RSShaderDrawableTest, RSOverlayNGShaderDrawableFullFlow001, TestSize.Level1)
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

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnGenerate001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnGenerate001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node);
    EXPECT_EQ(drawable, nullptr);

    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    node.GetMutableRenderProperties().SetBackgroundNGShader(shader);
    EXPECT_NE(DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node), nullptr);

    auto borderColor = Color(255, 255, 255, 255);
    auto borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    node.GetMutableRenderProperties().SetBorderColor({ borderColor, borderColor, borderColor, borderColor });
    node.GetMutableRenderProperties().SetBorderStyle({ borderStyle, borderStyle, borderStyle, borderStyle });

    EXPECT_NE(DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnUpdate001
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnUpdate001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node);
    EXPECT_EQ(drawable, nullptr);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    node.GetMutableRenderProperties().SetBackgroundNGShader(shader);
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSBackgroundNGShaderDrawable>(
        DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node));
    EXPECT_NE(drawableTwo, nullptr);
    EXPECT_TRUE(drawableTwo->OnUpdate(node));
    EXPECT_EQ(drawableTwo->stagingShader_, shader);
    EXPECT_EQ(drawableTwo->needSync_, true);
    node.GetMutableRenderProperties().SetBackgroundNGShader(nullptr);
    EXPECT_FALSE(drawableTwo->OnUpdate(node));
}

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnSync001
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnSync001, TestSize.Level1)
{
    DrawableV2::RSBackgroundNGShaderDrawable drawable;
    drawable.needSync_ = false;
    drawable.OnSync();
    EXPECT_FALSE(drawable.needSync_);

    drawable.needSync_ = true;
    drawable.OnSync();
    EXPECT_TRUE(drawable.needSync_);

    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    drawable.stagingShader_ = shader;
    drawable.OnSync();
    EXPECT_FALSE(drawable.needSync_);
    EXPECT_NE(drawable.visualEffectContainer_, nullptr);
}

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnDraw001
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnDraw001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundNGShaderDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    drawable->visualEffectContainer_ = std::make_shared<Drawing::GEVisualEffectContainer>();
    drawable->OnDraw(canvas.get(), nullptr);
    ASSERT_TRUE(true);

    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);

    Drawing::Canvas* filtercanvas = new Drawing::Canvas();
    auto rspaintfiltercanvas = std::make_shared<RSPaintFilterCanvas>(filtercanvas);
    drawable->OnDraw(rspaintfiltercanvas.get(), rect.get());
    ASSERT_TRUE(true);

    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData =
        std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    std::shared_ptr<Drawing::Image> cachedImage = std::make_shared<Drawing::Image>();
    effectData->cachedImage_ = cachedImage;
    rspaintfiltercanvas->SetEffectData(effectData);
    drawable->OnDraw(rspaintfiltercanvas.get(), rect.get());
    ASSERT_TRUE(true);

    rspaintfiltercanvas->SetEffectIntersectWithDRM(true);
    drawable->OnDraw(rspaintfiltercanvas.get(), rect.get());
}

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnUpdateSDFShape001
 * @tc.desc: Test OnUpdate with SDFShape set (true branch)
 * @tc.type:FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnUpdateSDFShape001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    node.GetMutableRenderProperties().SetBackgroundNGShader(shader);

    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    auto drawable = std::static_pointer_cast<DrawableV2::RSBackgroundNGShaderDrawable>(
        DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node));
    EXPECT_NE(drawable, nullptr);

    EXPECT_TRUE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSBackgroundNGShaderDrawableOnUpdateNoSDFShape001
 * @tc.desc: Test OnUpdate without SDFShape (false branch)
 * @tc.type:FUNC
 */
HWTEST_F(RSShaderDrawableTest, RSBackgroundNGShaderDrawableOnUpdateNoSDFShape001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto shader = RSNGRenderShaderBase::Create(RSNGEffectType::CONTOUR_DIAGONAL_FLOW_LIGHT);
    node.GetMutableRenderProperties().SetBackgroundNGShader(shader);

    auto drawable = std::static_pointer_cast<DrawableV2::RSBackgroundNGShaderDrawable>(
        DrawableV2::RSBackgroundNGShaderDrawable::OnGenerate(node));
    EXPECT_NE(drawable, nullptr);

    EXPECT_TRUE(drawable->OnUpdate(node));
}

} // namespace OHOS::Rosen

