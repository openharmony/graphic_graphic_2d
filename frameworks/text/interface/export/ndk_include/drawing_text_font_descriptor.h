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

#ifndef DRAWING_TEXT_FONT_DESCRIPTOR_H
#define DRAWING_TEXT_FONT_DESCRIPTOR_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides the font descriptor capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 14
 * @version 1.0
 */

/**
 * @file drawing_text_font_descriptor.h
 *
 * @brief Provide the ability to provide <b>OH_Drawing_FontDescriptor</b>.
 *
 * @kit ArkGraphics2D
 * @library libnative_drawing.so
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @since 14
 * @version 1.0
 */

#include "drawing_text_declaration.h"
#include "drawing_text_typography.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type style flag.
 *
 * @since 14
 * @version 1.0
 */
typedef enum {
    /** Italic font */
    ITALIC = 1 << 0,
    /** Bold font */
    BOLD = 1 << 1,
} OH_Drawing_FontTypeStyle;

/**
 * @brief An enumeration of system font types.
 *
 * @since 14
 * @version 1.0
 */
typedef enum {
    /** All font types */
    ALL = 1 << 0,
    /** System generic font type */
    GENERIC = 1 << 1,
    /** Stylish font type */
    STYLISH = 1 << 2,
    /** Installed font types */
    INSTALLED = 1 << 3,
    /** Customized font types */
    CUSTOMIZED = 1 << 4,
} OH_Drawing_SystemFontType;

/**
 * @brief An enumeration of font full descriptor attribute.
 *
 * @since 22
 */
typedef enum {
    /** The file path of the font */
    FULL_DESCRIPTOR_ATTR_S_PATH = 0,
    /** A name that uniquely identifies the font */
    FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME = 1,
    /** The full name of the font */
    FULL_DESCRIPTOR_ATTR_S_FULL_NAME = 2,
    /** The family name of the font */
    FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME = 3,
    /** The subfont family of the font */
    FULL_DESCRIPTOR_ATTR_S_SUB_FAMILY_NAME = 4,
    /** The weight of the font */
    FULL_DESCRIPTOR_ATTR_I_WEIGHT = 5,
    /** The width of the font */
    FULL_DESCRIPTOR_ATTR_I_WIDTH = 6,
    /** Whether the font is tilted */
    FULL_DESCRIPTOR_ATTR_I_ITALIC = 7,
    /** Whether the font is monospaced */
    FULL_DESCRIPTOR_ATTR_B_MONO = 8,
    /** whether symbolic fonts are supported */
    FULL_DESCRIPTOR_ATTR_B_SYMBOLIC = 9,
    /**
     * The local postscript name of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_LOCAL_POSTSCRIPT_NAME = 10,
    /**
     * The local full name of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_LOCAL_FULL_NAME = 11,
    /**
     * The local family name of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_LOCAL_FAMILY_NAME = 12,
    /**
     * The local sub family name of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_LOCAL_SUB_FAMILY_NAME = 13,
    /**
     * The version of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_VERSION = 14,
    /**
     * The manufacture of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_MANUFACTURE = 15,
    /**
     * The copyright of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_COPYRIGHT = 16,
    /**
     * The trademark of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_TRADEMARK = 17,
    /**
     * The license of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_S_LICENSE = 18,
    /**
     * The array of font variation axis
     * @since 24
     */
    FULL_DESCRIPTOR_ATTR_O_VARIATION_AXIS = 19,
    /**
     * The array of font variation instance
     * @since 24
     */
    FULL_DESCRIPTOR_ATTR_O_VARIATION_INSTANCE = 20,
    /**
     * The index of the font
     * @since 23
     */
    FULL_DESCRIPTOR_ATTR_I_INDEX = 21,
} OH_Drawing_FontFullDescriptorAttributeId;

/**
 * @brief An enumeration of font variation axis attribute.
 *
 * @since 24
 */
