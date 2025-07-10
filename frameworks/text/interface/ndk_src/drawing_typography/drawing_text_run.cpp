
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

#include <string>
#include "array_mgr.h"
#include "drawing_rect.h"
#include "drawing_text_run.h"
#include "skia_txt/run_impl.h"

using namespace OHOS::Rosen;
typedef AdapterTxt::RunImpl RunImpl;

uint32_t OH_Drawing_GetRunGlyphCount(OH_Drawing_Run* run)
{
    if (run == nullptr) {
        return 0;
    }
    return reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetGlyphCount();
}

void OH_Drawing_GetRunStringRange(OH_Drawing_Run* run, uint64_t* location, uint64_t* length)
{
    if (location == nullptr || length == nullptr) {
        return;
    } else if (run == nullptr) {
        *location = 0;
        *length = 0;
        return;
    }
    reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetStringRange(location, length);
}

OH_Drawing_Array* OH_Drawing_GetRunStringIndices(OH_Drawing_Run* run, int64_t start, int64_t length)
{
    if (run == nullptr || start < 0 || length < 0) {
        return nullptr;
    }
    auto stringIndices = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetStringIndices(start, length);
    if (stringIndices.size() == 0) {
        return nullptr;
    }
    ObjectArray* array = new (std::nothrow) ObjectArray;
    if (array == nullptr) {
        return nullptr;
    }
    uint64_t* stringIndicesArr = new (std::nothrow) uint64_t[stringIndices.size()];
    if (stringIndicesArr == nullptr) {
        delete array;
        return nullptr;
    }
    for (size_t i = 0; i < stringIndices.size(); ++i) {
        stringIndicesArr[i] = stringIndices[i];
    }
    array->addr = stringIndicesArr;
    array->num = stringIndices.size();
    array->type = ObjectType::TEXT_RUN;
    return reinterpret_cast<OH_Drawing_Array*>(array);
}

uint64_t OH_Drawing_GetRunStringIndicesByIndex(OH_Drawing_Array* stringIndicesArry, size_t index)
{
    ObjectArray* stringIndices = reinterpret_cast<ObjectArray*>(stringIndicesArry);
    if (stringIndices && stringIndices->type == ObjectType::TEXT_RUN &&
        index < stringIndices->num && stringIndices->addr) {
        return reinterpret_cast<uint64_t*>(stringIndices->addr)[index];
    }
    return 0;
}

void OH_Drawing_DestroyRunStringIndices(OH_Drawing_Array* stringIndicesArry)
{
    ObjectArray* stringIndices = reinterpret_cast<ObjectArray*>(stringIndicesArry);
    if (stringIndices && stringIndices->type == ObjectType::TEXT_RUN) {
        delete[] reinterpret_cast<uint64_t*>(stringIndices->addr);
        stringIndices->addr = nullptr;
        stringIndices->num = 0;
        stringIndices->type = ObjectType::INVALID;
        delete stringIndices;
    }
}

float OH_Drawing_GetRunTypographicBounds(OH_Drawing_Run* run, float* ascent, float* descent, float* leading)
{
    if (ascent == nullptr || descent == nullptr || leading == nullptr) {
        return 0.0;
    } else if (run == nullptr) {
        *ascent = 0;
        *descent = 0;
        *leading = 0;
        return 0.0;
    }
    return reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetTypographicBounds(ascent, descent, leading);
}

OH_Drawing_Rect* OH_Drawing_GetRunImageBounds(OH_Drawing_Run* run)
{
    if (run == nullptr) {
        return nullptr;
    }
    auto skRect = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetImageBounds();
    return OH_Drawing_RectCreate(skRect.GetLeft(), skRect.GetTop(), skRect.GetRight(), skRect.GetBottom());
}

void OH_Drawing_DestroyRunImageBounds(OH_Drawing_Rect* rect)
{
    if (rect) {
        OH_Drawing_RectDestroy(rect);
    }
}

OH_Drawing_Array* OH_Drawing_GetRunGlyphs(OH_Drawing_Run* run, int64_t start, int64_t length)
{
    if (run == nullptr || start < 0 || length < 0) {
        return nullptr;
    }
    auto glyphs = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetGlyphs(start, length);
    if (glyphs.size() == 0) {
        return nullptr;
    }
    ObjectArray* array = new (std::nothrow) ObjectArray;
    if (array == nullptr) {
        return nullptr;
    }
    uint16_t* glyphsArr = new (std::nothrow) uint16_t[glyphs.size()];
    if (glyphsArr == nullptr) {
        delete array;
        return nullptr;
    }
    for (size_t i = 0; i < glyphs.size() ; ++i) {
        glyphsArr[i] = glyphs[i];
    }
    array->addr = glyphsArr;
    array->num = glyphs.size();
    array->type = ObjectType::TEXT_RUN;
    return reinterpret_cast<OH_Drawing_Array*>(array);
}

uint16_t OH_Drawing_GetRunGlyphsByIndex(OH_Drawing_Array* glyphsArray, size_t index)
{
    ObjectArray* glyphs = reinterpret_cast<ObjectArray*>(glyphsArray);
    if (glyphs && glyphs->type == ObjectType::TEXT_RUN &&
        index < glyphs->num && glyphs->addr) {
        return reinterpret_cast<uint16_t*>(glyphs->addr)[index];
    }
    return 0;
}

