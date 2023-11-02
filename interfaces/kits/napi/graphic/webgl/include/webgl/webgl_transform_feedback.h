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

#ifndef WEBGL_TRANSFORM_FEEDBACK_H
#define WEBGL_TRANSFORM_FEEDBACK_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLTransformFeedback final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLTransformFeedback";
    inline static const int objectType = WEBGL_OBJECT_TRANSFORM_FEEDBACK;
    inline static const uint32_t DEFAULT_TRANSFORM_FEEDBACK = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLTransformFeedback** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLTransformFeedback>(env, instance);
    }

    void SetTransformFeedback(uint32_t transformFeedback)
    {
        transformFeedback_ = transformFeedback;
    }

    uint32_t GetTransformFeedback() const
    {
        return transformFeedback_;
    }

    GLenum GetTarget() const
    {
        return target_;
    }

    void SetTarget(GLenum target)
    {
        target_ = target;
    }

    explicit WebGLTransformFeedback() : transformFeedback_(0) {};
    WebGLTransformFeedback(napi_env env, napi_value exports) : NExporter(env, exports), transformFeedback_(0) {};
    ~WebGLTransformFeedback() {};
private:
    uint32_t transformFeedback_;
    GLenum target_ { 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_TRANSFORM_FEEDBACK_H
