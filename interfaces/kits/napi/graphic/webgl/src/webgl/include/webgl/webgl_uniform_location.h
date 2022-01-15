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

#ifndef ROSENRENDER_ROSEN_WEBGL_UNIFORM_LOCATION
#define ROSENRENDER_ROSEN_WEBGL_UNIFORM_LOCATION

#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGLUniformLocation final : public NExporter {
public:
    inline static const std::string className = "WebGLUniformLocation";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetUniformLocationId(int location)
    {
        m_location = location;
    }

    int GetUniformLocationId() const
    {
        return m_location;
    }

    explicit WebGLUniformLocation() : m_location(0) {};

    WebGLUniformLocation(napi_env env, napi_value exports) : NExporter(env, exports), m_location(0) {};

    ~WebGLUniformLocation() {};
private:
    int m_location;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_UNIFORM_LOCATION