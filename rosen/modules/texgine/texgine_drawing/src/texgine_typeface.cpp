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
TexgineTypeface::TexgineTypeface(): typeface_(SkTypeface::MakeDefault())
{
}

TexgineTypeface::TexgineTypeface(SkTypeface *tf) : TexgineTypeface(sk_sp<SkTypeface>(tf))
{
    if (tf) {
        tf->ref();
    }
}

TexgineTypeface::TexgineTypeface(sk_sp<SkTypeface> typeface): typeface_(typeface)
{
}

TexgineTypeface::TexgineTypeface(void *context)
{
    auto stf = reinterpret_cast<SkTypeface *>(context);
    typeface_ = sk_sp<SkTypeface>(stf);
    if (stf) {
        stf->ref();
    }
}

sk_sp<SkTypeface> TexgineTypeface::GetTypeface() const
{
    return typeface_;
}

size_t TexgineTypeface::GetTableSize(uint32_t tag) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getTableSize(tag);
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data) const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getTableData(tag, offset, length, data);
}

int TexgineTypeface::GetUnitsPerEm() const
{
    if (typeface_ == nullptr) {
        return 0;
    }
    return typeface_->getUnitsPerEm();
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromStream(
    std::unique_ptr<TexgineMemoryStream> stream, int index)
{
    if (stream == nullptr) {
        return nullptr;
    }
    auto skTypeface = SkTypeface::MakeFromStream(stream->GetStream());
    return std::make_shared<TexgineTypeface>(skTypeface);
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromFile(const std::string &path, int index)
{
    auto st = SkTypeface::MakeFromFile(path.c_str(), index);
    return std::make_shared<TexgineTypeface>(st);
}

void TexgineTypeface::GetFamilyName(TexgineString *name) const
{
    if (typeface_ == nullptr || name == nullptr) {
        return;
    }
    typeface_->getFamilyName(name->GetString());
}

std::shared_ptr<TexgineFontStyle> TexgineTypeface::GetFontStyle() const
{
    if (typeface_ == nullptr) {
        return nullptr;
    }

    auto style = typeface_->fontStyle();
    auto texgineFontStyle = std::make_shared<TexgineFontStyle>();
    texgineFontStyle->SetStyle(style);
    return texgineFontStyle;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
