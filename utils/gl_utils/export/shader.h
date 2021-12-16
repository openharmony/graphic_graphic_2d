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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_SHADER_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_SHADER_H

#include <cstdint>
#include <unordered_map>
#include <string>

#include <matrix.h>

class Shader {
public:
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    ~Shader();

    bool Available();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int32_t v);
    void SetUniform1f(const std::string& name, float v);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const Matrix<float>& matrix);

    int32_t GetAttribLocation(const std::string& name);

private:
    int32_t GetUniformLocation(const std::string& name);
    uint32_t CompileShader(uint32_t type, const std::string& source);
    uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    uint32_t rendererID_ = 0;
    std::unordered_map<std::string, int32_t> uniformLocationCache_;
};

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_SHADER_H
