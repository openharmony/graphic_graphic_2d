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

#include <limits>
#include <vector>

#include "gtest/gtest.h"
#include "pixel_map.h"

#include "animation/rs_render_particle_pool.h"
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
    void CreateParticle(const std::shared_ptr<ParticleRenderParams>& renderParams);
    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    std::shared_ptr<ParticleRenderParams> params;
    std::shared_ptr<RSRenderParticlePool> pool;
    size_t slot = 0;
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
    CreateParticle(params);
}
void RSRenderParticleTest::TearDown() {}

void RSRenderParticleTest::CreateParticle(const std::shared_ptr<ParticleRenderParams>& renderParams)
{
    params = renderParams;
    pool = std::make_shared<RSRenderParticlePool>();
    slot = pool->Acquire();
    if (params != nullptr) {
        InitParticle(*pool, slot, *params, 0);
    }
}

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
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto srfce = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    if (srfce == nullptr) {
        return nullptr;
    }
    auto cnvs = srfce->GetCanvas();
    if (cnvs == nullptr) {
        return nullptr;
    }
    cnvs->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    int a = 4;
    int b = 2;
    cnvs->AttachBrush(brush);
    cnvs->DrawRect(Drawing::Rect(width / a, height / a, width / a + width / b, height / a + height / b));
    cnvs->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: InitProperty001
 * @tc.desc: Verify the InitProperty
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, InitProperty001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty001 start";
    ASSERT_TRUE(params != nullptr);
    InitParticle(*pool, slot, *params, 0);
    EXPECT_TRUE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty001 end";
}

/**
 * @tc.name: InitProperty002
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, InitProperty002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty002 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(
        std::numeric_limits<uint64_t>::max() / NS_PER_MS, std::numeric_limits<uint64_t>::max() / NS_PER_MS + 1);
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
    CreateParticle(params);
    EXPECT_TRUE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty002 end";
}

/**
 * @tc.name: InitProperty003
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, InitProperty003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty003 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::RECT;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(0, 3000); // 3000 is lifeTime range end.
    ParticleType type = ParticleType::IMAGES;
    float radius = 1;
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    ASSERT_TRUE(rsImage != nullptr);
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
    CreateParticle(params);
    EXPECT_TRUE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty003 end";
}

/**
 * @tc.name: InitProperty004
 * @tc.desc: Verify the InitProperty image
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, InitProperty004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty004 start";
    int emitRate = 20;
    ShapeType emitShape = ShapeType::ELLIPSE;
    Vector2f position = Vector2f(0.f, 0.f);
    Vector2f emitSize = Vector2f(10.f, 10.f);
    int particleCount = 20;
    Range<int64_t> lifeTime = Range<int64_t>(-1, -1);
    ParticleType type = ParticleType::IMAGES;
    float radius = 1;
    std::shared_ptr<RSImage> rsImage = std::make_shared<RSImage>();
    ASSERT_TRUE(rsImage != nullptr);
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
    CreateParticle(params);
    EXPECT_TRUE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest InitProperty004 end";
}

/**
 * @tc.name: RSRenderParticle001
 * @tc.desc: Verify a slot without parameters never reports alive
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, RSRenderParticle001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest RSRenderParticle001 start";
    CreateParticle(nullptr);
    pool->SetActiveTime(slot, 0);
    pool->SetLifeTime(slot, 0);
    EXPECT_FALSE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest RSRenderParticle001 end";
}

/**
 * @tc.name: IsAlive001
 * @tc.desc: Verify the IsAlive
 * @tc.type:FUNC
 * @tc.require: issueIA6IWR
 */
HWTEST_F(RSRenderParticleTest, IsAlive001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderParticleTest IsAlive001 start";
    EXPECT_TRUE(pool->IsAlive(slot));
    pool->MarkDead(slot);
    EXPECT_FALSE(pool->IsAlive(slot));
    GTEST_LOG_(INFO) << "RSRenderParticleTest IsAlive001 end";
}

/**
 * @tc.name: IsAliveSoA001
 * @tc.desc: Verify the slot liveness is the only source of truth
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, IsAliveSoA001, TestSize.Level1)
{
    EXPECT_TRUE(pool->IsAlive(slot));
    pool->MarkDead(slot);
    EXPECT_FALSE(pool->IsAlive(slot));
    EXPECT_TRUE(pool->HasDead());
}

/**
 * @tc.name: SetIsDeadSoA001
 * @tc.desc: Verify MarkDead leaves no alive slot behind
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, SetIsDeadSoA001, TestSize.Level1)
{
    pool->MarkDead(slot);
    EXPECT_FALSE(pool->IsAlive(slot));
    EXPECT_TRUE(pool->HasDead());
}

/**
 * @tc.name: SpinTrig001
 * @tc.desc: Verify the cached sin/cos pair follows SetSpin
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderParticleTest, SpinTrig001, TestSize.Level1)
{
    float cosValue = 0.f;
    float sinValue = 0.f;

    pool->SetSpin(slot, 0.f);
    pool->GetSpinTrig(slot, cosValue, sinValue);
    EXPECT_FLOAT_EQ(cosValue, 1.f);
    EXPECT_FLOAT_EQ(sinValue, 0.f);

    pool->SetSpin(slot, 90.f);
    pool->GetSpinTrig(slot, cosValue, sinValue);
    EXPECT_NEAR(cosValue, 0.f, 1e-5f);
    EXPECT_NEAR(sinValue, 1.f, 1e-5f);

    pool->SetSpin(slot, 180.f);
    pool->GetSpinTrig(slot, cosValue, sinValue);
    EXPECT_NEAR(cosValue, -1.f, 1e-5f);
    EXPECT_NEAR(sinValue, 0.f, 1e-5f);
}

} // namespace Rosen
} // namespace OHOS
