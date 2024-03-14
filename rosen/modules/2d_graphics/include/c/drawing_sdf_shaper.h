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

#ifndef C_INCLUDE_DRAWING_SDF_SHAPER_H
#define C_INCLUDE_DRAWING_SDF_SHAPER_H

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
 * @file drawing_sdf_shaper.h
 *
 * @brief Declares functions related to the <b>SDF</b> object in the drawing module.
 *
 * @since 12
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    // Basic SDF primitives
    BEGIN = 0,
    SD_CIRCLE,
    SD_ELLIPSE,
    SD_SEGMENT,
    SD_BOX,
    SD_EQUILATERAL_TRIANGLE,
    SD_RHOMBUS,
    SD_PARALLELOGRAM,
    SD_PENTAGON,
    SD_HEXAGON,
    SD_OCTOGON,
    // SDF Operators
    OP_ROUND,
    OP_UNION,
    OP_ONION,
    OP_SUBSTRACTION,
    OP_INTERSECTION,
    OP_XOR,
    OP_SMOOTH_UNION,
    OP_SMOOTH_SUBTRACTION,
    OP_SMOOTH_INTERSECTIOPN,
    // SDF Advanced
    SD_CAPSULE,
    SD_ARROW,
    SD_CROSS,
    SD_RING,
    SD_PART_RING_APPROX,
    SD_PART_RING,
    SD_QUESTION_MARK,
    END,
} OH_Drawing_SDFType;
/**
 * @brief Creates an <b>OH_Drawing_SDF</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_SDF</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_SDF* OH_Drawing_SDFCreate(void);

/**
 * @brief Destroys an <b>OH_Drawing_SDF</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_SDF Indicates the pointer to an <b>OH_Drawing_SDF</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_SDFDestroy(OH_Drawing_SDF*);

void OH_Drawing_SDFBuildShader(OH_Drawing_SDF*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
