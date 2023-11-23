/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_OBJECT_H
#define WEBGL_OBJECT_H

#include <map>
#include "napi/n_exporter.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "napi/n_val.h"
#include "util/log.h"

namespace OHOS {
namespace Rosen {
class WebGLObject {
public:
    WebGLObject() {};
    ~WebGLObject() {}

    enum {
        WEBGL_OBJECT_PROGRAM = 0,
        WEBGL_OBJECT_SHADER,
        WEBGL_OBJECT_BUFFER,
        WEBGL_OBJECT_FRAME_BUFFER,
        WEBGL_OBJECT_RENDER_BUFFER,
        WEBGL_OBJECT_TEXTURE,
        WEBGL_OBJECT_ACTIVE_INFO,
        WEBGL_OBJECT_QUERY,
        WEBGL_OBJECT_UNIFORM_LOCATION,
        WEBGL_OBJECT_VERTEX_ARRAY,
        WEBGL_OBJECT_SAMPLE,
        WEBGL_OBJECT_TRANSFORM_FEEDBACK,
        WEBGL_OBJECT_MAX,
        WEBGL_OBJECT_SYNC,
    };

    template<class T>
    static NVal CreateObjectInstance(napi_env env, T** instance)
    {
        napi_value obj = NClass::InstantiateClass(env, T::className, {});
        if (obj == nullptr) {
            return {env, nullptr};
        }
        *instance = NClass::GetEntityOf<T>(env, obj);
        if (*instance == nullptr) {
            return {env, nullptr};
        }
        return NVal(env, obj);
    }

    template<class T>
    static T* GetObjectInstance(napi_env env, napi_value obj)
    {
        if (NVal(env, obj).IsNull()) {
            return nullptr;
        }
        NExporter* webGLObj = nullptr;
        napi_status status = napi_unwrap(env, obj, (void **)&webGLObj);
        if (status != napi_ok) {
            return nullptr;
        }
        if (webGLObj->GetClassName() == T::className) {
            return static_cast<T *>(webGLObj);
        }
        LOGE("Invalid object input object %{public}s real %{public}s",
            webGLObj->GetClassName().c_str(), T::className.c_str());
        return nullptr;
    }
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_OBJECT_H
