/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "rosen_text/properties/font_collection_txt.h"

#include "flutter/fml/icu_util.h"
#include "third_party/flutter/skia/include/core/SkFontMgr.h"
#include "third_party/flutter/skia/include/core/SkGraphics.h"
#include "third_party/flutter/skia/include/core/SkStream.h"
#include "third_party/flutter/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "txt/asset_font_manager.h"
#include "txt/test_font_manager.h"

#include "engine_adapter/skia_adapter/skia_canvas.h"
#include "rosen_text/properties/rosen_converter_txt.h"
#include "rosen_text/properties/typography_txt.h"

namespace rosen {
FontCollectionTxt::FontCollectionTxt()
{
    SkGraphics::Init();
    fml::icu::InitializeICU("/system/usr/ohos_icu/icudt67l.dat"); // position of icu
    txtCollection = std::make_shared<txt::FontCollection>();
    txtCollection->SetupDefaultFontManager();

    dynamicFontManager = sk_make_sp<txt::DynamicFontManager>();
    txtCollection->SetDynamicFontManager(dynamicFontManager);
    LoadSystemFont();
}

FontCollectionTxt::~FontCollectionTxt()
{
    txtCollection.reset();
    SkGraphics::PurgeFontCache();
}

std::shared_ptr<txt::FontCollection> FontCollectionTxt::GetFontCollection() const
{
    return txtCollection;
}

void FontCollectionTxt::LoadSystemFont()
{
    txtCollection->LoadSystemFont();
    // 0x4e2d is unicode for '中'.
    std::string locale = "en";
    txtCollection->MatchFallbackFont(0x4e2d, locale);
    txtCollection->GetMinikinFontCollectionForFamilies({"sans-serif"}, locale);
}

void FontCollectionTxt::RegisterTestFonts()
{
}

void FontCollectionTxt::LoadFontFromList(const uint8_t* font_data,
                                         int length,
                                         std::string family_name)
{
    std::unique_ptr<SkStreamAsset> font_stream =
        std::make_unique<SkMemoryStream>(font_data, length, true);
    sk_sp<SkTypeface> typeface =
        SkTypeface::MakeFromStream(std::move(font_stream));
    txt::TypefaceFontAssetProvider& font_provider = dynamicFontManager->font_provider();
    if (family_name.empty()) {
        font_provider.RegisterTypeface(typeface);
    } else {
        font_provider.RegisterTypeface(typeface, family_name);
    }
    txtCollection->ClearFontFamilyCache();
}
} // namespace rosen
