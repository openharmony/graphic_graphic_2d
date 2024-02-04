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

#ifndef WEBGL_ACTIVEINFO_H
#define WEBGL_ACTIVEINFO_H

#include <GLES2/gl2.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>

#include "napi/n_exporter.h"
#include "webgl_object.h"

#define WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH 128

namespace OHOS {
namespace Rosen {
class WebGLActiveInfo final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLActiveInfo";
    inline static const int objectType = WEBGL_OBJECT_ACTIVE_INFO;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static WebGLActiveInfo *GetWebGLActiveInfo(napi_env env, napi_callback_info info);
    static napi_value GetActiveName(napi_env env, napi_callback_info info);
    static napi_value GetActiveSize(napi_env env, napi_callback_info info);
    static napi_value GetActiveType(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLActiveInfo **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLActiveInfo>(env, instance);
    }

    void SetActiveName(const GLchar* activename, uint32_t activenameLength)
    {
        if (activenameLength > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
            return;
        }
        name_ = activename;
    }

    void SetActiveSize(int activesize)
    {
        size_ = activesize;
    }

    void SetActiveType(int activetype)
    {
        type_ = activetype;
    }

    std::string GetActiveName() const
    {
        return name_;
    }

    int GetActiveSize() const
    {
        return size_;
    }

    int GetActiveType() const
    {
        return type_;
    }

    explicit WebGLActiveInfo() : size_(0), type_(0) {};

    WebGLActiveInfo(napi_env env, napi_value exports) : NExporter(env, exports), size_(0), type_(0) {};
    ~WebGLActiveInfo() {};
private:
    std::string name_;
    int size_;
    int type_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_ACTIVEINFO_H
