/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "c/drawing_typeface.h"
#include "c/drawing_font_mgr.h"
#include <mutex>
#include <unordered_map>

#include "text/font_mgr.h"
#include "text/typeface.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_fontMgrLockMutex;
static std::unordered_map<void*, std::shared_ptr<FontMgr>> g_fontMgrMap;

static FontMgr* CastToFontMgr(OH_Drawing_FontMgr* cFontMgr)
{
    return reinterpret_cast<FontMgr*>(cFontMgr);
}

static FontStyleSet* CastToFontStyleSet(OH_Drawing_FontStyleSet* cFontStyleSet)
{
    return reinterpret_cast<FontStyleSet*>(cFontStyleSet);
}

OH_Drawing_FontMgr* OH_Drawing_FontMgrCreate()
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    std::lock_guard<std::mutex> lock(g_fontMgrLockMutex);
    g_fontMgrMap.insert({fontMgr.get(), fontMgr});
    return (OH_Drawing_FontMgr*)fontMgr.get();
}

void OH_Drawing_FontMgrDestroy(OH_Drawing_FontMgr* fontMgr)
{
    std::lock_guard<std::mutex> lock(g_fontMgrLockMutex);
    auto it = g_fontMgrMap.find(fontMgr);
    if (it == g_fontMgrMap.end()) {
        return;
    }
    g_fontMgrMap.erase(it);
}

int OH_Drawing_FontMgrGetFamiliesCount(OH_Drawing_FontMgr* cFontMgr)
{
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    if (fontMgr == nullptr) {
        return 0;
    }
    return fontMgr->CountFamilies();
}

char* OH_Drawing_FontMgrGetFamilyName(OH_Drawing_FontMgr* cFontMgr, int index, int* len)
{
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return fontMgr->GetFamilyName(index, len);
}

void OH_Drawing_DestroyFamilyName(char** familyName)
{
    if (familyName == nullptr || *familyName == nullptr) {
        return;
    }
    delete (*familyName);
    *familyName = nullptr;
}

OH_Drawing_FontStyleSet* OH_Drawing_FontStyleSetCreate(OH_Drawing_FontMgr* cFontMgr, int index)
{
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_FontStyleSet*)fontMgr->CreateStyleSet(index);
}

void OH_Drawing_DestroyFontStyleSet(OH_Drawing_FontStyleSet* cfontStyleSet)
{
    if (cfontStyleSet == nullptr) {
        return;
    }
    delete CastToFontStyleSet(cfontStyleSet);
}

OH_Drawing_FontStyleSet* OH_Drawing_FontMgrMatchFamily(OH_Drawing_FontMgr* cFontMgr, const char* familyName)
{
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_FontStyleSet*)fontMgr->MatchFamily(familyName);
}

OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyle(OH_Drawing_FontMgr* cFontMgr, const char* familyName,
    OH_Drawing_FontForm* fontForm)
{
    if (cFontMgr == nullptr || familyName == nullptr || fontForm == nullptr) {
        return nullptr;
    }
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    FontStyle* fontStyle = new FontStyle(fontForm->weight, fontForm->width,
        static_cast<FontStyle::Slant>(fontForm->slant));
    if (fontMgr == nullptr || fontStyle == nullptr) {
        return nullptr;
    }
    OH_Drawing_Typeface* typeface = (OH_Drawing_Typeface*)fontMgr->MatchFamilyStyle(familyName, *fontStyle);
    delete fontStyle;
    return typeface;
}

OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyleCharacter(OH_Drawing_FontMgr* cFontMgr, const char familyName[],
    OH_Drawing_FontForm* fontForm, const char* bcp47[], int bcp47Count, int32_t character)
{
    if (cFontMgr == nullptr || familyName == nullptr || fontForm == nullptr || bcp47 == nullptr) {
        return nullptr;
    }
    FontMgr* fontMgr = CastToFontMgr(cFontMgr);
    FontStyle* fontStyle = new FontStyle(fontForm->weight, fontForm->width,
        static_cast<FontStyle::Slant>(fontForm->slant));
    if (fontMgr == nullptr || fontStyle == nullptr) {
        return nullptr;
    }
    OH_Drawing_Typeface* typeface = (OH_Drawing_Typeface*)fontMgr->MatchFamilyStyleCharacter(familyName, *fontStyle, bcp47, bcp47Count, character);
    delete fontStyle;
    return typeface;
}


