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

#ifndef C_INCLUDE_DRAWING_FONT_MGR_H
#define C_INCLUDE_DRAWING_FONT_MGR_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 12
 * @version 1.0
 */

/**
 * @file drawing_font_mgr.h
 *
 * @brief Declares functions related to the <b>fontmgr</b> object in the drawing module.
 *
 * @since 12
 * @version 1.0
 */

#include "drawing_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates font weight.
 *
 * @since 12
 * @version 1.0
 */
enum OH_Drawing_FontStyleWeight {
    /* Invisible font weight */
    INVISIBLE_WEIGHT = 0,
    /* Thin font weight */
    THIN_WEIGHT = 100,
    /* Extra light font weight */
    EXTRA_LIGHT_WEIGHT = 200,
    /* Light font weight */
    LIGHT_WEIGHT = 300,
    /* Normal font weight */
    NORMAL_WEIGHT = 400,
    /* Medium font weight */
    MEDIUM_WEIGHT = 500,
    /* Semi blod font weight */
    SEMI_BOLD_WEIGHT = 600,
    /* Blod font weight */
    BOLD_WEIGHT = 700,
    /* Extra blod font weight */
    EXTRA_BOLD_WEIGHT = 800,
    /* Black font weight */
    BLACK_WEIGHT = 900,
    /* Extra black font weight */
    EXTRA_BLACK_WEIGHT = 1000,
};

/**
 * @brief Enumerates font width.
 *
 * @since 12
 * @version 1.0
 */
enum OH_Drawing_FontStyleWidth {
    /* Ultra condensed font width */
    ULTRA_CONDENSED_WIDTH = 1,
    /* Extra condensed font width */
    EXTRA_CONDENSED_WIDTH = 2,
    /* condensed font width */
    CONDENSED_WIDTH = 3,
    /* Semi condensed font width */
    SEMI_CONDENSED_WIDTH = 4,
    /* Normal font width */
    NORMAL_WIDTH = 5,
    /* Semi expanded font width */
    SEMI_EXPANDED_WIDTH = 6,
    /* Expanded font width */
    EXPANDED_WIDTH = 7,
    /* Extra expanded font width */
    EXTRA_EXPANDED_WIDTH = 8,
    /* Ultra expanded font width */
    ULTRA_EXPANDED_WIDTH = 9,
};

/**
 * @brief Enumerates font slant.
 *
 * @since 12
 * @version 1.0
 */
enum OH_Drawing_FontStyleSlant {
    /* Upright font slant */
    UPRIGHT_SLANT,
    /* Italic font slant */
    ITALIC_SLANT,
    /* Oblique font slant */
    OBLIQUE_SLANT,
};

/**
 * @brief Defines the font form.
 *
 * @since 12
 * @version 1.0
 */
typedef struct OH_Drawing_FontStyleStruct {
    /** Font weight */
    OH_Drawing_FontStyleWeight weight;
    /** Font width */
    OH_Drawing_FontStyleWidth width;
    /** Font slant */
    OH_Drawing_FontStyleSlant slant;
} OH_Drawing_FontStyleStruct;

/**
 * @brief Creates an <b>OH_Drawing_FontMgr</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_FontMgr</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_FontMgr* OH_Drawing_FontMgrCreate(void);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_FontMgr</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_FontMgrDestroy(OH_Drawing_FontMgr*);

/**
 * @brief Gets the count of font families.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @return Returns the count of font families.
 * @since 12
 * @version 1.0
 */
int OH_Drawing_FontMgrGetFamiliesCount(OH_Drawing_FontMgr*);

/**
 * @brief Gets the font family name by the index.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @param index Indicates the index to get the font family name.
 * @return Returns the font family name corresponding to the index value.
 * @since 12
 * @version 1.0
 */
char* OH_Drawing_FontMgrGetFamilyName(OH_Drawing_FontMgr*, int index);

/**
 * @brief Releases the memory occupied by font family name.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param familyName Indicates the font family name.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_DestroyFamilyName(char* familyName);

/**
 * @brief Creates an <b>OH_Drawing_FontStyleSet</b> object by <b>OH_Drawing_FontMgr</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @param index Indicates the index used to get the font style set object from the font manager object.
 * @return Returns the pointer to the <b>OH_Drawing_FontStyleSet</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_FontStyleSet* OH_Drawing_FontMgrCreateFontStyleSet(OH_Drawing_FontMgr*, int index);

/**
 * @brief Releases the memory occupied by an <b>OH_Drawing_FontStyleSet</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontStyleSet Indicates the pointer to an <b>OH_Drawing_FontStyleSet</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_FontMgrDestroyFontStyleSet(OH_Drawing_FontStyleSet*);

/**
 * @brief Get the pointer to an <b>OH_Drawing_FontStyleSet</b> object for the given font style set family name.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @param familyName Indicates the family name of a font style set to be matched.
 * @return Returns the pointer to the <b>OH_Drawing_FontStyleSet</b> object matched.
 * @since 12
 * @version 1.0
 */
OH_Drawing_FontStyleSet* OH_Drawing_FontMgrMatchFamily(OH_Drawing_FontMgr*, const char* familyName);

/**
 * @brief Get the pointer to an <b>OH_Drawing_Typeface</b> object based on the given font style and family name.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @param familyName Indicates the family name of a font style set to be matched.
 * @param OH_Drawing_FontStyleStruct Indicates the pointer to an <b>OH_Drawing_FontStyleStruct</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object matched.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyle(OH_Drawing_FontMgr*,
    const char* familyName, OH_Drawing_FontStyleStruct*);

/**
 * @brief Get the pointer to an <b>OH_Drawing_Typeface</b> object for the given character.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontMgr Indicates the pointer to an <b>OH_Drawing_FontMgr</b> object.
 * @param familyName Indicates the family name of a font style set to be matched.
 * @param OH_Drawing_FontStyleStruct Indicates the pointer to an <b>OH_Drawing_FontStyleStruct</b> object.
 * @param bcp47 Indicates an array of languages which indicate the language of character.
 * @param bcp47Count Indicates the array size of bcp47.
 * @param character Indicates a UTF8 value to be matched.
 * @return Returns the pointer to the <b>OH_Drawing_Typeface</b> object matched.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Typeface* OH_Drawing_FontMgrMatchFamilyStyleCharacter(OH_Drawing_FontMgr*, const char* familyName,
    OH_Drawing_FontStyleStruct*, const char* bcp47[], int bcp47Count, int32_t character);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
