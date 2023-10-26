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

#ifndef WEBGL_QUERY_H
#define WEBGL_QUERY_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLQuery final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLQuery";
    inline static const int objectType = WEBGL_OBJECT_QUERY;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    explicit WebGLQuery() : query_(0) {};
    WebGLQuery(napi_env env, napi_value exports) : NExporter(env, exports), query_(0) {};
    ~WebGLQuery() {};

    static NVal CreateObjectInstance(napi_env env, WebGLQuery** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLQuery>(env, instance);
    }

    void SetQuery(unsigned int query)
    {
        query_ = query;
    }

    unsigned int GetQuery() const
    {
        return query_;
    }

    void SetTarget(GLenum target)
    {
        target_ = target;
    }

    GLenum GetTarget() const
    {
        return target_;
    }
private:
    unsigned int query_;
    GLenum target_ { 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_QUERY_H
