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

#ifndef ROSENRENDER_ROSEN_WEBGL_VERTEX_ARRAY_OBJECT
#define ROSENRENDER_ROSEN_WEBGL_VERTEX_ARRAY_OBJECT

#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGLVertexArrayObject final : public NExporter {
public:
    inline static const std::string className = "WebGLVertexArrayObject";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetVertexArrays(unsigned int vertexArrays)
    {
        m_vertexArrays = vertexArrays;
    }

    unsigned int GetVertexArrays() const
    {
        return m_vertexArrays;
    }

    explicit WebGLVertexArrayObject() : m_vertexArrays(0) {};

    WebGLVertexArrayObject(napi_env env, napi_value exports) : NExporter(env, exports), m_vertexArrays(0) {};

    ~WebGLVertexArrayObject() {};
private:
    unsigned int m_vertexArrays;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_VERTEX_ARRAY_OBJECT