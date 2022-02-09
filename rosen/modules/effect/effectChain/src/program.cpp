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

#include "ec_log.h"
#include "program.h"

namespace OHOS {
namespace Rosen {
void Program::Compile(const std::string& vertexShader, const std::string& fragmentShader)
{
    vertexID_ = CreateShader(GL_VERTEX_SHADER, vertexShader);
    fragmentID_ = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (!initFlag_) {
        initFlag_ = true;
        programID_ = glCreateProgram();
    }
    glAttachShader(programID_, vertexID_);
    glAttachShader(programID_, fragmentID_);
    glLinkProgram(programID_);
    CheckProgramLinkErrors(programID_);
    glDeleteShader(vertexID_);
    glDeleteShader(fragmentID_);
}

void Program::UseProgram() const
{
    glUseProgram(programID_);
}

GLuint Program::CreateShader(GLuint type, const std::string& shaderCode)
{
    const GLchar* charShaderCode = (const GLchar*)shaderCode.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &charShaderCode, nullptr);
    glCompileShader(shader);
    if (type == GL_VERTEX_SHADER) {
        CheckShaderCompileErrors(shader, "vertex");
    } else if (type == GL_FRAGMENT_SHADER) {
        CheckShaderCompileErrors(shader, "fragment");
    }
    return shader;
}

void Program::CheckShaderCompileErrors(GLuint shader, const std::string& type)
{
    GLint complete = 0;
    GLchar infoLog[INFO_LOG_LENGTH];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &complete);
    if (!complete) {
        glGetShaderInfoLog(shader, INFO_LOG_LENGTH, nullptr, infoLog);
        LOGE("Shader compile error of type: %{public}s, errorInfo: %{public}s", type.c_str(), infoLog);
    }
}

void Program::CheckProgramLinkErrors(GLuint program)
{
    GLint complete = 0;
    GLchar infoLog[INFO_LOG_LENGTH];
    glGetProgramiv(program, GL_LINK_STATUS, &complete);
    if (!complete) {
        glGetProgramInfoLog(program, INFO_LOG_LENGTH, nullptr, infoLog);
        LOGE("Program linking error: %{public}s", infoLog);
    }
}
} // namespcae Rosen
} // namespace OHOS