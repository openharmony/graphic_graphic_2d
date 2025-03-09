/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_font_utils.h"
#include "txt/platform.h"

using namespace OHOS;
using namespace Rosen;

namespace {
constexpr char THEME_FONT[] = "OhosThemeFont";
// Default typeface does not support chinese characters, needs to load chinese character ttf file.
constexpr char ZH_CN_TTF[] = "/system/fonts/HarmonyOS_Sans_SC.ttf";
}

std::shared_ptr<Drawing::Typeface> LoadZhCnTypeface()
{
    auto typeface = Drawing::Typeface::MakeFromFile(ZH_CN_TTF);
    if (typeface == nullptr) {
        typeface = Drawing::Typeface::MakeDefault();
    }
    return typeface;
}

std::shared_ptr<Drawing::Typeface> DrawingFontUtils::GetZhCnTypeface()
{
    static std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = LoadZhCnTypeface();
    return zhCnTypeface;
}

std::shared_ptr<Drawing::Font> DrawingFontUtils::GetThemeFont(const Drawing::Font* font)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr = DrawingFontUtils::GetFontMgr();
    if (fontMgr == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Typeface> themeTypeface =
        std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(THEME_FONT, Drawing::FontStyle()));
    if (themeTypeface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::Font> themeFont = std::make_shared<Drawing::Font>(*font);
    themeFont->SetTypeface(themeTypeface);
    return themeFont;
}

std::shared_ptr<Drawing::Font> DrawingFontUtils::MatchThemeFont(const Drawing::Font* font, int32_t unicode)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr = DrawingFontUtils::GetFontMgr();
    if (fontMgr == nullptr) {
        return nullptr;
    }
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetDefaultFontFamilies();
    std::shared_ptr<Drawing::Font> themeFont = std::make_shared<Drawing::Font>(*font);
    for (const auto& family : themeFamilies) {
        std|::shared_ptr<Drawing::Typeface> themeTypeface =
            std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(family.c_str(), Drawing::FontStyle()));
        themeFont->SetTypeface(themeTypeface);
        if (themeFont->UnicharToGlyph(unicode)) {
            return themeFont;
        }
    }
    return nullptr;
}

std::shared_ptr<Drawing::FontMgr> DrawingFontUtils::GetFontMgr(const Drawing::Font* font)
{
    if (!font->IsThemeFontFollowed() || font->GetTypeface() != GetZhCnTypeface()) {
        return nullptr;
    }
    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    if (fontCollection == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return fontMgr;
}