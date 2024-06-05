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

#include "drawing_text_blob.h"

#include <mutex>
#include <unordered_map>

#include "drawing_canvas_utils.h"

#include "text/text_blob_builder.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::mutex g_textBlobLockMutex;
static std::unordered_map<void*, std::shared_ptr<TextBlob>> g_textBlobMap;

static TextBlobBuilder* CastToTextBlobBuilder(OH_Drawing_TextBlobBuilder* cTextBlobBuilder)
{
    return reinterpret_cast<TextBlobBuilder*>(cTextBlobBuilder);
}

static const Font& CastToFont(const OH_Drawing_Font& cFont)
{
    return reinterpret_cast<const Font&>(cFont);
}

static TextBlob* CastToTextBlob(OH_Drawing_TextBlob* cTextBlob)
{
    return reinterpret_cast<TextBlob*>(cTextBlob);
}

static const TextBlob* CastToTextBlob(const OH_Drawing_TextBlob* cTextBlob)
{
    return reinterpret_cast<const TextBlob*>(cTextBlob);
}

static const Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Rect*>(cRect);
}

static const Point CastToPoint(const OH_Drawing_Point2D& cPoint)
{
    return {cPoint.x, cPoint.y};
}

OH_Drawing_TextBlobBuilder* OH_Drawing_TextBlobBuilderCreate()
{
    return (OH_Drawing_TextBlobBuilder*)new TextBlobBuilder;
}

OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromText(const void* text, size_t byteLength,
    const OH_Drawing_Font* cFont, OH_Drawing_TextEncoding cTextEncoding)
{
    if (text == nullptr || cFont == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTextEncoding < TEXT_ENCODING_UTF8 || cTextEncoding > TEXT_ENCODING_GLYPH_ID) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    const Font& font = CastToFont(*cFont);
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(text,
        byteLength, font, static_cast<TextEncoding>(cTextEncoding));
    if (textBlob == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    g_textBlobMap.insert({textBlob.get(), textBlob});
    return (OH_Drawing_TextBlob*)textBlob.get();
}

OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromPosText(const void* text, size_t byteLength,
    OH_Drawing_Point2D* cPoints, const OH_Drawing_Font* cFont, OH_Drawing_TextEncoding cTextEncoding)
{
    if (text == nullptr || cFont == nullptr || cPoints == nullptr || byteLength == 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTextEncoding < TEXT_ENCODING_UTF8 || cTextEncoding > TEXT_ENCODING_GLYPH_ID) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    const Font& font = CastToFont(*cFont);
    const int count = font.CountText(text, byteLength, static_cast<TextEncoding>(cTextEncoding));
    if (count <= 0) {
        return nullptr;
    }
    Point* pts = new Point[count];
    if (pts == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < count; ++i) {
        pts[i] = CastToPoint(cPoints[i]);
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromPosText(text, byteLength,
        pts, font, static_cast<TextEncoding>(cTextEncoding));
    if (textBlob == nullptr) {
        delete [] pts;
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    g_textBlobMap.insert({textBlob.get(), textBlob});
    delete [] pts;
    return (OH_Drawing_TextBlob*)textBlob.get();
}

OH_Drawing_TextBlob* OH_Drawing_TextBlobCreateFromString(const char* str,
    const OH_Drawing_Font* cFont, OH_Drawing_TextEncoding cTextEncoding)
{
    if (str == nullptr || cFont == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    if (cTextEncoding < TEXT_ENCODING_UTF8 || cTextEncoding > TEXT_ENCODING_GLYPH_ID) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return nullptr;
    }
    const Font& font = CastToFont(*cFont);
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromString(str,
        font, static_cast<TextEncoding>(cTextEncoding));
    if (textBlob == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    g_textBlobMap.insert({textBlob.get(), textBlob});
    return (OH_Drawing_TextBlob*)textBlob.get();
}

void OH_Drawing_TextBlobGetBounds(OH_Drawing_TextBlob* cTextBlob, OH_Drawing_Rect* cRect)
{
    Rect* outRect = const_cast<Rect*>(CastToRect(cRect));
    if (outRect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    TextBlob* textblob = CastToTextBlob(cTextBlob);
    if (textblob == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    std::shared_ptr<Rect> rect = textblob->Bounds();
    *outRect = Rect(rect->GetLeft(), rect->GetTop(), rect->GetRight(), rect->GetBottom());
}

uint32_t OH_Drawing_TextBlobUniqueID(const OH_Drawing_TextBlob* cTextBlob)
{
    const TextBlob* textblob = CastToTextBlob(cTextBlob);
    if (cTextBlob == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return textblob->UniqueID();
}

const OH_Drawing_RunBuffer* OH_Drawing_TextBlobBuilderAllocRunPos(OH_Drawing_TextBlobBuilder* cTextBlobBuilder,
    const OH_Drawing_Font* cFont, int32_t count, const OH_Drawing_Rect* cRect)
{
    if (cFont == nullptr || count <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    TextBlobBuilder* textBlobBuilder = CastToTextBlobBuilder(cTextBlobBuilder);
    if (textBlobBuilder == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (const OH_Drawing_RunBuffer*)&textBlobBuilder->AllocRunPos(CastToFont(*cFont), count, CastToRect(cRect));
}

OH_Drawing_TextBlob* OH_Drawing_TextBlobBuilderMake(OH_Drawing_TextBlobBuilder* cTextBlobBuilder)
{
    TextBlobBuilder* textBlobBuilder = CastToTextBlobBuilder(cTextBlobBuilder);
    if (textBlobBuilder == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<TextBlob> textBlob = textBlobBuilder->Make();
    if (textBlob == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    g_textBlobMap.insert({textBlob.get(), textBlob});
    return (OH_Drawing_TextBlob*)textBlob.get();
}

void OH_Drawing_TextBlobDestroy(OH_Drawing_TextBlob* cTextBlob)
{
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    auto it = g_textBlobMap.find(cTextBlob);
    if (it == g_textBlobMap.end()) {
        return;
    }
    g_textBlobMap.erase(it);
}

void OH_Drawing_TextBlobBuilderDestroy(OH_Drawing_TextBlobBuilder* cTextBlobBuilder)
{
    delete CastToTextBlobBuilder(cTextBlobBuilder);
}
