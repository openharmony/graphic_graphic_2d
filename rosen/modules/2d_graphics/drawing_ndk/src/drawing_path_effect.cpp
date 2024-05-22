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

#include "drawing_path_effect.h"
#include <mutex>
#include <unordered_map>
#include "drawing_helper.h"
#include "effect/path_effect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_PathEffect* OH_Drawing_CreateDashPathEffect(float* intervals, int count, float phase)
{
    if (intervals == nullptr || count <= 0) {
        return nullptr;
    }
    NativeHandle<PathEffect>* pathEffectHandle = new NativeHandle<PathEffect>;
    if (pathEffectHandle == nullptr) {
        return nullptr;
    }
    pathEffectHandle->value = PathEffect::CreateDashPathEffect(intervals, count, phase);
    return Helper::CastTo<NativeHandle<PathEffect>*, OH_Drawing_PathEffect*>(pathEffectHandle);
}

void OH_Drawing_PathEffectDestroy(OH_Drawing_PathEffect* cPathEffect)
{
    delete Helper::CastTo<OH_Drawing_PathEffect*, NativeHandle<PathEffect>*>(cPathEffect);
}
