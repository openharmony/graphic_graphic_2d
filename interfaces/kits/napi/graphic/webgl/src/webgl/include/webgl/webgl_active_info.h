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

#ifndef ROSENRENDER_ROSEN_WEBGL_ACTIVEINFO
#define ROSENRENDER_ROSEN_WEBGL_ACTIVEINFO

#include <GLES2/gl2.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

#include "../../../common/napi/n_exporter.h"

#define WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH 128

namespace OHOS {
namespace Rosen {
class WebGLActiveInfo final : public NExporter {
public:
    inline static const std::string className = "WebGLActiveInfo";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetActiveName(GLchar* activename)
    {
        m_name = activename;
    }

    void SetActiveSize(int activesize)
    {
        m_size = activesize;
    }

    void SetActiveType(int activetype)
    {
        m_type = activetype;
    }

    explicit WebGLActiveInfo() : m_size(0), m_type(0) {};

    WebGLActiveInfo(napi_env env, napi_value exports) : NExporter(env, exports), m_size(0), m_type(0) {};

    ~WebGLActiveInfo() {};
private:
    std::string m_name;
    int m_size;
    int m_type;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_PROGRAM
