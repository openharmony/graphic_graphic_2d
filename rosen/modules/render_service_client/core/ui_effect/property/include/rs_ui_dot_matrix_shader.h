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
#ifndef RENDER_SERVICE_BASE_CORE_RENDER_RS_DOT_MATRIX_SHADER_H
#define RENDER_SERVICE_BASE_CORE_RENDER_RS_DOT_MATRIX_SHADER_H

#include "render/rs_shader.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
class RSNGShaderBase;

class RSB_EXPORT RSNGDotMatrixShaderAdapter {
public:
    RSNGDotMatrixShaderAdapter();
    ~RSNGDotMatrixShaderAdapter();

    void SetNormalParams(Rosen::Drawing::Color dotColor, float dotSpacing, float dotRadius);
    void SetRippleEffect(std::vector<Rosen::Drawing::Color> effectColors, Vector2f colorFractions,
        std::vector<Rosen::Drawing::Point> startPoints, float pathWidth, bool inverseEffect);
    void SetRotateEffect(int pathDirection, std::vector<Rosen::Drawing::Color> effectColors);
    void SetProgress(float progress);
    std::shared_ptr<RSAnimatableProperty<float>> GetProgress();
    std::shared_ptr<Rosen::RSNGShaderBase> GetNGShaderInstance();

private:
    std::shared_ptr<Rosen::RSNGShaderBase> shader_;
};
} // namespace Rosen
} // namespace OHOS

#endif
