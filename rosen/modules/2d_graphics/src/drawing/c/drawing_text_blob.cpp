/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c/drawing_text_blob.h"

#include <unordered_map>

#include "text/text_blob_builder.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static std::unordered_map<void*, std::shared_ptr<TextBlob>> g_textBlobMap;

static TextBlobBuilder* CastToTextBlobBuilder(OH_Drawing_TextBlobBuilder* cTextBlobBuilder)
{
    return reinterpret_cast<TextBlobBuilder*>(cTextBlobBuilder);
}

static const Font& CastToFont(const OH_Drawing_Font& cFont)
{
    return reinterpret_cast<const Font&>(cFont);
}

static const Rect* CastToRect(const OH_Drawing_Rect* cRect)
{
    return reinterpret_cast<const Rect*>(cRect);
}

OH_Drawing_TextBlobBuilder* OH_Drawing_TextBlobBuilderCreate()
{
    return (OH_Drawing_TextBlobBuilder*)new TextBlobBuilder;
}

const OH_Drawing_RunBuffer* OH_Drawing_TextBlobBuilderAllocRunPos(OH_Drawing_TextBlobBuilder* cTextBlobBuilder,
    const OH_Drawing_Font* cFont, int32_t count, const OH_Drawing_Rect* cRect)
{
    if (cFont == nullptr) {
        return nullptr;
    }
    TextBlobBuilder* textBlobBuilder = CastToTextBlobBuilder(cTextBlobBuilder);
    if (textBlobBuilder == nullptr) {
        return nullptr;
    }
    return (const OH_Drawing_RunBuffer*)&textBlobBuilder->AllocRunPos(CastToFont(*cFont), count, CastToRect(cRect));
}

OH_Drawing_TextBlob* OH_Drawing_TextBlobBuilderMake(OH_Drawing_TextBlobBuilder* cTextBlobBuilder)
{
    TextBlobBuilder* textBlobBuilder = CastToTextBlobBuilder(cTextBlobBuilder);
    if (textBlobBuilder == nullptr) {
        return nullptr;
    }
    std::shared_ptr<TextBlob> textBlob = textBlobBuilder->Make();
    g_textBlobMap.insert({textBlob.get(), textBlob});
    return (OH_Drawing_TextBlob*)textBlob.get();
}

void OH_Drawing_TextBlobDestroy(OH_Drawing_TextBlob* cTextBlob)
{
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
