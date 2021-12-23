/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rotation_animation.h"

#include <chrono>

#include <GLES2/gl2.h>
#include <gslogger.h>

namespace OHOS {
namespace {
DEFINE_HILOG_LABEL("RotationAnimation");
} // namespace

GSError RotationAnimation::Init(struct RotationAnimationParam &param)
{
    auto vertexShader = "attribute vec4 pos;\n"
                        "attribute vec2 texCoord;\n"
                        "varying vec2 v_texCoord;\n"
                        "uniform mat4 transform;\n"
                        "void main()\n{\n"
                        "   gl_Position = transform * pos;\n"
                        "   v_texCoord = texCoord;\n"
                        "}\n";
    auto fragmentShader = "precision mediump float;\n"
                          "varying vec2 v_texCoord;\n"
                          "uniform sampler2D texture;\n"
                          "uniform float alpha;\n"
                          "void main()\n{\n"
                          "   gl_FragColor = texture2D(texture, v_texCoord);\n"
                          "   gl_FragColor.a = alpha;\n"
                          "}\n";

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ZERO);

    param_ = param;

    shader_ = std::make_unique<Shader>(vertexShader, fragmentShader);
    texture_ = std::make_unique<Texture>(param_.data->ptr.get(), param_.width, param_.height);

    shader_->Bind();

    const GLfloat verts[][0x2] = { {-1.0, -1.0}, {-1.0, 1.0}, {1.0, -1.0}, {1.0, 1.0} };
    auto positionLoc = shader_->GetAttribLocation("pos");
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 0x2, GL_FLOAT, GL_FALSE, 0, verts);

    const GLfloat texCoords[][0x2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    auto texCoordLoc = shader_->GetAttribLocation("texCoord");
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 0x2, GL_FLOAT, GL_FALSE, 0, texCoords);
    return GSERROR_OK;
}

bool RotationAnimation::Draw()
{
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    if (now - param_.startTime > param_.duration) {
        return false;
    }

    glViewport(0, 0, param_.width, param_.height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    double complete = static_cast<double>(now - param_.startTime) / param_.duration;
    GSLOG2HI(DEBUG) << "complete: " << complete;

    auto matrix = Matrix<GLfloat>::RotateMatrixZ(complete * param_.degree);
    double alpha = 1.0 - complete;

    shader_->Bind();
    shader_->SetUniform1f("alpha", alpha);
    texture_->Bind(0);
    shader_->SetUniform1i("texture", 0);
    shader_->SetUniformMat4f("transform", matrix);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 0x4);
    return true;
}
} // namespace OHOS
