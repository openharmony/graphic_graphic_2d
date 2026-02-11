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

#include "buffer_handle.h"
#include "buffer_handle_utils.h"
#include "gtest/gtest.h"
#include "parameters.h"
#include "surface_buffer_impl.h"

#include "drawable/rs_property_drawable_background.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "ge_visual_effect_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRSBinarizationDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

void RSRSBinarizationDrawableTest::SetUpTestCase() {}
void RSRSBinarizationDrawableTest::TearDownTestCase() {}
void RSRSBinarizationDrawableTest::SetUp() {}
void RSRSBinarizationDrawableTest::TearDown() {}

/**
 * @tc.name: RSShadowDrawable001
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSShadowDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSShadowDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.GetMutableRenderProperties().SetShadowIsFilled(true);
    node.GetMutableRenderProperties().SetShadowDisableSDFBlur(true);
    node.GetMutableRenderProperties().GetEffect().shadow_->radius_ = 1.0f;
    ASSERT_TRUE(node.GetRenderProperties().IsShadowValid());
    node.GetMutableRenderProperties().GetEffect().shadow_->SetMask(true);
    std::shared_ptr<RSDrawable> drawableTwo = DrawableV2::RSShadowDrawable::OnGenerate(node);
    node.GetMutableRenderProperties().GetEffect().shadow_->SetMask(false);
    node.GetMutableRenderProperties().GetEffect().shadow_->SetElevation(1.0f);
    ASSERT_TRUE(node.GetRenderProperties().GetShadowElevation() > 0.f);
    std::shared_ptr<RSDrawable> drawableThree = DrawableV2::RSShadowDrawable::OnGenerate(node);
    std::shared_ptr<DrawableV2::RSShadowDrawable> rsShadowDrawable =
        std::static_pointer_cast<DrawableV2::RSShadowDrawable>(drawableThree);
    ASSERT_NE(rsShadowDrawable, nullptr);
    rsShadowDrawable->OnSync();
    ASSERT_FALSE(rsShadowDrawable->needSync_);
    rsShadowDrawable->needSync_ = true;
    rsShadowDrawable->OnSync();
    ASSERT_FALSE(rsShadowDrawable->needSync_);
    node.GetMutableRenderProperties().GetEffect().shadow_->SetElevation(0);
    std::shared_ptr<RSDrawable> drawableFour = DrawableV2::RSShadowDrawable::OnGenerate(node);
    ASSERT_NE(drawableFour, nullptr);
}

/**
 * @tc.name: RSShadowDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSShadowDrawable002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSShadowDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetCacheType(Drawing::CacheType::ENABLED);
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    filterCanvas->SetCacheType(Drawing::CacheType::UNDEFINED);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    drawable->radius_ = 1.0f;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->radius_ = 0.f;
    drawable->elevation_ = 1.0f;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSShadowDrawable003
 * @tc.desc: Shadow test with sdf filter
 * @tc.type: FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSShadowDrawable003, TestSize.Level1)
{
    NodeId id = 3;
    RSRenderNode node(id);
    auto shadowDrawable = std::make_shared<DrawableV2::RSShadowDrawable>();
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    EXPECT_NE(sdfShape, nullptr);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);

    Color shadowColor = Color();
    node.GetMutableRenderProperties().SetShadowColor(shadowColor);
    node.GetMutableRenderProperties().SetShadowRadius(1.0f);
    shadowDrawable->OnUpdate(node);
    shadowDrawable->OnSync();
    EXPECT_TRUE(shadowDrawable->geContainer_ != nullptr);

    auto rect = std::make_shared<Drawing::Rect>();
    Drawing::Canvas canvas;
    shadowDrawable->OnDraw(&canvas, rect.get());
    ASSERT_TRUE(true);

    shadowDrawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);

    shadowDrawable->OnDraw(&canvas, nullptr);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaskDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaskDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSMaskDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    mask->type_ = MaskType::SVG;
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_EQ(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->svgPicture_ = std::make_shared<Drawing::Picture>();
    node.GetMutableRenderProperties().GetEffect().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::GRADIENT;
    node.GetMutableRenderProperties().GetEffect().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::PATH;
    node.GetMutableRenderProperties().GetEffect().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::PIXEL_MAP;
    Media::InitializationOptions opts;
    opts.size.width = 50;
    opts.size.height = 50;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelMap.release());
    mask->SetPixelMap(shpPixelMap);
    node.GetMutableRenderProperties().GetEffect().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundColorDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundColorDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundColorDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.GetMutableRenderProperties().SetBackgroundColor(Color(1, 1, 1, 1));
    ASSERT_NE(DrawableV2::RSBackgroundColorDrawable::OnGenerate(node), nullptr);
    std::optional<RSDynamicBrightnessPara> params = RSDynamicBrightnessPara();
    node.GetMutableRenderProperties().SetBgBrightnessParams(params);
    node.GetMutableRenderProperties().SetBgBrightnessFract(0.0f);
    ASSERT_NE(DrawableV2::RSBackgroundColorDrawable::OnGenerate(node), nullptr);
    auto borderColor = Color(255, 255, 255, 255);
    auto borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    node.GetMutableRenderProperties().SetBorderColor({ borderColor, borderColor, borderColor, borderColor });
    node.GetMutableRenderProperties().SetBorderStyle({ borderStyle, borderStyle, borderStyle, borderStyle });
    ASSERT_NE(DrawableV2::RSBackgroundColorDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundShaderDrawable
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundShaderDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    node.GetMutableRenderProperties().SetBackgroundShader(shader);
    ASSERT_NE(DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node), nullptr);
    auto borderColor = Color(255, 255, 255, 255);
    auto borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    node.GetMutableRenderProperties().SetBorderColor({ borderColor, borderColor, borderColor, borderColor });
    node.GetMutableRenderProperties().SetBorderStyle({ borderStyle, borderStyle, borderStyle, borderStyle });
    ASSERT_NE(DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node), nullptr);
}
/**
 * @tc.name: RSBackgroundNGShaderDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundNGShaderDrawable001, TestSize.Level1)
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
 * @tc.name: RSBackgroundNGShaderDrawable002
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundNGShaderDrawable002, TestSize.Level1)
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
 * @tc.name: RSBackgroundNGShaderDrawable003
 * @tc.desc: Test Onsync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundNGShaderDrawable003, TestSize.Level1)
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
 * @tc.name: RSBackgroundNGShaderDrawable004
 * @tc.desc: Test Onsync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundNGShaderDrawable004, TestSize.Level1)
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
 * @tc.name: RSBackgroundImageDrawable001
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundImageDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSImage> shader = std::make_shared<RSImage>();
    auto pixelmap = std::make_shared<Media::PixelMap>();
    shader->SetPixelMap(pixelmap);
    node.GetMutableRenderProperties().SetBgImage(shader);
    ASSERT_NE(DrawableV2::RSBackgroundImageDrawable::OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSBackgroundImageDrawable002
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable002, TestSize.Level1)
{
    DrawableV2::RSBackgroundImageDrawable drawable;
    drawable.needSync_ = false;
    drawable.boundsRect_ = Drawing::Rect(0, 0, 200, 200);
    drawable.OnSync();
    EXPECT_EQ(drawable.boundsRect_, Drawing::Rect(0, 0, 200, 200));
    drawable.needSync_ = true;
    drawable.OnSync();
    EXPECT_EQ(drawable.needSync_, false);
}

/**
 * @tc.name: RSBackgroundImageDrawable003
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable003, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundImageDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
    auto image = std::make_shared<RSImage>();
    Media::InitializationOptions opts;
    opts.size.width = 50;
    opts.size.height = 50;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelmap.release());
    shpPixelMap->SetAstc(true);
    image->SetPixelMap(shpPixelMap);
    drawable->bgImage_ = image;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RSBackgroundImageDrawable004
 * @tc.desc: Test GetColorTypeFromVKFormat
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable004, TestSize.Level1)
{
    EXPECT_EQ(Drawing::COLORTYPE_RGBA_8888,
        DrawableV2::RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat::VK_FORMAT_R8G8B8A8_UNORM));
    EXPECT_EQ(Drawing::COLORTYPE_RGBA_F16,
        DrawableV2::RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT));
    EXPECT_EQ(Drawing::COLORTYPE_RGB_565,
        DrawableV2::RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat::VK_FORMAT_R5G6B5_UNORM_PACK16));
    EXPECT_EQ(Drawing::COLORTYPE_RGBA_8888,
        DrawableV2::RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat::VK_FORMAT_R8_SRGB));
    EXPECT_EQ(Drawing::COLORTYPE_RGBA_1010102,
        DrawableV2::RSBackgroundImageDrawable::GetColorTypeFromVKFormat(VkFormat::VK_FORMAT_A2B10G10R10_UNORM_PACK32));
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RSBackgroundImageDrawable005
 * @tc.desc: Test MakeFromTextureForVK
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable005, TestSize.Level1)
{
    DrawableV2::RSBackgroundImageDrawable drawable;
    Drawing::Canvas canvas;
    drawable.MakeFromTextureForVK(canvas, nullptr);
    ASSERT_TRUE(true);
    SurfaceBufferImpl* buffer = new SurfaceBufferImpl();
    drawable.MakeFromTextureForVK(canvas, buffer);
    ASSERT_TRUE(true);
    BufferHandle* handle = AllocateBufferHandle(1025, 1025);
    buffer->SetBufferHandle(handle);
    drawable.MakeFromTextureForVK(canvas, buffer);
    ASSERT_TRUE(true);
    delete buffer;
}
#endif

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
/**
 * @tc.name: RSBackgroundImageDrawable006
 * @tc.desc: Test SetCompressedDataForASTC
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable006, TestSize.Level1)
{
    auto image = std::make_shared<RSImage>();
    Media::InitializationOptions opts;
    opts.size.width = 50;
    opts.size.height = 50;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto shpPixelMap = std::shared_ptr<Media::PixelMap>(pixelmap.release());
    DrawableV2::RSBackgroundImageDrawable drawable;
    drawable.bgImage_ = image;
    drawable.SetCompressedDataForASTC();
    ASSERT_TRUE(true);
    drawable.bgImage_->SetPixelMap(shpPixelMap);
    drawable.SetCompressedDataForASTC();
    ASSERT_TRUE(true);
}
#endif

/**
 * @tc.name: RSBackgroundImageDrawable007
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable007, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundImageDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    auto image = std::make_shared<RSImage>();
    drawable->bgImage_ = image;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    auto shpPixelMap = std::shared_ptr<Media::PixelMap>();
    image->SetPixelMap(shpPixelMap);
    drawable->bgImage_ = image;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    auto shpPixelMap1 = std::shared_ptr<Media::PixelMap>();
    image->SetPixelMap(shpPixelMap1);
    drawable->bgImage_ = image;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    auto shpPixelMap2 = std::shared_ptr<Media::PixelMap>();
    shpPixelMap2->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    image->SetPixelMap(shpPixelMap2);
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);

    auto surfaceBuffer = SurfaceBuffer::Create();
    surfaceBuffer->SetBufferHandle(nullptr);
    image->GetPixelMap()->context_ = surfaceBuffer;
    drawable->bgImage_ = image;
    drawable->OnDraw(canvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSBackgroundFilterDrawable
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundFilterDrawable, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::shared_ptr<RSFilter> backgroundFilter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node.GetMutableRenderProperties().backgroundFilter_ = backgroundFilter;
    ASSERT_NE(DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node), nullptr);
    RSEffectRenderNode nodeTwo(id);
    nodeTwo.GetMutableRenderProperties().backgroundFilter_ = backgroundFilter;
    ASSERT_TRUE(nodeTwo.IsInstanceOf<RSEffectRenderNode>());
    ASSERT_TRUE(nodeTwo.GetRenderProperties().GetBackgroundFilter());
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSBackgroundEffectDrawable>(
        DrawableV2::RSBackgroundFilterDrawable::OnGenerate(nodeTwo));
    ASSERT_NE(drawableTwo, nullptr);
    drawableTwo->OnSync();
    auto drawableThree = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    node.GetMutableRenderProperties().backgroundFilter_ = nullptr;
    ASSERT_FALSE(drawableThree->OnUpdate(node));
    auto drawableFour = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_FALSE(drawableFour->OnUpdate(node));
}

/**
 * @tc.name: RSBackgroundEffectDrawableOnDrawTest001
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundEffectDrawableOnDrawTest001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    int width = 1270;
    int height = 2560;
    Drawing::ImageInfo imageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    Drawing::Surface* surfacePtr = surface.get();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(surfacePtr);
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSBackgroundEffectDrawableOnDrawTest002
 * @tc.desc: Test OnDraw with empty rect
 * @tc.type:FUNC
 * @tc.require: issue20322
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundEffectDrawableOnDrawTest002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    int width = 1270;
    int height = 2560;
    Drawing::ImageInfo imageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    Drawing::Surface* surfacePtr = surface.get();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(surfacePtr);
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSBackgroundEffectDrawableOnDrawTest003
 * @tc.desc: Test OnDraw
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundEffectDrawableOnDrawTest003, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    int width = 1270;
    int height = 2560;
    Drawing::ImageInfo imageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    Drawing::Surface* surfacePtr = surface.get();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(surfacePtr);
    auto rect = std::make_shared<Drawing::Rect>(0, 0, 100, 100);
    auto bound = RectF(rect->GetLeft(), rect->GetTop(), rect->GetWidth(), rect->GetHeight());
    auto boundsRect = drawable->GetAbsRenderEffectRect(*filterCanvas, EffectRectType::SNAPSHOT, bound);
    ASSERT_NE(boundsRect, Drawing::RectI());
    drawable->OnDraw(filterCanvas.get(), rect.get());
    drawable->filter_ = std::make_shared<RSDrawingFilter>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    drawable->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->renderRelativeRectInfo_->snapshotRect_ = bound;
    drawable->renderRelativeRectInfo_->drawRect_ = bound;
    bound = RectF();
    boundsRect = drawable->GetAbsRenderEffectRect(*filterCanvas, EffectRectType::SNAPSHOT, bound);
    ASSERT_NE(boundsRect, Drawing::RectI());
    drawable->OnDraw(filterCanvas.get(), nullptr);
}

/**
 * @tc.name: RSUseEffectDrawable001
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.GetMutableRenderProperties().SetUseEffect(true);
    auto drawableTwo =
        std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(DrawableV2::RSUseEffectDrawable::OnGenerate(node));
    ASSERT_NE(drawableTwo, nullptr);
    ASSERT_TRUE(drawableTwo->OnUpdate(node));
    node.GetMutableRenderProperties().SetUseEffect(false);
    ASSERT_FALSE(drawableTwo->OnUpdate(node));
    auto nodeTwo = std::make_shared<RSEffectRenderNode>(2);
    auto nodeThree = std::make_shared<RSRenderNode>(3);
    nodeTwo->AddChild(nodeThree, 1);
    nodeThree->GetMutableRenderProperties().SetUseEffect(true);
    auto drawableThree = std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(
        DrawableV2::RSUseEffectDrawable::OnGenerate(*nodeThree.get()));
    ASSERT_TRUE(drawableThree->OnUpdate(*nodeThree.get()));
    auto nodeFour = std::make_shared<RSRenderNode>(4);
    auto nodeFive = std::make_shared<RSRenderNode>(5);
    nodeFour->AddChild(nodeFive, 1);
    nodeFive->GetMutableRenderProperties().SetUseEffect(true);
    auto drawableFour = std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(
        DrawableV2::RSUseEffectDrawable::OnGenerate(*nodeFive.get()));
    ASSERT_TRUE(drawableFour->OnUpdate(*nodeFive.get()));
}

/**
 * @tc.name: RSUseEffectDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable002, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->OnDraw(nullptr, nullptr);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSUseEffectDrawable003
 * @tc.desc: Test RSUseEffectDrawable behind window branch OnGenerate, OnUpdate and OnSync
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(1);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto useEffectDrawable = std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(drawable);
    ASSERT_EQ(useEffectDrawable->useEffectType_, UseEffectType::BEHIND_WINDOW);
    node.GetMutableRenderProperties().SetUseEffectType(0);
    ASSERT_TRUE(useEffectDrawable->OnUpdate(node));
    ASSERT_EQ(useEffectDrawable->stagingUseEffectType_, UseEffectType::EFFECT_COMPONENT);
    ASSERT_TRUE(useEffectDrawable->needSync_);
    useEffectDrawable->OnSync();
    ASSERT_EQ(useEffectDrawable->useEffectType_, UseEffectType::EFFECT_COMPONENT);
    ASSERT_FALSE(useEffectDrawable->needSync_);
}

/**
 * @tc.name: RSUseEffectDrawable004
 * @tc.desc: Test RSUseEffectDrawable behind window branch OnDraw
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable004, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(1);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSUseEffectDrawable005
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable005, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(1);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetEffectIntersectWithDRM(false);
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable006
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable006, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(2);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetEffectIntersectWithDRM(true);
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable007
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable007, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetEffectIntersectWithDRM(true);
    ASSERT_TRUE(filterCanvas->GetEffectIntersectWithDRM());
    auto rect = std::make_shared<Drawing::Rect>();
    auto rsContext = std::make_shared<RSContext>();
    auto rsRenderNode = std::make_shared<RSRenderNode>(9, rsContext);
    rsRenderNode->GetMutableRenderProperties().SetUseEffect(true);
    rsRenderNode->GetMutableRenderProperties().SetUseEffectType(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto drawableTwo = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(rsRenderNode);
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>(drawableTwo);
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable008
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable008, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetEffectIntersectWithDRM(false);
    ASSERT_FALSE(filterCanvas->GetEffectIntersectWithDRM());
    auto rect = std::make_shared<Drawing::Rect>();
    auto rsContext = std::make_shared<RSContext>();
    auto rsRenderNode = std::make_shared<RSRenderNode>(9, rsContext);
    rsRenderNode->GetMutableRenderProperties().SetUseEffect(true);
    rsRenderNode->GetMutableRenderProperties().SetUseEffectType(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto drawableTwo = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(rsRenderNode);
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>(drawableTwo);
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable009
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable009, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    filterCanvas->SetIsParallelCanvas(true);

    auto rect = std::make_shared<Drawing::Rect>();
    auto rsContext = std::make_shared<RSContext>();
    auto rsRenderNode = std::make_shared<RSRenderNode>(9, rsContext);
    rsRenderNode->GetMutableRenderProperties().SetUseEffect(true);
    rsRenderNode->GetMutableRenderProperties().SetUseEffectType(0);
    ASSERT_NE(rsRenderNode, nullptr);
    auto drawableTwo = std::make_shared<ConcreteRSRenderNodeDrawableAdapter>(rsRenderNode);
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>(drawableTwo);
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable010
 * @tc.desc: Test RSUseEffectDrawable behind window branch OnDraw
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable010, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(1);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();

    filterCanvas->SetIsDrawingCache(true);
    filterCanvas->SetIsWindowFreezeCapture(false);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    filterCanvas->SetIsDrawingCache(false);
    filterCanvas->SetIsWindowFreezeCapture(true);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    filterCanvas->SetIsDrawingCache(true);
    filterCanvas->SetIsWindowFreezeCapture(true);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>();
    filterCanvas->SetCacheBehindWindowData(data);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    filterCanvas->SetIsWindowFreezeCapture(false);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSUseEffectDrawable011
 * @tc.desc: Test RSUseEffectDrawable behind window branch OnDraw
 * @tc.type:FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable011, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().SetUseEffect(true);
    node.GetMutableRenderProperties().SetUseEffectType(0);
    auto drawable = DrawableV2::RSUseEffectDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();

    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    filterCanvas->SetEffectData(effectData);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    effectData = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    EXPECT_NE(effectData, nullptr);
    effectData->cachedImage_ = nullptr;
    filterCanvas->SetEffectData(effectData);
    drawable->OnDraw(filterCanvas.get(), rect.get());

    std::shared_ptr<Drawing::Image> cachedImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(cachedImage, nullptr);
    effectData->cachedImage_ = cachedImage;
    filterCanvas->SetEffectData(effectData);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSDynamicLightUpDrawable001
 * @tc.desc: Test OnSync
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSDynamicLightUpDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSDynamicLightUpDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    std::optional<float> rate = { 1.0f };
    node.GetMutableRenderProperties().SetDynamicLightUpRate(rate);
    std::optional<float> lightUpDegree = { 0.0f };
    node.GetMutableRenderProperties().SetDynamicLightUpDegree(lightUpDegree);
    ASSERT_TRUE(node.GetMutableRenderProperties().IsDynamicLightUpValid());
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSDynamicLightUpDrawable>(
        DrawableV2::RSDynamicLightUpDrawable::OnGenerate(node));
    ASSERT_NE(drawableTwo, nullptr);
    ASSERT_TRUE(drawableTwo->OnUpdate(node));
    node.GetMutableRenderProperties().SetDynamicLightUpRate(lightUpDegree);
    ASSERT_FALSE(drawableTwo->OnUpdate(node));
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->needSync_);
    drawableTwo->OnSync();
    ASSERT_FALSE(drawableTwo->needSync_);
    ASSERT_TRUE(DrawableV2::RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(1.0f, 1.0f, 1.0f));
    ASSERT_TRUE(DrawableV2::RSDynamicLightUpDrawable::MakeDynamicLightUpBlender(1.0f, 1.0f, 1.0f));
}

/**
 * @tc.name: RSDynamicLightUpDrawable002
 * @tc.desc: Test OnDraw
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSDynamicLightUpDrawable002, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawable = std::make_shared<DrawableV2::RSDynamicLightUpDrawable>(1.f, 1.f);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    filterCanvas->SetUICapture(true);
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnGenerate001
 * @tc.desc: Test OnGenerate
 * @tc.type: FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnGenerate001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    node.GetMutableRenderProperties().GetEffect().materialFilter_ = nullptr;
    auto drawable = DrawableV2::RSMaterialFilterDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnGenerate002
 * @tc.desc: Test OnGenerate
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnGenerate002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSFilter> filter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node.GetMutableRenderProperties().GetEffect().materialFilter_ = filter;
    auto drawable = DrawableV2::RSMaterialFilterDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnUpdate001
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnUpdate001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_FALSE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSMaterialFilterDrawableOnUpdate002
 * @tc.desc: Test OnUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnUpdate002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    std::shared_ptr<RSFilter> filter =
        std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node.GetMutableRenderProperties().GetEffect().materialFilter_ = filter;
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    ASSERT_TRUE(drawable->OnUpdate(node));
}

/**
 * @tc.name: RSMaterialFilterDrawableGetAbsRenderEffectRect001
 * @tc.desc: Test GetAbsRenderEffectRect
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableGetAbsRenderEffectRect001, TestSize.Level1)
{
    auto bound = RectF(-5.0f, -5.0f, 8.0f, 8.0f);
    Drawing::Canvas canvas;
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    auto absRenderEffectRect = drawable->GetAbsRenderEffectRect(canvas, EffectRectType::SNAPSHOT, bound);
    auto surface = Drawing::Surface::MakeRasterN32Premul(10, 10);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    paintFilterCanvas.surface_ = surface.get();

    drawable->renderRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->renderRelativeRectInfo_->snapshotRect_ = bound;
    drawable->renderRelativeRectInfo_->drawRect_ = bound;
    absRenderEffectRect = drawable->GetAbsRenderEffectRect(paintFilterCanvas, EffectRectType::SNAPSHOT, bound);
    auto deviceRect = RectI(0, 0, surface->Width(), surface->Height());
    auto effectRect = RectI(std::floor(bound.GetLeft()), std::floor(bound.GetTop()),
        std::ceil(bound.GetWidth()), std::ceil(bound.GetHeight()));
    auto result = effectRect.IntersectRect(deviceRect);
    EXPECT_EQ(absRenderEffectRect,
        Drawing::RectI(result.GetLeft(), result.GetTop(), result.GetRight(), result.GetBottom()));
}

/**
 * @tc.name: RSMaterialFilterDrawableCalVisibleRect001
 * @tc.desc: Test CalVisibleRect
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableCalVisibleRect001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    Drawing::Canvas canvas;
    RectF defaultRelativeRect(0.0f, 0.0f, 10.0f, 10.0f);
    drawable->CalVisibleRect(canvas.GetTotalMatrix(), std::nullopt, defaultRelativeRect);
    EXPECT_EQ(drawable->stagingVisibleRectInfo_, nullptr);

    RectF snapshotRect = RectF(-5.0f, -5.0f, 15.0f, 15.0f);
    RectF drawRect = RectF(-2.0f, -2.0f, 18.0f, 18.0f);
    RectF totalRect = snapshotRect.JoinRect(drawRect);
    RectI clipRect = RectI(-4, -4, 17, 17);
    drawable->stagingRelativeRectInfo_ = std::make_unique<DrawableV2::RSFilterDrawable::FilterRectInfo>();
    drawable->stagingRelativeRectInfo_->snapshotRect_ = snapshotRect;
    drawable->stagingRelativeRectInfo_->drawRect_ = drawRect;
    drawable->CalVisibleRect(canvas.GetTotalMatrix(), clipRect, defaultRelativeRect);
    ASSERT_NE(drawable->stagingVisibleRectInfo_, nullptr);
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->snapshotRect_, snapshotRect.ConvertTo<int>().IntersectRect(clipRect));
    EXPECT_EQ(drawable->stagingVisibleRectInfo_->totalRect_, totalRect.ConvertTo<int>().IntersectRect(clipRect));
}

/**
 * @tc.name: RSMaterialFilterDrawableOnSync001
 * @tc.desc: needSync == false
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnSync001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    drawable->needSync_ = false;
    drawable->stagingEmptyShape_ = true;
    drawable->OnSync();
    ASSERT_FALSE(drawable->emptyShape_);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnSync002
 * @tc.desc: needSync == true
 * @tc.type:FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnSync002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    drawable->needSync_ = true;
    drawable->stagingEmptyShape_ = true;
    drawable->OnSync();
    ASSERT_TRUE(drawable->emptyShape_);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnDraw001
 * @tc.desc: emptyShape_ == true
 * @tc.type: FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnDraw001, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    drawable->emptyShape_ = true;
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);

    drawable->OnDraw(&canvas, &rect);
    ASSERT_TRUE(drawable->emptyShape_);

    drawable->emptyShape_ = false;
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.0f, fractionStops, GradientDirection::LEFT);
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.0f, 1.0f);
    drawable->stagingFilter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    drawable->stagingFilter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    drawable->OnSync();
    drawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    drawable->renderIntersectWithDRM_ = true;
    drawable->OnDraw(&canvas, &rect);
    ASSERT_FALSE(drawable->emptyShape_);
}

/**
 * @tc.name: RSMaterialFilterDrawableOnDraw002
 * @tc.desc: filter->SetGeoMetry
 * @tc.type: FUNC
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSMaterialFilterDrawableOnDraw002, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSMaterialFilterDrawable>();
    drawable->needSync_ = true;

    Drawing::Canvas canvas;
    std::vector<std::pair<float, float>> fractionStops;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, GradientDirection::LEFT);
    auto shaderFilter = std::make_shared<RSLinearGradientBlurShaderFilter>(para, 1.f, 1.f);
    drawable->stagingFilter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    drawable->stagingFilter_->SetFilterType(RSFilter::LINEAR_GRADIENT_BLUR);
    drawable->OnSync();

    // Test rect == nullptr
    drawable->OnDraw(&canvas, nullptr);
    auto drawingFilter = std::static_pointer_cast<RSDrawingFilter>(drawable->filter_);
    ASSERT_NE(drawingFilter->visualEffectContainer_, nullptr);

    // Test rect != nullptr
    Drawing::Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    drawable->OnDraw(&canvas, &rect);
    ASSERT_NE(drawingFilter->visualEffectContainer_, nullptr);
}
} // namespace OHOS::Rosen