typedef enum {
    /** The key of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_S_KEY = 0,
    /** The min value of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_D_MIN_VALUE = 1,
    /** The max value of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_D_MAX_VALUE = 2,
    /** The default value of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_D_DEFAULT_VALUE = 3,
    /** The flags of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_I_FLAGS = 4,
    /** The name of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_S_NAME = 5,
    /** The local name of the font variation aixs */
    FONT_VARIATION_AIXS_ATTR_S_LOCAL_NAME = 6,
} OH_Drawing_FontVariationAxisAttributeId;

/**
 * @brief An enumeration of font variation instance attribute.
 *
 * @since 24
 */
typedef enum {
    /** The name of the font variation instance */
    FONT_VARIATION_INSTANCE_ATTR_S_NAME = 0,
    /** The local name of the font variation instance */
    FONT_VARIATION_INSTANCE_ATTR_S_LOCAL_NAME = 1,
} OH_Drawing_FontVariationInstanceAttributeId;

/**
 * @brief Defines the font variation coordinate.
 *
 * @since 24
 */
typedef struct {
    /** key of font variation coordinate */
    char* axisKey;
    /** value of font variation coordinate */
    double value;
} OH_Drawing_FontVariationInstanceCoordinate;

/**
 * @brief Obtain all system font descriptive symbols that match the specified font descriptor. Where the 'path' and
 * 'size' fields are not considered as valid matching values, It takes effect when the remaining fields are not
 * default values, If all the fields of the parameters <b>OH_Drawing_FontDescriptor</b> are default, obtain all system
 * font descriptors. If the match fails, return nullptr.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontDescriptor The pointer to the <b>OH_Drawing_FontDescriptor</b> object. It is recommended to
 * use <b>OH_Drawing_CreateFontDescriptor</b> to obtain a valid <b>OH_Drawing_FontDescriptor</b> instance.
 * If you create your own <b>OH_Drawing_FontDescriptor</b> object, ensure that fields not intended for matching are
 * set to their default values.
 * @param size_t Indicates the count of obtained <b>OH_Drawing_FontDescriptor</b>.
 * @return Returns an array of <b>OH_Drawing_FontDescriptor</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_FontDescriptor* OH_Drawing_MatchFontDescriptors(OH_Drawing_FontDescriptor*, size_t*);

/**
 * @brief Releases the <b>OH_Drawing_FontDescriptor</b> array.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_FontDescriptor Pointer to <b>OH_Drawing_FontDescriptor</b> array.
 * @param size_t Represents the number of members of the <b>OH_Drawing_FontDescriptor</b> array.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroyFontDescriptors(OH_Drawing_FontDescriptor*, size_t);

/**
 * @brief Get the <b>OH_Drawing_FontDescriptor</b> object by the font full name and the font type, supporting generic
 * fonts, stylish fonts, and installed fonts.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_String Indicates the full name object <b>OH_Drawing_String</b>.
 * @param OH_Drawing_SystemFontType Indicates enumerates of system font type object <b>OH_Drawing_SystemFontType</b>.
 * @return Returns the pointer to a font descriptor object <b>OH_Drawing_FontDescriptor</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_FontDescriptor* OH_Drawing_GetFontDescriptorByFullName(const OH_Drawing_String*, OH_Drawing_SystemFontType);

/**
 * @brief Obtain the corresponding font full name array by the font type.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_SystemFontType Indicates enumerates of system font type object <b>OH_Drawing_SystemFontType</b>.
 * @return Returns the pointer to full name array object <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
OH_Drawing_Array* OH_Drawing_GetSystemFontFullNamesByType(OH_Drawing_SystemFontType);

/**
 * @brief Get the specified full name object <b>OH_Drawing_String</b> by index from the
 * <b>OH_Drawing_Array</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Array Indicates an array of full name object <b>OH_Drawing_Array</b>.
 * @param size_t The index of full name.
 * @return Returns a full name object <b>OH_Drawing_String</b>.
 * @since 14
 * @version 1.0
 */
const OH_Drawing_String* OH_Drawing_GetSystemFontFullNameByIndex(OH_Drawing_Array*, size_t);

/**
 * @brief Releases the memory occupied by an array of font full names.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Array Indicates an array of full name object <b>OH_Drawing_Array</b>.
 * @since 14
 * @version 1.0
 */
