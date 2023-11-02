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

#ifndef WEBGL_SYNC_H
#define WEBGL_SYNC_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLSync final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLSync";
    inline static const int objectType = WEBGL_OBJECT_SYNC;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLSync **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLSync>(env, instance);
    }

    void SetSync(int64_t sync)
    {
        sync_ = sync;
    }

    int64_t GetSync() const
    {
        return sync_;
    }

    explicit WebGLSync() : sync_(0) {};
    WebGLSync(napi_env env, napi_value exports) : NExporter(env, exports), sync_(0) {};
    ~WebGLSync() {};
private:
    int64_t sync_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_SYNC_H
