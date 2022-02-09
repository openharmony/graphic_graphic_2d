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

#include "scale_filter.h"

namespace OHOS {
namespace Rosen {
ScaleFilter::ScaleFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

void ScaleFilter::SetValue(const std::string& key, void* value, int size)
{
    if (key == "scale" && size > 0) {
        float* scale = (float*)value;
        scale_ = *(scale);
    }
    LOGD("The scale is %{public}f.", scale_);
}

void ScaleFilter::SetScale(float scale)
{
    scale_ = scale;
}

void ScaleFilter::DoProcess(ProcessData& data)
{
    data.textureHeight = std::floorf(scale_ * data.textureHeight);
    data.textureWidth = std::floorf(scale_ * data.textureWidth);
    Prepare(data);
    Draw(data);
}

std::string ScaleFilter::GetVertexShader()
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

std::string ScaleFilter::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        void main()
        {
            fragColor = texture(uTexture, texCoord);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS