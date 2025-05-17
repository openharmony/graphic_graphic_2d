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
 
#include "render/rs_complex_shader.h"
#include "common/rs_common_def.h"
#include "effect/runtime_shader_builder.h"
#include "ge_visual_effect_impl.h"
#include "platform/common/rs_log.h"
#include "ge_external_dynamic_loader.h"
#include "ext/gex_complex_shader.h"
  
namespace OHOS {
namespace Rosen {
  
RSComplexShader::RSComplexShader()
{
    type_ = ShaderType::COMPLEX;
}
  
RSComplexShader::RSComplexShader(GexComplexShaderType type)
    : shaderType_{type}
{
    type_ = ShaderType::COMPLEX;
}
  
void RSComplexShader::MakeDrawingShader(const RectF &rect, std::vector<float> params)
{
    shaderEffect_ =
        GetShaderEffect(params, Drawing::Rect{rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight()});
}
  
const std::shared_ptr<Drawing::ShaderEffect> &RSComplexShader::GetDrawingShader() const
{
    return shaderEffect_;
}
  
bool RSComplexShader::Marshalling(Parcel &parcel)
{
    return parcel.WriteUint32(static_cast<uint32_t>(shaderType_));
}
  
bool RSComplexShader::Unmarshalling(Parcel &parcel, bool &needReset)
{
    auto type = parcel.ReadUint32();
    if (type < static_cast<uint32_t>(GexComplexShaderType::MAX)) {
        shaderType_ = static_cast<GexComplexShaderType>(type);
        return true;
    }
    return false;
}
  
std::shared_ptr<Drawing::ShaderEffect> RSComplexShader::GetShaderEffect(
    const std::vector<float> &effectParam, const Drawing::RectF &rect)
{
    auto shader = GEXComplexShader::CreateDynamicImpl(GEXComplexShaderParams{shaderType_, effectParam});
    if (shader == nullptr) {
        return nullptr;
    }
    shader->MakeDrawingShader(rect, 0);
    return shader->GetDrawingShader();
}
  
GexComplexShaderType RSComplexShader::GetShaderType() const
{
    return shaderType_;
}
  
}  // namespace Rosen
}  // namespace OHOS