void OH_Drawing_DestroySystemFontFullNames(OH_Drawing_Array*);

/**
 * @brief Retrieves an array of font full descriptors from raw binary data.
 *
 * @param data Pointer to the raw binary font data buffer.
 * @param size Size of the font data buffer in bytes.
 * @return Returns a pointer to <b>OH_Drawing_Array</b> containing font full descriptors.
 *         Returns <b>NULL</b> if the operation fails due to invalid data format or parsing errors.
 * @since 22
 */
OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorsFromStream(const void* data, size_t size);

/**
 * @brief Obtains an array of font full descriptors from font file path.
 *
 * @param path The font file path.
 * @return Returns a pointer to <b>OH_Drawing_Array</b> structure containing font full descriptors.
 *         Returns <b>NULL</b> if no fonts are found, invalid path, no permission, or non-font file.
 * @since 22
 */
OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorsFromPath(const char* path);

/**
 * @brief Retrieves a font full descriptor from an array by index.
 *
 * @param descriptorArray Pointer to the array of font full descriptors <b>OH_Drawing_Array</b>.
 * @param index Zero-based index position of the descriptor to retrieve.
 * @return Returns a pointer to <b>OH_Drawing_FontFullDescriptor</b> at the specified index.
 *         Returns <b>NULL</b> if the index is out of bounds or the array is invalid.
 * @since 22
 */
const OH_Drawing_FontFullDescriptor* OH_Drawing_GetFontFullDescriptorByIndex(
    OH_Drawing_Array* descriptorArray, size_t index);

/**
 * @brief Releases the memory occupied by an array of font full descriptors.
 *
 * @param descriptorArray Pointer to the array of font full descriptors <b>OH_Drawing_Array</b>.
 * @since 22
 */
void OH_Drawing_DestroyFontFullDescriptors(OH_Drawing_Array* descriptorArray);

/**
 * @brief Defines an <b>OH_Drawing_GetFontUnicodeArrayFromFile</b>, which is used to get unicode from font file.
 *
 * @param fontSrc Indicates the path of the font file.
 * @param index Indicates the index of the font data in the ttc file.
 * @param unicodeArray Output parameter to receive unicode, When no longer needed, use 'free()' to release.
 * @param arrayLength Output parameter to receive the length of the unicode array.
 * @return error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the fontSrc is invalid or non-font file.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontUnicodeArrayFromFile(const char* fontSrc, uint32_t index,
    int32_t** unicodeArray, int32_t* arrayLength);

/**
 * @brief Defines an <b>OH_Drawing_GetFontUnicodeArrayFromBuffer</b>, which is used to get unicode from font buffer.
 *
 * @param fontBuffer Indicates the font data.
 * @param length Indicates the font data length.
 * @param index Indicates the index of the font data in the ttc file.
 * @param unicodeArray Output parameter to receive unicode, When no longer needed, use 'free()' to release.
 * @param arrayLength Output parameter to receive the length of the unicode array.
 * @return error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the fontBuffer is invalid or non-font buffer.
 * @since 23
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontUnicodeArrayFromBuffer(uint8_t* fontBuffer, size_t length, uint32_t index,
    int32_t** unicodeArray, int32_t* arrayLength);

/**
 * @brief Retrieves an integer attribute value from a font full descriptor.
 *
 * @param descriptor Pointer to the font full descriptor <b>OH_Drawing_FontFullDescriptor</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontFullDescriptorAttributeId</b> enumeration.
 * @param value Output parameter to receive the requested integer attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the descriptor or value is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.
 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeInt(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, int* value);

/**
 * @brief Retrieves a boolean attribute value from a font full descriptor.
 *
 * @param descriptor Pointer to the font full descriptor <b>OH_Drawing_FontFullDescriptor</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontFullDescriptorAttributeId</b> enumeration.
 * @param value Output parameter to receive the requested boolean attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the descriptor or value is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.
 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeBool(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, bool* value);

/**
 * @brief Retrieves a string attribute value from a font full descriptor.
 *
 * @note The caller is responsible for manually releasing the internal <b>strData</b> member of the
 *       <b>OH_Drawing_String</b> structure when it is no longer needed.
 *
 * @param descriptor Pointer to the font full descriptor <b>OH_Drawing_FontFullDescriptor</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontFullDescriptorAttributeId</b> enumeration.
 * @param str Output parameter to receive pointer to the requested string attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the descriptor or str is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.
 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontFullDescriptorAttributeString(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id, OH_Drawing_String* str);

/**
 * @brief Retrieves a object array attribute value from a font full descriptor.
 *
 * @param descriptor Pointer to the font full descriptor <b>OH_Drawing_FontFullDescriptor</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontFullDescriptorAttributeId</b> enumeration.
 * @return Return the object array
 * @since 24
 */
