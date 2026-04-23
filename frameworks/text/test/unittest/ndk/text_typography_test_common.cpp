/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "text_typography_test_common.h"

namespace OHOS {

void NdkTypographyTest::SetUp()
{
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(fTypoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, DEFAULT_FONT_SIZE);
}

void NdkTypographyTest::TearDown()
{
    if (fHandler != nullptr) {
        OH_Drawing_DestroyTypographyHandler(fHandler);
        fHandler = nullptr;
    }
    if (fTypography != nullptr) {
        OH_Drawing_DestroyTypography(fTypography);
        fTypography = nullptr;
    }
    if (fTypoStyle != nullptr) {
        OH_Drawing_DestroyTypographyStyle(fTypoStyle);
        fTypoStyle = nullptr;
    }
    if (fBitmap != nullptr) {
        OH_Drawing_BitmapDestroy(fBitmap);
        fBitmap = nullptr;
    }
    if (fCanvas != nullptr) {
        OH_Drawing_CanvasDestroy(fCanvas);
        fCanvas = nullptr;
    }
}

void NdkTypographyTest::CreateTypographyHandler()
{
    fHandler = OH_Drawing_CreateTypographyHandler(fTypoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(fHandler, nullptr);
}

void NdkTypographyTest::CreateTypography()
{
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
}

void NdkTypographyTest::AddText()
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
}

void NdkTypographyTest::Layout()
{
    OH_Drawing_TypographyLayout(fTypography, fLayoutWidth);
}

void NdkTypographyTest::Paint()
{
    double position[2] = {10.0, 15.0};
    fBitmap = OH_Drawing_BitmapCreate();
    fCanvas = OH_Drawing_CanvasCreate();
    ASSERT_NE(fTypography, nullptr);
    OH_Drawing_CanvasBind(fCanvas, fBitmap);
    OH_Drawing_CanvasClear(fCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_TypographyPaint(fTypography, fCanvas, position[0], position[1]);
}

TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style)
{
    return reinterpret_cast<TypographyStyle*>(style);
}

TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style)
{
    return reinterpret_cast<TextStyle*>(style);
}

} // namespace OHOS
