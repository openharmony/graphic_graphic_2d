/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_CONVERTOR_H
#define C_INCLUDE_DRAWING_TEXT_CONVERTOR_H

#include "drawing_text_declaration.h"
#include "rosen_text/ui/font_collection.h"
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"

namespace rosen {
/**
 * @brief convert OH_Drawing_FontCollection to FontCollection.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontCollection a pointer to OH_Drawing_FontCollection object
 * @return FontCollection a pointer to FontCollection
 */
FontCollection* ConvertToOriginalText(OH_Drawing_FontCollection* fontCollection);

/**
 * @brief convert FontCollection to OH_Drawing_FontCollection.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param FontCollection a pointer to FontCollection object
 * @return OH_Drawing_FontCollection a pointer to OH_Drawing_FontCollection
 */
OH_Drawing_FontCollection* ConvertToNDKText(FontCollection* fontCollection);

/**
 * @brief convert OH_Drawing_TypographyStyle to TypographyStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle object
 * @return TypographyStyle a pointer to TypographyStyle
 */
TypographyStyle* ConvertToOriginalText(OH_Drawing_TypographyStyle* style);

/**
 * @brief convert TypographyStyle to OH_Drawing_TypographyStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param TypographyStyle a pointer to TypographyStyle object
 * @return OH_Drawing_TypographyStyle a pointer to OH_Drawing_TypographyStyle
 */
OH_Drawing_TypographyStyle* ConvertToNDKText(TypographyStyle* style);

/**
 * @brief convert OH_Drawing_TypographyCreate to TypographyCreate.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TypographyCreate a pointer to OH_Drawing_TypographyCreate object
 * @return TypographyCreate a pointer to TypographyCreate
 */
TypographyCreate* ConvertToOriginalText(OH_Drawing_TypographyCreate* handler);

/**
 * @brief convert TypographyCreate to OH_Drawing_TypographyCreate.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param TypographyCreate a pointer to TypographyCreate object
 * @return OH_Drawing_TypographyCreate a pointer to rOH_Drawing_TypographyCreate
 */
OH_Drawing_TypographyCreate* ConvertToNDKText(TypographyCreate* handler);

/**
 * @brief convert OH_Drawing_TextStyle to TextStyle.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_TextStyle a pointer to OH_Drawing_TextStyle object
 * @return TextStyle a pointer to TextStyle
 */
TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style);

/**
 * @brief convert OH_Drawing_Typography to osen::Typography.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 * @return Typography a pointer to Typography
 */
Typography* ConvertToOriginalText(OH_Drawing_Typography* typography);

/**
 * @brief convert Typography to OH_Drawing_Typography.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param Typography a pointer to Typography object
 * @return OH_Drawing_Typography a pointer to OH_Drawing_Typography
 */
OH_Drawing_Typography* ConvertToNDKText(Typography* typography);
}

#endif

