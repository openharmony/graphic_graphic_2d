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

#ifndef ROSEN_MODULES_SPTEXT_ASSET_FONT_MANAGER_H
#define ROSEN_MODULES_SPTEXT_ASSET_FONT_MANAGER_H

#include <memory>
#include <utility>

#include "utils.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkStream.h"
#include "txt/font_asset_provider.h"
#include "txt/typeface_font_asset_provider.h"

namespace txt {
class AssetFontManager : public SkFontMgr {
public:
    explicit AssetFontManager(std::unique_ptr<FontAssetProvider> fontProvider);

    ~AssetFontManager() override;

protected:
#ifdef USE_M133_SKIA
    sk_sp<SkFontStyleSet> onMatchFamily(const char familyName[]) const override;
#else
    SkFontStyleSet* onMatchFamily(const char familyName[]) const override;
#endif

    std::unique_ptr<FontAssetProvider> fontProvider_;

private:
    int onCountFamilies() const override;

    void onGetFamilyName(int index, SkString* familyName) const override;

#ifdef USE_M133_SKIA
    sk_sp<SkFontStyleSet> onCreateStyleSet(int index) const override;

    sk_sp<SkTypeface> onMatchFamilyStyle(const char familyName[], const SkFontStyle&) const override;

    sk_sp<SkTypeface> onMatchFamilyStyleCharacter(const char familyName[], const SkFontStyle&, const char* bcp47[],
        int bcp47Count, SkUnichar character) const override;
#else
    SkFontStyleSet* onCreateStyleSet(int index) const override;

    SkTypeface* onMatchFamilyStyle(const char familyName[], const SkFontStyle&) const override;

    SkTypeface* onMatchFamilyStyleCharacter(const char familyName[], const SkFontStyle&, const char* bcp47[],
        int bcp47Count, SkUnichar character) const override;
#endif

    sk_sp<SkTypeface> onMakeFromData(sk_sp<SkData>, int ttcIndex) const override;

    sk_sp<SkTypeface> onMakeFromStreamIndex(std::unique_ptr<SkStreamAsset>, int ttcIndex) const override;

    sk_sp<SkTypeface> onMakeFromStreamArgs(std::unique_ptr<SkStreamAsset>, const SkFontArguments&) const override;

    sk_sp<SkTypeface> onMakeFromFile(const char path[], int ttcIndex) const override;

    sk_sp<SkTypeface> onLegacyMakeTypeface(const char familyName[], SkFontStyle) const override;

    DISALLOW_COPY_AND_ASSIGN(AssetFontManager);
};

class DynamicFontManager : public AssetFontManager {
public:
    DynamicFontManager() : AssetFontManager(
        std::make_unique<TypefaceFontAssetProvider>()) {}

    TypefaceFontAssetProvider& font_provider() const
    {
        return static_cast<TypefaceFontAssetProvider&>(*fontProvider_);
    }
    int ParseInstallFontConfig(const std::string& configPath, std::vector<std::string>& fontPathVec);
};

class TestFontManager : public AssetFontManager {
public:
    TestFontManager(std::unique_ptr<FontAssetProvider> fontProvider, std::vector<std::string> familyNames);

    ~TestFontManager() override;

private:
    std::vector<std::string> testFontFamilyNames_;

#ifdef USE_M133_SKIA
    sk_sp<SkFontStyleSet> onMatchFamily(const char familyName[]) const override;
#else
    SkFontStyleSet* onMatchFamily(const char familyName[]) const override;
#endif

    DISALLOW_COPY_AND_ASSIGN(TestFontManager);
};
} // namespace txt

#endif // ROSEN_MODULES_SPTEXT_ASSET_FONT_MANAGER_H
