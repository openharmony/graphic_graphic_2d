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

#include <cmath>

#include "gtest/gtest.h"
#include "render/rs_particles_drawable.h"
#include "animation/rs_render_particle.h"
#include "render/rs_image.h"
#include "common/rs_rect.h"
#include "draw/canvas.h"
#include "pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSParticlesDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSRenderParticle> CreatePointParticle(bool alive = true, float opacity = 1.0f,
        float scale = 1.0f);
    static std::shared_ptr<RSRenderParticle> CreateImageParticle(bool alive = true, float opacity = 1.0f,
        float scale = 1.0f, int imageFit = static_cast<int>(ImageFit::COVER));
    static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height);
};

void RSParticlesDrawableTest::SetUpTestCase()
{
    // Reset static shared image before tests
    RSParticlesDrawable::sharedCircleImage_ = nullptr;
}

void RSParticlesDrawableTest::TearDownTestCase()
{
    RSParticlesDrawable::sharedCircleImage_ = nullptr;
}

void RSParticlesDrawableTest::SetUp() {}

void RSParticlesDrawableTest::TearDown() {}

std::shared_ptr<Media::PixelMap> RSParticlesDrawableTest::CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelMap = Media::PixelMap::Create(opts);
    return std::shared_ptr<Media::PixelMap>(pixelMap.release());
}

std::shared_ptr<RSRenderParticle> RSParticlesDrawableTest::CreatePointParticle(bool alive, float opacity, float scale)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    auto particle = std::make_shared<RSRenderParticle>(particleParams);
    particle->SetParticleType(ParticleType::POINTS);
    particle->SetRadius(10.0f);
    particle->SetPosition(Vector2f(50.0f, 50.0f));
    particle->SetOpacity(opacity);
    particle->SetScale(scale);
    particle->SetColor(Color(0, 0, 0, 0));
    // Set alive state: lifeTime_ = -1 means infinite lifetime (IsAlive returns true)
    particle->lifeTime_ = -1;
    particle->activeTime_ = 0;
    particle->dead_ = !alive;
    return particle;
}

std::shared_ptr<RSRenderParticle> RSParticlesDrawableTest::CreateImageParticle(bool alive, float opacity,
    float scale, int imageFit)
{
    auto particleParams = std::make_shared<ParticleRenderParams>();
    particleParams->SetImageIndex(0);
    auto particle = std::make_shared<RSRenderParticle>(particleParams);
    particle->SetParticleType(ParticleType::IMAGES);
    particle->SetPosition(Vector2f(50.0f, 50.0f));
    particle->SetOpacity(opacity);
    particle->SetScale(scale);
    particle->SetSpin(45.0f);
    particle->SetImageSize(Vector2f(100.0f, 100.0f));
    particle->SetColor(Color(0, 0, 0, 0));
    particle->imageIndex_ = 0;

    auto rsImage = std::make_shared<RSImage>();
    rsImage->SetImageFit(imageFit);
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    particle->SetImage(rsImage);

    // Set alive state: lifeTime_ = -1 means infinite lifetime (IsAlive returns true)
    particle->lifeTime_ = -1;
    particle->activeTime_ = 0;
    particle->dead_ = !alive;
    return particle;
}

