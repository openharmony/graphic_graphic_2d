/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "contrast_filter.h"

namespace OHOS {
namespace Rosen {
ContrastFilter::ContrastFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

void ContrastFilter::SetValue(const std::string& key, void* value, int size)
{
    if (key == "contrast" && size > 0) {
        float* contrast = (float*)value;
        contrast_ = *(contrast);
    }
    LOGD("The contrast is %{public}f.", contrast_);
}

void ContrastFilter::LoadFilterParams()
{
    Use();
    contrastID_ = glGetUniformLocation(program_->programID_, "contrast");
    glUniform1f(contrastID_, contrast_);
}

std::string ContrastFilter::GetVertexShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;

        layout (location = 0) in vec3 vertexCoord;
        layout (location = 1) in vec2 inputTexCoord;
        out vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0);
            texCoord = inputTexCoord;
        }
    )SHADER";
}

std::string ContrastFilter::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float contrast;
        void main()
        {
            vec4 textureColor = texture(uTexture, texCoord);
            fragColor = vec4(((textureColor.rgb - 0.5) * contrast + 0.5), textureColor.a);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS