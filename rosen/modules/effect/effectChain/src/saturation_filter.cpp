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

#include "saturation_filter.h"

namespace OHOS {
namespace Rosen {
SaturationFilter::SaturationFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

void SaturationFilter::SetValue(const std::string& key, void* value, int size)
{
    if (key == "saturation" && size > 0) {
        float* saturation = (float*)value;
        saturation_ = *(saturation);
    }
    LOGD("The saturation is %{public}f.", saturation_);
}

void SaturationFilter::LoadFilterParams()
{
    Use();
    saturationID_ = glGetUniformLocation(program_->programID_, "saturation");
    glUniform1f(saturationID_, saturation_);
}

std::string SaturationFilter::GetVertexShader()
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

std::string SaturationFilter::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        uniform float saturation;
        const mediump vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);
        void main()
        {
            vec4 textureColor = texture(uTexture, texCoord);
            float luminance = dot(textureColor.rgb, luminanceWeighting);
            vec3 greyScaleColor = vec3(luminance);

            fragColor = vec4(mix(greyScaleColor, textureColor.rgb, saturation), textureColor.a);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS