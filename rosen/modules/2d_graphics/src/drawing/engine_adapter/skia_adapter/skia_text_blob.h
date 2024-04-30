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

#ifndef SKIA_TEXT_BLOB_H
#define SKIA_TEXT_BLOB_H

#include <cstdint>
#include <memory>

#include "include/core/SkTextBlob.h"

#include "impl_interface/text_blob_impl.h"
#include "text/text_blob.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaTextBlob : public TextBlobImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    explicit SkiaTextBlob(sk_sp<SkTextBlob> skTextBlob);
    ~SkiaTextBlob() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    static std::shared_ptr<TextBlob> MakeFromText(const void* text, size_t byteLength,
        const Font& font, TextEncoding encoding);
    static std::shared_ptr<TextBlob> MakeFromPosText(const void* text, size_t byteLength,
        const Point pos[], const Font& font, TextEncoding encoding);
    static std::shared_ptr<TextBlob> MakeFromRSXform(const void* text, size_t byteLength,
        const RSXform xform[], const Font& font, TextEncoding encoding);

    sk_sp<SkTextBlob> GetTextBlob() const;

    std::shared_ptr<Data> Serialize(void* ctx) const override;

    static std::shared_ptr<TextBlob> Deserialize(const void* data, size_t size, void* ctx);
    static void GetDrawingGlyphIDforTextBlob(const TextBlob* blob, std::vector<uint16_t>& glyphIds);
    static Path GetDrawingPathforTextBlob(uint16_t glyphId, const TextBlob* blob);
    static void GetDrawingPointsForTextBlob(const TextBlob* blob, std::vector<Point>& points);

    std::shared_ptr<Rect> Bounds() const override;

    uint32_t UniqueID() const override;

private:
    sk_sp<SkTextBlob> skTextBlob_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif