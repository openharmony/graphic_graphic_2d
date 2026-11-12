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

#include <algorithm>
#include <cmath>

#include "platform/common/rs_log.h"
#include "render/rs_particles_drawable.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
constexpr float DOUBLE = 2.f;
constexpr float DEFAULT_RADIUS = 32;
constexpr float HALF_TEXEL = 0.5f;
constexpr int ATLAS_PADDING = 1;
constexpr int ATLAS_MAX_DIM = 2048;
constexpr int64_t ATLAS_MAX_PIXELS = 1LL << 20;
constexpr int MAX_ATLAS_COUNT = 2000;

namespace {
Color ParticleColor(const RSRenderParticle& data, size_t slot)
{
    return Color(static_cast<int16_t>(data.colorR[slot]),
        static_cast<int16_t>(data.colorG[slot]),
        static_cast<int16_t>(data.colorB[slot]),
        static_cast<int16_t>(data.colorA[slot]));
}

Vector2f ParticlePosition(const RSRenderParticle& data, size_t slot)
{
    return Vector2f(data.posX[slot], data.posY[slot]);
}
} // namespace

std::shared_ptr<Drawing::Image> RSParticlesDrawable::sharedCircleImage_ = nullptr;

RSParticlesDrawable::RSParticlesDrawable(const std::shared_ptr<RSRenderParticlePool>& pool,
    const std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount)
    : pool_(pool), imageVector_(imageVector), imageCount_(imageCount)
{
    drawingImageCache_.resize(imageCount_);
    fitCache_.resize(imageCount_);
    count_.resize(imageCount_);
    imageRsxform_.resize(imageCount_);
    imageTex_.resize(imageCount_);
    imageColors_.resize(imageCount_);
}

std::shared_ptr<Drawing::Image> RSParticlesDrawable::GetSharedCircleImage()
{
    if (sharedCircleImage_ == nullptr) {
        sharedCircleImage_ = MakeCircleImage(DEFAULT_RADIUS);
    }
    return sharedCircleImage_;
}

void RSParticlesDrawable::UpdateData(const std::shared_ptr<RSRenderParticlePool>& pool,
    const std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount)
{
    const bool poolChanged = (pool_ != pool);
    pool_ = pool;
    imageVector_ = imageVector;
    imageCount_ = imageCount;
    if (poolChanged) {
        imageSlots_.clear();
        newImageSlots_.clear();
        poolGeneration_ = pool_ == nullptr ? 0 : pool_->Generation();
        rebuildImageOrder_ = true;
    }
    if (count_.size() != imageCount_) {
        drawingImageCache_.resize(imageCount_);
        fitCache_.resize(imageCount_);
        count_.resize(imageCount_);
        imageRsxform_.resize(imageCount_);
        imageTex_.resize(imageCount_);
        imageColors_.resize(imageCount_);
    }
}

std::shared_ptr<Drawing::Image> RSParticlesDrawable::GetCachedDrawingImage(
    size_t imageIndex, const std::shared_ptr<RSImage>& rsImage)
{
    if (rsImage == nullptr || imageIndex >= drawingImageCache_.size()) {
        return nullptr;
    }
    auto& entry = drawingImageCache_[imageIndex];
    const uint64_t uniqueId = rsImage->GetUniqueId();
    if (entry.image != nullptr && entry.owner == rsImage && entry.uniqueId == uniqueId) {
        return entry.image;
    }
    const auto pixelmap = rsImage->GetPixelMap();
    auto drawingImage = pixelmap != nullptr ? RSPixelMapUtil::ExtractDrawingImage(pixelmap) : rsImage->GetImage();
    if (drawingImage == nullptr) {
        entry = DrawingImageCache {};
        return nullptr;
    }
    entry = { rsImage, uniqueId, drawingImage };
    return entry.image;
}

const std::shared_ptr<RSImage>& RSParticlesDrawable::ImageForSlot(size_t slot) const
{
    static const std::shared_ptr<RSImage> empty;
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return empty;
    }
    const uint32_t index = pool_->Data().imageIndex[slot];
    return index < imageVector_.size() ? imageVector_[index] : empty;
}

std::shared_ptr<Drawing::Image> RSParticlesDrawable::MakeCircleImage(int radius)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE };
    bitmap.Build(radius * DOUBLE, radius * DOUBLE, format);
    bitmap.ClearWithColor(Drawing::Color::COLOR_TRANSPARENT);
    auto recordingCanvas = Drawing::Canvas();
    recordingCanvas.Bind(bitmap);
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::CLEAR);
    Drawing::Rect r = { 0, 0, radius * DOUBLE, radius * DOUBLE };
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawRect(r);
    recordingCanvas.DetachBrush();
    brush.SetColor(Drawing::Color::COLOR_BLACK);
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    brush.SetAntiAlias(true);
    recordingCanvas.AttachBrush(brush);
    recordingCanvas.DrawOval(r);
    recordingCanvas.DetachBrush();
    return bitmap.MakeImage();
}

/**
 *  A compressed form of a rotation+scale matrix.
 *
 *  [ cos     -sin    tx ]
 *  [ sin      cos    ty ]
 *  [  0        0      1 ]
 */
Drawing::RSXform RSParticlesDrawable::MakeRSXform(
    Vector2f center, Vector2f position, float cosValue, float sinValue, float scale)
{
    const float cos = cosValue * scale;
    const float sin = sinValue * scale;
    const float tx = position.x_ - cos * center.x_ + sin * center.y_;
    const float ty = position.y_ - sin * center.x_ - cos * center.y_;
    return Drawing::RSXform::Make(cos, sin, tx, ty);
}

void RSParticlesDrawable::CaculatePointAtlsArry(size_t slot, Vector2f position, float opacity, float scale)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return;
    }
    const auto& data = pool_->Data();
    const float radius = data.radius[slot];
    Color color = ParticleColor(data, slot);
    const auto alpha = color.GetAlpha();
    color.SetAlpha(alpha * opacity);
    pointRsxform_.push_back(MakeRSXform(Vector2f(DEFAULT_RADIUS, DEFAULT_RADIUS), position, 1.0f, 0.0f,
        radius * scale / DEFAULT_RADIUS));
    pointTex_.push_back(Drawing::Rect(0, 0, DEFAULT_RADIUS * DOUBLE, DEFAULT_RADIUS * DOUBLE));
    pointColors_.push_back(Drawing::Color(color.AsArgbInt()).CastToColorQuad());
    pointCount_++;
}

void RSParticlesDrawable::ResolveFittedSize(size_t imageIndex, const std::shared_ptr<RSImage>& image,
    const Vector2f& imageSize, const Vector2f& position, float& drawWidth, float& drawHeight)
{
    const uint64_t uniqueId = image->GetUniqueId();
    const ImageFit fit = image->GetImageFit();
    if (imageIndex < fitCache_.size()) {
        const ImageFitCache& cached = fitCache_[imageIndex];
        if (cached.valid && cached.uniqueId == uniqueId && cached.fit == fit && ROSEN_EQ(cached.frameW, imageSize.x_) &&
            ROSEN_EQ(cached.frameH, imageSize.y_)) {
            drawWidth = cached.width;
            drawHeight = cached.height;
            return;
        }
    }
    // Miss: resolve once per image, since the fitted size only depends on the image, fit mode and frame size.
    image->SetFrameRect(RectF(position.x_, position.y_, imageSize.x_, imageSize.y_));
    image->ApplyImageFit();
    const auto dstRect = image->GetDstRect();
    drawWidth = dstRect.GetWidth();
    drawHeight = dstRect.GetHeight();
    if (imageIndex < fitCache_.size()) {
        ImageFitCache& entry = fitCache_[imageIndex];
        entry.uniqueId = uniqueId;
        entry.fit = fit;
        entry.frameW = imageSize.x_;
        entry.frameH = imageSize.y_;
        entry.width = drawWidth;
        entry.height = drawHeight;
        entry.valid = true;
    }
}

