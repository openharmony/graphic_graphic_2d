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

#include "skia_typeface.h"

#include "include/core/SkStream.h"
#include "include/core/SkString.h"
#include "include/core/SkFontStyle.h"

#include "skia_adapter/skia_convert_utils.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTypeface::SkiaTypeface(sk_sp<SkTypeface> skTypeface) : skTypeface_(skTypeface) {}

sk_sp<SkTypeface> SkiaTypeface::GetTypeface() const
{
    return skTypeface_;
}

std::string SkiaTypeface::GetFamilyName() const
{
    std::string name;
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return name;
    }
    SkString skName;
    skTypeface_->getFamilyName(&skName);
    SkiaConvertUtils::SkStringCastToStdString(skName, name);
    return name;
}

FontStyle SkiaTypeface::GetFontStyle() const
{
    FontStyle fontStyle;
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return fontStyle;
    }
    SkFontStyle skFontStyle = skTypeface_->fontStyle();
    SkiaConvertUtils::SkFontStyleCastToDrawingFontStyle(skFontStyle, fontStyle);
    return fontStyle;
}

size_t SkiaTypeface::GetTableSize(uint32_t tag) const
{
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return 0;
    }
    return skTypeface_->getTableSize(tag);
}

size_t SkiaTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void* data) const
{
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return 0;
    }
    return skTypeface_->getTableData(tag, offset, length, data);
}

bool SkiaTypeface::GetItalic() const
{
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }
    return skTypeface_->isItalic();
}

uint32_t SkiaTypeface::GetUniqueID() const
{
    if (!skTypeface_) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return 0;
    }
    return skTypeface_->uniqueID();
}

std::shared_ptr<Typeface> SkiaTypeface::MakeFromFile(const char path[])
{
    sk_sp<SkTypeface> skTypeface = SkTypeface::MakeFromFile(path);
    if (!skTypeface) {
        LOGE("skTypeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TypefaceImpl> typefaceImpl = std::make_shared<SkiaTypeface>(skTypeface);
    return std::make_shared<Typeface>(typefaceImpl);
}

sk_sp<SkData> SkiaTypeface::SerializeTypeface(SkTypeface* typeface, void* ctx)
{
    if (!typeface) {
        LOGE("typeface nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    return typeface->serialize();
}

sk_sp<SkTypeface> SkiaTypeface::DeserializeTypeface(const void* data, size_t length, void* ctx)
{
    SkMemoryStream stream(data, length);
    return SkTypeface::MakeDeserialize(&stream);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS