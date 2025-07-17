/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "drawing_add_text_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::Rosen::Drawing {
void OHDrawingAddTextTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::string str = fdp.ConsumeRandomLengthString();

    OH_Drawing_FontCollection* fc = OH_Drawing_GetFontCollectionGlobalInstance();
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fc);
    OH_Drawing_TypographyHandlerAddEncodedText(handler, str.c_str(), str.size(), TEXT_ENCODING_UTF8);
    OH_Drawing_TypographyHandlerAddEncodedText(handler, str.c_str(), str.size(), TEXT_ENCODING_UTF16);
    OH_Drawing_TypographyHandlerAddEncodedText(handler, str.c_str(), str.size(), TEXT_ENCODING_UTF32);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, 1000.0f);

    OH_Drawing_DestroyFontCollection(fc);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
}
} // namespace OHOS::Rosen::Drawing

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingAddTextTest(data, size);
    return 0;
}