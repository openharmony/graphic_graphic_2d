/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "text/text_blob.h"

#include <cstring>

#include "impl_interface/text_blob_impl.h"
#include "static_factory.h"
#include "text/font_mgr.h"
#include "text/text_blob_builder.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
constexpr size_t GLYPH_ID_BYTES = sizeof(uint16_t);
constexpr size_t POS_PER_GLYPH = 2; // x and y coordinates for each glyph position

std::vector<std::shared_ptr<TextBlob>> SingleTextBlob(std::shared_ptr<TextBlob> textBlob)
{
    std::vector<std::shared_ptr<TextBlob>> textBlobs;
    if (textBlob != nullptr) {
        textBlobs.push_back(std::move(textBlob));
    }
    return textBlobs;
}

// Builds one text blob per fallback run. When pos is null, glyph positions start at currentX and
// advance within each run, so all blobs share the coordinate system of the whole string; when pos
// is provided, each glyph keeps the caller-supplied position of its codepoint.
std::vector<std::shared_ptr<TextBlob>> MakeFallbackTextBlobs(const std::vector<FontFallbackInfo>& fallbacks,
    const Font& font, const Point pos[])
{
    std::vector<std::shared_ptr<TextBlob>> textBlobs;
    textBlobs.reserve(fallbacks.size());
    scalar currentX = 0.f;
    size_t offset = 0;
    for (const auto& run : fallbacks) {
        if (run.glyphIds.empty()) {
            continue;
        }
        auto glyphCount = static_cast<int>(run.glyphIds.size());
        Font runFont(font);
        if (run.typeface != nullptr) {
            runFont.SetTypeface(run.typeface);
        }
        std::vector<Point> glyphPos;
        if (pos == nullptr) {
            glyphPos.resize(run.glyphIds.size());
            runFont.GetPos(run.glyphIds.data(), glyphCount, glyphPos.data(), Point(currentX, 0.f));
        }
        TextBlobBuilder textBlobBuilder;
        const TextBlobBuilder::RunBuffer& runBuffer = textBlobBuilder.AllocRunPos(runFont, glyphCount);
        for (size_t i = 0; i < run.glyphIds.size(); i++) {
            runBuffer.glyphs[i] = run.glyphIds[i];
            Point glyphPoint = pos != nullptr ? pos[offset + i] : glyphPos[i];
            runBuffer.pos[POS_PER_GLYPH * i] = glyphPoint.GetX();
            runBuffer.pos[POS_PER_GLYPH * i + 1] = glyphPoint.GetY();
        }
        if (pos == nullptr) {
            currentX += runFont.MeasureText(run.glyphIds.data(), run.glyphIds.size() * GLYPH_ID_BYTES,
                TextEncoding::GLYPH_ID);
        }
        offset += run.glyphIds.size();
        std::shared_ptr<TextBlob> textBlob = textBlobBuilder.Make();
        if (textBlob != nullptr) {
            textBlobs.push_back(std::move(textBlob));
        }
    }
    return textBlobs;
}
} // namespace

TextBlob::TextBlob(std::shared_ptr<TextBlobImpl> textBlobImpl) noexcept : textBlobImpl_(textBlobImpl)
{
    options_.bits.textContrast = static_cast<uint32_t>(ProcessTextConstrast::Instance().GetTextContrast());
}

std::shared_ptr<TextBlob> TextBlob::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    return StaticFactory::MakeFromText(text, byteLength, font, encoding);
}

std::shared_ptr<TextBlob> TextBlob::MakeFromPosText(const void* text, size_t byteLength,
    const Point pos[], const Font& font, TextEncoding encoding)
{
    return StaticFactory::MakeFromPosText(text, byteLength, pos, font, encoding);
}

std::shared_ptr<TextBlob> TextBlob::MakeFromString(const char* str, const Font& font, TextEncoding encoding)
{
    if (!str) {
        return nullptr;
    }
    return MakeFromText(str, strlen(str), font, encoding);
}

std::shared_ptr<TextBlob> TextBlob::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    return StaticFactory::MakeFromRSXform(text, byteLength, xform, font, encoding);
}

std::vector<std::shared_ptr<TextBlob>> TextBlob::MakeFromTextWithFallback(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    if (text == nullptr || byteLength == 0) {
        return {};
    }
    if (encoding == TextEncoding::GLYPH_ID) {
        return SingleTextBlob(MakeFromText(text, byteLength, font, encoding));
    }
    std::vector<FontFallbackInfo> fallbacks = font.TextToGlyphsWithFallback(text, byteLength, encoding);
    if (fallbacks.empty()) {
        return {};
    }
    return MakeFallbackTextBlobs(fallbacks, font, nullptr);
}

std::vector<std::shared_ptr<TextBlob>> TextBlob::MakeFromPosTextWithFallback(const void* text, size_t byteLength,
    const Point pos[], const Font& font, TextEncoding encoding)
{
    if (text == nullptr || byteLength == 0 || pos == nullptr) {
        return {};
    }
    if (encoding == TextEncoding::GLYPH_ID) {
        return SingleTextBlob(MakeFromPosText(text, byteLength, pos, font, encoding));
    }
    std::vector<FontFallbackInfo> fallbacks = font.TextToGlyphsWithFallback(text, byteLength, encoding);
    if (fallbacks.empty()) {
        return {};
    }
    size_t totalGlyphs = 0;
    for (const auto& run : fallbacks) {
        totalGlyphs += run.glyphIds.size();
    }
    int posCount = font.CountText(text, byteLength, encoding);
    if (posCount <= 0 || totalGlyphs > static_cast<size_t>(posCount)) {
        LOGE("MakeFromPosTextWithFallback: fallback glyphs %zu exceed pos count %d", totalGlyphs, posCount);
        return {};
    }
    return MakeFallbackTextBlobs(fallbacks, font, pos);
}

std::vector<std::shared_ptr<TextBlob>> TextBlob::MakeFromStringWithFallback(const char* str, const Font& font,
    TextEncoding encoding)
{
    if (!str) {
        return {};
    }
    return MakeFromTextWithFallback(str, strlen(str), font, encoding);
}

int TextBlob::GetIntercepts(const SkScalar bounds[], SkScalar intervals[], const Paint *paint)
{
    if (textBlobImpl_) {
        return textBlobImpl_->GetIntercepts(bounds, intervals, paint);
    }
    return 0;
}
std::shared_ptr<Data> TextBlob::Serialize(void* ctx) const
{
    if (!textBlobImpl_) {
        return nullptr;
    }
    return textBlobImpl_->Serialize(ctx);
}

std::shared_ptr<TextBlob> TextBlob::Deserialize(const void* data, size_t size, void* ctx)
{
    return StaticFactory::DeserializeTextBlob(data, size, ctx);
}

void TextBlob::GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
{
    StaticFactory::GetDrawingGlyphIDforTextBlob(blob, glyphIds);
}

Path TextBlob::GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob)
{
    return StaticFactory::GetDrawingPathforTextBlob(glyphId, blob);
}

void TextBlob::GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points)
{
    return StaticFactory::GetDrawingPointsForTextBlob(blob, points);
}

std::shared_ptr<Rect> TextBlob::Bounds() const
{
    if (textBlobImpl_) {
        return textBlobImpl_->Bounds();
    }
    return nullptr;
}

uint32_t TextBlob::UniqueID() const
{
    if (textBlobImpl_) {
        return textBlobImpl_->UniqueID();
    }
    return 0;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
