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

#ifndef C_INCLUDE_DRAWING_RECORD_CMD_H
#define C_INCLUDE_DRAWING_RECORD_CMD_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 13
 * @version 1.0
 */

/**
 * @file drawing_record_cmd.h
 *
 * @brief Declares functions related to the <b>RecordCmd</b> object in the drawing module.
 *
 * @library libnative_drawing.so
 * @since 13
 * @version 1.0
 */

#include "drawing_types.h"
#include "drawing_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_RecordCmdUtils</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_RecordCmdUtils</b> object created.
 * @since 13
 * @version 1.0
 */
OH_Drawing_RecordCmdUtils* OH_Drawing_RecordCmdUtilsCreate(void);

/**
 * @brief Destroys an <b>OH_Drawing_RecordCmdUtils</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param recordCmdUtils Indicates the pointer to an <b>OH_Drawing_RecordCmdUtils</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if recordCmdUtils is nullptr.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsDestroy(OH_Drawing_RecordCmdUtils* recordCmdUtils);

/**
 * @brief Get the canvas that records the drawing command.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param recordCmdUtils Indicates the pointer to an <b>OH_Drawing_RecordCmdUtils</b> object.
 * @param width Width of canvas object.
 * @param height Height of canvas object.
 * @param canvas Indicates a secondary pointer to an <b>OH_Drawing_Canvas</b>object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if recordCmdUtils or canvas is nullptr,
 *         width less than or equal to 0 or height less than or equal to 0.
 *         Returns {@link OH_DRAWING_ERROR_ALLOCATION_FAILED} if no memory.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsBeginRecording(OH_Drawing_RecordCmdUtils* recordCmdUtils,
    int32_t width, int32_t height, OH_Drawing_Canvas** canvas);

/**
 * @brief Finish the recording and get the recording command object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param recordCmdUtils Indicates the pointer to an <b>OH_Drawing_RecordCmdUtils</b> object.
 * @param recordCmd Indicates a secondary pointer to an <b>OH_Drawing_RecordCmd</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if recordCmdUtils or recordCmd is nullptr.
 *         Returns {@link OH_DRAWING_ERROR_ALLOCATION_FAILED} if no memory.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_RecordCmdUtilsFinishRecording(OH_Drawing_RecordCmdUtils* recordCmdUtils,
    OH_Drawing_RecordCmd** recordCmd);

/**
 * @brief Destroys an <b>OH_Drawing_RecordCmd</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param recordCmd Indicates the pointer to an <b>OH_Drawing_RecordCmd</b> object.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if recordCmd is nullptr.
 * @since 13
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_RecordCmdDestroy(OH_Drawing_RecordCmd* recordCmd);

#ifdef __cplusplus
}
#endif
/** @} */
#endif