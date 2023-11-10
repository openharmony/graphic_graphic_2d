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
    std::shared_ptr<SkiaFont> skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGE("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkTextEncoding skEncoding = static_cast<SkTextEncoding>(encoding);
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::MakeFromText(text, byteLength, skiaFont->GetFont(), skEncoding);
    if (!skTextBlob) {
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

std::shared_ptr<TextBlob> SkiaTextBlob::MakeFromRSXform(const void* text, size_t byteLength,
    const RSXform xform[], const Font& font, TextEncoding encoding)
{
    std::shared_ptr<SkiaFont> skiaFont = font.GetImpl<SkiaFont>();
    if (!skiaFont) {
        LOGE("skiaFont nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

std::shared_ptr<Data> SkiaTextBlob::Serialize() const
{
    if (!skTextBlob_) {
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    SkSerialProcs procs;
    procs.fTypefaceProc = &SkiaTypeface::SerializeTypeface;
    auto skData = skTextBlob_->serialize(procs);
    auto data = std::make_shared<Data>();
    auto skiaData = data->GetImpl<SkiaData>();
    if (!skiaData) {
        LOGE("skiaData nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    skiaData->SetSkData(skData);
    return data;
}

std::shared_ptr<TextBlob> SkiaTextBlob::Deserialize(const void* data, size_t size)
{
    SkDeserialProcs procs;
    procs.fTypefaceProc = &SkiaTypeface::DeserializeTypeface;
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::Deserialize(data, size, procs);
    if (!skTextBlob) {
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS