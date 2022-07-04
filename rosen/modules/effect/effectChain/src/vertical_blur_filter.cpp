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

#include "vertical_blur_filter.h"

namespace OHOS {
namespace Rosen {
VerticalBlurFilter::VerticalBlurFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

void VerticalBlurFilter::SetValue(const std::string& key, std::shared_ptr<void> value, int size)
{
    if (key == "offset" && size > 0) {
        std::shared_ptr<float> offset = std::static_pointer_cast<float>(value);
        for (int i = 0; i < size; i++) {
            offset_[i] = *(offset.get() + i);
        }
    }
    if (key == "weight" && size > 0) {
        std::shared_ptr<float> weight = std::static_pointer_cast<float>(value);
        for (int i = 0; i < size; i++) {
            weight_[i] = *(weight.get() + i);
        }
    }
}

void VerticalBlurFilter::LoadFilterParams()
{
    Use();
    weightID_ = glGetUniformLocation(program_->programID_, "weight");
    offsetID_ = glGetUniformLocation(program_->programID_, "offset");
    glUniform1fv(weightID_, RADIUS, weight_);
    glUniform1fv(offsetID_, RADIUS, offset_);
}

std::string VerticalBlurFilter::GetVertexShader()
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

std::string VerticalBlurFilter::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        precision mediump int;

        const int RADIUS = 3;
        in vec2 texCoord;
        out vec4 fragColor;
        
        uniform sampler2D uTexture;
        uniform float weight[3];
        uniform float offset[3];

        void main()
        {
            float tex_offset = 1.0 / float(textureSize(uTexture, 0).y); // gets size of single texel
            vec3 result = texture(uTexture, texCoord).rgb * weight[0]; // current fragment's contribution

            for (int i = 1; i < RADIUS; ++i)
            {
                result += texture(uTexture, texCoord + vec2(0.0, tex_offset) * offset[i]).rgb * weight[i];
                result += texture(uTexture, texCoord - vec2(0.0, tex_offset) * offset[i]).rgb * weight[i];
            }

            fragColor = vec4(result, 1.0);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS