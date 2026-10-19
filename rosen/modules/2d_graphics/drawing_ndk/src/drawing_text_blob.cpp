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

#include <cstring>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "drawing_canvas_utils.h"
#include "drawing_font_utils.h"

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

static const Font* CastToFont(const OH_Drawing_Font* cFont)
{
    return reinterpret_cast<const Font*>(cFont);
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

// Wraps the inner text blob list as an API-allocated handle array and keeps every blob alive
// through g_textBlobMap; the array is allocated before any registration, so a failure leaves
// no partial state behind.
static OH_Drawing_ErrorCode CreateTextBlobsArray(const std::vector<std::shared_ptr<TextBlob>>& textBlobList,
    uint32_t* textBlobsCount, OH_Drawing_TextBlob*** textBlobs)
{
    *textBlobsCount = 0;
    *textBlobs = nullptr;
    if (textBlobList.empty()) {
        return OH_DRAWING_SUCCESS;
    }
    auto textBlobArray = new (std::nothrow) OH_Drawing_TextBlob* [textBlobList.size()];
    if (textBlobArray == nullptr) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    {
        std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
        for (size_t i = 0; i < textBlobList.size(); i++) {
            textBlobArray[i] = reinterpret_cast<OH_Drawing_TextBlob*>(textBlobList[i].get());
            g_textBlobMap.insert({textBlobArray[i], textBlobList[i]});
        }
    }
    *textBlobsCount = static_cast<uint32_t>(textBlobList.size());
    *textBlobs = textBlobArray;
    return OH_DRAWING_SUCCESS;
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
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(text,
        byteLength, *font, static_cast<TextEncoding>(cTextEncoding));
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
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    const int count = font->CountText(text, byteLength, static_cast<TextEncoding>(cTextEncoding));
    if (count <= 0) {
        return nullptr;
    }
    Point* pts = new (std::nothrow) Point[count];
    if (pts == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < count; ++i) {
        pts[i] = CastToPoint(cPoints[i]);
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromPosText(text, byteLength,
        pts, *font, static_cast<TextEncoding>(cTextEncoding));
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
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromString(str,
        *font, static_cast<TextEncoding>(cTextEncoding));
    if (textBlob == nullptr) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_textBlobLockMutex);
    g_textBlobMap.insert({textBlob.get(), textBlob});
    return (OH_Drawing_TextBlob*)textBlob.get();
}

OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromTextWithFallback(const void *text, uint32_t byteLength,
    const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding, OH_Drawing_TextBlob ***textBlobs,
    uint32_t *textBlobsCount)
{
    if (text == nullptr || cFont == nullptr || byteLength == 0 ||
        textBlobsCount == nullptr || textBlobs == nullptr) {
        LOGE("OH_Drawing_TextBlobCreateFromTextWithFallback: any of text, font, textBlobsCount and "
            "textBlobs is nullptr or byteLength is 0.");
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (textEncoding < TEXT_ENCODING_UTF8 || textEncoding > TEXT_ENCODING_GLYPH_ID) {
        LOGE("OH_Drawing_TextBlobCreateFromTextWithFallback: encoding is invalid.");
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }

    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }

    std::vector<std::shared_ptr<TextBlob>> textBlobList =
        TextBlob::MakeFromTextWithFallback(text, byteLength, *font, static_cast<TextEncoding>(textEncoding));
    return CreateTextBlobsArray(textBlobList, textBlobsCount, textBlobs);
}

OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromPosTextWithFallback(const void *text, uint32_t byteLength,
    OH_Drawing_Point2D *cPoints, const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding,
    OH_Drawing_TextBlob ***textBlobs, uint32_t *textBlobsCount)
{
    if (text == nullptr || cPoints == nullptr || cFont == nullptr || byteLength == 0 ||
        textBlobsCount == nullptr ||textBlobs == nullptr) {
        LOGE("OH_Drawing_TextBlobCreateFromPosTextWithFallback: any of text, points, font, "
            "textBlobsCount and textBlobs is nullptr or byteLength is 0.");
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (textEncoding < TEXT_ENCODING_UTF8 || textEncoding > TEXT_ENCODING_GLYPH_ID) {
        LOGE("OH_Drawing_TextBlobCreateFromPosTextWithFallback: encoding is invalid.");
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }

    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    const int count = font->CountText(text, byteLength, static_cast<TextEncoding>(textEncoding));
    if (count <= 0) {
        *textBlobsCount = 0;
        *textBlobs = nullptr;
        return OH_DRAWING_SUCCESS;
    }
    Point* pts = new (std::nothrow) Point[count];
    if (pts == nullptr) {
        return OH_DRAWING_ERROR_ALLOCATION_FAILED;
    }
    for (int i = 0; i < count; ++i) {
        pts[i] = CastToPoint(cPoints[i]);
    }
    std::vector<std::shared_ptr<TextBlob>> textBlobList = TextBlob::MakeFromPosTextWithFallback(text,
        byteLength, pts, *font, static_cast<TextEncoding>(textEncoding));
    delete[] pts;
    return CreateTextBlobsArray(textBlobList, textBlobsCount, textBlobs);
}

OH_Drawing_ErrorCode OH_Drawing_TextBlobCreateFromStringWithFallback(const char *str,
    const OH_Drawing_Font *cFont, OH_Drawing_TextEncoding textEncoding,
    OH_Drawing_TextBlob ***textBlobs,  uint32_t *textBlobsCount)
{
    if (str == nullptr || cFont == nullptr || textBlobsCount == nullptr || textBlobs == nullptr) {
        LOGE("OH_Drawing_TextBlobCreateFromStringWithFallback: any of str, font, textBlobsCount and "
            "textBlobs is nullptr.");
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    if (textEncoding < TEXT_ENCODING_UTF8 || textEncoding > TEXT_ENCODING_GLYPH_ID) {
        LOGE("OH_Drawing_TextBlobCreateFromTextWithFallback: encoding is invalid.");
        return OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
    }

    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }

    std::vector<std::shared_ptr<TextBlob>> textBlobList =
        TextBlob::MakeFromStringWithFallback(str, *font, static_cast<TextEncoding>(textEncoding));
    return CreateTextBlobsArray(textBlobList, textBlobsCount, textBlobs);
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
    if (rect == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    *outRect = Rect(rect->GetLeft(), rect->GetTop(), rect->GetRight(), rect->GetBottom());
}

uint32_t OH_Drawing_TextBlobUniqueID(const OH_Drawing_TextBlob* cTextBlob)
{
    if (cTextBlob == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    const TextBlob* textblob = CastToTextBlob(cTextBlob);
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
    const Font* font = CastToFont(cFont);
    std::shared_ptr<Font> themeFont = DrawingFontUtils::GetThemeFont(font);
    if (themeFont != nullptr) {
        font = themeFont.get();
    }
    return (const OH_Drawing_RunBuffer*)&textBlobBuilder->AllocRunPos(*font, count, CastToRect(cRect));
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
    if (!cTextBlobBuilder) {
        return;
    }
    delete CastToTextBlobBuilder(cTextBlobBuilder);
}


OH_Drawing_ErrorCode OH_Drawing_TextBlobsArrayDestroy(OH_Drawing_TextBlob **textBlobs, uint32_t count)
{
    if (textBlobs == nullptr || count == 0) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    for (uint32_t i = 0; i < count; i++) {
        OH_Drawing_TextBlobDestroy(textBlobs[i]);
    }
    delete[] textBlobs;
    return OH_DRAWING_SUCCESS;
}