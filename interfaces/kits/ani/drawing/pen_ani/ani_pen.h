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

#ifndef OHOS_ROSEN_ANI_PEN_H
#define OHOS_ROSEN_ANI_PEN_H

#include "ani_drawing_utils.h"
#include "draw/pen.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPen final {
public:
    explicit AniPen(std::shared_ptr<Pen> pen = nullptr) : pen_(pen) {}
    ~AniPen();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithPen(ani_env* env, ani_object obj, ani_object aniPenObj);

    static ani_int GetAlpha(ani_env* env, ani_object obj);
    static void SetAlpha(ani_env* env, ani_object obj, ani_int alpha);
    static void SetColorFilter(ani_env* env, ani_object obj, ani_object objColorFilter);
    static void SetBlendMode(ani_env* env, ani_object obj, ani_enum_item aniBlendMode);
    static void Reset(ani_env*  env, ani_object obj);
    static ani_int GetHexColor(ani_env* env, ani_object obj);
    static void SetColor(ani_env* env, ani_object obj, ani_int color);
    static void SetColorWithColor(ani_env* env, ani_object obj, ani_object colorObj);
    static void SetColorWithNumber(ani_env* env, ani_object obj, ani_int alpha,
        ani_int red, ani_int green, ani_int blue);
    static void SetDither(ani_env* env, ani_object obj, ani_boolean dither);
    static void SetAntiAlias(ani_env* env, ani_object obj, ani_boolean antiAlias);
    static void SetStrokeWidth(ani_env* env, ani_object obj, ani_double width);
    static void SetCapStyle(ani_env* env, ani_object obj, ani_enum_item aniCapstyle);
    static ani_object GetColorFilter(ani_env* env, ani_object obj);
    static ani_boolean GetFillPath(ani_env* env, ani_object obj, ani_object src, ani_object dst);
    static void SetMiterLimit(ani_env* env, ani_object obj, ani_double miter);
    static ani_enum_item GetCapStyle(ani_env* env, ani_object obj);
    static void SetMaskFilter(ani_env* env, ani_object obj, ani_object aniMaskFilterObj);
    static ani_boolean IsAntiAlias(ani_env* env, ani_object obj);
    static ani_object GetColor(ani_env* env, ani_object obj);
    static void SetJoinStyle(ani_env* env, ani_object obj, ani_enum_item joinStyle);
    static ani_enum_item GetJoinStyle(ani_env* env, ani_object obj);
    static ani_double GetWidth(ani_env* env, ani_object obj);
    static ani_double GetMiterLimit(ani_env* env, ani_object obj);
    static void SetPathEffect(ani_env* env, ani_object obj, ani_object aniPathEffectObj);
    static void SetImageFilter(ani_env* env, ani_object obj, ani_object aniImageFilterObj);
    static void SetShadowLayer(ani_env* env, ani_object obj, ani_object aniShadowLayerObj);
    static void SetShaderEffect(ani_env* env, ani_object obj, ani_object aniShaderEffectObj);
    static void SetColor4f(ani_env* env, ani_object obj, ani_object aniColor4f, ani_object aniColorSpace);
    static ani_object GetColor4f(ani_env* env, ani_object obj);

    std::shared_ptr<Pen> GetPen();

private:
    static ani_object PenTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetPenAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Pen>* GetPenPtrAddr();
    std::shared_ptr<Pen> pen_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PEN_H