/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "txt/typeface_font_asset_provider.h"

#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"

namespace txt {
TypefaceFontAssetProvider::TypefaceFontAssetProvider() = default;

TypefaceFontAssetProvider::~TypefaceFontAssetProvider() = default;

size_t TypefaceFontAssetProvider::GetFamilyCount() const
{
    return familyNames_.size();
}

std::string TypefaceFontAssetProvider::GetFamilyName(int index) const
{
    return familyNames_[index];
}

SkFontStyleSet* TypefaceFontAssetProvider::MatchFamily(const std::string& familyName)
{
    auto found = registeredFamilies_.find(CanonicalFamilyName(familyName));
    if (found == registeredFamilies_.end()) {
        return nullptr;
    }
    return (SkFontStyleSet*)(found->second.get());
}

void TypefaceFontAssetProvider::RegisterTypeface(sk_sp<SkTypeface> typeface)
{
    if (typeface == nullptr) {
        return;
    }

    SkString name;
    typeface->getFamilyName(&name);

    std::string familyName(name.c_str(), name.size());
    RegisterTypeface(std::move(typeface), std::move(familyName));
}

void TypefaceFontAssetProvider::RegisterTypeface(sk_sp<SkTypeface> typeface, std::string familyNameAlias)
{
    if (familyNameAlias.empty()) {
        return;
    }

    std::string canonicalName = CanonicalFamilyName(familyNameAlias);
    auto iter = registeredFamilies_.find(canonicalName);
    if (iter == registeredFamilies_.end()) {
        familyNames_.push_back(familyNameAlias);
        auto value = std::make_pair(canonicalName, sk_make_sp<TypefaceFontStyleSet>());
        iter = registeredFamilies_.emplace(value).first;
    }
    iter->second->registerTypeface(std::move(typeface));
}

TypefaceFontStyleSet::TypefaceFontStyleSet() = default;

TypefaceFontStyleSet::~TypefaceFontStyleSet() = default;

void TypefaceFontStyleSet::registerTypeface(sk_sp<SkTypeface> typeface)
{
    if (typeface != nullptr) {
        typefaces_.emplace_back(std::move(typeface));
    }
}

int TypefaceFontStyleSet::count()
{
    return typefaces_.size();
}

void TypefaceFontStyleSet::getStyle(int index, SkFontStyle* style, SkString* name)
{
    SkASSERT(static_cast<size_t>(index) < typefaces_.size());
    if (style) {
        *style = typefaces_[index]->fontStyle();
    }
    if (name) {
        name->reset();
    }
}

SkTypeface* TypefaceFontStyleSet::createTypeface(int index)
{
    if (index < typefaces_.size()) {
        return (SkTypeface*)(typefaces_[index].get());
    }
    return nullptr;
}

SkTypeface* TypefaceFontStyleSet::matchStyle(const SkFontStyle& pattern)
{
    return matchStyleCSS3(pattern);
}
} // namespace txt
