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

#ifndef C_INCLUDE_DRAWING_TEXT_DECLARATION_H
#define C_INCLUDE_DRAWING_TEXT_DECLARATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OH_Drawing_FontCollection is to load fonts.
 * @param OH_Drawing_Typography a pointer to OH_Drawing_Typography object
 */
struct OH_Drawing_FontCollection;

/**
 * @brief OH_Drawing_Typography is to manage layout and paint text.
 */
struct OH_Drawing_Typography;

/**
 * @brief OH_Drawing_TextStyle is to manage font color, decoration, etc.
 */
struct OH_Drawing_TextStyle;

/**
 * @brief OH_Drawing_TypographyStyle is to manage paragraph layout.
 */
struct OH_Drawing_TypographyStyle;

/**
 * @brief OH_Drawing_TypographyCreate is to create OH_Drawing_Typography.
 */
struct OH_Drawing_TypographyCreate;

#ifdef __cplusplus
}
#endif

#endif