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
 
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COMPLEX_SHADER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COMPLEX_SHADER_H
  
#include <memory>
#include <string>
#include <vector>
  
#include "common/rs_macros.h"
#include "animation/rs_interpolator.h"
#include "effect/runtime_effect.h"
#include "ext/gex_complex_shader.h"
#include "platform/common/rs_log.h"
#include "rs_shader.h"
  
namespace OHOS {
namespace Rosen {
  
class RSB_EXPORT RSComplexShader : public RSShader {
public:
    RSComplexShader();
    RSComplexShader(GexComplexShaderType type);
    ~RSComplexShader() = default;

    void MakeDrawingShader(const RectF& rect, std::vector<float> params) override;
    const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() const override;
  
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& needReset) override;
  
private:
    std::shared_ptr<Drawing::ShaderEffect> GetShaderEffect(
        const std::vector<float> &effectParam, const Drawing::RectF &rect);
    GexComplexShaderType GetShaderType() const;
    GexComplexShaderType shaderType_ = GexComplexShaderType::NONE;
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect_;
};
}  // namespace Rosen
}  // namespace OHOS
  
#endif  // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_COMPLEX_SHADER_H
