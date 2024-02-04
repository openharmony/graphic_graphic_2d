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

#include "texgine_typeface.h"

#include "src/ports/SkFontMgr_custom.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
TexgineTypeface::TexgineTypeface(): typeface_(SkTypeface::MakeDefault()) {}

TexgineTypeface::TexgineTypeface(sk_sp<SkTypeface> typeface): typeface_(typeface) {}
#else
TexgineTypeface::TexgineTypeface(): typeface_(RSTypeface::MakeDefault()) {}

TexgineTypeface::TexgineTypeface(std::shared_ptr<RSTypeface> typeface): typeface_(typeface) {}
#endif

TexgineTypeface::TexgineTypeface(void *context)
{
#ifndef USE_ROSEN_DRAWING
    auto stf = reinterpret_cast<SkTypeface *>(context);
    typeface_ = sk_sp<SkTypeface>(stf);
    if (stf) {
        stf->ref();
    }
#else
    auto stf = reinterpret_cast<RSTypeface *>(context);
    typeface_ = std::shared_ptr<RSTypeface>{stf, [](auto p) {}};
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkTypeface> TexgineTypeface::GetTypeface() const
#else
std::shared_ptr<RSTypeface> TexgineTypeface::GetTypeface() const
#endif
{
    return typeface_;
}

size_t TexgineTypeface::GetTableSize(uint32_t tag) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return typeface_->getTableSize(tag);
#else
    return typeface_->GetTableSize(tag);
#endif
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return typeface_->getTableData(tag, offset, length, data);
#else
    return typeface_->GetTableData(tag, offset, length, data);
#endif
}

int TexgineTypeface::GetUnitsPerEm() const
{
    if (typeface_ == nullptr) {
        return 0;
    }
#ifndef USE_ROSEN_DRAWING
    return typeface_->getUnitsPerEm();
#else
    return typeface_->GetUnitsPerEm();
#endif
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromStream(
    std::unique_ptr<TexgineMemoryStream> stream, int index)
{
    if (stream == nullptr) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto skTypeface = SkTypeface::MakeFromStream(stream->GetStream());
#else
    auto skTypeface = RSTypeface::MakeFromStream(stream->GetStream());
#endif
    return std::make_shared<TexgineTypeface>(skTypeface);
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromFile(const std::string &path, int index)
{
#ifndef USE_ROSEN_DRAWING
    auto st = SkTypeface::MakeFromFile(path.c_str(), index);
#else
    auto st = RSTypeface::MakeFromFile(path.c_str(), index);
#endif
    return std::make_shared<TexgineTypeface>(st);
}

void TexgineTypeface::GetFamilyName(TexgineString *name) const
{
    if (typeface_ == nullptr || name == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    typeface_->getFamilyName(name->GetString());
#else
    *(name->GetString()) = typeface_->GetFamilyName();
#endif
}

std::shared_ptr<TexgineFontStyle> TexgineTypeface::GetFontStyle() const
{
    if (typeface_ == nullptr) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    auto style = typeface_->fontStyle();
#else
    auto style = typeface_->GetFontStyle();
#endif
    auto texgineFontStyle = std::make_shared<TexgineFontStyle>();
    texgineFontStyle->SetStyle(style);
    return texgineFontStyle;
}

size_t TexgineTypeface::FontStyleDetection()
{
#ifndef USE_ROSEN_DRAWING
    return static_cast<size_t>(typeface_->fontStyle().slant());
#else
    return static_cast<size_t>(typeface_->GetFontStyle().GetSlant());
#endif
}

void TexgineTypeface::InputOriginalStyle(bool primitivism)
{
    rawInformation_ = primitivism;
}

bool TexgineTypeface::DetectRawInformation() const
{
    return rawInformation_;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