float RSParticlesDrawable::CalculateImageDepth(size_t slot, const Vector2f& position, float scale)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return 0.f;
    }
    auto& data = pool_->Data();
    // Reset first: a particle that cannot be fitted must not reuse a stale width from the previous frame.
    data.imageDrawWidth[slot] = 0.f;
    const Vector2f imageSize(
        data.imageSizeX[slot], data.imageSizeY[slot]);
    float width = imageSize.x_;
    float height = imageSize.y_;
    const auto& image = ImageForSlot(slot);
    if (image == nullptr || (image->GetPixelMap() == nullptr && image->GetImage() == nullptr)) {
        return 0.f;
    }
    const uint32_t imageIndex = data.imageIndex[slot];
    if (image->GetImageFit() != ImageFit::FILL) {
        ResolveFittedSize(imageIndex, image, imageSize, position, width, height);
        data.imageDrawWidth[slot] = width;
    }
    return width * height * scale * scale;
}

void RSParticlesDrawable::MeasureImageParticle(size_t slot, const Vector2f& position, float scale,
    bool listedLastFrame)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return;
    }
    auto& data = pool_->Data();
    const uint32_t imageIndex = data.imageIndex[slot];
    if (ImageForSlot(slot) == nullptr || imageIndex >= imageCount_) {
        data.depth[slot] = -1.f;
        return;
    }
    data.depth[slot] = CalculateImageDepth(slot, position, scale);
    data.drawStamp[slot] = frameStamp_;
    if (!listedLastFrame || rebuildImageOrder_) {
        newImageSlots_.push_back(slot);
    }
}

void RSParticlesDrawable::BuildImageSprites()
{
    if (pool_ == nullptr) {
        imageSlots_.clear();
        newImageSlots_.clear();
        return;
    }
    imageSlots_.erase(
        std::remove_if(imageSlots_.begin(), imageSlots_.end(), [this](size_t slot) {
            return slot >= pool_->Count() || pool_->Data().drawStamp[slot] != frameStamp_;
        }),
        imageSlots_.end());
    if (!newImageSlots_.empty()) {
        imageSlots_.reserve(imageSlots_.size() + newImageSlots_.size());
        imageSlots_.insert(imageSlots_.end(), newImageSlots_.begin(), newImageSlots_.end());
    }

    // Depth rarely changes, so a linear is_sorted pass replaces a full sort in the common case.
    const auto byDepth = [this](size_t lhs, size_t rhs) {
        return pool_->Data().depth[lhs] < pool_->Data().depth[rhs];
    };
    if (imageSlots_.size() > 1 && !std::is_sorted(imageSlots_.begin(), imageSlots_.end(), byDepth)) {
        std::stable_sort(imageSlots_.begin(), imageSlots_.end(), byDepth);
    }
    rebuildImageOrder_ = false;
}

bool RSParticlesDrawable::IsAtlasCacheValid() const
{
    // Compares in place to avoid building a key vector on every frame.
    if (!atlasEvaluated_ || atlasKey_.size() != imageCount_) {
        return false;
    }
    for (size_t i = 0; i < imageCount_; i++) {
        const auto& rsImage = i < imageVector_.size() ? imageVector_[i] : nullptr;
        const uint64_t uniqueId = rsImage != nullptr ? rsImage->GetUniqueId() : 0;
        if (atlasKey_[i] != uniqueId) {
            return false;
        }
    }
    return true;
}

void RSParticlesDrawable::RefreshAtlasKey()
{
    atlasKey_.resize(imageCount_);
    for (size_t i = 0; i < imageCount_; i++) {
        const auto& rsImage = i < imageVector_.size() ? imageVector_[i] : nullptr;
        atlasKey_[i] = rsImage != nullptr ? rsImage->GetUniqueId() : 0;
    }
    atlasEvaluated_ = true;

    // Only rebuild when the image set changed: the atlas owns a CPU-side copy of every particle image.
    atlasSlots_.assign(imageCount_, AtlasSlot {});
    atlasImage_ = nullptr;
}

bool RSParticlesDrawable::CollectAtlasSources(std::vector<AtlasSource>& sources)
{
    for (size_t i = 0; i < imageCount_; i++) {
        if (i >= imageVector_.size()) {
            continue;
        }
        // FILL images never sample the atlas, so they get no slot.
        const auto& rsImage = imageVector_[i];
        if (rsImage == nullptr || rsImage->GetImageFit() == ImageFit::FILL) {
            continue;
        }
        auto image = GetCachedDrawingImage(i, rsImage);
        if (image == nullptr || image->GetWidth() <= 0 || image->GetHeight() <= 0) {
            continue;
        }
        sources.push_back({ i, image });
    }
    return !sources.empty();
}

bool RSParticlesDrawable::MeasureAtlas(std::vector<AtlasSource>& sources, int& atlasWidth)
{
    if (sources.empty()) {
        return false;
    }
    // Tallest first so the shelves stay full.
    std::stable_sort(sources.begin(), sources.end(), [](const AtlasSource& lhs, const AtlasSource& rhs) {
        return lhs.image->GetHeight() > rhs.image->GetHeight();
    });

    // All images must fit, so an oversized set falls back whole to per-image draws.
    int64_t totalArea = 0;
    int widest = 0;
    for (const auto& source : sources) {
        const int cellW = static_cast<int>(source.image->GetWidth()) + ATLAS_PADDING * 2;
        const int cellH = static_cast<int>(source.image->GetHeight()) + ATLAS_PADDING * 2;
        const int64_t cellArea = static_cast<int64_t>(cellW) * cellH;
        if (cellW > ATLAS_MAX_DIM || cellH > ATLAS_MAX_DIM || totalArea + cellArea > ATLAS_MAX_PIXELS) {
            ROSEN_LOGD("RSParticlesDrawable::BuildAtlas image %{public}zu cannot fit the atlas budget, falling back",
                source.index);
            return false;
        }
        totalArea += cellArea;
        widest = std::max(widest, cellW);
    }
    const int side = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(totalArea))));
    atlasWidth = std::clamp(side, widest, ATLAS_MAX_DIM);
    return true;
}

bool RSParticlesDrawable::PlaceAtlasSources(const std::vector<AtlasSource>& sources, int atlasWidth,
    std::vector<AtlasPlacement>& placements, int& atlasHeight)
{
    int penX = 0;
    int penY = 0;
    int rowHeight = 0;
    for (const auto& source : sources) {
        const int cellW = static_cast<int>(source.image->GetWidth()) + ATLAS_PADDING * 2;
        const int cellH = static_cast<int>(source.image->GetHeight()) + ATLAS_PADDING * 2;
        if (penX + cellW > atlasWidth) {
            penX = 0;
            penY += rowHeight;
            rowHeight = 0;
        }
        if (penY + cellH > ATLAS_MAX_DIM) {
            ROSEN_LOGD("RSParticlesDrawable::BuildAtlas image %{public}zu does not fit the shelves, falling back",
                source.index);
            return false;
        }
        placements.push_back({ source.index, source.image, penX + ATLAS_PADDING, penY + ATLAS_PADDING });
        penX += cellW;
        rowHeight = std::max(rowHeight, cellH);
    }
    if (placements.empty()) {
        return false;
    }
    // Size by what was placed, not by the last shelf, so a wrapped row leaves no empty band.
    atlasHeight = 0;
    for (const auto& placement : placements) {
        atlasHeight = std::max(atlasHeight, placement.y + static_cast<int>(placement.image->GetHeight()) +
            ATLAS_PADDING);
    }
    return atlasHeight > 0;
}

bool RSParticlesDrawable::RasterizeAtlas(const std::vector<AtlasPlacement>& placements, int atlasWidth,
    int atlasHeight)
{
    Drawing::Bitmap bitmap;
    Drawing::BitmapFormat format { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    if (!bitmap.Build(atlasWidth, atlasHeight, format)) {
        ROSEN_LOGD("RSParticlesDrawable::BuildAtlas bitmap build failed");
        return false;
    }
    bitmap.ClearWithColor(Drawing::Color::COLOR_TRANSPARENT);
    Drawing::Canvas atlasCanvas;
    atlasCanvas.Bind(bitmap);
    for (const auto& placement : placements) {
        atlasCanvas.DrawImage(*placement.image, placement.x, placement.y, Drawing::SamplingOptions());
        const float width = static_cast<float>(placement.image->GetWidth());
        const float height = static_cast<float>(placement.image->GetHeight());
        // Half-texel inset keeps the linear filter off the transparent padding.
        const float inset = (width > 1.f && height > 1.f) ? HALF_TEXEL : 0.f;
        const float left = static_cast<float>(placement.x);
        const float top = static_cast<float>(placement.y);
        AtlasSlot slot;
        slot.tex_ = Drawing::Rect(left + inset, top + inset, left + width - inset, top + height - inset);
        slot.width_ = width;
        slot.height_ = height;
        atlasSlots_[placement.index] = slot;
    }

    atlasImage_ = bitmap.MakeImage();
    if (atlasImage_ == nullptr) {
        ROSEN_LOGD("RSParticlesDrawable::BuildAtlas make image failed");
        return false;
    }
    return true;
}

bool RSParticlesDrawable::BuildAtlas()
{
    // A valid cache also memoises a failure: a set that cannot be packed is not retried until the images change.
    if (IsAtlasCacheValid()) {
        return atlasImage_ != nullptr;
    }
    RefreshAtlasKey();

    std::vector<AtlasSource> sources;
    if (!CollectAtlasSources(sources)) {
        return false;
    }
    int atlasWidth = 0;
    if (!MeasureAtlas(sources, atlasWidth)) {
        return false;
    }
    std::vector<AtlasPlacement> placements;
    int atlasHeight = 0;
    if (!PlaceAtlasSources(sources, atlasWidth, placements, atlasHeight)) {
        return false;
    }
    return RasterizeAtlas(placements, atlasWidth, atlasHeight);
}

void RSParticlesDrawable::CollectSprites()
{
    if (pool_ != nullptr && poolGeneration_ != pool_->Generation()) {
        RemapImageSlots();
    }
    if (imageCount_ > 0) {
        spriteRsxform_.clear();
        spriteTex_.clear();
        spriteColor_.clear();
        frameStamp_++;
        if (frameStamp_ == 0) {
            frameStamp_ = 1;
        }
        if (!BuildAtlas()) {
            ROSEN_LOGD("RSParticlesDrawable::CollectSprites atlas unavailable, falling back to per-sprite draws");
        }
        newImageSlots_.clear();
    }
}

void RSParticlesDrawable::RemapImageSlots()
{
    if (pool_ == nullptr) {
        imageSlots_.clear();
        poolGeneration_ = 0;
        rebuildImageOrder_ = true;
        return;
    }
    const uint32_t generation = pool_->Generation();
    // A gap means draw missed one or more compactions; the slot map no longer covers the old list.
    if (generation != poolGeneration_ + 1 || pool_->SlotMap().empty()) {
        imageSlots_.clear();
        poolGeneration_ = generation;
        rebuildImageOrder_ = true;
        return;
    }
    const auto& slotMap = pool_->SlotMap();
    for (size_t& slot : imageSlots_) {
        slot = slot < slotMap.size() ? slotMap[slot] : RSRenderParticlePool::INVALID_SLOT;
    }
    imageSlots_.erase(std::remove_if(imageSlots_.begin(), imageSlots_.end(),
        [](size_t slot) { return slot == RSRenderParticlePool::INVALID_SLOT; }), imageSlots_.end());
    poolGeneration_ = pool_->Generation();
}

bool RSParticlesDrawable::HasAtlasSlot(size_t imageIndex) const
{
    return imageIndex < atlasSlots_.size() && atlasSlots_[imageIndex].width_ > 0.f &&
        atlasSlots_[imageIndex].height_ > 0.f;
}

bool RSParticlesDrawable::HasBucketedSprites() const
{
    const size_t count = std::min({ imageCount_, count_.size(), imageRsxform_.size() });
    for (size_t i = 0; i < count; i++) {
        if (count_[i] > 0 && i < imageCount_ && i < imageVector_.size() && imageVector_[i] != nullptr) {
            return true;
        }
    }
    return false;
}

bool RSParticlesDrawable::ResolveSpriteSource(size_t imageIndex, Drawing::Rect& tex)
{
    // Atlas slot if packed, otherwise the whole source image, so per-sprite draws still have a texture.
    if (HasAtlasSlot(imageIndex)) {
        tex = atlasSlots_[imageIndex].tex_;
        return true;
    }
    auto source = imageIndex < imageVector_.size() ?
        GetCachedDrawingImage(imageIndex, imageVector_[imageIndex]) : nullptr;
    if (source == nullptr || source->GetWidth() <= 0 || source->GetHeight() <= 0) {
        return false;
    }
    tex = Drawing::Rect(0.0f, 0.0f, static_cast<float>(source->GetWidth()),
        static_cast<float>(source->GetHeight()));
    return true;
}

float RSParticlesDrawable::SafeScaleFactor(float fittedWidth, float sourceWidth)
{
    if (sourceWidth > 0.0f) {
        return fittedWidth / sourceWidth;
    }
    return 0.0f;
}

bool RSParticlesDrawable::BuildImageSprite(size_t slot, ImageSprite& sprite)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return false;
    }
    const auto& data = pool_->Data();
    const uint32_t imageIndex = data.imageIndex[slot];
    if (imageIndex >= imageCount_) {
        return false;
    }
    // FILL has its own path; other fits only scale, which RSXform carries.
    const auto& image = ImageForSlot(slot);
    if (image != nullptr && image->GetImageFit() == ImageFit::FILL) {
        return false;
    }
    const float drawWidth = data.imageDrawWidth[slot];
    Drawing::Rect tex;
    if (drawWidth <= 0.f || !ResolveSpriteSource(imageIndex, tex)) {
        return false;
    }
    const float width = tex.GetWidth();
    const float height = tex.GetHeight();
    const float scaleFactor = SafeScaleFactor(drawWidth, width);
    if (scaleFactor <= 0.f) {
        return false;
    }
    const Vector2f position = ParticlePosition(data, slot);
    Color color = ParticleColor(data, slot);
    color.SetAlpha(color.GetAlpha() * data.opacity[slot]);
    const float spinCos = data.spinCos[slot];
    const float spinSin = data.spinSin[slot];
    sprite.xform_ = MakeRSXform(Vector2f(width / DOUBLE, height / DOUBLE), position, spinCos, spinSin,
        scaleFactor * data.scale[slot]);
    sprite.tex_ = tex;
    sprite.color_ = static_cast<Drawing::ColorQuad>(color.AsArgbInt());
    return true;
}

