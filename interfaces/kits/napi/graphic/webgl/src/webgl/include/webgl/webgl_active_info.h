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

#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGLActiveInfo final : public NExporter {
public:
    inline static const std::string className = "WebGLActiveInfo";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetActiveName(char activename)
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

    char GetActiveName() const
    {
        return m_name;
    }

    int GetActiveSize() const
    {
        return m_size;
    }

    int GetActiveType() const
    {
        return m_type;
    }

    explicit WebGLActiveInfo() : m_name(0), m_size(0), m_type(0) {};

    WebGLActiveInfo(napi_env env, napi_value exports) : NExporter(env, exports), m_name(0), m_size(0), m_type(0) {};

    ~WebGLActiveInfo() {};
private:
    char m_name;
    int m_size;
    int m_type;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_PROGRAM