/**
 * @tc.name: Constructor001
 * @tc.desc: Test RSParticlesDrawable constructor with empty particles
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Constructor001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> imageVector;
    size_t imageCount = 0;

    RSParticlesDrawable drawable(particles, imageVector, imageCount);
    EXPECT_TRUE(drawable.particles_.empty());
    EXPECT_EQ(drawable.imageCount_, 0);
}

/**
 * @tc.name: Constructor002
 * @tc.desc: Test RSParticlesDrawable constructor with particles and images
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Constructor002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreatePointParticle());

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(std::make_shared<RSImage>());
    size_t imageCount = 1;

    RSParticlesDrawable drawable(particles, imageVector, imageCount);
    EXPECT_EQ(drawable.particles_.size(), 1);
    EXPECT_EQ(drawable.imageCount_, 1);
    EXPECT_EQ(drawable.count_.size(), 1);
    EXPECT_EQ(drawable.imageRsxform_.size(), 1);
    EXPECT_EQ(drawable.imageTex_.size(), 1);
    EXPECT_EQ(drawable.imageColors_.size(), 1);
}

/**
 * @tc.name: DefaultConstructor001
 * @tc.desc: Test RSParticlesDrawable default constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DefaultConstructor001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    EXPECT_TRUE(drawable.particles_.empty());
    EXPECT_EQ(drawable.imageCount_, 0);
}

/**
 * @tc.name: GetSharedCircleImage001
 * @tc.desc: Test GetSharedCircleImage creates image when null
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, GetSharedCircleImage001, TestSize.Level1)
{
    RSParticlesDrawable::sharedCircleImage_ = nullptr;
    auto image = RSParticlesDrawable::GetSharedCircleImage();
    EXPECT_NE(image, nullptr);
    EXPECT_NE(RSParticlesDrawable::sharedCircleImage_, nullptr);
}

/**
 * @tc.name: GetSharedCircleImage002
 * @tc.desc: Test GetSharedCircleImage returns cached image (singleton pattern)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, GetSharedCircleImage002, TestSize.Level1)
{
    RSParticlesDrawable::sharedCircleImage_ = nullptr;
    auto image1 = RSParticlesDrawable::GetSharedCircleImage();
    EXPECT_NE(image1, nullptr);
    auto image2 = RSParticlesDrawable::GetSharedCircleImage();
    EXPECT_NE(image2, nullptr);
    // Verify same instance is returned (caching works)
    EXPECT_EQ(image1.get(), image2.get());
    EXPECT_EQ(RSParticlesDrawable::sharedCircleImage_.get(), image1.get());
}

/**
 * @tc.name: MakeCircleImage001
 * @tc.desc: Test MakeCircleImage creates valid image with correct dimensions
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, MakeCircleImage001, TestSize.Level1)
{
    const int radius = 32;
    auto image = RSParticlesDrawable::MakeCircleImage(radius);
    EXPECT_NE(image, nullptr);
    // Image should be 2*radius x 2*radius
    EXPECT_EQ(image->GetWidth(), radius * 2);
    EXPECT_EQ(image->GetHeight(), radius * 2);
}

/**
 * @tc.name: UpdateData001
 * @tc.desc: Test UpdateData with same imageCount
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, UpdateData001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> imageVector;
    size_t imageCount = 2;

    RSParticlesDrawable drawable(particles, imageVector, imageCount);

    std::vector<std::shared_ptr<RSRenderParticle>> newParticles;
    newParticles.push_back(CreatePointParticle());
    std::vector<std::shared_ptr<RSImage>> newImageVector;
    newImageVector.push_back(std::make_shared<RSImage>());
    newImageVector.push_back(std::make_shared<RSImage>());

    drawable.UpdateData(newParticles, newImageVector, imageCount);

    EXPECT_EQ(drawable.particles_.size(), 1);
    EXPECT_EQ(drawable.imageVector_.size(), 2);
    EXPECT_EQ(drawable.imageCount_, 2);
    // count_ size should remain same since imageCount didn't change
    EXPECT_EQ(drawable.count_.size(), 2);
}

/**
 * @tc.name: UpdateData002
 * @tc.desc: Test UpdateData with different imageCount (resize branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, UpdateData002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> imageVector;
    size_t imageCount = 1;

    RSParticlesDrawable drawable(particles, imageVector, imageCount);
    EXPECT_EQ(drawable.count_.size(), 1);

    std::vector<std::shared_ptr<RSRenderParticle>> newParticles;
    std::vector<std::shared_ptr<RSImage>> newImageVector;
    newImageVector.push_back(std::make_shared<RSImage>());
    newImageVector.push_back(std::make_shared<RSImage>());
    newImageVector.push_back(std::make_shared<RSImage>());
    size_t newImageCount = 3;

    drawable.UpdateData(newParticles, newImageVector, newImageCount);

    EXPECT_EQ(drawable.imageCount_, 3);
    EXPECT_EQ(drawable.count_.size(), 3);
    EXPECT_EQ(drawable.imageRsxform_.size(), 3);
    EXPECT_EQ(drawable.imageTex_.size(), 3);
    EXPECT_EQ(drawable.imageColors_.size(), 3);
}

/**
 * @tc.name: Draw001
 * @tc.desc: Test Draw with empty particles
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    std::vector<std::shared_ptr<RSImage>> imageVector;
    RSParticlesDrawable drawable(particles, imageVector, 0);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 100, 100);
    // Should return early without crash
    drawable.Draw(canvas, bounds);
    EXPECT_TRUE(drawable.particles_.empty());
}

/**
 * @tc.name: Draw002
 * @tc.desc: Test Draw with bounds (clip branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw002, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreatePointParticle(true, 1.0f, 1.0f));
    std::vector<std::shared_ptr<RSImage>> imageVector;
    RSParticlesDrawable drawable(particles, imageVector, 0);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 100, 100);
    drawable.Draw(canvas, bounds);
    // After Draw, buffers should be cleared
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: Draw003
 * @tc.desc: Test Draw without bounds - verifies reserve and ClearBuffers still execute
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw003, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreatePointParticle(true, 1.0f, 1.0f));

    auto rsImage = std::make_shared<RSImage>();
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    // When bounds is nullptr, particles are skipped but reserve() and ClearBuffers() still run
    drawable.Draw(canvas, nullptr);
    // Verify ClearBuffers was called
    EXPECT_EQ(drawable.pointCount_, 0);
    EXPECT_TRUE(drawable.pointRsxform_.empty());
}

/**
 * @tc.name: Draw004
 * @tc.desc: Test Draw with image particle - full path including DrawImages
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw004, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::COVER)));

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // After Draw, count_ should be cleared by ClearBuffers
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: Draw005
 * @tc.desc: Test Draw with image particle using ImageFit::FILL - triggers DrawImageFill
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw005, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL)));

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // FILL path doesn't use atlas, count_ stays 0
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: DrawCircle001
 * @tc.desc: Test DrawCircle with points less than MAX_ATLAS_COUNT (covers count > 0 branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawCircle001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;

    // Add some points - tests the "if (count > 0)" branch in DrawCircle
    const int pointCount = 10;
    for (int i = 0; i < pointCount; i++) {
        drawable.pointRsxform_.push_back(Drawing::RSXform::Make(1, 0, i * 10.0f, 0));
        drawable.pointTex_.push_back(Drawing::Rect(0, 0, 64, 64));
        drawable.pointColors_.push_back(0xFFFF0000);
    }
    drawable.pointCount_ = pointCount;

    // Verify data was added correctly before draw
    EXPECT_EQ(drawable.pointRsxform_.size(), pointCount);
    EXPECT_EQ(drawable.pointTex_.size(), pointCount);
    EXPECT_EQ(drawable.pointColors_.size(), pointCount);
    EXPECT_EQ(drawable.pointCount_, pointCount);

    // DrawCircle should execute without crash
    drawable.DrawCircle(canvas);

    // Verify sharedCircleImage was created
    EXPECT_NE(RSParticlesDrawable::sharedCircleImage_, nullptr);
}

/**
 * @tc.name: DrawCircle002
 * @tc.desc: Test DrawCircle with points more than MAX_ATLAS_COUNT (batch drawing while loop)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawCircle002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;

    // Add more than MAX_ATLAS_COUNT (2000) points - tests the while loop
    const int pointCount = 2500;
    for (int i = 0; i < pointCount; i++) {
        drawable.pointRsxform_.push_back(Drawing::RSXform::Make(1, 0, i * 10.0f, 0));
        drawable.pointTex_.push_back(Drawing::Rect(0, 0, 64, 64));
        drawable.pointColors_.push_back(0xFFFF0000);
    }
    drawable.pointCount_ = pointCount;

    // Verify large batch data was added
    EXPECT_EQ(drawable.pointRsxform_.size(), pointCount);
    EXPECT_EQ(drawable.pointCount_, pointCount);
    EXPECT_GT(drawable.pointCount_, 2000);  // More than MAX_ATLAS_COUNT

    // DrawCircle should handle batch drawing without crash
    drawable.DrawCircle(canvas);

    // Verify sharedCircleImage was created
    EXPECT_NE(RSParticlesDrawable::sharedCircleImage_, nullptr);
}

/**
 * @tc.name: DrawImages001
 * @tc.desc: Test DrawImages with valid image data (covers main drawing path)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages001, TestSize.Level1)
{
    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable({}, imageVector, 1);

    // Add valid image data - covers the "if (count > 0)" branch
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 100, 100));
    drawable.imageColors_[0].push_back(0xFFFF0000);
    drawable.count_[0] = 1;

    // Verify data setup
    EXPECT_EQ(drawable.imageRsxform_[0].size(), 1);
    EXPECT_EQ(drawable.imageTex_[0].size(), 1);
    EXPECT_EQ(drawable.imageColors_[0].size(), 1);
    EXPECT_EQ(drawable.count_[0], 1);

    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);

    // DrawImages should complete without crash
    EXPECT_NE(drawable.imageVector_[0], nullptr);
}

/**
 * @tc.name: DrawImages002
 * @tc.desc: Test DrawImages with image count more than MAX_ATLAS_COUNT (batch while loop)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages002, TestSize.Level1)
{
    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable({}, imageVector, 1);

    // Add more than MAX_ATLAS_COUNT items - covers the while loop
    const int imageCount = 2500;
    for (int i = 0; i < imageCount; i++) {
        drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, i * 10.0f, 0));
        drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 100, 100));
        drawable.imageColors_[0].push_back(0xFFFF0000);
    }
    drawable.count_[0] = imageCount;

    // Verify large batch data was added
    EXPECT_EQ(drawable.imageRsxform_[0].size(), imageCount);
    EXPECT_EQ(drawable.count_[0], imageCount);
    EXPECT_GT(drawable.count_[0], 2000);  // More than MAX_ATLAS_COUNT

    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);

    // DrawImages should handle batch drawing without crash
    EXPECT_NE(drawable.imageVector_[0], nullptr);
}

/**
 * @tc.name: CheckImageNull001
 * @tc.desc: Test CheckImageNull when image is not null (first branch - return false)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CheckImageNull001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    auto image = RSParticlesDrawable::MakeCircleImage(32);
    std::shared_ptr<Drawing::Image> drawImage = nullptr;

    bool result = drawable.CheckImageNull(image, drawImage);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckImageNull002
 * @tc.desc: Test CheckImageNull when image is null but drawImage is not null (assigns image)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CheckImageNull002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto drawImage = RSParticlesDrawable::MakeCircleImage(32);

    bool result = drawable.CheckImageNull(image, drawImage);
    EXPECT_FALSE(result);
    EXPECT_EQ(image, drawImage);  // image should be assigned
}

/**
 * @tc.name: ClearBuffers001
 * @tc.desc: Test ClearBuffers clears all point and image buffers (covers for loop)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, ClearBuffers001, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(std::make_shared<RSImage>());
    imageVector.push_back(std::make_shared<RSImage>());

    RSParticlesDrawable drawable({}, imageVector, 2);

    // Add point data
    drawable.pointRsxform_.push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.pointTex_.push_back(Drawing::Rect(0, 0, 64, 64));
    drawable.pointColors_.push_back(0xFFFF0000);
    drawable.pointCount_ = 1;

    // Add image data to both image arrays
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 100, 100));
    drawable.imageColors_[0].push_back(0xFFFF0000);
    drawable.count_[0] = 1;

    drawable.imageRsxform_[1].push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.imageTex_[1].push_back(Drawing::Rect(0, 0, 100, 100));
    drawable.imageColors_[1].push_back(0xFF00FF00);
    drawable.count_[1] = 1;

    drawable.ClearBuffers();

    // Verify point buffers cleared
    EXPECT_TRUE(drawable.pointRsxform_.empty());
    EXPECT_TRUE(drawable.pointTex_.empty());
    EXPECT_TRUE(drawable.pointColors_.empty());
    EXPECT_EQ(drawable.pointCount_, 0);
    // Verify image buffers cleared (tests for loop over imageCount_)
    EXPECT_TRUE(drawable.imageRsxform_[0].empty());
    EXPECT_TRUE(drawable.imageRsxform_[1].empty());
    EXPECT_EQ(drawable.count_[0], 0);
    EXPECT_EQ(drawable.count_[1], 0);
}

/**
 * @tc.name: DrawParticles001
 * @tc.desc: Test DrawParticles with points triggers DrawCircle and DrawImages
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawParticles001, TestSize.Level1)
{
    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable({}, imageVector, 1);

    // Add point data - triggers "if (pointCount_ > 0)" branch
    drawable.pointRsxform_.push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.pointTex_.push_back(Drawing::Rect(0, 0, 64, 64));
    drawable.pointColors_.push_back(0xFFFF0000);
    drawable.pointCount_ = 1;

    // Add image data - triggers "if (imageCount_ > 0)" branch
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, 100, 100));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 100, 100));
    drawable.imageColors_[0].push_back(0xFF00FF00);
    drawable.count_[0] = 1;

    // Verify data setup before DrawParticles
    EXPECT_EQ(drawable.pointCount_, 1);
    EXPECT_EQ(drawable.count_[0], 1);
    EXPECT_GT(drawable.imageCount_, 0);

    Drawing::Canvas canvas;
    drawable.DrawParticles(canvas);

    // Verify sharedCircleImage was created by DrawCircle
    EXPECT_NE(RSParticlesDrawable::sharedCircleImage_, nullptr);
    // Verify imageVector is valid
    EXPECT_NE(drawable.imageVector_[0], nullptr);
}

/**
 * @tc.name: Draw006
 * @tc.desc: Test Draw with multiple particles triggers reserve for image buffers
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw006, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    for (int i = 0; i < 100; i++) {
        auto particle = CreatePointParticle(true, 1.0f, 1.0f);
        particle->SetPosition(Vector2f(10.0f + i, 10.0f + i));
        particles.push_back(particle);
    }

    auto rsImage = std::make_shared<RSImage>();
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable(particles, imageVector, 2);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // Verify ClearBuffers was called
    EXPECT_EQ(drawable.pointCount_, 0);
    EXPECT_TRUE(drawable.pointRsxform_.empty());
}

/**
 * @tc.name: DrawImages003
 * @tc.desc: Test DrawImages with multiple images in imageVector (covers for loop)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages003, TestSize.Level1)
{
    auto rsImage1 = std::make_shared<RSImage>();
    auto pixelMap1 = CreatePixelMap(100, 100);
    rsImage1->SetPixelMap(pixelMap1);

    auto rsImage2 = std::make_shared<RSImage>();
    auto pixelMap2 = CreatePixelMap(50, 50);
    rsImage2->SetPixelMap(pixelMap2);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage1);
    imageVector.push_back(rsImage2);

    RSParticlesDrawable drawable({}, imageVector, 2);

    // Add data for first image
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 100, 100));
    drawable.imageColors_[0].push_back(0xFFFF0000);
    drawable.count_[0] = 1;

    // Add data for second image
    drawable.imageRsxform_[1].push_back(Drawing::RSXform::Make(1, 0, 50, 50));
    drawable.imageTex_[1].push_back(Drawing::Rect(0, 0, 50, 50));
    drawable.imageColors_[1].push_back(0xFF00FF00);
    drawable.count_[1] = 1;

    // Verify data setup before DrawImages
    EXPECT_EQ(drawable.imageCount_, 2);
    EXPECT_EQ(drawable.count_[0], 1);
    EXPECT_EQ(drawable.count_[1], 1);
    EXPECT_EQ(drawable.imageRsxform_[0].size(), 1);
    EXPECT_EQ(drawable.imageRsxform_[1].size(), 1);

    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);

    // Verify both images in imageVector are valid after DrawImages
    EXPECT_NE(drawable.imageVector_[0], nullptr);
    EXPECT_NE(drawable.imageVector_[1], nullptr);
}

/**
 * @tc.name: DrawImages004
 * @tc.desc: Test DrawImages with image having Drawing::Image but no pixelmap (CheckImageNull branch)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages004, TestSize.Level1)
{
    auto rsImage = std::make_shared<RSImage>();
    // Create a Drawing::Image directly - tests CheckImageNull assigning image
    auto drawingImage = RSParticlesDrawable::MakeCircleImage(32);
    rsImage->SetImage(drawingImage);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable({}, imageVector, 1);

    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1, 0, 0, 0));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 64, 64));
    drawable.imageColors_[0].push_back(0xFFFF0000);
    drawable.count_[0] = 1;

    // Verify data setup
    EXPECT_EQ(drawable.count_[0], 1);
    EXPECT_EQ(drawable.imageRsxform_[0].size(), 1);
    EXPECT_NE(drawingImage, nullptr);

    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);

    // Verify rsImage has Drawing::Image set (CheckImageNull path)
    EXPECT_NE(drawable.imageVector_[0], nullptr);
    // drawingImage should be valid with correct dimensions
    EXPECT_EQ(drawingImage->GetWidth(), 64);
    EXPECT_EQ(drawingImage->GetHeight(), 64);
}

/**
 * @tc.name: Draw007
 * @tc.desc: Test Draw full flow with point particle - covers reserve, ClearBuffers, DrawCircle
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw007, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    auto particle = CreatePointParticle(true, 1.0f, 1.0f);
    particles.push_back(particle);

    auto rsImage = std::make_shared<RSImage>();
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 100, 100);
    drawable.Draw(canvas, bounds);
    // After draw, all buffers cleared
    EXPECT_EQ(drawable.pointCount_, 0);
    EXPECT_TRUE(drawable.pointRsxform_.empty());
}

/**
 * @tc.name: Draw008
 * @tc.desc: Test Draw full flow with image particle - covers CaculateImageAtlsArry, DrawImages
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw008, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    auto particle = CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::COVER));
    particles.push_back(particle);

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // After draw, count_ cleared by ClearBuffers
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: Draw009
 * @tc.desc: Test Draw with ImageFit::FILL triggers DrawImageFill path
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw009, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    particles.push_back(CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL)));

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);

    RSParticlesDrawable drawable(particles, imageVector, 1);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // FILL path doesn't add to atlas
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: Draw010
 * @tc.desc: Test Draw with multiple particles triggers reserve for all image buffers
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw010, TestSize.Level1)
{
    std::vector<std::shared_ptr<RSRenderParticle>> particles;
    for (int i = 0; i < 50; i++) {
        auto particle = CreatePointParticle(true, 1.0f, 1.0f);
        particle->SetPosition(Vector2f(10.0f + i, 10.0f + i));
        particles.push_back(particle);
    }

    auto rsImage = std::make_shared<RSImage>();
    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable(particles, imageVector, 2);

    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0, 0, 200, 200);
    drawable.Draw(canvas, bounds);
    // Verify ClearBuffers cleared all
    EXPECT_EQ(drawable.pointCount_, 0);
    EXPECT_TRUE(drawable.pointRsxform_.empty());
}

/**
 * @tc.name: CaculatePointAtlsArry001
 * @tc.desc: Test CaculatePointAtlsArry adds data to buffers correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CaculatePointAtlsArry001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    auto particle1 = CreatePointParticle(true, 1.0f, 1.0f);
    auto particle2 = CreatePointParticle(true, 0.5f, 2.0f);

    drawable.CaculatePointAtlsArry(particle1, Vector2f(50, 50), 1.0f, 1.0f);
    drawable.CaculatePointAtlsArry(particle2, Vector2f(100, 100), 0.5f, 2.0f);

    EXPECT_EQ(drawable.pointCount_, 2);
    EXPECT_EQ(drawable.pointRsxform_.size(), 2);
    EXPECT_EQ(drawable.pointTex_.size(), 2);
    EXPECT_EQ(drawable.pointColors_.size(), 2);
}

/**
 * @tc.name: CaculateImageAtlsArry001
 * @tc.desc: Test CaculateImageAtlsArry with valid image (non-FILL) adds to atlas
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CaculateImageAtlsArry001, TestSize.Level1)
{
    auto particle = CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::COVER));

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable({}, imageVector, 1);

    Drawing::Canvas canvas;
    drawable.CaculateImageAtlsArry(canvas, particle, Vector2f(50, 50), 1.0f, 1.0f);
    EXPECT_EQ(drawable.count_[0], 1);
    EXPECT_EQ(drawable.imageRsxform_[0].size(), 1);
}

/**
 * @tc.name: CaculateImageAtlsArry002
 * @tc.desc: Test CaculateImageAtlsArry with ImageFit::FILL calls DrawImageFill (not atlas)
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CaculateImageAtlsArry002, TestSize.Level1)
{
    auto particle = CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL));

    auto rsImage = std::make_shared<RSImage>();
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);

    std::vector<std::shared_ptr<RSImage>> imageVector;
    imageVector.push_back(rsImage);
    RSParticlesDrawable drawable({}, imageVector, 1);

    Drawing::Canvas canvas;
    drawable.CaculateImageAtlsArry(canvas, particle, Vector2f(50, 50), 1.0f, 1.0f);
    // FILL doesn't add to atlas
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: DrawImageFill001
 * @tc.desc: Test DrawImageFill with valid particle executes canvas operations
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageFill001, TestSize.Level1)
{
    auto particle = CreateImageParticle(true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL));

    // Verify particle is properly set up for FILL mode
    EXPECT_NE(particle, nullptr);
    EXPECT_FALSE(particle->IsAlive());
    EXPECT_EQ(particle->GetParticleType(), ParticleType::IMAGES);
    auto rsImage = particle->GetImage();
    EXPECT_NE(rsImage, nullptr);
    EXPECT_EQ(rsImage->GetImageFit(), ImageFit::FILL);

    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;

    // DrawImageFill should execute canvas save/restore operations without crash
    drawable.DrawImageFill(canvas, particle, Vector2f(50, 50), 1.0f, 1.0f);

    // Verify particle image is still valid after draw
    EXPECT_NE(particle->GetImage(), nullptr);
}

} // namespace OHOS::Rosen
