/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <vector>

#include "gtest/gtest.h"
#include "pixel_map.h"

#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "common/rs_vector2.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"
#include "render/rs_image.h"
#include "render/rs_image_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderParticleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticle> particle;
};

void RSRenderParticleTest::SetUpTestCase() {}
void RSRenderParticleTest::TearDownTestCase() {}
void RSRenderParticleTest::SetUp()
{
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(3000, 3000); // 3000 is lifeTime.
    ParticleType type = ParticleType::POINTS;
    float radius = 1;
    std::shared_ptr<RSImage> image;
    Vector2f imageSize = Vector2f(1.f, 1.f);
    EmitterConfig emitterConfig =
        EmitterConfig(emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, image, imageSize);
    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particle = std::make_shared<RSRenderParticle>(params);
}
void RSRenderParticleTest::TearDown() {}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(),
<<<<<<< HEAD
        Drawing::ColorType::COLORTYPE_RGBA_8888,  Drawing::AlphaType::ALPHATYPE_PREMUL };
=======
    Drawing::ColorType::COLORTYPE_RGBA_8888,  Drawing::AlphaType::ALPHATYPE_PREMUL };
>>>>>>> zhangpeng/master
    auto srfce = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    auto cnvs = srfce->GetCanvas();
    cnvs->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    int a = 4;
    int b = 2;
    cnvs->AttachBrush(brush);
    cnvs->DrawRect(Drawing::Rect(width/a, height/a, width/a + width/b, height/a + height/b));
    cnvs->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: InitProperty001
 * @tc.desc: Verify the InitProperty
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, InitProperty001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty001 start";
    particle->InitProperty();
    EXPECT_TRUE(particle != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty001 end";
}

/**
 * @tc.name: InitProperty002
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, InitProperty002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty002 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(3000, 3000); // 3000 is lifeTime.
    ParticleType type = ParticleType::IMAGES;
    float radius = 1;
    std::shared_ptr<RSImage> rsImage = nullptr;
    Vector2f imageSize = Vector2f(200.f, 300.f);
    EmitterConfig emitterConfig = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, rsImage, imageSize);
    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particle = std::make_shared<RSRenderParticle>(params);
    particle->InitProperty();
    EXPECT_TRUE(particle != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty002 end";
}

/**
 * @tc.name: InitProperty003
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, InitProperty003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty003 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(3000, 3000); // 3000 is lifeTime.
    ParticleType type = ParticleType::IMAGES;
    float radius = 1;
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    rsImage->SetPixelMap(pixelMap);
    Vector2f imageSize = Vector2f(200.f, 300.f);
    EmitterConfig emitterConfig = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, rsImage, imageSize);
    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particle = std::make_shared<RSRenderParticle>(params);
    particle->InitProperty();
    EXPECT_TRUE(particle != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty003 end";
}

/**
 * @tc.name: InitProperty004
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, InitProperty004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty003 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::ELLIPSE;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(-1, -1);
    ParticleType type = ParticleType::IMAGES;
    float radius = 1;
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> pixelMap;
    int width = 200;
    int height = 300;
    pixelMap = CreatePixelMap(width, height);
    rsImage->SetPixelMap(pixelMap);
    Vector2f imageSize = Vector2f(200.f, 300.f);
    EmitterConfig emitterConfig = EmitterConfig(
        emitRate, emitShape, position, emitSize, particleCount, lifeTime, type, radius, rsImage, imageSize);
    ParticleVelocity velocity;
    RenderParticleAcceleration acceleration;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    params = std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    particle = std::make_shared<RSRenderParticle>(params);
    particle->InitProperty();
    EXPECT_TRUE(particle != nullptr);
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty004 end";
}
/**
 * @tc.name: IsAlive001
 * @tc.desc: Verify the IsAlive
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, IsAlive001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest IsAlive001 start";
    EXPECT_TRUE(particle->IsAlive() == true);
    particle->SetIsDead();
    EXPECT_TRUE(particle->IsAlive() == false);
    GTEST_LOG_(INFO) << "RSRenderParticleTest IsAlive001 end";
}

/**
 * @tc.name: ColorLerp001
 * @tc.desc: Verify the Color Lerp
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, ColorLerp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest ColorLerp001 start";
    Color start = Color(0, 0, 0, 128);
    Color end = Color(255, 255, 255, 255);
    float t = 0.5f;
    Color color = particle->Lerp(start, end, t);
    EXPECT_TRUE((color != start) && (color != end));
    GTEST_LOG_(INFO) << "RSRenderParticleTest ColorLerp001 end";
}

} // namespace Rosen
} // namespace OHOS