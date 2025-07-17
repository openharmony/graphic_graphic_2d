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
#include "skia_image.h"
#include "skia_image_info.h"

#include "draw/color.h"
#include "image/image_info.h"
#include "render/rs_particles_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSParticlesDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override
    {
        // Create a canvas for drawing operations
        canvas_ = std::make_shared<Drawing::Canvas>();
    }
    void TearDown() override;
};

void RSParticlesDrawableTest::SetUpTestCase() {}
void RSParticlesDrawableTest::TearDownTestCase() {}
void RSParticlesDrawableTest::SetUp() {}
void RSParticlesDrawableTest::TearDown() {}

/**
 * @tc.name: ConstructorTest001
 * @tc.desc: Test the default constructor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    auto bounds = std::make_shared<RectF>();
    // Should not crash
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: ConstructorTest002
 * @tc.desc: Test constructor with empty vectors
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: ConstructorTest003
 * @tc.desc: Test constructor with image count mismatch (greater than actual)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest003, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    images.push_back(std::make_shared<RSImage>());

    // imageCount is larger than images.size()
    RSParticlesDrawable drawable(particles, images, 5);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: ConstructorTest004
 * @tc.desc: Test constructor with null particle pointer in vector
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest004, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(nullptr);
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: ConstructorTest005
 * @tc.desc: Test constructor with null image pointer in vector
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest005, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    images.push_back(nullptr);

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest001
 * @tc.desc: Test drawing a single point particle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest001, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest002
 * @tc.desc: Test drawing multiple point particles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    for (int i = 0; i < 100; ++i) {
        auto particle = std::make_shared<RSRenderParticle>();
        particle->SetParticleType(ParticleType::POINT);
        particle->SetPosition({ (float)i * 2.0f, (float)i * 2.0f });
        particles.push_back(particle);
    }

    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest003
 * @tc.desc: Test drawing a point with zero scale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest003, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetScale(0.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest004
 * @tc.desc: Test drawing a point with negative scale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest004, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetScale(-1.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest005
 * @tc.desc: Test drawing a point with zero opacity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest005, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetOpacity(0.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest006
 * @tc.desc: Test drawing a point with some spin (rotation)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest006, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetSpin(90.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest007
 * @tc.desc: Test drawing a point with a large radius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest007, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetRadius(1000);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest001
 * @tc.desc: Test drawing a single image particle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest001, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest002
 * @tc.desc: Test drawing multiple image particles using the same image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    for (int i = 0; i < 50; ++i) {
        auto particle = std::make_shared<RSRenderParticle>();
        particle->SetParticleType(ParticleType::IMAGE);
        particle->SetImageIndex(0);
        particles.push_back(particle);
    }

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest003
 * @tc.desc: Test drawing multiple image particles using different images
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest003, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;

    for (int i = 0; i < 10; ++i) {
        auto drawingImage = std::make_shared<Drawing::Image>();
        auto rsImage = std::make_shared<RSImage>(drawingImage);
        images.push_back(rsImage);

        auto particle = std::make_shared<RSRenderParticle>();
        particle->SetParticleType(ParticleType::IMAGE);
        particle->SetImageIndex(i);
        particles.push_back(particle);
    }

    RSParticlesDrawable drawable(particles, images, 10);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest004
 * @tc.desc: Test image particle with an out-of-bounds index
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest004, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(5); // Index is out of bounds

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage }; // Only one image at index 0

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas); // Should not crash
}

/**
 * @tc.name: DrawImageTest005
 * @tc.desc: Test image particle with a negative index
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest005, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(-1); // Index is negative

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas); // Should not crash
}

/**
 * @tc.name: DrawImageTest006
 * @tc.desc: Test drawing an image particle where the RSImage contains a null Drawing::Image
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest006, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    // Create an RSImage that holds a nullptr
    auto rsImage = std::make_shared<RSImage>(nullptr);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest007
 * @tc.desc: Test image particle with zero scale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest007, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);
    particle->SetScale(0.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMixedTest001
 * @tc.desc: Test drawing a mix of point and image particles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMixedTest001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;

    // Add an image and its particle
    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    images.push_back(rsImage);
    auto imageParticle = std::make_shared<RSRenderParticle>();
    imageParticle->SetParticleType(ParticleType::IMAGE);
    imageParticle->SetImageIndex(0);
    particles.push_back(imageParticle);

    // Add a point particle
    auto pointParticle = std::make_shared<RSRenderParticle>();
    pointParticle->SetParticleType(ParticleType::POINT);
    particles.push_back(pointParticle);

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMixedTest002
 * @tc.desc: Test drawing many mixed particles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMixedTest002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    const int imageCount = 5;

    // Create images
    for (int i = 0; i < imageCount; ++i) {
        auto drawingImage = std::make_shared<Drawing::Image>();
        auto rsImage = std::make_shared<RSImage>(drawingImage);
        images.push_back(rsImage);
    }

    // Create 100 mixed particles
    for (int i = 0; i < 100; ++i) {
        auto particle = std::make_shared<RSRenderParticle>();
        if (i % 2 == 0) {
            particle->SetParticleType(ParticleType::POINT);
        } else {
            particle->SetParticleType(ParticleType::IMAGE);
            particle->SetImageIndex(i % imageCount);
        }
        particles.push_back(particle);
    }

    RSParticlesDrawable drawable(particles, images, imageCount);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMixedTest003
 * @tc.desc: Test mix of particles including null particle pointers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMixedTest003, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;

    // Add a valid point particle
    particles.push_back(std::make_shared<RSRenderParticle>());

    // Add a null particle
    particles.push_back(nullptr);

    // Add a valid image particle
    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    images.push_back(rsImage);
    auto imageParticle = std::make_shared<RSRenderParticle>();
    imageParticle->SetParticleType(ParticleType::IMAGE);
    imageParticle->SetImageIndex(0);
    particles.push_back(imageParticle);

    // Add another null particle
    particles.push_back(nullptr);

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest001
 * @tc.desc: Test Draw with a null bounds pointer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest001, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    // Pass a null shared_ptr for bounds
    drawable.Draw(*canvas_, nullptr);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest002
 * @tc.desc: Test with a very large number of particles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    const int particleCount = 5000;

    for (int i = 0; i < particleCount; ++i) {
        auto particle = std::make_shared<RSRenderParticle>();
        particle->SetParticleType(ParticleType::POINT);
        particles.push_back(particle);
    }

    std::vector<std::shared_ptr<RSImage>> images;
    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest003
 * @tc.desc: Test with particles having extreme position values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest003, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;

    auto particle1 = std::make_shared<RSRenderParticle>();
    particle1->SetPosition({ -1000000.0f, -1000000.0f });
    particles.push_back(particle1);

    auto particle2 = std::make_shared<RSRenderParticle>();
    particle2->SetPosition({ 1000000.0f, 1000000.0f });
    particles.push_back(particle2);

    std::vector<std::shared_ptr<RSImage>> images;
    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest004
 * @tc.desc: Test with particles having extreme scale values
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest004, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;

    auto particle1 = std::make_shared<RSRenderParticle>();
    particle1->SetScale(10000.0f);
    particles.push_back(particle1);

    auto particle2 = std::make_shared<RSRenderParticle>();
    particle2->SetScale(0.00001f);
    particles.push_back(particle2);

    std::vector<std::shared_ptr<RSImage>> images;
    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest005
 * @tc.desc: Test with a large number of images
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest005, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    const int imageCount = 200;

    for (int i = 0; i < imageCount; ++i) {
        images.push_back(std::make_shared<RSImage>(std::make_shared<Drawing::Image>()));
        auto particle = std::make_shared<RSRenderParticle>();
        particle->SetParticleType(ParticleType::IMAGE);
        particle->SetImageIndex(i);
        particles.push_back(particle);
    }

    RSParticlesDrawable drawable(particles, images, imageCount);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest006
 * @tc.desc: Test where imageCount in constructor is 0, but images are provided
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest006, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;

    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);
    particles.push_back(particle);

    images.push_back(std::make_shared<RSImage>(std::make_shared<Drawing::Image>()));

    // imageCount is 0, which should prevent any images from being drawn.
    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

// --- Additional Robustness and Edge Case Tests ---

/**
 * @tc.name: RobustnessTest001
 * @tc.desc: Test with particle position as NaN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, RobustnessTest001, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetPosition({ std::numeric_limits<float>::quiet_NaN(), 0.0f });
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: RobustnessTest002
 * @tc.desc: Test with particle scale as infinity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, RobustnessTest002, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetScale(std::numeric_limits<float>::infinity());
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: RobustnessTest003
 * @tc.desc: Test with particle spin as negative infinity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, RobustnessTest003, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetSpin(-std::numeric_limits<float>::infinity());
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: RobustnessTest004
 * @tc.desc: Test with image particle properties as NaN and infinity
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, RobustnessTest004, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);
    particle->SetScale(std::numeric_limits<float>::quiet_NaN());
    particle->SetSpin(std::numeric_limits<float>::infinity());

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images = { std::make_shared<RSImage>(std::make_shared<Drawing::Image>()) };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest008
 * @tc.desc: Test drawing a point with negative radius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest008, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetRadius(-10);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest009
 * @tc.desc: Test drawing a point with zero radius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest009, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetRadius(0);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest008
 * @tc.desc: Test image particle index that equals imageCount (out of bounds)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest008, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(1); // Index 1 is out of bounds for imageCount 1

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest009
 * @tc.desc: Test drawing multiple image particles, some with invalid indices
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest009, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    // Valid particle
    auto particle1 = std::make_shared<RSRenderParticle>();
    particle1->SetParticleType(ParticleType::IMAGE);
    particle1->SetImageIndex(0);
    particles.push_back(particle1);

    // Invalid particle
    auto particle2 = std::make_shared<RSRenderParticle>();
    particle2->SetParticleType(ParticleType::IMAGE);
    particle2->SetImageIndex(10);
    particles.push_back(particle2);

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest007
 * @tc.desc: Test calling Draw multiple times on the same object
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest007, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();

    drawable.Draw(*canvas_, bounds);
    drawable.Draw(*canvas_, bounds);
    drawable.Draw(*canvas_, bounds);

    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest008
 * @tc.desc: Test with empty particles vector but non-empty images
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest008, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    images.push_back(std::make_shared<RSImage>(std::make_shared<Drawing::Image>()));

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds); // Should do nothing and not crash
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMiscTest009
 * @tc.desc: Test with bounds having negative dimensions
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMiscTest009, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>(RectF { 100.f, 100.f, 50.f, 50.f });
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawMixedTest004
 * @tc.desc: Interleaving nullptrs with valid point and image particles
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawMixedTest004, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;
    images.push_back(std::make_shared<RSImage>(std::make_shared<Drawing::Image>()));

    particles.push_back(std::make_shared<RSRenderParticle>()); // Point
    particles.push_back(nullptr);
    auto imageParticle = std::make_shared<RSRenderParticle>();
    imageParticle->SetParticleType(ParticleType::IMAGE);
    imageParticle->SetImageIndex(0);
    particles.push_back(imageParticle); // Image
    particles.push_back(nullptr);
    particles.push_back(std::make_shared<RSRenderParticle>()); // Point

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: ConstructorTest006
 * @tc.desc: Test constructor with a very large imageCount
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, ConstructorTest006, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> images;

    // A very large imageCount that might stress memory allocation
    RSParticlesDrawable drawable(particles, images, 100000);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawPointTest010
 * @tc.desc: Point particle with large spin value
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawPointTest010, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::POINT);
    particle->SetSpin(36000.f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };
    std::vector<std::shared_ptr<RSImage>> images;

    RSParticlesDrawable drawable(particles, images, 0);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}

/**
 * @tc.name: DrawImageTest010
 * @tc.desc: Image particle with negative scale
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageTest010, TestSize.Level1)
{
    auto particle = std::make_shared<RSRenderParticle>();
    particle->SetParticleType(ParticleType::IMAGE);
    particle->SetImageIndex(0);
    particle->SetScale(-2.0f);

    std::vector<std::shared_ptr<RSRenderParticle>> particles = { particle };

    auto drawingImage = std::make_shared<Drawing::Image>();
    auto rsImage = std::make_shared<RSImage>(drawingImage);
    std::vector<std::shared_ptr<RSImage>> images = { rsImage };

    RSParticlesDrawable drawable(particles, images, 1);
    auto bounds = std::make_shared<RectF>();
    drawable.Draw(*canvas_, bounds);
    ASSERT_TRUE(canvas);
}
