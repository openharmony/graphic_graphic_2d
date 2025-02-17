/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
 * @file drawing_path_effect.h
 *
 * @brief Declares functions related to the <b>pathEffect</b> object in the drawing module.
 *
 * @since 12
 * @version 1.0
 */

#ifndef C_INCLUDE_DRAWING_PATH_EFFECT_H
#define C_INCLUDE_DRAWING_PATH_EFFECT_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerate path effect types.
 *
 * @since 16
 * @version 1.0
 */
typedef enum {
    /** Indicates that the path effect is a translation effect. */
    PATH_EFFECT_TRANSLATE,
    /** Indicates that the path effect is a rotation effect. */
    PATH_EFFECT_ROTATE,
    /** Indicates that the path effect is a morph effect. */
    PATH_EFFECT_MORPH,
} OH_Drawing_PathEffectType;

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object that is a combination of paths,
 * applying the inner path effect first and then the outer path effect.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param outer Indicates an <b>OH_Drawing_PathEffect</b> object
 * @param inner Indicates an <b>OH_Drawing_PathEffect</b> object
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is outer or inner is nullptr.
 * @since 16
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreateComposePathEffect(OH_Drawing_PathEffect* outer, OH_Drawing_PathEffect* inner);

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object
 * that turns the included angle of the path into a fillet of a specified radius.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param radius Indicates the degree of curvature of the arc, the radius must be greater than zero.
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is radius is zero or less.
 * @since 16
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreateCornerPathEffect(float radius);

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param intervals Indicates a array which contain an even number of entries.
 * @param count Indicates the number of elements of the intervals array.
 * @param phase Indicates the offset into intervals array.
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreateDashPathEffect(float* intervals, int count, float phase);

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object
 * that breaks the path and creates an irregular distribution on the path.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param segLength Indicates the maximum segment length of the path.
 * @param deviation Indicates the deviation during drawing.
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 * @since 16
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreateDiscretePathEffect(float segLength, float deviation);

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object and sets the path effect to a dash effect.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param path Indicates the pointer to an <b>OH_Drawing_Path</b> object.
 * @param advance Indicates the distance between the dashed segments.
 * @param phase Indicates the offset into intervals array.
 * @param type Indicates the type of the path effect.
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is advance and phase are zero or less.
 * @since 16
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreatePathDashEffect(const OH_Drawing_Path* path, float advance, float phase,
    OH_Drawing_PathEffectType type);

/**
 * @brief Creates an <b>OH_Drawing_PathEffect</b> object by overlaying two path effects.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param firstPathEffect Indicates the pointer to an <b>OH_Drawing_PathEffect</b> object.
 * @param secondPathEffect Indicates the pointer to an <b>OH_Drawing_PathEffect</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_PathEffect</b> object created.
 *         If nullptr is returned, the creation fails.
 *         The possible cause of the failure is firstPathEffect or secondPathEffect is nullptr.
 * @since 16
 * @version 1.0
 */
OH_Drawing_PathEffect* OH_Drawing_CreateSumPathEffect(OH_Drawing_PathEffect* firstPathEffect,
    OH_Drawing_PathEffect* secondPathEffect);

/**
 * @brief Destroys an <b>OH_Drawing_PathEffect</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param pathEffect Indicates the pointer to an <b>OH_Drawing_PathEffect</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_PathEffectDestroy(OH_Drawing_PathEffect* pathEffect);

#ifdef __cplusplus
}
#endif
/** @} */
#endif
