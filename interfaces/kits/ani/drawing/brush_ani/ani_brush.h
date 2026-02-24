/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_ANI_BRUSH_H
#define OHOS_ROSEN_ANI_BRUSH_H

#include "ani_drawing_utils.h"
#include "draw/brush.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniBrush final {
public:
    explicit AniBrush(std::shared_ptr<Brush> brush = nullptr) : brush_(brush) {}
    ~AniBrush();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithBrush(ani_env* env, ani_object obj, ani_object aniBrushObj);
    static void SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter);
    static ani_int GetAlpha(ani_env* env, ani_object obj);
    static void SetColorWithObject(ani_env* env, ani_object obj, ani_object aniColor);
    static void SetColor4f(ani_env* env, ani_object obj, ani_object aniColor4f, ani_object aniColorSpace);
    static ani_object GetColor4f(ani_env* env, ani_object obj);
    static void SetColorWithARGB(ani_env* env, ani_object obj, ani_int alpha,
        ani_int red, ani_int green, ani_int blue);
    static void SetColor(ani_env* env, ani_object obj, ani_int color);
    static ani_object GetColor(ani_env* env, ani_object obj);
    static ani_int GetHexColor(ani_env* env, ani_object obj);
    static void SetAntiAlias(ani_env* env, ani_object obj, ani_boolean aa);
    static ani_boolean IsAntiAlias(ani_env* env, ani_object obj);
    static void SetAlpha(ani_env* env, ani_object obj, ani_int alpha);
    static void SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode);
    static void Reset(ani_env*  env, ani_object obj);
    static ani_object GetColorFilter(ani_env* env, ani_object obj);
    static void SetImageFilter(ani_env* env, ani_object obj, ani_object imageFilterObj);
    static void SetMaskFilter(ani_env* env, ani_object obj, ani_object maskFilterObj);
    static void SetShadowLayer(ani_env* env, ani_object obj, ani_object shadowLayerObj);
    static void SetShaderEffect(ani_env* env, ani_object obj, ani_object shaderEffectObj);

    std::shared_ptr<Brush> GetBrush();

private:
    static ani_object BrushTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetBrushAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Brush>* GetBrushPtrAddr();
    std::shared_ptr<Brush> brush_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_BRUSH_H