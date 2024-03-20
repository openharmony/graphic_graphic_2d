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

#include "skia_text_blob.h"

#include "include/core/SkFontTypes.h"
#include "include/core/SkRSXform.h"
#include "include/core/SkSerialProcs.h"

#include "skia_adapter/skia_convert_utils.h"
#include "skia_adapter/skia_data.h"
#include "skia_adapter/skia_font.h"
#include "skia_adapter/skia_path.h"
#include "skia_adapter/skia_typeface.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTextBlob::SkiaTextBlob(sk_sp<SkTextBlob> skTextBlob) : skTextBlob_(skTextBlob) {}

sk_sp<SkTextBlob> SkiaTextBlob::GetTextBlob() const
{
    return skTextBlob_;
}

std::shared_ptr<TextBlob> SkiaTextBlob::MakeFromText(const void* text, size_t byteLength,
    const Font& font, TextEncoding encoding)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::MakeFromText(text, byteLength, skiaFont->GetFont(), skEncoding);
    if (!skTextBlob) {
        LOGD("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

std::shared_ptr<TextBlob> SkiaTextBlob::MakeFromPosText(const void* text, size_t byteLength,
    const Point pos[], const Font& font, TextEncoding encoding)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    auto skFont = skiaFont->GetFont();
    const int count = skFont.countText(text, byteLength, skEncoding);
    SkPoint skPts[count];
    for (int i = 0; i < count; ++i) {
        skPts[i] = {pos[i].GetX(), pos[i].GetY()};
    }
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::MakeFromPosText(text, byteLength, skPts, skFont, skEncoding);
    if (!skTextBlob) {
        LOGD("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

std::shared_ptr<TextBlob> SkiaTextBlob::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    auto skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGD("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    SkRSXform skXform;
    if (xform) {
        SkiaConvertUtils::DrawingRSXformCastToSkXform(*xform, skXform);
    }
    sk_sp<SkTextBlob> skTextBlob =
        SkTextBlob::MakeFromRSXform(text, byteLength, xform ? &skXform : nullptr, skiaFont->GetFont(), skEncoding);
    if (!skTextBlob) {
        LOGD("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

std::shared_ptr<Data> SkiaTextBlob::Serialize(void* ctx) const
{
    if (!skTextBlob_) {
        LOGD("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkSerialProcs procs;
    procs.fTypefaceProc = &SkiaTypeface::SerializeTypeface;
    procs.fTypefaceCtx = ctx;
    auto skData = skTextBlob_->serialize(procs);
    auto data = std::make_shared<Data>();
    auto skiaData = data->GetImpl<SkiaData>();
    if (!skiaData) {
        LOGD("skiaData nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    skiaData->SetSkData(skData);
    return data;
}

std::shared_ptr<TextBlob> SkiaTextBlob::Deserialize(const void* data, size_t size, void* ctx)
{
    SkDeserialProcs procs;
    procs.fTypefaceProc = &SkiaTypeface::DeserializeTypeface;
    procs.fTypefaceCtx = ctx;
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::Deserialize(data, size, procs);
    if (!skTextBlob) {
        LOGD("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

void SkiaTextBlob::GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds)
{
    SkTextBlob* skTextBlob = nullptr;
    if (blob) {
        auto skiaBlobImpl = blob->GetImpl<SkiaTextBlob>();
        if (skiaBlobImpl != nullptr) {
            skTextBlob = skiaBlobImpl->GetTextBlob().get();
        }
    }
    GetGlyphIDforTextBlob(skTextBlob, glyphIds);
}

Path SkiaTextBlob::GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob)
{
    SkTextBlob* skTextBlob = nullptr;
    if (blob) {
        skTextBlob = blob->GetImpl<SkiaTextBlob>()->GetTextBlob().get();
    }
    SkPath skPath = GetPathforTextBlob(glyphId, skTextBlob);
    Path path;
    path.GetImpl<SkiaPath>()->SetPath(skPath);
    return path;
}

void SkiaTextBlob::GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points)
{
    SkTextBlob* skTextBlob = nullptr;
    if (blob) {
        auto skiaBlobImpl = blob->GetImpl<SkiaTextBlob>();
        if (skiaBlobImpl != nullptr) {
            skTextBlob = skiaBlobImpl->GetTextBlob().get();
        }
    }
    std::vector<SkPoint> skPoints;
    GetPointsForTextBlob(skTextBlob, skPoints);

    points.reserve(skPoints.size());
    for (const auto& p : skPoints) {
        points.emplace_back(p.x(), p.y());
    }
}

std::shared_ptr<Rect> SkiaTextBlob::Bounds() const
{
    if (skTextBlob_) {
        auto bounds = skTextBlob_->bounds();
        return std::make_shared<Rect>(bounds.left(), bounds.top(), bounds.right(), bounds.bottom());
    }
    return nullptr;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS