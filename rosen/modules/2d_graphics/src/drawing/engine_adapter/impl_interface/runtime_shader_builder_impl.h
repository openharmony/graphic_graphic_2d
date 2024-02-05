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

#ifndef RUNTIME_SHADER_BUILDER_IMPL_H
#define RUNTIME_SHADER_BUILDER_IMPL_H

#include "base_impl.h"

#include "utils/matrix.h"
#include "utils/matrix44.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class RuntimeEffect;
class Image;
class ShaderEffect;
class GPUContext;
class ImageInfo;
class RuntimeShaderBuilderImpl : public BaseImpl {
public:
    RuntimeShaderBuilderImpl() {}
    ~RuntimeShaderBuilderImpl() override {}

    virtual std::shared_ptr<ShaderEffect> MakeShader(const Matrix* localMatrix, bool isOpaque) = 0;
    virtual std::shared_ptr<Image> MakeImage(GPUContext* gpuContext,
        const Matrix* localMatrix, ImageInfo resultInfo, bool mipmapped) = 0;

    virtual void SetChild(const std::string& name, std::shared_ptr<ShaderEffect> shader) = 0;
    virtual void SetUniform(const std::string& name, float val) = 0;
    virtual void SetUniform(const std::string& name, float x, float y) = 0;
    virtual void SetUniform(const std::string& name, float x, float y, float z) = 0;
    virtual void SetUniform(const std::string& name, float x, float y, float width, float height) = 0;
    virtual void SetUniform(const std::string& name, const float values[], size_t size) = 0;
    virtual void SetUniform(const std::string& name, const Matrix& uniformMatrix33) = 0;
    virtual void SetUniform(const std::string& name, const Matrix44& uniformMatrix44) = 0;
    virtual void SetUniformVec4(const std::string& name, float x, float y, float z, float w) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif