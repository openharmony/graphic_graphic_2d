/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_TEXT_DECLARATION_H
#define C_INCLUDE_DRAWING_TEXT_DECLARATION_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides 2d drawing functions.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_text_declaration.h
 *
 * @brief declare structs of the Drawing module.
 *
 * @since 8
 * @version 1.0
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OH_Drawing_FontCollection is to load fonts.
 * @since 8
 * @version 1.0
 */
struct OH_Drawing_FontCollection;

/**
 * @brief OH_Drawing_Typography is to manage layout and paint text.
 * @since 8
 * @version 1.0
 */
struct OH_Drawing_Typography;

/**
 * @brief OH_Drawing_TextStyle is to manage font color, decoration, etc.
 * @since 8
 * @version 1.0
 */
struct OH_Drawing_TextStyle;

/**
 * @brief OH_Drawing_TypographyStyle is to manage typography layout.
 * @since 8
 * @version 1.0
 */
struct OH_Drawing_TypographyStyle;

/**
 * @brief OH_Drawing_TypographyCreate is to create OH_Drawing_Typography.
 * @since 8
 * @version 1.0
 */
struct OH_Drawing_TypographyCreate;

#ifdef __cplusplus
}
#endif
/** @} */
#endif