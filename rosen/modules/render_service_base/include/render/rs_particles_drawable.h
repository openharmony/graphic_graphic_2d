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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_PARTICLE_DRAWABLE_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_PARTICLE_DRAWABLE_H

#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_pool.h"
#include "draw/canvas.h"
#include "image/image.h"
#include "render/rs_image.h"
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSParticlesDrawable {
public:
    explicit RSParticlesDrawable(const std::shared_ptr<RSRenderParticlePool>& pool,
        const std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount);
    RSParticlesDrawable() = default;
    ~RSParticlesDrawable() = default;
    void Draw(Drawing::Canvas& canvas, std::shared_ptr<RectF> bounds);
    void UpdateData(const std::shared_ptr<RSRenderParticlePool>& pool,
        const std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount);

private:
    struct DrawingImageCache {
        std::shared_ptr<RSImage> owner;
        uint64_t uniqueId = 0;
        std::shared_ptr<Drawing::Image> image;
    };

    // One source image in the atlas; tex_ is the sampled sub-rect, inset half a texel to keep the filter off padding.
    struct AtlasSlot {
        Drawing::Rect tex_;
        float width_ = 0.f;
        float height_ = 0.f;
    };

    struct ImageSprite {
        Drawing::RSXform xform_;
        Drawing::Rect tex_;
        Drawing::ColorQuad color_ = 0;
    };

    struct ImageFitCache {
        uint64_t uniqueId = 0;
        ImageFit fit = ImageFit::COVER;
        float frameW = 0.f;
        float frameH = 0.f;
        float width = 0.f;
        float height = 0.f;
        bool valid = false;
    };

    struct AtlasSource {
        size_t index = 0;
        std::shared_ptr<Drawing::Image> image;
    };

    struct AtlasPlacement {
        size_t index = 0;
        std::shared_ptr<Drawing::Image> image;
        int x = 0;
        int y = 0;
    };

    static std::shared_ptr<Drawing::Image> GetSharedCircleImage();
    static std::shared_ptr<Drawing::Image> MakeCircleImage(int radius);
    // Same transform from an already evaluated sin/cos pair (see the SPIN_COS / SPIN_SIN rows).
    Drawing::RSXform MakeRSXform(
        Vector2f ofs, Vector2f position, float cosValue, float sinValue, float scale);
    void CaculatePointAtlsArry(size_t slot, Vector2f position, float opacity, float scale);
    void DrawCircle(Drawing::Canvas& canvas);
    void DrawImages(Drawing::Canvas& canvas);
    void ReserveBuffers(size_t maxCount);
    void ClipToBounds(Drawing::Canvas& canvas, const std::shared_ptr<RectF>& bounds);
    bool BuildAtlas();
    // True when the cached atlas already describes the current image set; a cached failure counts as valid too.
    bool IsAtlasCacheValid() const;
    void RefreshAtlasKey();
    bool CollectAtlasSources(std::vector<AtlasSource>& sources);
    // Sorts the sources into shelves and rejects the whole set when it cannot fit the atlas budget.
    bool MeasureAtlas(std::vector<AtlasSource>& sources, int& atlasWidth);
    bool PlaceAtlasSources(const std::vector<AtlasSource>& sources, int atlasWidth,
        std::vector<AtlasPlacement>& placements, int& atlasHeight);
    bool RasterizeAtlas(const std::vector<AtlasPlacement>& placements, int atlasWidth, int atlasHeight);
    void CollectSprites();
    void MeasureImageParticle(size_t slot, const Vector2f& position, float scale, bool listedLastFrame);
    void BuildImageSprites();
    bool BuildImageSprite(size_t slot, ImageSprite& sprite);
    // Fills tex with the atlas sub-rect, or with the whole source image when the atlas could not pack it.
    bool ResolveSpriteSource(size_t imageIndex, Drawing::Rect& tex);
    bool AppendImageSprite(size_t slot);
    // Routes one image particle to its draw path: FILL drawn immediately, the rest to the atlas run or a bucket.
    void AppendImageParticleSprite(Drawing::Canvas& canvas, size_t slot);
    void AppendImageSpriteToBucket(size_t slot, size_t imageIndex);
    // Fitted width over source width; returns 0 when the source width is not positive.
    static float SafeScaleFactor(float fittedWidth, float sourceWidth);
    bool HasAtlasSlot(size_t imageIndex) const;
    // True when at least one image particle fell back to the per-image buckets.
    bool HasBucketedSprites() const;
    void DrawImageFill(Drawing::Canvas& canvas, size_t slot, Vector2f position, float opacity, float scale);
    void DrawSpriteRange(Drawing::Canvas& canvas, size_t begin, size_t end);
    void DrawParticlesInOrder(Drawing::Canvas& canvas);
    float CalculateImageDepth(size_t slot, const Vector2f& position, float scale);
    void ResolveFittedSize(size_t imageIndex, const std::shared_ptr<RSImage>& image, const Vector2f& imageSize,
        const Vector2f& position, float& drawWidth, float& drawHeight);
    std::shared_ptr<Drawing::Image> GetCachedDrawingImage(
        size_t imageIndex, const std::shared_ptr<RSImage>& rsImage);
    const std::shared_ptr<RSImage>& ImageForSlot(size_t slot) const;
    void ClearBuffers();
    void RemapImageSlots();

    std::shared_ptr<RSRenderParticlePool> pool_;
    static std::shared_ptr<Drawing::Image> sharedCircleImage_;
    std::vector<std::shared_ptr<RSImage>> imageVector_;
    size_t imageCount_ = 0;

    size_t pointCount_ = 0;
    std::vector<Drawing::RSXform> pointRsxform_;
    std::vector<Drawing::Rect> pointTex_;
    std::vector<Drawing::ColorQuad> pointColors_;
    std::vector<DrawingImageCache> drawingImageCache_;

    uint32_t frameStamp_ = 0;
    uint32_t poolGeneration_ = 0;
    bool rebuildImageOrder_ = false;
    // Parallel arrays DrawAtlas consumes, so each sprite is stored once.
    std::vector<Drawing::RSXform> spriteRsxform_;
    std::vector<Drawing::Rect> spriteTex_;
    std::vector<Drawing::ColorQuad> spriteColor_;
    std::vector<size_t> imageSlots_;
    std::vector<size_t> newImageSlots_;
    std::vector<int> count_; // Counts the number of each image
    std::vector<std::vector<Drawing::RSXform>> imageRsxform_;
    std::vector<std::vector<Drawing::Rect>> imageTex_;
    std::vector<std::vector<Drawing::ColorQuad>> imageColors_;
    std::vector<ImageFitCache> fitCache_;
    std::vector<AtlasSlot> atlasSlots_;
    std::shared_ptr<Drawing::Image> atlasImage_;
    std::vector<uint64_t> atlasKey_;
    bool atlasEvaluated_ = false;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_PARTICLE_DRAWABLE_H