OH_Drawing_Array* OH_Drawing_GetFontFullDescriptorAttributeArray(const OH_Drawing_FontFullDescriptor* descriptor,
    OH_Drawing_FontFullDescriptorAttributeId id);

/**
 * @brief Retrieves a font variation axis from an array by index.
 *
 * @param array Pointer to the array of font variation axis <b>OH_Drawing_Array</b>.
 * @param index Zero-based index position of the variation axis to retrieve.
 * @return Returns a pointer to the <b>OH_Drawing_FontVariationAxis</b> at the specified index.
 *         Returns <b>NULL</b> if the index is out of bounds or the array is invalid.
 * @since 24
 */
OH_Drawing_FontVariationAxis* OH_Drawing_GetFontVariationAxisByIndex(OH_Drawing_Array* array, size_t index);

/**
 * @brief Releases the memory occupied by an array of font variation axis.
 *
 * @param fontVariaAxisArray Indicates an array of font variaiton axis object <b>OH_Drawing_Array</b>.
 * @since 24
 */
void OH_Drawing_DestroyFontVariationAxis(OH_Drawing_Array* fontVariaAxisArray);

/**
 * @brief Retrieves a double attribute value from a font variation axis.
 *
 * @param variationAxis Pointer to the font variation axis <b>OH_Drawing_FontVariationAxis</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontVariationAxisAttributeId</b> enumeration.
 * @param value Output parameter to receive the requested double attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the variationAxis or value is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.
 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeDouble(OH_Drawing_FontVariationAxis* variationAxis,
    OH_Drawing_FontVariationAxisAttributeId id, double *value);

/**
 * @brief Retrieves a int attribute value from a font variation axis.
 *
 * @param variationAxis Pointer to the font variation axis <b>OH_Drawing_FontVariationAxis</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontVariationAxisAttributeId</b> enumeration.
 * @param value Output parameter to receive the requested int attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the variationAxis or value is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.
 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeInt(OH_Drawing_FontVariationAxis* variationAxis,
    OH_Drawing_FontVariationAxisAttributeId id, int *value);

/**
 * @brief Retrieves a string attribute value from a font variation axis.
 *
 * @note The caller is responsible for manually releasing the internal <b>strData</b> member of the
 *       <b>OH_Drawing_String></b> structure when it is no longer needed.
 *
 * @param variationAxis Pointer to the font variation axis <b>OH_Drawing_FontVariationAxis</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontVariationAxisAttributeId</b> enumeration.
 * @param str Output parameter to receive the requested string attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the variationAxis or str is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.

 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontVariationAxisAttributeStr(OH_Drawing_FontVariationAxis* variationAxis,
    OH_Drawing_FontVariationAxisAttributeId id, OH_Drawing_String *str);

/**
 * @brief Retrieves a font variation instance from an array by index.
 *
 * @param array Pointer to the array of font variation instance <b>OH_Drawing_Array</b>.
 * @param index Zero-based index position of the variation instance to retrieve.
 * @return Returns a pointer to the <b>OH_Drawing_FontVariationInstance</b> at the specified index.
 *         Returns <b>NULL</b> if the index is out of bounds or the array is invalid.
 * @since 24
 */
