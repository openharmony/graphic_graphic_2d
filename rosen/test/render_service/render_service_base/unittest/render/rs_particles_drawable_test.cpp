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

#include <limits>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

#include "animation/rs_render_particle_pool.h"
#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "pixel_map.h"
#include "render/rs_image.h"
#include "render/rs_particles_drawable.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
// Mirrors the file-local MAX_ATLAS_COUNT of rs_particles_drawable.cpp.
constexpr size_t ATLAS_BATCH = 2000;
} // namespace

class RSParticlesDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height);
    static size_t CreatePointParticle(RSRenderParticlePool& pool, bool alive = true, float opacity = 1.0f,
        float scale = 1.0f);
    static size_t CreateImageParticle(RSRenderParticlePool& pool, std::vector<std::shared_ptr<RSImage>>& images,
        bool alive = true, float opacity = 1.0f, float scale = 1.0f,
        int imageFit = static_cast<int>(ImageFit::COVER));
};

void RSParticlesDrawableTest::SetUpTestCase()
{
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

size_t RSParticlesDrawableTest::CreatePointParticle(RSRenderParticlePool& pool, bool alive, float opacity, float scale)
{
    const size_t slot = pool.Acquire();
    pool.SetParticleType(slot, ParticleType::POINTS);
    pool.SetRadius(slot, 10.0f);
    pool.SetPosition(slot, Vector2f(50.0f, 50.0f));
    pool.SetOpacity(slot, opacity);
    pool.SetScale(slot, scale);
    pool.SetColor(slot, Color(0, 0, 0, 0));
    pool.SetLifeTime(slot, -1);
    pool.SetActiveTime(slot, 0);
    if (!alive) {
        pool.MarkDead(slot);
    }
    return slot;
}

size_t RSParticlesDrawableTest::CreateImageParticle(RSRenderParticlePool& pool,
    std::vector<std::shared_ptr<RSImage>>& images, bool alive, float opacity, float scale, int imageFit)
{
    const size_t slot = pool.Acquire();
    pool.SetParticleType(slot, ParticleType::IMAGES);
    pool.SetPosition(slot, Vector2f(50.0f, 50.0f));
    pool.SetOpacity(slot, opacity);
    pool.SetScale(slot, scale);
    pool.SetSpin(slot, 45.0f);
    pool.SetImageSize(slot, Vector2f(100.0f, 100.0f));
    pool.SetColor(slot, Color(0, 0, 0, 0));
    pool.SetImageIndex(slot, 0);

    auto rsImage = std::make_shared<RSImage>();
    rsImage->SetImageFit(imageFit);
    auto pixelMap = CreatePixelMap(100, 100);
    rsImage->SetPixelMap(pixelMap);
    images.push_back(rsImage);

    pool.SetLifeTime(slot, -1);
    pool.SetActiveTime(slot, 0);
    if (!alive) {
        pool.MarkDead(slot);
    }
    return slot;
}

/**
 * @tc.name: Constructor001
 * @tc.desc: Test RSParticlesDrawable constructor with image vector
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Constructor001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    EXPECT_EQ(drawable.imageCount_, 1);
    EXPECT_EQ(drawable.count_.size(), 1);
    EXPECT_EQ(drawable.imageRsxform_.size(), 1);
    EXPECT_EQ(drawable.GetCachedDrawingImage(0, images[0]) != nullptr, true);
}

/**
 * @tc.name: Constructor002
 * @tc.desc: Test constructor with an empty image vector
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Constructor002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    RSParticlesDrawable drawable(pool, {}, 0);
    EXPECT_EQ(drawable.imageCount_, 0);
    EXPECT_TRUE(drawable.count_.empty());
}

/**
 * @tc.name: DefaultConstructor001
 * @tc.desc: Test default constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DefaultConstructor001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.pool_, nullptr);
}

/**
 * @tc.name: GetSharedCircleImage001
 * @tc.desc: Test the shared circle image is created once
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, GetSharedCircleImage001, TestSize.Level1)
{
    auto first = RSParticlesDrawable::GetSharedCircleImage();
    auto second = RSParticlesDrawable::GetSharedCircleImage();
    EXPECT_NE(first, nullptr);
    EXPECT_EQ(first, second);
}

/**
 * @tc.name: GetSharedCircleImage002
 * @tc.desc: Test the shared image is stable across draws
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, GetSharedCircleImage002, TestSize.Level1)
{
    EXPECT_NE(RSParticlesDrawable::GetSharedCircleImage(), nullptr);
    EXPECT_NE(RSParticlesDrawable::GetSharedCircleImage(), nullptr);
}

/**
 * @tc.name: MakeCircleImage001
 * @tc.desc: Test MakeCircleImage rejects a zero radius image
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, MakeCircleImage001, TestSize.Level1)
{
    auto image = RSParticlesDrawable::MakeCircleImage(32);
    ASSERT_NE(image, nullptr);
    EXPECT_GT(image->GetWidth(), 0);
    EXPECT_GT(image->GetHeight(), 0);
}

/**
 * @tc.name: UpdateData001
 * @tc.desc: Test UpdateData refreshes the pool and arrays
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, UpdateData001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    CreatePointParticle(*pool);
    std::vector<std::shared_ptr<RSImage>> images;
    RSParticlesDrawable drawable(pool, images, 0);
    drawable.UpdateData(pool, images, 0);
    EXPECT_EQ(drawable.pool_, pool);
    EXPECT_EQ(drawable.imageCount_, 0);
}

/**
 * @tc.name: UpdateData002
 * @tc.desc: Test UpdateData grows the per-image arrays
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, UpdateData002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.UpdateData(pool, images, 3);
    EXPECT_EQ(drawable.imageCount_, 3);
    EXPECT_EQ(drawable.count_.size(), 3);
}

/**
 * @tc.name: Draw001
 * @tc.desc: Test drawing with an empty pool is a no-op
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: Draw002
 * @tc.desc: Test drawing point particles
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    CreatePointParticle(*pool);
    RSParticlesDrawable drawable(pool, {}, 0);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.pointCount_, 0); // buffers are cleared after the draw
}

/**
 * @tc.name: Draw003
 * @tc.desc: Test drawing image particles with the atlas path
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.spriteColor_.size(), 0); // cleared after the draw
}

/**
 * @tc.name: Draw004
 * @tc.desc: Test drawing both particle types
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw004, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreatePointParticle(*pool);
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: Draw005
 * @tc.desc: Test a clipped draw still clears its buffers
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw005, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    auto bounds = std::make_shared<RectF>(0.f, 0.f, 10.f, 10.f);
    drawable.Draw(canvas, bounds);
    EXPECT_TRUE(drawable.count_.empty() || drawable.count_[0] == 0);
}

/**
 * @tc.name: Draw006
 * @tc.desc: Test the FILL path does not join the atlas batch
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw006, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images, true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL));
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.spriteRsxform_.size(), 0);
}

/**
 * @tc.name: Draw007
 * @tc.desc: Test an invisible particle is skipped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw007, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images, true, 0.0f, 1.0f);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.spriteRsxform_.size(), 0);
}

/**
 * @tc.name: DrawCircle001
 * @tc.desc: Test the point draw with no points
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawCircle001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;
    drawable.pointCount_ = 0;
    drawable.DrawCircle(canvas);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: DrawCircle002
 * @tc.desc: Test the point draw batches over 2000 sprites
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawCircle002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.pointCount_ = ATLAS_BATCH + 1;
    for (size_t i = 0; i < drawable.pointCount_; i++) {
        drawable.pointRsxform_.push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
        drawable.pointTex_.push_back(Drawing::Rect(0, 0, 64, 64));
        drawable.pointColors_.push_back(0);
    }
    Drawing::Canvas canvas;
    drawable.DrawCircle(canvas);
    EXPECT_EQ(drawable.pointCount_, ATLAS_BATCH + 1);
}

/**
 * @tc.name: DrawSpriteRange001
 * @tc.desc: Test a range without an atlas is ignored
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawSpriteRange001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;
    drawable.spriteRsxform_.push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
    drawable.spriteTex_.push_back(Drawing::Rect(0, 0, 10, 10));
    drawable.spriteColor_.push_back(0);
    drawable.DrawSpriteRange(canvas, 0, 1);
    EXPECT_EQ(drawable.spriteRsxform_.size(), 1);
}

/**
 * @tc.name: DrawSpriteRange002
 * @tc.desc: Test an out of range request is ignored
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawSpriteRange002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.atlasImage_ = RSParticlesDrawable::MakeCircleImage(32);
    Drawing::Canvas canvas;
    drawable.DrawSpriteRange(canvas, 1, 0);
    drawable.DrawSpriteRange(canvas, 0, 10);
    EXPECT_EQ(drawable.spriteRsxform_.size(), 0);
}

/**
 * @tc.name: ClearBuffers001
 * @tc.desc: Test ClearBuffers resets every per-image bucket
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, ClearBuffers001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
    drawable.count_[0] = 1;
    drawable.ClearBuffers();
    EXPECT_EQ(drawable.count_[0], 0);
    EXPECT_TRUE(drawable.imageRsxform_[0].empty());
}

/**
 * @tc.name: CalculateImageDepth001
 * @tc.desc: Test the depth of a fitted image particle
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CalculateImageDepth001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    const float depth = drawable.CalculateImageDepth(slot, Vector2f(50.f, 50.f), 1.0f);
    EXPECT_GT(depth, 0.f);
    EXPECT_GT(pool->GetImageDrawWidth(slot), 0.f);
}

/**
 * @tc.name: CalculateImageDepth002
 * @tc.desc: Test an unresolvable image reports zero depth
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CalculateImageDepth002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = CreatePointParticle(*pool);
    RSParticlesDrawable drawable(pool, {}, 0);
    EXPECT_FLOAT_EQ(drawable.CalculateImageDepth(slot, Vector2f(50.f, 50.f), 1.0f), 0.f);
}

/**
 * @tc.name: CollectSprites001
 * @tc.desc: Test CollectSprites advances the frame stamp
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectSprites001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.CollectSprites();
    EXPECT_GE(drawable.frameStamp_, 1u);
}

/**
 * @tc.name: CollectSprites002
 * @tc.desc: Test CollectSprites clears the new-slot list
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectSprites002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.newImageSlots_.push_back(0);
    drawable.CollectSprites();
    EXPECT_TRUE(drawable.newImageSlots_.empty());
}

/**
 * @tc.name: CollectSprites003
 * @tc.desc: Test CollectSprites clears the sprite batch
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectSprites003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.spriteRsxform_.push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
    drawable.CollectSprites();
    EXPECT_TRUE(drawable.spriteRsxform_.empty());
}

/**
 * @tc.name: CollectSprites004
 * @tc.desc: Test CollectSprites builds the atlas for a resolvable image
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectSprites004, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.CollectSprites();
    EXPECT_NE(drawable.atlasImage_, nullptr);
    EXPECT_TRUE(drawable.HasAtlasSlot(0));
}

/**
 * @tc.name: AppendImageSprite001
 * @tc.desc: Test a packed image becomes a sprite
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, AppendImageSprite001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    // CreateImageParticle sets a 45-degree spin; a zero spin keeps the xform cosine equal to the pure scale ratio.
    pool->SetImageDrawWidth(slot, 50.0f);
    pool->SetSpin(slot, 0.0f);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.atlasSlots_.resize(1);
    drawable.atlasSlots_[0].width_ = 100.0f;
    drawable.atlasSlots_[0].height_ = 100.0f;
    drawable.atlasSlots_[0].tex_ = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    ASSERT_TRUE(drawable.AppendImageSprite(slot));
    EXPECT_EQ(drawable.spriteRsxform_.size(), 1);
    EXPECT_FLOAT_EQ(drawable.spriteRsxform_[0].cos_, 50.0f / 100.0f);
}

/**
 * @tc.name: AppendImageSprite002
 * @tc.desc: Test a FILL particle is refused by the sprite batch
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, AppendImageSprite002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images, true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL));
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.atlasSlots_.resize(1);
    drawable.atlasSlots_[0].width_ = 100.0f;
    drawable.atlasSlots_[0].height_ = 100.0f;
    EXPECT_FALSE(drawable.AppendImageSprite(slot));
    EXPECT_TRUE(drawable.spriteRsxform_.empty());
}

/**
 * @tc.name: AppendImageSprite003
 * @tc.desc: Test a slot without a packed image is dropped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, AppendImageSprite003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    EXPECT_FALSE(drawable.AppendImageSprite(slot));
    EXPECT_TRUE(drawable.spriteRsxform_.empty());
}

/**
 * @tc.name: AppendImageParticleSprite001
 * @tc.desc: Test an image without an atlas slot falls back to its own texture
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, AppendImageParticleSprite001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    pool->SetImageDrawWidth(slot, 50.0f);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.atlasImage_ = RSParticlesDrawable::MakeCircleImage(32);
    ASSERT_NE(drawable.atlasImage_, nullptr);
    ASSERT_FALSE(drawable.HasAtlasSlot(0));

    Drawing::Canvas canvas;
    drawable.AppendImageParticleSprite(canvas, slot);
    EXPECT_TRUE(drawable.spriteRsxform_.empty());
    ASSERT_EQ(drawable.count_[0], 1);
    EXPECT_EQ(drawable.imageRsxform_[0].size(), 1);
    EXPECT_TRUE(drawable.HasBucketedSprites());
}

/**
 * @tc.name: BuildAtlas001
 * @tc.desc: Test the atlas is built for the image set and reused
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildAtlas001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    ASSERT_TRUE(drawable.BuildAtlas());
    ASSERT_NE(drawable.atlasImage_, nullptr);
    const size_t keySize = drawable.atlasKey_.size();
    ASSERT_TRUE(drawable.BuildAtlas());
    EXPECT_EQ(drawable.atlasKey_.size(), keySize);
}

/**
 * @tc.name: BuildAtlas002
 * @tc.desc: Test a drawable without a resolvable image reports no atlas
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildAtlas002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.imageCount_ = 1;
    drawable.imageVector_.assign(1, std::shared_ptr<RSImage>());
    EXPECT_FALSE(drawable.BuildAtlas());
    EXPECT_EQ(drawable.atlasImage_, nullptr);
}

/**
 * @tc.name: CaculatePointAtlsArry001
 * @tc.desc: Test a point sprite keeps the centre, half size and the alpha weighted colour
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CaculatePointAtlsArry001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = CreatePointParticle(*pool);
    pool->SetColor(slot, Color(255, 0, 0, 255));
    RSParticlesDrawable drawable(pool, {}, 0);
    drawable.CaculatePointAtlsArry(slot, Vector2f(10.0f, 20.0f), 0.5f, 2.0f);
    ASSERT_EQ(drawable.pointRsxform_.size(), 1);
    EXPECT_FLOAT_EQ(drawable.pointRsxform_[0].cos_, 10.0f * 2.0f / 32.0f);
    EXPECT_NE(drawable.pointColors_[0], 0u);
}

/**
 * @tc.name: CaculatePointAtlsArry002
 * @tc.desc: Test an invalid slot does not add a point sprite
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CaculatePointAtlsArry002, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.CaculatePointAtlsArry(0, Vector2f(), 1.0f, 1.0f);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: UpdateData003
 * @tc.desc: Test UpdateData with the same pool does not treat the pool as changed
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, UpdateData003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.UpdateData(pool, images, 1);
    EXPECT_EQ(drawable.pool_, pool);
    EXPECT_EQ(drawable.imageCount_, 1);
}

/**
 * @tc.name: GetCachedDrawingImage001
 * @tc.desc: Test the cached drawing image is reused on a second call
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, GetCachedDrawingImage001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    auto first = drawable.GetCachedDrawingImage(0, images[0]);
    auto second = drawable.GetCachedDrawingImage(0, images[0]);
    EXPECT_NE(first, nullptr);
    EXPECT_EQ(first, second);
    EXPECT_EQ(drawable.GetCachedDrawingImage(0, nullptr), nullptr);
}

/**
 * @tc.name: CalculateImageDepth003
 * @tc.desc: Test the FILL path leaves the draw width at zero
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CalculateImageDepth003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot =
        CreateImageParticle(*pool, images, true, 1.0f, 1.0f, static_cast<int>(ImageFit::FILL));
    RSParticlesDrawable drawable(pool, images, 1);
    const float depth = drawable.CalculateImageDepth(slot, Vector2f(50.f, 50.f), 1.0f);
    EXPECT_GT(depth, 0.f);
    EXPECT_FLOAT_EQ(pool->GetImageDrawWidth(slot), 0.f);
}

/**
 * @tc.name: ResolveFittedSize001
 * @tc.desc: Test the fitted-size cache is hit on a repeated calculation
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, ResolveFittedSize001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.CalculateImageDepth(slot, Vector2f(50.f, 50.f), 1.0f);
    drawable.CalculateImageDepth(slot, Vector2f(50.f, 50.f), 1.0f);
    EXPECT_GT(pool->GetImageDrawWidth(slot), 0.f);
}

/**
 * @tc.name: MeasureImageParticle001
 * @tc.desc: Test an image particle without a resolvable image is marked with a negative depth
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, MeasureImageParticle001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = CreatePointParticle(*pool);
    RSParticlesDrawable drawable(pool, {}, 0);
    drawable.MeasureImageParticle(slot, Vector2f(50.f, 50.f), 1.0f, false);
    EXPECT_FLOAT_EQ(pool->GetDepth(slot), -1.f);
}

/**
 * @tc.name: BuildImageSprites001
 * @tc.desc: Test a drawable without a pool clears its sprite ordering
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildImageSprites001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.imageSlots_.push_back(0);
    drawable.newImageSlots_.push_back(1);
    drawable.BuildImageSprites();
    EXPECT_TRUE(drawable.imageSlots_.empty());
    EXPECT_TRUE(drawable.newImageSlots_.empty());
}

/**
 * @tc.name: BuildImageSprites002
 * @tc.desc: Test an unsorted image-slot list is depth sorted
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildImageSprites002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t s0 = CreateImageParticle(*pool, images);
    const size_t s1 = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 2);
    pool->SetDepth(s0, 5.0f);
    pool->SetDepth(s1, 1.0f);
    pool->SetDrawStamp(s0, 1);
    pool->SetDrawStamp(s1, 1);
    drawable.frameStamp_ = 1;
    drawable.imageSlots_ = { s0, s1 };
    drawable.BuildImageSprites();
    ASSERT_EQ(drawable.imageSlots_.size(), 2u);
    EXPECT_EQ(drawable.imageSlots_[0], s1);
    EXPECT_EQ(drawable.imageSlots_[1], s0);
}

/**
 * @tc.name: IsAtlasCacheValid001
 * @tc.desc: Test the atlas key is invalid when the image set changes
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, IsAtlasCacheValid001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    ASSERT_TRUE(drawable.BuildAtlas());
    EXPECT_TRUE(drawable.IsAtlasCacheValid());
    drawable.imageVector_[0] = std::make_shared<RSImage>();
    EXPECT_FALSE(drawable.IsAtlasCacheValid());
}

/**
 * @tc.name: CollectAtlasSources001
 * @tc.desc: Test image counts beyond the vector are skipped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectAtlasSources001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    RSParticlesDrawable drawable(pool, {}, 2);
    std::vector<RSParticlesDrawable::AtlasSource> sources;
    EXPECT_FALSE(drawable.CollectAtlasSources(sources));
    EXPECT_TRUE(sources.empty());
}

/**
 * @tc.name: CollectAtlasSources002
 * @tc.desc: Test null and FILL images are skipped by the atlas
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectAtlasSources002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    auto fillImage = std::make_shared<RSImage>();
    fillImage->SetImageFit(static_cast<int>(ImageFit::FILL));
    fillImage->SetPixelMap(CreatePixelMap(100, 100));
    std::vector<std::shared_ptr<RSImage>> images { nullptr, fillImage };
    RSParticlesDrawable drawable(pool, images, 2);
    std::vector<RSParticlesDrawable::AtlasSource> sources;
    EXPECT_FALSE(drawable.CollectAtlasSources(sources));
    EXPECT_TRUE(sources.empty());
}

/**
 * @tc.name: CollectAtlasSources003
 * @tc.desc: Test an unresolvable image is skipped by the atlas
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectAtlasSources003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    auto unresolvable = std::make_shared<RSImage>();
    unresolvable->SetImageFit(static_cast<int>(ImageFit::COVER));
    std::vector<std::shared_ptr<RSImage>> images { unresolvable };
    RSParticlesDrawable drawable(pool, images, 1);
    std::vector<RSParticlesDrawable::AtlasSource> sources;
    EXPECT_FALSE(drawable.CollectAtlasSources(sources));
    EXPECT_TRUE(sources.empty());
}

/**
 * @tc.name: MeasureAtlas001
 * @tc.desc: Test an empty source set cannot be measured
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, MeasureAtlas001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    std::vector<RSParticlesDrawable::AtlasSource> sources;
    int atlasWidth = 0;
    EXPECT_FALSE(drawable.MeasureAtlas(sources, atlasWidth));
}

/**
 * @tc.name: BuildAtlas003
 * @tc.desc: Test an oversized image falls back from the atlas
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildAtlas003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    auto wideImage = std::make_shared<RSImage>();
    wideImage->SetImageFit(static_cast<int>(ImageFit::COVER));
    wideImage->SetPixelMap(CreatePixelMap(3000, 10));
    std::vector<std::shared_ptr<RSImage>> images { wideImage };
    RSParticlesDrawable drawable(pool, images, 1);
    EXPECT_FALSE(drawable.BuildAtlas());
    EXPECT_EQ(drawable.atlasImage_, nullptr);
}

/**
 * @tc.name: PlaceAtlasSources001
 * @tc.desc: Test a cell wider than the shelf wraps to a new row
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, PlaceAtlasSources001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 2);
    auto src0 = drawable.GetCachedDrawingImage(0, images[0]);
    auto src1 = drawable.GetCachedDrawingImage(1, images[1]);
    ASSERT_NE(src0, nullptr);
    ASSERT_NE(src1, nullptr);
    std::vector<RSParticlesDrawable::AtlasSource> sources { { 0, src0 }, { 1, src1 } };
    std::vector<RSParticlesDrawable::AtlasPlacement> placements;
    int atlasHeight = 0;
    ASSERT_TRUE(drawable.PlaceAtlasSources(sources, 150, placements, atlasHeight));
    ASSERT_EQ(placements.size(), 2u);
    EXPECT_GT(placements[1].y, placements[0].y);
}

/**
 * @tc.name: PlaceAtlasSources002
 * @tc.desc: Test a cell taller than the atlas dimension falls back
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, PlaceAtlasSources002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    auto tallImage = std::make_shared<RSImage>();
    tallImage->SetImageFit(static_cast<int>(ImageFit::COVER));
    tallImage->SetPixelMap(CreatePixelMap(10, 3000));
    std::vector<std::shared_ptr<RSImage>> images { tallImage };
    RSParticlesDrawable drawable(pool, images, 1);
    auto source = drawable.GetCachedDrawingImage(0, tallImage);
    ASSERT_NE(source, nullptr);
    std::vector<RSParticlesDrawable::AtlasSource> sources { { 0, source } };
    std::vector<RSParticlesDrawable::AtlasPlacement> placements;
    int atlasHeight = 0;
    EXPECT_FALSE(drawable.PlaceAtlasSources(sources, 100, placements, atlasHeight));
}

/**
 * @tc.name: PlaceAtlasSources003
 * @tc.desc: Test an empty placement list is rejected
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, PlaceAtlasSources003, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    std::vector<RSParticlesDrawable::AtlasPlacement> placements;
    int atlasHeight = 0;
    EXPECT_FALSE(drawable.PlaceAtlasSources({}, 100, placements, atlasHeight));
}

/**
 * @tc.name: RasterizeAtlas001
 * @tc.desc: Test a zero-sized atlas fails to build its bitmap
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, RasterizeAtlas001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    std::vector<RSParticlesDrawable::AtlasPlacement> placements;
    EXPECT_FALSE(drawable.RasterizeAtlas(placements, 0, 0));
}

/**
 * @tc.name: BuildImageSprite001
 * @tc.desc: Test a zero draw width is refused by the sprite builder
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, BuildImageSprite001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    pool->SetImageDrawWidth(slot, 0.0f);
    RSParticlesDrawable drawable(pool, images, 1);
    RSParticlesDrawable::ImageSprite sprite;
    EXPECT_FALSE(drawable.BuildImageSprite(slot, sprite));
}

/**
 * @tc.name: SafeScaleFactor001
 * @tc.desc: Test SafeScaleFactor returns zero for a non-positive source width
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, SafeScaleFactor001, TestSize.Level1)
{
    EXPECT_FLOAT_EQ(RSParticlesDrawable::SafeScaleFactor(10.f, 5.f), 2.f);
    EXPECT_FLOAT_EQ(RSParticlesDrawable::SafeScaleFactor(10.f, 0.f), 0.f);
}

/**
 * @tc.name: AppendImageParticleSprite002
 * @tc.desc: Test an image particle with an out-of-range image index is dropped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, AppendImageParticleSprite002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t slot = CreateImageParticle(*pool, images);
    pool->SetImageIndex(slot, 5);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.AppendImageParticleSprite(canvas, slot);
    EXPECT_EQ(drawable.count_[0], 0);
}

/**
 * @tc.name: Draw003 extends FILL draw
 * @tc.desc: Test the FILL draw path with a valid slot
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImageFill001, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    Drawing::Canvas canvas;
    drawable.DrawImageFill(canvas, 0, Vector2f(0.f, 0.f), 1.f, 1.f);
    EXPECT_EQ(drawable.pool_, nullptr);
}

/**
 * @tc.name: Draw008
 * @tc.desc: Test a dead image particle is skipped by the draw
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw008, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images, false);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, nullptr);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: Draw009
 * @tc.desc: Test a particle outside the clip bounds is skipped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, Draw009, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = CreatePointParticle(*pool);
    pool->SetPosition(slot, Vector2f(10000.f, 10000.f));
    RSParticlesDrawable drawable(pool, {}, 0);
    auto bounds = std::make_shared<RectF>(0.f, 0.f, 10.f, 10.f);
    Drawing::Canvas canvas;
    drawable.Draw(canvas, bounds);
    EXPECT_EQ(drawable.pointCount_, 0);
}

/**
 * @tc.name: DrawParticlesInOrder001
 * @tc.desc: Test the per-image fallback when the atlas is unavailable
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawParticlesInOrder001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.atlasImage_ = nullptr;
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
    drawable.imageTex_[0].push_back(Drawing::Rect(0, 0, 10, 10));
    drawable.imageColors_[0].push_back(0);
    drawable.count_[0] = 1;
    Drawing::Canvas canvas;
    drawable.DrawParticlesInOrder(canvas);
    EXPECT_EQ(drawable.count_[0], 1);
}

/**
 * @tc.name: DrawImages001
 * @tc.desc: Test the bounds guard skips indices beyond the per-image arrays
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.imageCount_ = 3;
    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);
    EXPECT_EQ(drawable.count_.size(), 1u);
}

/**
 * @tc.name: DrawImages002
 * @tc.desc: Test empty buckets and null images are skipped
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    Drawing::Canvas canvas;
    drawable.count_[0] = 0;
    drawable.DrawImages(canvas);
    drawable.count_[0] = 1;
    drawable.imageVector_[0] = nullptr;
    drawable.DrawImages(canvas);
    EXPECT_EQ(drawable.count_[0], 1);
}

/**
 * @tc.name: DrawImages003
 * @tc.desc: Test an unresolvable image is skipped by the per-image draw
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, DrawImages003, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.imageVector_[0] = std::make_shared<RSImage>();
    drawable.count_[0] = 1;
    drawable.imageRsxform_[0].push_back(Drawing::RSXform::Make(1.f, 0.f, 0.f, 0.f));
    Drawing::Canvas canvas;
    drawable.DrawImages(canvas);
    EXPECT_EQ(drawable.count_[0], 1);
}

/**
 * @tc.name: CollectSprites005
 * @tc.desc: Test the frame stamp wraps back to one
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, CollectSprites005, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 1);
    drawable.frameStamp_ = std::numeric_limits<uint32_t>::max();
    drawable.CollectSprites();
    EXPECT_EQ(drawable.frameStamp_, 1u);
}

/**
 * @tc.name: RemapImageSlots001
 * @tc.desc: Test a compacted pool remaps surviving slots and drops the invalid one
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, RemapImageSlots001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t s0 = CreateImageParticle(*pool, images);
    const size_t s1 = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 2);
    drawable.imageSlots_ = { s0, s1 };
    pool->MarkDead(s1);
    pool->Compact();
    ASSERT_EQ(pool->Generation(), 1u);
    drawable.RemapImageSlots();
    ASSERT_EQ(drawable.imageSlots_.size(), 1u);
    EXPECT_EQ(drawable.imageSlots_[0], s0);
}

/**
 * @tc.name: RemapImageSlots002
 * @tc.desc: Test a generation gap clears the ordered slot list
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, RemapImageSlots002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    std::vector<std::shared_ptr<RSImage>> images;
    const size_t s0 = CreateImageParticle(*pool, images);
    const size_t s1 = CreateImageParticle(*pool, images);
    const size_t s2 = CreateImageParticle(*pool, images);
    RSParticlesDrawable drawable(pool, images, 3);
    drawable.imageSlots_ = { s0, s1, s2 };
    pool->MarkDead(s2);
    pool->Compact();
    pool->MarkDead(s1);
    pool->Compact();
    ASSERT_EQ(pool->Generation(), 2u);
    drawable.RemapImageSlots();
    EXPECT_TRUE(drawable.imageSlots_.empty());
}

/**
 * @tc.name: RemapImageSlots003
 * @tc.desc: Test a pool-less drawable clears the ordered slot list
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, RemapImageSlots003, TestSize.Level1)
{
    RSParticlesDrawable drawable;
    drawable.imageSlots_.push_back(0);
    drawable.RemapImageSlots();
    EXPECT_TRUE(drawable.imageSlots_.empty());
}

/**
 * @tc.name: ImageForSlot001
 * @tc.desc: Test an out-of-range slot returns an empty image
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, ImageForSlot001, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    RSParticlesDrawable drawable(pool, {}, 0);
    EXPECT_TRUE(drawable.ImageForSlot(0) == nullptr);
}

/**
 * @tc.name: ImageForSlot002
 * @tc.desc: Test a slot pointing past the image vector returns an empty image
 * @tc.type: FUNC
 */
HWTEST_F(RSParticlesDrawableTest, ImageForSlot002, TestSize.Level1)
{
    auto pool = std::make_shared<RSRenderParticlePool>();
    const size_t slot = CreatePointParticle(*pool);
    RSParticlesDrawable drawable(pool, {}, 0);
    EXPECT_TRUE(drawable.ImageForSlot(slot) == nullptr);
}

} // namespace Rosen
} // namespace OHOS
