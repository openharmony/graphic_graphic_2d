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

#include "brightness_filter.h"

namespace OHOS {
namespace Rosen {
BrightnessFilter::BrightnessFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

void BrightnessFilter::SetValue(const std::string& key, void* value, int size)
{
    if (key == "brightness" && size > 0) {
        float* brightness = (float*)value;
        brightness_ = *(brightness);
    }
    LOGD("The brightness is %{public}f.", brightness_);
}

void BrightnessFilter::LoadFilterParams()
{
    Use();
    brightnessID_ = glGetUniformLocation(program_->programID_, "brightness");
    glUniform1f(brightnessID_, brightness_);
}

std::string BrightnessFilter::GetVertexShader()
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

std::string BrightnessFilter::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float brightness;
        void main()
        {
            vec4 textureColor = texture(uTexture, texCoord);
            fragColor = vec4((textureColor.rgb + brightness), textureColor.a);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS