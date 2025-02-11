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
#include "ge_log.h"
#include "ext/gex_dot_matrix_shader.h"
#include "ge_visual_effect_impl.h"
#include "ge_external_dynamic_loader.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<GEXDotMatrixShader> GEXDotMatrixShader::CreateDynamicImpl(DotMatrixNormalParams& param)
{
    auto type = static_cast<uint32_t>(Drawing::GEVisualEffectImpl::FilterType::DOT_MATRIX);
    auto impl = GEExternalDynamicLoader::GetInstance().CreateGEXObjectByType(
        type, sizeof(DotMatrixNormalParams), static_cast<void*>(&param));
    if (!impl) {
        GE_LOGE("GEXDotMatrixShader::CreateDynamicImpl create object failed.");
        return nullptr;
    }
    std::shared_ptr<GEXDotMatrixShader> dmShader(static_cast<GEXDotMatrixShader*>(impl));
    return dmShader;
}

const std::string GEXDotMatrixShader::GetDescription() const
{
    return "GEXDotMatrixShader";
}

} // namespace Rosen
} // namespace OHOS
