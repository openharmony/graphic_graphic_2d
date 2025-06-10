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

#include "drawable/rs_property_drawable_background.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_effect_render_node.h"
#include "render/rs_drawing_filter.h"
#include "include/core/SkStream.h"
#include "surface_buffer_impl.h"
#include "buffer_handle.h"
#include "buffer_handle_utils.h"
#include "parameters.h"

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
    node.GetMutableRenderProperties().shadow_->radius_ = 1.0f;
    ASSERT_TRUE(node.GetRenderProperties().IsShadowValid());
    node.GetMutableRenderProperties().shadow_->SetMask(true);
    std::shared_ptr<RSDrawable> drawableTwo = DrawableV2::RSShadowDrawable::OnGenerate(node);
    node.GetMutableRenderProperties().shadow_->SetMask(false);
    node.GetMutableRenderProperties().shadow_->SetElevation(1.0f);
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
    node.GetMutableRenderProperties().shadow_->SetElevation(0);
    std::shared_ptr<RSDrawable> drawableFour = DrawableV2::RSShadowDrawable::OnGenerate(node);
    ASSERT_NE(drawableFour, nullptr);
}

/**
 * @tc.name: RSShadowDrawable002
 * @tc.desc: Test CreateDrawFunc
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    filterCanvas->SetCacheType(Drawing::CacheType::UNDEFINED);
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->colorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    drawable->radius_ = 1.0f;
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawable->radius_ = 0.f;
    drawable->elevation_ = 1.0f;
    drawFunc(filterCanvas.get(), rect.get());
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
    std::shared_ptr<RSMask> mask =  std::make_shared<RSMask>();
    mask->type_ = MaskType::SVG;
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_EQ(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->svgPicture_ = std::make_shared<Drawing::Picture>();
    node.GetMutableRenderProperties().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::GRADIENT;
    node.GetMutableRenderProperties().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::PATH;
    node.GetMutableRenderProperties().mask_.reset();
    node.GetMutableRenderProperties().SetMask(mask);
    ASSERT_NE(DrawableV2::RSMaskDrawable::OnGenerate(node), nullptr);
    mask->type_ = MaskType::PIXEL_MAP;
    Media::InitializationOptions opts;
    opts.size.width = 50;
    opts.size.height = 50;
    auto pixelMap = Media::PixelMap::Create(opts);
    auto shpPixelMap =  std::shared_ptr<Media::PixelMap>(pixelMap.release());
    mask->SetPixelMap(shpPixelMap);
    node.GetMutableRenderProperties().mask_.reset();
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
    node.GetMutableRenderProperties().SetBorderColor(
        { borderColor, borderColor, borderColor, borderColor });
    node.GetMutableRenderProperties().SetBorderStyle(
        { borderStyle, borderStyle, borderStyle, borderStyle });
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
    node.GetMutableRenderProperties().SetBorderColor(
        { borderColor, borderColor, borderColor, borderColor });
    node.GetMutableRenderProperties().SetBorderStyle(
        { borderStyle, borderStyle, borderStyle, borderStyle });
    ASSERT_NE(DrawableV2::RSBackgroundShaderDrawable::OnGenerate(node), nullptr);
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
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundImageDrawable003, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundImageDrawable>();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(canvas.get(), rect.get());
    ASSERT_TRUE(true);
    auto image = std::make_shared<RSImage>();
    Media::InitializationOptions opts;
    opts.size.width = 50;
    opts.size.height = 50;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto shpPixelMap =  std::shared_ptr<Media::PixelMap>(pixelmap.release());
    shpPixelMap->SetAstc(true);
    image->SetPixelMap(shpPixelMap);
    drawable->bgImage_ = image;
    drawFunc(canvas.get(), rect.get());
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
    auto shpPixelMap =  std::shared_ptr<Media::PixelMap>(pixelmap.release());
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
    ASSERT_TRUE(drawableTwo->CreateDrawFunc());
    auto drawableThree = std::make_shared<DrawableV2::RSBackgroundFilterDrawable>();
    node.GetMutableRenderProperties().backgroundFilter_ = nullptr;
    ASSERT_FALSE(drawableThree->OnUpdate(node));
    auto drawableFour = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    ASSERT_FALSE(drawableFour->OnUpdate(node));
}

/**
 * @tc.name: RSBackgroundFilterDrawable002
 * @tc.desc: Test NeedBehindWindow branch
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundFilterDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNode node(id, rsContext);
    // GetBehindWindowFilter test
    ASSERT_EQ(DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node), nullptr);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list { std::make_shared<RSDrawCmdListRenderModifier>(property) };
    node.drawCmdModifiers_.emplace(RSModifierType::BEHIND_WINDOW_FILTER_RADIUS, list);
    float radius = 0.f;
    ASSERT_TRUE(DrawableV2::RSBackgroundFilterDrawable::GetModifierProperty(node,
        RSModifierType::BEHIND_WINDOW_FILTER_RADIUS, radius));
    ASSERT_EQ(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(node), nullptr);
    auto property2 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property2->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list2 { std::make_shared<RSDrawCmdListRenderModifier>(property2) };
    node.drawCmdModifiers_.emplace(RSModifierType::BEHIND_WINDOW_FILTER_SATURATION, list2);
    ASSERT_EQ(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(node), nullptr);
    auto property3 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property3->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list3 { std::make_shared<RSDrawCmdListRenderModifier>(property3) };
    node.drawCmdModifiers_.emplace(RSModifierType::BEHIND_WINDOW_FILTER_BRIGHTNESS, list3);
    ASSERT_EQ(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(node), nullptr);
    auto property4 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property4->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> list4 { std::make_shared<RSDrawCmdListRenderModifier>(property4) };
    node.drawCmdModifiers_.emplace(RSModifierType::BEHIND_WINDOW_FILTER_MASK_COLOR, list4);
    ASSERT_NE(DrawableV2::RSBackgroundFilterDrawable::GetBehindWindowFilter(node), nullptr);
    // OnGenerate and OnUpdate test
    std::shared_ptr<RSDrawable> drawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    ASSERT_EQ(drawable, nullptr);
    node.childrenBlurBehindWindow_.emplace(id + 1);
    drawable = DrawableV2::RSBackgroundFilterDrawable::OnGenerate(node);
    auto filterDrawable = std::static_pointer_cast<DrawableV2::RSFilterDrawable>(drawable);
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(filterDrawable->stagingFilter_, nullptr);
    ASSERT_TRUE(filterDrawable->needSync_);
    ASSERT_TRUE(filterDrawable->stagingNeedDrawBehindWindow_);
    RectI region(0, 0, 1, 1);
    filterDrawable->stagingDrawBehindWindowRegion_ = region;
    filterDrawable->OnSync();
    ASSERT_TRUE(filterDrawable->needDrawBehindWindow_);
    ASSERT_TRUE(filterDrawable->drawBehindWindowRegion_ == filterDrawable->stagingDrawBehindWindowRegion_);
}

/**
 * @tc.name: RSBackgroundEffectDrawable
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSBackgroundEffectDrawable, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSBackgroundEffectDrawable>();
    int width = 1270;
    int height = 2560;
    Drawing::ImageInfo imageInfo{ width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRaster(imageInfo);
    Drawing::Surface* surfacePtr = surface.get();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(surfacePtr);
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
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
    auto drawableTwo = std::static_pointer_cast<DrawableV2::RSUseEffectDrawable>(
        DrawableV2::RSUseEffectDrawable::OnGenerate(node));
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
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSUseEffectDrawable002, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    drawFunc(nullptr, nullptr);
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
 * @tc.desc: Test RSUseEffectDrawable behind window branch CreateDrawFunc
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSUseEffectDrawable005
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require:
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable006
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require:
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
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable007
 * @tc.desc: Test CreateDrawFunc
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
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
}

/**
 * @tc.name: RSUseEffectDrawable008
 * @tc.desc: Test CreateDrawFunc
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
    auto drawable = std::make_shared<DrawableV2::RSUseEffectDrawable>();
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
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
 * @tc.desc: Test CreateDrawFunc
 * @tc.type:FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSRSBinarizationDrawableTest, RSDynamicLightUpDrawable002, TestSize.Level1)
{
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    auto drawable = std::make_shared<DrawableV2::RSDynamicLightUpDrawable>(1.f, 1.f);
    auto drawFunc = drawable->CreateDrawFunc();
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
    filterCanvas->SetUICapture(true);
    drawFunc(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen