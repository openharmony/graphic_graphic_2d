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

#include "drawing_path_effect.h"

#include <mutex>
#include <unordered_map>

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "effect/path_effect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Path& CastToPath(const OH_Drawing_Path& cPath)
{
    return reinterpret_cast<const Path&>(cPath);
}

OH_Drawing_PathEffect* OH_Drawing_CreateComposePathEffect(OH_Drawing_PathEffect* outer, OH_Drawing_PathEffect* inner)
{
    if (outer == nullptr || inner == nullptr) {
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    NativeHandle<PathEffect>* pathEffect1 = Helper::CastTo<OH_Drawing_PathEffect*, NativeHandle<PathEffect>*>(outer);
    NativeHandle<PathEffect>* pathEffect2 = Helper::CastTo<OH_Drawing_PathEffect*, NativeHandle<PathEffect>*>(inner);
    if (!pathEffect1->value || !pathEffect2->value) {
        delete pathEffectHandle;
        return nullptr;
    }
    pathEffectHandle->value = PathEffect::CreateComposePathEffect(*pathEffect1->value, *pathEffect2->value);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreateCornerPathEffect(float radius)
{
    if (radius <= 0) {
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    pathEffectHandle->value = PathEffect::CreateCornerPathEffect(radius);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreateDashPathEffect(float* intervals, int count, float phase)
{
    if (intervals == nullptr || count <= 0) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    pathEffectHandle->value = PathEffect::CreateDashPathEffect(intervals, count, phase);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreateDiscretePathEffect(float segLength, float deviation)
{
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    pathEffectHandle->value = PathEffect::CreateDiscretePathEffect(segLength, deviation);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreatePathDashEffect(const OH_Drawing_Path* path, float advance, float phase,
    OH_Drawing_PathEffectType type)
{
    if (path == nullptr || advance <= 0) {
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    pathEffectHandle->value = PathEffect::CreatePathDashEffect(CastToPath(*path), advance,
        phase, static_cast<PathDashStyle>(type));
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

OH_Drawing_PathEffect* OH_Drawing_CreateSumPathEffect(OH_Drawing_PathEffect* firstPathEffect,
    OH_Drawing_PathEffect* secondPathEffect)
{
    if (firstPathEffect == nullptr || secondPathEffect == nullptr) {
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    NativeHandle<PathEffect>* pathEffectHandleFirst = Helper::CastTo<OH_Drawing_PathEffect*,
        NativeHandle<PathEffect>*>(firstPathEffect);
    NativeHandle<PathEffect>* pathEffectHandleSecond = Helper::CastTo<OH_Drawing_PathEffect*,
        NativeHandle<PathEffect>*>(secondPathEffect);
    if (!pathEffectHandleFirst->value || !pathEffectHandleSecond->value) {
        delete pathEffectHandle;
        return nullptr;
    }
    pathEffectHandle->value = PathEffect::CreateSumPathEffect(*pathEffectHandleFirst->value,
        *pathEffectHandleSecond->value);
    if (pathEffectHandle->value == nullptr) {
        delete pathEffectHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

void OH_Drawing_PathEffectDestroy(OH_Drawing_PathEffect* cPathEffect)
{
    if (!cPathEffect) {
        return;
    }
    delete Helper::CastTo<OH_Drawing_PathEffect*, NativeHandle<PathEffect>*>(cPathEffect);
}
