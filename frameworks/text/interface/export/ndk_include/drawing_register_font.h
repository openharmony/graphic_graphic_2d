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

#ifndef C_INCLUDE_DRAWING_REGISTER_FONT_H
#define C_INCLUDE_DRAWING_REGISTER_FONT_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides the 2D drawing capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 11
 * @version 1.0
 */

/**
 * @file drawing_register_font.h
 *
 * @brief Declares functions related to <b>FontManager</b> in the drawing module.
 *
 * @since 11
 * @version 1.0
 */

#include "drawing_text_declaration.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Defines an <b>OH_Drawing_RegisterFont</b>, which is used to register a customized font in the FontManager.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @param fontFamily Indicates the family-name of the font which need to register.
 * @param familySrc Indicates the path of the font file which need to register.
 * @return error code.
 * @since 11
 * @version 1.0
 */
uint32_t OH_Drawing_RegisterFont(OH_Drawing_FontCollection*, const char* fontFamily, const char* familySrc);

/**
 * @brief Defines an <b>OH_Drawing_RegisterFontBuffer</b>, which is used to register a customized font in the
 *        FontManager.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @param fontFamily Indicates the family-name of the font which need to register.
 * @param fontBuffer Indicates the buffer of the font file which need to register.
 * @param length Indicates the length of the font file which need to register.
 * @return error code.
 * @since 11
 * @version 1.0
 */
uint32_t OH_Drawing_RegisterFontBuffer(OH_Drawing_FontCollection*, const char* fontFamily, uint8_t* fontBuffer,
    size_t length);

/**
 * @brief Defines an <b>OH_Drawing_RegisterFontByIndex</b>, which is used to register font from ttc file.
 *
 * @param fontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @param fontFamily Indicates the family name of the font which need to register.
 * @param familySrc Indicates the path of the font file which need to register.
 * @param index Indicates the index of the font data in the ttc file.
 * @return error code.
 * @since 23
 */
uint32_t OH_Drawing_RegisterFontByIndex(OH_Drawing_FontCollection* fontCollection,
    const char* fontFamily, const char* familySrc, uint32_t index);

/**
 * @brief Defines an <b>OH_Drawing_RegisterFontBufferByIndex</b>, which is used to register font from ttc buffer.
 *
 * @param fontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @param fontFamily Indicates the family name of the font which need to register.
 * @param fontBuffer Indicates the font data which need to register.
 * @param length Indicates the font data length.
 * @param index Indicates the index of the font data in the ttc file.
 * @return error code.
 * @since 23
 */
uint32_t OH_Drawing_RegisterFontBufferByIndex(OH_Drawing_FontCollection* fontCollection,
    const char* fontFamily, uint8_t* fontBuffer, size_t length, uint32_t index);

/**
 * @brief Unregister a customized font by the font family.
 * Unregistering a font that is currently in use by UI components may lead to text rendering anomalies,
 * including garbled characters or missing glyphs.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param fontCollection Indicates the pointer to an <b>OH_Drawing_FontCollection</b> object.
 * @param fontFamily Indicates the family-name of the font which need to be unregistered.
 * @return error code.
 * @since 20
 * @version 1.0
 */
uint32_t OH_Drawing_UnregisterFont(OH_Drawing_FontCollection* fontCollection, const char* fontFamily);

/**
 * @brief Checks if the font format specified by the path is supported.
 *
 * @param path The absolute path to the font file.
 * @return Returns true if the font is supported; otherwise, returns false.
 * @since 23
 */
bool OH_Drawing_IsFontSupportedFromPath(const char* fontPath);

/**
 * @brief Checks if the font format specified by the buffer is supported.
 *
 * @param data A pointer to the memory buffer containing the font data.
 * @param dataLength The size of the font data in bytes.
 * @return Returns true if the font is supported; otherwise, returns false.
 * @since 23
 */
bool OH_Drawing_IsFontSupportedFromBuffer(uint8_t* fontBuffer, size_t length);
#ifdef __cplusplus
}
#endif
/** @} */
#endif