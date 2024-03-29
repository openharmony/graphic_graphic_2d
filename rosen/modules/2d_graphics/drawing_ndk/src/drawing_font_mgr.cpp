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

#include "drawing_typeface.h"
#include "drawing_font_mgr.h"
#include <mutex>
#include <unordered_map>
#include <securec.h>

#include "text/font_mgr.h"
#include "text/typeface.h"
#include "utils/object_mgr.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_fontMgrLockMutex;
static std::mutex g_fontStyleSetLockMutex;
static std::unordered_map<void*, std::shared_ptr<FontMgr>> g_fontMgrMap;
static std::unordered_map<void*, std::shared_ptr<FontStyleSet>> g_fontStyleSetMap;

static FontMgr* CastToFontMgr(OH_Drawing_FontMgr* drawingFontMgr)
{
    FontMgr* fontMgr = reinterpret_cast<FontMgr*>(drawingFontMgr);
    auto it = g_fontMgrMap.find(fontMgr);
    if (it != g_fontMgrMap.end()) {
        return it->second.get();
    }
    return fontMgr;
}

OH_Drawing_FontMgr* OH_Drawing_FontMgrCreate()
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    std::lock_guard<std::mutex> lock(g_fontMgrLockMutex);
    g_fontMgrMap.insert({fontMgr.get(), fontMgr});
    return (OH_Drawing_FontMgr*)fontMgr.get();
}

void OH_Drawing_FontMgrDestroy(OH_Drawing_FontMgr* drawingFontMgr)
{
    std::lock_guard<std::mutex> lock(g_fontMgrLockMutex);
    auto it = g_fontMgrMap.find(drawingFontMgr);
    if (it == g_fontMgrMap.end()) {
        return;
    }
    g_fontMgrMap.erase(it);
}

int OH_Drawing_FontMgrGetFamilyCount(OH_Drawing_FontMgr* drawingFontMgr)
{
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return 0;
    }
    return fontMgr->CountFamilies();
}

static bool CopyStrData(char** destination, const std::string& source)
{
    if (source.empty()) {
        return false;
    }
    size_t destinationSize = source.size() + 1;
    *destination = new char[destinationSize];
    if (*destination == nullptr) {
        return false;
    }
    auto retCopy = strcpy_s(*destination, destinationSize, source.c_str());
    if (retCopy != 0) {
        delete[] *destination;
        return false;
    }
    return true;
}

char* OH_Drawing_FontMgrGetFamilyName(OH_Drawing_FontMgr* drawingFontMgr, int index)
{
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    std::string strFamilyName = "";
    fontMgr->GetFamilyName(index, strFamilyName);
    char* familyName = nullptr;
    auto ret = CopyStrData(&familyName, strFamilyName);
    if (!ret) {
        return nullptr;
    }
    return familyName;
}

void OH_Drawing_FontMgrDestroyFamilyName(char* familyName)
{
    if (familyName == nullptr) {
        return;
    }
    delete[] familyName;
}

OH_Drawing_FontStyleSet* OH_Drawing_FontMgrCreateFontStyleSet(OH_Drawing_FontMgr* drawingFontMgr, int index)
{
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    FontStyleSet* fontStyleSet = fontMgr->CreateStyleSet(index);
    std::shared_ptr<FontStyleSet> sharedFontStyleSet(fontStyleSet);
    std::lock_guard<std::mutex> lock(g_fontStyleSetLockMutex);
    g_fontStyleSetMap.insert({sharedFontStyleSet.get(), sharedFontStyleSet});
    return (OH_Drawing_FontStyleSet*)sharedFontStyleSet.get();
}

void OH_Drawing_FontMgrDestroyFontStyleSet(OH_Drawing_FontStyleSet* drawingFontStyleSet)
{
    std::lock_guard<std::mutex> lock(g_fontStyleSetLockMutex);
    auto it = g_fontStyleSetMap.find(drawingFontStyleSet);
    if (it == g_fontStyleSetMap.end()) {
        return;
    }
    g_fontStyleSetMap.erase(it);
}

OH_Drawing_FontStyleSet* OH_Drawing_FontMgrMatchFamily(OH_Drawing_FontMgr* drawingFontMgr, const char* familyName)
{
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    FontStyleSet* fontStyleSet = fontMgr->MatchFamily(familyName);
    if (fontStyleSet == nullptr) {
        return nullptr;
    }
    std::shared_ptr<FontStyleSet> sharedFontStyleSet(fontStyleSet);
    std::lock_guard<std::mutex> lock(g_fontStyleSetLockMutex);
    g_fontStyleSetMap.insert({sharedFontStyleSet.get(), sharedFontStyleSet});
    return (OH_Drawing_FontStyleSet*)sharedFontStyleSet.get();
}

OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyle(OH_Drawing_FontMgr* drawingFontMgr, const char* familyName,
    OH_Drawing_FontStyleStruct* fontStyle)
{
    if (drawingFontMgr == nullptr|| fontStyle == nullptr) {
        return nullptr;
    }
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    Typeface* typeface = fontMgr->MatchFamilyStyle(familyName,
        FontStyle(fontStyle->weight, fontStyle->width, static_cast<FontStyle::Slant>(fontStyle->slant)));
    if (typeface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Typeface> sharedTypeface(typeface);
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(sharedTypeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, sharedTypeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyleCharacter(OH_Drawing_FontMgr* drawingFontMgr,
    const char* familyName, OH_Drawing_FontStyleStruct* fontStyle, const char* bcp47[],
    int bcp47Count, int32_t character)
{
    if (drawingFontMgr == nullptr || fontStyle == nullptr) {
        return nullptr;
    }
    FontMgr* fontMgr = CastToFontMgr(drawingFontMgr);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    Typeface* typeface = fontMgr->MatchFamilyStyleCharacter(familyName,
        FontStyle(fontStyle->weight, fontStyle->width, static_cast<FontStyle::Slant>(fontStyle->slant)),
        bcp47, bcp47Count, character);
    if (typeface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Typeface> sharedTypeface(typeface);
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(sharedTypeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, sharedTypeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_FontStyleSetCreateTypeface(OH_Drawing_FontStyleSet* fontStyleSet, int index)
{
    FontStyleSet* converFontStyleSet = reinterpret_cast<FontStyleSet*>(fontStyleSet);
    if (converFontStyleSet == nullptr) {
        return nullptr;
    }
    auto drawingTypeface = converFontStyleSet->CreateTypeface(index);
    if (!drawingTypeface) {
        return nullptr;
    }
    std::shared_ptr<Typeface> typeface(drawingTypeface);
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return reinterpret_cast<OH_Drawing_Typeface*>(drawingTypeface);
}

OH_Drawing_FontStyleStruct OH_Drawing_FontStyleSetGetStyle(OH_Drawing_FontStyleSet* fontStyleSet, int32_t index,
    char** styleName)
{
    OH_Drawing_FontStyleStruct fontStyleStruct;
    fontStyleStruct.weight = FONT_WEIGHT_400;
    fontStyleStruct.width = FONT_WIDTH_NORMAL;
    fontStyleStruct.slant = FONT_STYLE_NORMAL;
    FontStyleSet* converFontStyleSet = reinterpret_cast<FontStyleSet*>(fontStyleSet);
    if (converFontStyleSet == nullptr) {
        return fontStyleStruct;
    }
    FontStyle tempFontStyle;
    std::string tempStringPtr;
    converFontStyleSet->GetStyle(index, &tempFontStyle, &tempStringPtr);
    size_t len = tempStringPtr.length() + 1;
    char* allocatedMemoryForStyleName = (char*)(malloc(len * sizeof(char)));
    if (allocatedMemoryForStyleName != nullptr) {
        auto retCopy = strcpy_s(allocatedMemoryForStyleName, len, tempStringPtr.c_str());
        if (retCopy != 0) {
            delete[] allocatedMemoryForStyleName;
            return fontStyleStruct;
        }
    } else {
        return fontStyleStruct;
    }
    *styleName = allocatedMemoryForStyleName;
    fontStyleStruct.weight = static_cast<OH_Drawing_FontWeight>(tempFontStyle.GetWeight());
    fontStyleStruct.width = static_cast<OH_Drawing_FontWidth>(tempFontStyle.GetWidth());
    fontStyleStruct.slant = static_cast<OH_Drawing_FontStyle>(tempFontStyle.GetSlant());
    return fontStyleStruct;
}

void OH_Drawing_FontStyleSetFreeStyleName(char** styleName)
{
    if (*styleName != nullptr) {
        free(*styleName);
        *styleName = nullptr;
    }
    return;
}

OH_Drawing_Typeface* OH_Drawing_FontStyleSetMatchStyle(OH_Drawing_FontStyleSet* fontStyleSet,
    OH_Drawing_FontStyleStruct fontStyleStruct)
{
    FontStyleSet* converFontStyleSet = reinterpret_cast<FontStyleSet*>(fontStyleSet);
    if (converFontStyleSet == nullptr) {
        return nullptr;
    }
    auto drawingTypeface = converFontStyleSet->MatchStyle(
        FontStyle(fontStyleStruct.weight, fontStyleStruct.width,
        static_cast<FontStyle::Slant>(fontStyleStruct.slant)));
    if (!drawingTypeface) {
        return nullptr;
    }
    std::shared_ptr<Typeface> typeface(drawingTypeface);
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return reinterpret_cast<OH_Drawing_Typeface*>(drawingTypeface);
}

int OH_Drawing_FontStyleSetCount(OH_Drawing_FontStyleSet* fontStyleSet)
{
    FontStyleSet* converFontStyleSet = reinterpret_cast<FontStyleSet*>(fontStyleSet);
    if (converFontStyleSet == nullptr) {
        return 0;
    }
    return converFontStyleSet->Count();
}