bool RSParticlesDrawable::AppendImageSprite(size_t slot)
{
    ImageSprite sprite;
    if (!BuildImageSprite(slot, sprite)) {
        return false;
    }
    spriteRsxform_.push_back(sprite.xform_);
    spriteTex_.push_back(sprite.tex_);
    spriteColor_.push_back(sprite.color_);
    return true;
}

void RSParticlesDrawable::AppendImageSpriteToBucket(size_t slot, size_t imageIndex)
{
    ImageSprite sprite;
    if (!BuildImageSprite(slot, sprite)) {
        return;
    }
    imageRsxform_[imageIndex].push_back(sprite.xform_);
    imageTex_[imageIndex].push_back(sprite.tex_);
    imageColors_[imageIndex].push_back(sprite.color_);
    count_[imageIndex]++;
}

void RSParticlesDrawable::AppendImageParticleSprite(Drawing::Canvas& canvas, size_t slot)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return;
    }
    auto& data = pool_->Data();
    const auto& image = ImageForSlot(slot);
    if (image == nullptr) {
        return;
    }
    const Vector2f position = ParticlePosition(data, slot);
    const float opacity = data.opacity[slot];
    const float scale = data.scale[slot];
    if (image->GetImageFit() == ImageFit::FILL) {
        DrawImageFill(canvas, slot, position, opacity, scale);
        return;
    }
    const uint32_t imageIndex = data.imageIndex[slot];
    if (imageIndex >= imageCount_) {
        return;
    }
    if (atlasImage_ != nullptr && HasAtlasSlot(imageIndex)) {
        AppendImageSprite(slot);
        return;
    }
    // No atlas, or this image was not packed into it (e.g. it was not resolvable when the atlas was built):
    // fall back to its own texture instead of dropping the particle.
    AppendImageSpriteToBucket(slot, imageIndex);
}

void RSParticlesDrawable::ReserveBuffers(size_t maxCount)
{
    pointRsxform_.reserve(maxCount);
    pointTex_.reserve(maxCount);
    pointColors_.reserve(maxCount);

    if (imageCount_ > 0) {
        spriteRsxform_.reserve(maxCount);
        spriteTex_.reserve(maxCount);
        spriteColor_.reserve(maxCount);
        imageSlots_.reserve(maxCount);
        imageSlots_.reserve(maxCount);
    }
}

