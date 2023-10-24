/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_PROGRAM_H
#define WEBGL_PROGRAM_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {

class WebGLProgram final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLProgram";
    inline static const int objectType = WEBGL_OBJECT_PROGRAM;
    inline static const int DEFAULT_PROGRAM_ID = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLProgram** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLProgram>(env, instance);
    }

    static WebGLProgram* GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLProgram>(env, obj);
    }

    void SetProgramId(uint32_t programId)
    {
        programId_ = programId;
    }

    uint32_t GetProgramId() const
    {
        return programId_;
    }

    explicit WebGLProgram() : programId_(0) {};
    WebGLProgram(napi_env env, napi_value exports);
    ~WebGLProgram();
    bool AttachShader(uint32_t index, uint32_t shaderId);
    bool DetachShader(uint32_t index, uint32_t shaderId);
private:
    GLuint attachedShader_[2] = { 0 }; // 2 is default attachShader type : Vertex shader and Fragment shader.
    uint32_t programId_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_PROGRAM_H
