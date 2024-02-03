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

#ifndef RUNTIME_SHADER_BUILDER_H
#define RUNTIME_SHADER_BUILDER_H

#include "drawing/engine_adapter/impl_interface/runtime_shader_builder_impl.h"
#include "utils/drawing_macros.h"
#include "utils/matrix44.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffect;
class DRAWING_API RuntimeShaderBuilder {
public:
    RuntimeShaderBuilder(std::shared_ptr<RuntimeEffect> runtimeEffect) noexcept;
    virtual ~RuntimeShaderBuilder() = default;

    std::shared_ptr<ShaderEffect> MakeShader(const Matrix* localMatrix, bool isOpaque);
    std::shared_ptr<Image> MakeImage(GPUContext* gpuContext,
        const Matrix* localMatrix, ImageInfo resultInfo, bool mipmapped);

    void SetChild(const std::string& name, std::shared_ptr<ShaderEffect> shader);
    void SetUniform(const std::string& name, float val);
    void SetUniform(const std::string& name, float x, float y);
    void SetUniform(const std::string& name, float x, float y, float z);
    void SetUniform(const std::string& name, float x, float y, float width, float height);
    void SetUniform(const std::string& name, const float values[], size_t size);
    void SetUniform(const std::string& name, const Matrix& uniformMatrix33);
    void SetUniform(const std::string& name, const Matrix44& uniformMatrix44);
    void SetUniformVec4(const std::string& name, float x, float y, float z, float w);
private:
    std::shared_ptr<RuntimeShaderBuilderImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // RUNTIMR_SHADER_BUILDER_H