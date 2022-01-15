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

#ifndef ROSENRENDER_ROSEN_WEBGL_QUERY
#define ROSENRENDER_ROSEN_WEBGL_QUERY

#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGLQuery final : public NExporter {
public:
    inline static const std::string className = "WebGLQuery";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetQuery(unsigned int query)
    {
        m_query = query;
    }

    unsigned int GetQuery() const
    {
        return m_query;
    }

    explicit WebGLQuery() : m_query(0) {};

    WebGLQuery(napi_env env, napi_value exports) : NExporter(env, exports), m_query(0) {};

    ~WebGLQuery() {};
private:
    unsigned int m_query;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_QUERY