void OH_Drawing_DestroyRunGlyphs(OH_Drawing_Array* glyphsArray)
{
    ObjectArray* glyphs = reinterpret_cast<ObjectArray*>(glyphsArray);
    if (glyphs && glyphs->type == ObjectType::TEXT_RUN) {
        delete[] reinterpret_cast<uint16_t*>(glyphs->addr);
        glyphs->addr = nullptr;
        glyphs->num = 0;
        glyphs->type = ObjectType::INVALID;
        delete glyphs;
    }
}

OH_Drawing_Array* OH_Drawing_GetRunPositions(OH_Drawing_Run* run, int64_t start, int64_t length)
{
    if (run == nullptr || start < 0 || length < 0) {
        return nullptr;
    }

    auto positions = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetPositions(start, length);
    if (positions.size() == 0) {
        return nullptr;
    }
    Drawing::Point* positionsArr = new (std::nothrow) Drawing::Point[positions.size()];
    if (positionsArr == nullptr) {
        return nullptr;
    }
    ObjectArray* array = new (std::nothrow) ObjectArray;
    if (array == nullptr) {
        delete[] positionsArr;
        return nullptr;
    }
    for (size_t i = 0; i < positions.size(); ++i) {
        positionsArr[i] = positions[i];
    }
    array->addr = positionsArr;
    array->num = positions.size();
    array->type = ObjectType::TEXT_RUN;
    return reinterpret_cast<OH_Drawing_Array*>(array);
}

OH_Drawing_Point* OH_Drawing_GetRunPositionsByIndex(OH_Drawing_Array* positionsArr, size_t index)
{
    ObjectArray* positions = reinterpret_cast<ObjectArray*>(positionsArr);
    if (positions && positions->type == ObjectType::TEXT_RUN &&
        index < positions->num && positions->addr) {
        Drawing::Point* positionsArr =  reinterpret_cast<Drawing::Point*>(positions->addr);
        return reinterpret_cast<OH_Drawing_Point*>(&positionsArr[index]);
    }
    return nullptr;
}

void OH_Drawing_DestroyRunPositions(OH_Drawing_Array* positionsArray)
{
    ObjectArray* positions = reinterpret_cast<ObjectArray*>(positionsArray);
    if (positions && positions->addr && positions->type == ObjectType::TEXT_RUN) {
        delete[] reinterpret_cast<Drawing::Point*>(positions->addr);
        positions->num = 0;
        positions->type = ObjectType::INVALID;
        positions->addr = nullptr;
        delete positions;
    }
}

OH_Drawing_Font* OH_Drawing_GetRunFont(OH_Drawing_Run* run)
{
    if (run == nullptr) {
        return nullptr;
    }

    Drawing::Font* font = new Drawing::Font(reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetFont());
    return reinterpret_cast<OH_Drawing_Font*>(font);
}

OH_Drawing_TextDirection OH_Drawing_GetRunTextDirection(OH_Drawing_Run* run)
{
    if (run == nullptr) {
        return TEXT_DIRECTION_LTR;
    }

    auto textDirection = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetTextDirection();
    if (textDirection == TextDirection::RTL) {
        return TEXT_DIRECTION_RTL;
    } else {
        return TEXT_DIRECTION_LTR;
    }
}

OH_Drawing_Array* OH_Drawing_GetRunGlyphAdvances(OH_Drawing_Run* run, uint32_t start, uint32_t length)
{
    if (run == nullptr) {
        return nullptr;
    }

    auto advances = reinterpret_cast<AdapterTxt::RunImpl*>(run)->GetAdvances(start, length);
    if (advances.size() == 0) {
        return nullptr;
    }
    Drawing::Point* advancesArr = new (std::nothrow) Drawing::Point[advances.size()];
    if (advancesArr == nullptr) {
        return nullptr;
    }
    ObjectArray* array = new (std::nothrow) ObjectArray;
    if (array == nullptr) {
        delete[] advancesArr;
        return nullptr;
    }
    for (size_t i = 0; i < advances.size(); ++i) {
        advancesArr[i] = advances[i];
    }
    array->addr = advancesArr;
    array->num = advances.size();
    array->type = ObjectType::TEXT_RUN;
    return reinterpret_cast<OH_Drawing_Array*>(array);
}

OH_Drawing_Point* OH_Drawing_GetRunGlyphAdvanceByIndex(OH_Drawing_Array* advances, size_t index)
{
    ObjectArray* advancesArray = reinterpret_cast<ObjectArray*>(advances);
    if (advancesArray != nullptr && advancesArray->type == ObjectType::TEXT_RUN &&
        index < advancesArray->num && advancesArray->addr) {
        Drawing::Point* advancesArr =  reinterpret_cast<Drawing::Point*>(advancesArray->addr);
        return reinterpret_cast<OH_Drawing_Point*>(&advancesArr[index]);
    }
    return nullptr;
}

void OH_Drawing_DestroyRunGlyphAdvances(OH_Drawing_Array* advances)
{
    ObjectArray* advancesArray = reinterpret_cast<ObjectArray*>(advances);
    if (advancesArray != nullptr && advancesArray->addr && advancesArray->type == ObjectType::TEXT_RUN) {
        delete[] reinterpret_cast<Drawing::Point*>(advancesArray->addr);
        advancesArray->num = 0;
        advancesArray->type = ObjectType::INVALID;
        advancesArray->addr = nullptr;
        delete advancesArray;
    }
}

void OH_Drawing_RunPaint(OH_Drawing_Canvas* canvas, OH_Drawing_Run* run, double x, double y)
{
    if (canvas == nullptr || run == nullptr) {
        return;
    }
    reinterpret_cast<AdapterTxt::RunImpl*>(run)->Paint(reinterpret_cast<Drawing::Canvas*>(canvas), x, y);
}