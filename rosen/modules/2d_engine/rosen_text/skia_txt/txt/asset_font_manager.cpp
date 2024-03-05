/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#include "txt/asset_font_manager.h"

#include <memory>

#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"

namespace txt {
AssetFontManager::AssetFontManager(std::unique_ptr<FontAssetProvider> fontProvider)
    : fontProvider_(std::move(fontProvider))
{
    SkASSERT(fontProvider_ != nullptr);
}

AssetFontManager::~AssetFontManager() = default;

int AssetFontManager::onCountFamilies() const
{
    return fontProvider_->GetFamilyCount();
}

void AssetFontManager::onGetFamilyName(int index, SkString* familyName) const
{
    familyName->set(fontProvider_->GetFamilyName(index).c_str());
}

SkFontStyleSet* AssetFontManager::onCreateStyleSet(int index) const
{
    SkASSERT(false);
    return nullptr;
}

SkFontStyleSet* AssetFontManager::onMatchFamily(const char name[]) const
{
    std::string familyName(name);
    return fontProvider_->MatchFamily(familyName);
}

SkTypeface* AssetFontManager::onMatchFamilyStyle(const char familyName[], const SkFontStyle& style) const
{
    SkFontStyleSet* fontStyleSet = fontProvider_->MatchFamily(std::string(familyName ? familyName : ""));
    if (fontStyleSet == nullptr)
        return nullptr;
    return fontStyleSet->matchStyle(style);
}

SkTypeface* AssetFontManager::onMatchFamilyStyleCharacter(
    const char familyName[], const SkFontStyle&, const char* bcp47[], int bcp47Count, SkUnichar character) const
{
    return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromData(sk_sp<SkData>, int ttcIndex) const
{
    SkASSERT(false);
    return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromStreamIndex(std::unique_ptr<SkStreamAsset>, int ttcIndex) const
{
    SkASSERT(false);
    return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromStreamArgs(std::unique_ptr<SkStreamAsset>, const SkFontArguments&) const
{
    SkASSERT(false);
    return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onMakeFromFile(const char path[], int ttcIndex) const
{
    SkASSERT(false);
    return nullptr;
}

sk_sp<SkTypeface> AssetFontManager::onLegacyMakeTypeface(const char familyName[], SkFontStyle) const
{
    return nullptr;
}

TestFontManager::TestFontManager(
    std::unique_ptr<FontAssetProvider> fontProvider, std::vector<std::string> familyNames)
    : AssetFontManager(std::move(fontProvider)), testFontFamilyNames_(familyNames)
{}

TestFontManager::~TestFontManager() = default;

SkFontStyleSet* TestFontManager::onMatchFamily(const char familyName[]) const
{
    std::string requestedName(familyName);
    std::string sanitizedName = testFontFamilyNames_[0];
    for (const std::string& family : testFontFamilyNames_) {
        if (requestedName == family) {
            sanitizedName = family;
        }
    }
    return AssetFontManager::onMatchFamily(sanitizedName.c_str());
}
} // namespace txt
