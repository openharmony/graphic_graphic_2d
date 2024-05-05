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
TexgineTypeface::TexgineTypeface(): typeface_(RSTypeface::MakeDefault()) {}

TexgineTypeface::TexgineTypeface(std::shared_ptr<RSTypeface> typeface): typeface_(typeface) {}

TexgineTypeface::TexgineTypeface(void *context) : typeface_(
    std::shared_ptr<RSTypeface>{reinterpret_cast<RSTypeface *>(context), [](auto p) {}}) {}

std::shared_ptr<RSTypeface> TexgineTypeface::GetTypeface() const
{
    return typeface_;
}

size_t TexgineTypeface::GetTableSize(uint32_t tag) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->GetTableSize(tag);
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->GetTableData(tag, offset, length, data);
}

int TexgineTypeface::GetUnitsPerEm() const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->GetUnitsPerEm();
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromStream(
    std::unique_ptr<TexgineMemoryStream> stream, int index)
{
    if (stream == nullptr) {
        return nullptr;
    }
    auto skTypeface = RSTypeface::MakeFromStream(stream->GetStream());
    return std::make_shared<TexgineTypeface>(skTypeface);
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromFile(const std::string &path, int index)
{
    auto st = RSTypeface::MakeFromFile(path.c_str(), index);
    return std::make_shared<TexgineTypeface>(st);
}

void TexgineTypeface::GetFamilyName(TexgineString *name) const
{
    if (typeface_ == nullptr || name == nullptr) {
        return;
    }
    *(name->GetString()) = typeface_->GetFamilyName();
}

std::shared_ptr<TexgineFontStyle> TexgineTypeface::GetFontStyle() const
{
    if (typeface_ == nullptr) {
        return nullptr;
    }
    auto style = typeface_->GetFontStyle();
    auto texgineFontStyle = std::make_shared<TexgineFontStyle>();
    texgineFontStyle->SetStyle(style);
    return texgineFontStyle;
}

size_t TexgineTypeface::FontStyleDetection()
{
    return static_cast<size_t>(typeface_->GetFontStyle().GetSlant());
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
