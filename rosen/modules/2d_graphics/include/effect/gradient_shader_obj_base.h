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

#ifndef GRADIENT_SHADER_OBJ_BASE_H
#define GRADIENT_SHADER_OBJ_BASE_H

#include "draw/ui_color.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_obj.h"

#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

class GradientShaderObjBase : public ShaderEffectObj {
public:
    virtual ~GradientShaderObjBase() = default;

protected:
    GradientShaderObjBase(int32_t subType);
    GradientShaderObjBase(int32_t subType, const std::vector<UIColor>& colors,
        std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
        TileMode mode, const Matrix* matrix);

#ifdef ROSEN_OHOS
    // Helper methods for marshalling common gradient data
    bool MarshalCommonData(Parcel& parcel) const;
    bool UnmarshalCommonData(Parcel& parcel);
#endif

    // Common gradient data
    std::vector<UIColor> colors_;
    std::shared_ptr<ColorSpace> colorSpace_;
    std::vector<scalar> pos_;
    TileMode mode_;
    std::shared_ptr<Matrix> matrix_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRADIENT_SHADER_OBJ_BASE_H
