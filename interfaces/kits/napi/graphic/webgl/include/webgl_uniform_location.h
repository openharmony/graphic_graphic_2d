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

#ifndef WEBGL_UNIFORM_LOCATION_H
#define WEBGL_UNIFORM_LOCATION_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLUniformLocation final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLUniformLocation";
    inline static const int objectType = WEBGL_OBJECT_UNIFORM_LOCATION;
    inline static const int DEFAULT_ID = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLUniformLocation** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLUniformLocation>(env, instance);
    }

    void SetUniformLocationId(int location)
    {
        location_ = location;
    }

    int GetUniformLocationId() const
    {
        return location_;
    }

    explicit WebGLUniformLocation() : location_(0) {};
    WebGLUniformLocation(napi_env env, napi_value exports) : NExporter(env, exports), location_(0) {};
    ~WebGLUniformLocation() {};

    static WebGLUniformLocation* GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLUniformLocation>(env, obj);
    }

    const std::string &GetUniformLocationName() const
    {
        return name_;
    }

    void SetUniformLocationName(const std::string &name)
    {
        name_ = std::move(name);
    }
private:
    std::string name_ {};
    int location_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_UNIFORM_LOCATION_H
