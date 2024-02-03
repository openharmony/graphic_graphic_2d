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

#ifndef RUNTIME_EFFECT_IMPL_H
#define RUNTIME_EFFECT_IMPL_H

#include <memory>

#include "base_impl.h"

#include "effect/blender.h"
#include "effect/shader_effect.h"
#include "utils/data.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffectOptions;
class RuntimeEffectImpl : public BaseImpl {
public:
    RuntimeEffectImpl() noexcept {}
    ~RuntimeEffectImpl() override {}

    virtual void InitForShader(const std::string& sl, const RuntimeEffectOptions& options) = 0;
    virtual void InitForShader(const std::string& sl) = 0;
    virtual void InitForES3Shader(const std::string& sl) = 0;
    virtual void InitForBlender(const std::string& sl) = 0;
    virtual std::shared_ptr<ShaderEffect> MakeShader(std::shared_ptr<Data> uniforms,
        std::shared_ptr<ShaderEffect> children[], size_t childCount, const Matrix* localMatrix,
        bool isOpaque) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif