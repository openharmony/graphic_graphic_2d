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

#ifndef SAFUZZ_RANDOM_RS_NG_SHADER_H
#define SAFUZZ_RANDOM_RS_NG_SHADER_H

#include "effect/rs_render_shader_base.h"

namespace OHOS {
namespace Rosen {
class RandomRSNGShaderPtr {
public:
    static std::shared_ptr<RSNGRenderShaderBase> GetRandomValue();

private:
    static std::shared_ptr<RSNGRenderShaderBase> GetRandomSingleShader();
    static std::shared_ptr<RSNGRenderShaderBase> GetRandomShaderChain();
    static std::shared_ptr<RSNGRenderShaderBase> GetNullValue();

#define DECLARE_SHADER(ShaderName, ShaderType, ...) \
    static std::shared_ptr<RSNGRenderShaderBase> GetRandom##ShaderName()

#include "effect/rs_render_shader_def.in"

#undef DECLARE_SHADER
};

} // namespace Rosen
} // namespace OHOS
#endif // SAFUZZ_RANDOM_RS_NG_SHADER_H
