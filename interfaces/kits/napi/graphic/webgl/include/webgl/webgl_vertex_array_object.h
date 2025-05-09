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

#ifndef WEBGL_VERTEX_ARRAY_OBJECT_H
#define WEBGL_VERTEX_ARRAY_OBJECT_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLVertexArrayObject final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLVertexArrayObject";
    inline static const int objectType = WEBGL_OBJECT_VERTEX_ARRAY;
    inline static const int DEFAULT_VERTEX_ARRAY_OBJECT = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLVertexArrayObject** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLVertexArrayObject>(env, instance);
    }

    void SetVertexArrays(uint32_t vertexArrays)
    {
        vertexArrays_ = vertexArrays;
    }

    uint32_t GetVertexArrays() const
    {
        return vertexArrays_;
    }

    explicit WebGLVertexArrayObject() : vertexArrays_(0) {};
    WebGLVertexArrayObject(napi_env env, napi_value exports) : NExporter(env, exports), vertexArrays_(0) {};
    ~WebGLVertexArrayObject() {};
private:
    uint32_t vertexArrays_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_VERTEX_ARRAY_OBJECT_H
