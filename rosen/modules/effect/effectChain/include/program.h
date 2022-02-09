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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <GLES3/gl32.h>

namespace OHOS {
namespace Rosen {
class Program {
public:
    void Compile(const std::string& vertexShader, const std::string& fragmentShader);
    void UseProgram() const;
    unsigned int programID_;

private:
    static constexpr int INFO_LOG_LENGTH = 512;
    bool initFlag_ = false;
    unsigned int vertexID_;
    unsigned int fragmentID_;
    GLuint CreateShader(GLuint type, const std::string& shaderCode);
    void CheckShaderCompileErrors(GLuint shader, const std::string& type);
    void CheckProgramLinkErrors(GLuint program);
};
} // namespace Rosen
} // namespace OHOS
#endif // PROGRAM_H