void RSParticlesDrawable::ClipToBounds(Drawing::Canvas& canvas, const std::shared_ptr<RectF>& bounds)
{
    if (bounds) {
        canvas.Save();
        canvas.ClipRect({ bounds->left_, bounds->top_, bounds->left_ + bounds->width_,
            bounds->top_ + bounds->height_ }, Drawing::ClipOp::INTERSECT, true);
    }
}

void RSParticlesDrawable::DrawImageFill(Drawing::Canvas& canvas, size_t slot, Vector2f position, float opacity,
    float scale)
{
    if (pool_ == nullptr || slot >= pool_->Count()) {
        return;
    }
    const auto& image = ImageForSlot(slot);
    if (image == nullptr) {
        return;
    }
    const auto& data = pool_->Data();
    const Vector2f imageSize(
        data.imageSizeX[slot], data.imageSizeY[slot]);
    const float spin = data.spin[slot];
    const float left = position.x_;
    const float top = position.y_;
    const float right = position.x_ + imageSize.x_ * scale;
    const float bottom = position.y_ + imageSize.y_ * scale;
    canvas.Save();
    canvas.Translate(position.x_, position.y_);
    canvas.Rotate(spin, imageSize.x_ * scale / DOUBLE, imageSize.y_ * scale / DOUBLE);
    image->SetScale(scale);
    image->SetImageRepeat(0);
    Drawing::Rect rect { left, top, right, bottom };
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetAlphaF(opacity);
    canvas.AttachBrush(brush);
    image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
    canvas.DetachBrush();
    canvas.Restore();
}

void RSParticlesDrawable::Draw(Drawing::Canvas& canvas, std::shared_ptr<RectF> bounds)
{
    if (pool_ == nullptr || pool_->Count() == 0) {
        ROSEN_LOGE("RSParticlesDrawable::Draw pool is empty");
        return;
    }
    const uint32_t previousStamp = frameStamp_;
    ReserveBuffers(pool_->Count());
    ClipToBounds(canvas, bounds);
    CollectSprites();
    for (size_t slot = 0; slot < pool_->Count(); slot++) {
        if (!pool_->IsAlive(slot)) {
            continue;
        }
        const auto& data = pool_->Data();
        const Vector2f position = ParticlePosition(data, slot);
        const float opacity = data.opacity[slot];
        const float scale = data.scale[slot];
        if (bounds == nullptr || !(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
            continue;
        }
        const auto type = static_cast<ParticleType>(data.particleType[slot]);
        if (type == ParticleType::POINTS) {
            CaculatePointAtlsArry(slot, position, opacity, scale);
        } else {
            const bool listedLastFrame =
                previousStamp != 0 && data.drawStamp[slot] == previousStamp;
            MeasureImageParticle(slot, position, scale, listedLastFrame);
        }
    }

    DrawParticlesInOrder(canvas);

    if (bounds) {
        canvas.Restore();
    }

    ClearBuffers();
}

void RSParticlesDrawable::DrawParticlesInOrder(Drawing::Canvas& canvas)
{
    if (pointCount_ > 0) {
        DrawCircle(canvas);
    }
    if (imageCount_ == 0) {
        imageSlots_.clear();
        return;
    }
    BuildImageSprites();
    spriteRsxform_.reserve(imageSlots_.size());
    for (const size_t slot : imageSlots_) {
        AppendImageParticleSprite(canvas, slot);
    }
    if (atlasImage_ != nullptr) {
        DrawSpriteRange(canvas, 0, spriteColor_.size());
        // Images the atlas could not take still owe a draw; paint them from their own textures.
        if (HasBucketedSprites()) {
            DrawImages(canvas);
        }
        return;
    }
    DrawImages(canvas);
}

void RSParticlesDrawable::DrawSpriteRange(Drawing::Canvas& canvas, size_t begin, size_t end)
{
    // The three arrays are filled together, so one bounds check covers the range.
    if (atlasImage_ == nullptr || begin >= end || end > spriteRsxform_.size() || end > spriteTex_.size() ||
        end > spriteColor_.size()) {
        return;
    }
    // One DrawAtlas call per MAX_ATLAS_COUNT sprites, so the draw order is the sprite order.
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    const Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR);
    canvas.AttachBrush(brush);
    for (size_t offset = begin; offset < end; offset += static_cast<size_t>(MAX_ATLAS_COUNT)) {
        const size_t count = std::min(end - offset, static_cast<size_t>(MAX_ATLAS_COUNT));

        const Drawing::RSXform* xform = &spriteRsxform_[offset];
        const Drawing::Rect* tex = &spriteTex_[offset];
        const Drawing::ColorQuad* colors = &spriteColor_[offset];
        canvas.DrawAtlas(atlasImage_.get(), xform, tex, colors, static_cast<int>(count),
            Drawing::BlendMode::SRC_IN, samplingOptions, nullptr);
    }
    canvas.DetachBrush();
}