OH_Drawing_FontVariationInstance* OH_Drawing_GetFontVariationInstanceByIndex(OH_Drawing_Array* array, size_t index);

/**
 * @brief Releases the memory occupied by an array of font variation instance.
 *
 * @param fontVariaAxisInstance Indicates an array of font variaiton instance object <b>OH_Drawing_Array</b>.
 * @since 24
 */
void OH_Drawing_DestroyFontVariationInstance(OH_Drawing_Array* FontVariationInstance);

/**
 * @brief Retrieves a string attribute value from a font variation instance.
 *
 * @note The caller is responsible for manually releasing the internal <b>strData</b> member of the
 *       <b>OH_Drawing_String></b> structure when it is no longer needed.
 *
 * @param variationInstance Pointer to the font variation instance <b>OH_Drawing_FontVariationInstance</b>.
 * @param id Attribute identifier from <b>OH_Drawing_FontVariationInstanceAttributeId</b> enumeration.
 * @param str Output parameter to receive pointer to the requested string attribute value.
 * @return Returns the error code.
 *         Returns <b>OH_DRAWING_SUCCESS</b> if the operation is successful.
 *         Returns <b>OH_DRAWING_ERROR_INCORRECT_PARAMETER</b> if the variationInstance or str is NULL.
 *         Returns <b>OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH</b> if the attribute id is not recognized or supported.

 * @since 22
 */
OH_Drawing_ErrorCode OH_Drawing_GetFontVariationInstanceAttributeStr(OH_Drawing_FontVariationInstance* variationInstance,
    OH_Drawing_FontVariationInstanceAttributeId id, OH_Drawing_String *str);

/**
 * @brief Get the <b>OH_Drawing_FontVariationInstanceCoordinate</b> object.
 *
 * @param variationInstance Indicates the variation instance.
 * @param arrayLength Indicates the length of OH_Drawing_FontVariationInstanceCoordinate list.
 * @return Returns the pointer to a font variation coordinate list.
 * @since 24
 */
OH_Drawing_FontVariationInstanceCoordinate* OH_Drawing_GetFontVariationInstanceCoordinate(
    OH_Drawing_FontVariationInstance* variationInstance, size_t *arrayLength);

/**
 * @brief Get the <b>OH_Drawing_FontFullDescriptor</b> object by the font full name and the font type
 *
 * @param fullName Indicates the full name object <b>OH_Drawing_String</b>.
 * @param fontType Indicates the enumerates of system font type object <b>OH_Drawing_SystemFontType</b>.
 * @return Returns the pointer to a font full descriptor object <b>OH_Drawing_FontFullDescriptor</b>.
 * @since 24
 */
OH_Drawing_FontFullDescriptor* OH_Drawing_GetFontFullDescriptorByFullName(const OH_Drawing_String* fullName,
    OH_Drawing_SystemFontType fontType);

/**
 * @brief Defines an <b>OH_Drawing_GetFontCountFromFile</b>, which is used to get font count from font file.
 *
 * @param fontSrc Indicates the path of the font file.
 * @return font count.
 * @since 23
 */
uint32_t OH_Drawing_GetFontCountFromFile(const char* fontSrc);

/**
  * @brief Defines an <b>OH_Drawing_GetFontCountFromBuffer</b>, which is used to get font count from font buffer.
  *
  * @param fontBuffer Indicates the font data.
  * @param length Indicates the font data length.
  * @return font count.
  * @since 23
  */
uint32_t OH_Drawing_GetFontCountFromBuffer(uint8_t* fontBuffer, size_t length);

/**
 * @brief Obtains an array of font paths from the system.
 * @param fontType The type of system fonts to retrieve.
 * @param pathCount Output parameter to receive the number of font paths retrieved.
 * @return Returns a pointer to an array of <b>OH_Drawing_String</b>
 *         representing the font paths. Returns <b>NULL</b> if no fonts are found or an error occurs.
 * @since 23
 *
 */
OH_Drawing_String* OH_Drawing_GetFontPathsByType(OH_Drawing_SystemFontType fontType, size_t* pathCount);
#ifdef __cplusplus
}
#endif
/** @} */
#endif