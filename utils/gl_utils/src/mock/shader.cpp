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

#include "shader.h"

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
}

Shader::~Shader()
{
}

bool Shader::Available()
{
    return false;
}

void Shader::Bind() const
{
}

void Shader::Unbind() const
{
}

void Shader::SetUniform1i(const std::string& name, int32_t v)
{
}

void Shader::SetUniform1f(const std::string& name, float v)
{
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
}

void Shader::SetUniformMat4f(const std::string& name, const Matrix<float>& matrix)
{
}

int32_t Shader::GetAttribLocation(const std::string& name)
{
    return -1;
}

int32_t Shader::GetUniformLocation(const std::string& name)
{
    return -1;
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
    return -1;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    return -1;
}
