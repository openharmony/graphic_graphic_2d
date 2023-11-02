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

#ifndef WEBGL_SHADER_H
#define WEBGL_SHADER_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLShader final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLShader";
    inline static const int objectType = WEBGL_OBJECT_SHADER;
    inline static const int DEFAULT_SHADER_ID = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLShader** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLShader>(env, instance);
    }

    void SetShaderId(uint32_t shaderId)
    {
        shaderId_ = shaderId;
    }

    uint32_t GetShaderId() const
    {
        return shaderId_;
    }

    void SetShaderRes(const std::string& res)
    {
        res_ = std::move(res);
    }

    const std::string& GetShaderRes() const
    {
        return res_;
    }

    explicit WebGLShader() : shaderId_(0), type_(0) {};

    WebGLShader(napi_env env, napi_value exports) : NExporter(env, exports), shaderId_(0), type_(0) {};

    ~WebGLShader() {};

    static WebGLShader* GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLShader>(env, obj);
    }

    void SetShaderType(GLenum type)
    {
        type_ = type;
    }

    GLenum GetShaderType() const
    {
        return type_;
    }
private:
    std::string res_ = {};
    uint32_t shaderId_ { 0 };
    GLenum type_ { 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_SHADER_H
