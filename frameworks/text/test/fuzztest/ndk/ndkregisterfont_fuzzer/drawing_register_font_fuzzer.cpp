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

#include "drawing_register_font_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::Rosen::Drawing {
void UseCustomFontToLayout(OH_Drawing_FontCollection* fc, std::string familyName)
{
    constexpr double layoutWidth = 100.0;
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    const char* fontFamilies[] = { familyName.c_str() };
    OH_Drawing_SetTextStyleFontFamilies(txtStyle, 1, fontFamilies);
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextStyle(typoStyle, txtStyle);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fc);
    OH_Drawing_TypographyHandlerAddText(handler, "const char *");
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
}

void OHDrawingRegisterFontTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_FontCollection* fc = OH_Drawing_GetFontCollectionGlobalInstance();
    std::string familyName = fdp.ConsumeRandomLengthString();
    OH_Drawing_RegisterFont(fc, familyName.c_str(), "random path");
    UseCustomFontToLayout(fc, familyName);
    OH_Drawing_UnregisterFont(fc, familyName.c_str());
    std::vector<uint8_t> fontBuffer = fdp.ConsumeRemainingBytes<uint8_t>();
    OH_Drawing_RegisterFontBuffer(fc, familyName.c_str(), fontBuffer.data(), fontBuffer.size());
    UseCustomFontToLayout(fc, familyName);
    OH_Drawing_UnregisterFont(fc, familyName.c_str());
}
} // namespace OHOS::Rosen::Drawing

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingRegisterFontTest(data, size);
    return 0;
}