void RSParticlesDrawable::DrawCircle(Drawing::Canvas& canvas)
{
    auto circleImage = GetSharedCircleImage();
    if (circleImage == nullptr) {
        return;
    }

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::RSXform* rsxform = pointRsxform_.data();
    Drawing::Rect* tex = pointTex_.data();
    Drawing::ColorQuad* colors = pointColors_.data();
    const Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR);
    canvas.AttachBrush(brush);
    size_t count = pointCount_;
    while (count > MAX_ATLAS_COUNT) {
        canvas.DrawAtlas(circleImage.get(), rsxform, tex, colors, MAX_ATLAS_COUNT, Drawing::BlendMode::DST_IN,
            samplingOptions, nullptr);
        count -= MAX_ATLAS_COUNT;
        rsxform += MAX_ATLAS_COUNT;
        tex += MAX_ATLAS_COUNT;
        colors += MAX_ATLAS_COUNT;
    }
    if (count > 0) {
        canvas.DrawAtlas(circleImage.get(), rsxform, tex, colors, count, Drawing::BlendMode::DST_IN,
            samplingOptions, nullptr);
    }
    canvas.DetachBrush();
}

void RSParticlesDrawable::DrawImages(Drawing::Canvas& canvas)
{
    size_t imageCountCopy = imageCount_;
    const Drawing::SamplingOptions samplingOptions(Drawing::FilterMode::LINEAR);
    for (size_t i = 0; i < imageCountCopy; i++) {
        // One image without particles or without a resolvable texture must not stop the following ones.
        if (i >= count_.size() || i >= imageRsxform_.size() || i >= imageTex_.size() ||
            i >= imageColors_.size() || i >= imageVector_.size()) {
            continue;
        }
        if (count_[i] <= 0 || imageVector_[i] == nullptr) {
            continue;
        }

        auto image = GetCachedDrawingImage(i, imageVector_[i]);
        if (image == nullptr) {
            // Retried on the next frame by the per-image path, so keep this at debug level.
            ROSEN_LOGD("RSParticlesDrawable::Draw image %{public}d is not resolvable", static_cast<int32_t>(i));
            continue;
        }

        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        int count = count_[i];
        Drawing::RSXform* rsxform = imageRsxform_[i].data();
        Drawing::Rect* tex = imageTex_[i].data();
        Drawing::ColorQuad* colors = imageColors_[i].data();
        canvas.AttachBrush(brush);
        while (count > MAX_ATLAS_COUNT) {
            canvas.DrawAtlas(image.get(), rsxform, tex, colors, MAX_ATLAS_COUNT, Drawing::BlendMode::SRC_IN,
                samplingOptions, nullptr);
            count -= MAX_ATLAS_COUNT;
            rsxform += MAX_ATLAS_COUNT;
            tex += MAX_ATLAS_COUNT;
            colors += MAX_ATLAS_COUNT;
        }
        if (count > 0) {
            canvas.DrawAtlas(image.get(), rsxform, tex, colors, count, Drawing::BlendMode::SRC_IN,
                samplingOptions, nullptr);
        }
        canvas.DetachBrush();
    }
}

void RSParticlesDrawable::ClearBuffers()
{
    pointRsxform_.clear();
    pointTex_.clear();
    pointColors_.clear();
    pointCount_ = 0;
    for (size_t i = 0; i < imageCount_; i++) {
        imageRsxform_[i].clear();
        imageTex_[i].clear();
        imageColors_[i].clear();
        count_[i] = 0;
    }

    if (imageCount_ > 0) {
        spriteRsxform_.clear();
        spriteTex_.clear();
        spriteColor_.clear();
    }
}

} // namespace Rosen
} // namespace OHOS
