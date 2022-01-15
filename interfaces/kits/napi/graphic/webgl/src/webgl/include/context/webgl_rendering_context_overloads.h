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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_OVERLOADS
#define ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_OVERLOADS

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "../../../common/napi/n_exporter.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class WebGLRenderingContextOverloads {
public:
    static napi_value BufferData(napi_env env, napi_callback_info info);

    static napi_value BufferSubData(napi_env env, napi_callback_info info);

    static napi_value CompressedTexImage2D(napi_env env, napi_callback_info info);

    static napi_value CompressedTexSubImage2D(napi_env env, napi_callback_info info);

    static napi_value ReadPixels(napi_env env, napi_callback_info info);

    static napi_value TexImage2D(napi_env env, napi_callback_info info);

    static napi_value TexSubImage2D(napi_env env, napi_callback_info info);

    static napi_value Uniform1fv(napi_env env, napi_callback_info info);

    static napi_value Uniform2fv(napi_env env, napi_callback_info info);

    static napi_value Uniform3fv(napi_env env, napi_callback_info info);

    static napi_value Uniform4fv(napi_env env, napi_callback_info info);

    static napi_value Uniform1iv(napi_env env, napi_callback_info info);

    static napi_value Uniform2iv(napi_env env, napi_callback_info info);

    static napi_value Uniform3iv(napi_env env, napi_callback_info info);

    static napi_value Uniform4iv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix2fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix3fv(napi_env env, napi_callback_info info);

    static napi_value UniformMatrix4fv(napi_env env, napi_callback_info info);
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_OVERLOADS
