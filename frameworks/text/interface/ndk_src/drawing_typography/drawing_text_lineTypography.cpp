
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

#include "drawing_text_lineTypography.h"
#include "array_mgr.h"
#include "rosen_text/typography_create.h"
#include "skia_txt/text_line_base.h"
#include "skia/txt/paragraph_line_fetcher.h"
#include "utils/text_log.h"

using namespace OHOS::Rosen;
typedef OHOS::Rosen::AdapterTxt::TextLineBaseImpl LineImpl;

OH_Drawing_LineTypography* OH_Drawing_CreateLineTypography(OH_Drawing_TypographyCreate* handler)
{
    if (handler == nullptr) {
        TEXT_LOGE("Null handler");
        return nullptr;
    }
    TypographyCreate* rosenHandler = reinterpret_cast<TypographyCreate*>(handler);
    std::unique_ptr<LineTypography> lineTypography = rosenHandler->CreateLineTypography();
    if (lineTypography == nullptr) {
        TEXT_LOGE("Failed to create line typography");
        return nullptr;
    }
    return reinterpret_cast<OH_Drawing_LineTypography*>(lineTypography.release());
}

void OH_Drawing_DestroyLineTypography(OH_Drawing_LineTypography* lineTypography)
{
    delete reinterpret_cast<LineTypography*>(lineTypography);
}

size_t OH_Drawing_LineTypographyGetLineBreak(OH_Drawing_LineTypography *lineTypograph, size_t startIndex, double width)
{
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypograph);
    size_t limitSize = innerlineTypography->GetUnicodeSize();
    if (startIndex >= limitSize || width <= 0) {
        return 0;
    }
    return innerlineTypography->GetLineBreak(startIndex, width);
}

OH_Drawing_TextLine* OH_Drawing_LineTypographyCreateLine(OH_Drawing_LineTypography *lineTypograph,
                                                         size_t startIndex, size_t count)
{
    LineTypography* innerlineTypography = reinterpret_cast<LineTypography*>(lineTypograph);
    size_t limitSize = innerlineTypography->GetUnicodeSize();
    if (startIndex >= limitSize || count + startIndex > limitSize) {
        TEXT_LOGE("Invalid param");
        return nullptr;
    }
    void* lineFetcher = innerlineTypography->GetLineFetcher();
    if (lineFetcher == nullptr) {
        TEXT_LOGE("Failed to get line fetcher");
        return nullptr;
    }
    SPText::ParagraphLineFetcher* spLineFetcher = reinterpret_cast<SPText::ParagraphLineFetcher*>(lineFetcher);
    auto line = spLineFetcher->CreateLine(startIndex, count);
    if (line == nullptr) {
        TEXT_LOGE("Failed to get line");
        return nullptr;
    }
    LineImpl* lineImpl = new (std::nothrow) LineImpl(std::move(line));
    if (lineImpl == nullptr) {
        TEXT_LOGE("Failed to new text line");
        return nullptr;
    }
    LineObject* lineObject = new (std::nothrow) LineObject();
    if (lineObject == nullptr) {
        TEXT_LOGE("Failed to new line object");
        delete lineImpl;
        return nullptr;
    }
    lineObject->line = reinterpret_cast<void*>(lineImpl);
    lineObject->isArray = false;
    return reinterpret_cast<OH_Drawing_TextLine*>(lineObject);
}
