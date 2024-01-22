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

#include <memory>
#include <vector>

#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "context/webgl2_rendering_context.h"
#include "napi/n_val.h"
#include "webgl/webgl_active_info.h"
#include "webgl/webgl_buffer.h"
#include "webgl/webgl_framebuffer.h"
#include "webgl/webgl_program.h"
#include "webgl/webgl_query.h"
#include "webgl/webgl_renderbuffer.h"
#include "webgl/webgl_sampler.h"
#include "webgl/webgl_sync.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_texture.h"
#include "webgl/webgl_transform_feedback.h"
#include "webgl/webgl_uniform_location.h"
#include "webgl/webgl_vertex_array_object.h"
#include "webgl/webgl_shader_precision_format.h"
#include "util/log.h"
#include "util/object_manager.h"
#include "util/util.h"

using namespace std;

namespace OHOS {
namespace Rosen {
static napi_value ExportWebGlObj(napi_env env, napi_value exports)
{
    std::vector<unique_ptr<NExporter>> products;
    products.emplace_back(make_unique<WebGLActiveInfo>(env, exports));
    products.emplace_back(make_unique<WebGLBuffer>(env, exports));
    products.emplace_back(make_unique<WebGLFramebuffer>(env, exports));
    products.emplace_back(make_unique<WebGLProgram>(env, exports));
    products.emplace_back(make_unique<WebGLQuery>(env, exports));
    products.emplace_back(make_unique<WebGLRenderbuffer>(env, exports));
    products.emplace_back(make_unique<WebGLSampler>(env, exports));
    products.emplace_back(make_unique<WebGLSync>(env, exports));
    products.emplace_back(make_unique<WebGLShader>(env, exports));
    products.emplace_back(make_unique<WebGLTexture>(env, exports));
    products.emplace_back(make_unique<WebGLTransformFeedback>(env, exports));
    products.emplace_back(make_unique<WebGLUniformLocation>(env, exports));
    products.emplace_back(make_unique<WebGLVertexArrayObject>(env, exports));
    products.emplace_back(make_unique<WebGLShaderPrecisionFormat>(env, exports));

    for (auto &&product : products) {
        if (!product->Export(env, exports)) {
            return nullptr;
        }
    }
    return exports;
}

static napi_value Export(napi_env env, napi_value exports)
{
    string idStr;
    std::vector<std::string> vec;
    bool succ = Util::GetContextInfo(env, exports, idStr, vec);
    if (!succ) {
        return nullptr;
    }
    size_t webglItem = vec[0].find("webgl");
    string webgl2Str = vec[0].substr(webglItem, 6); // length of webgl2
    string webgl1Str = vec[0].substr(webglItem, 5); // length of webgl
    if (webgl2Str == "webgl2") {
        WebGL2RenderingContext *webGl2RenderingContext =
            static_cast<WebGL2RenderingContext *>(ObjectManager::GetInstance().GetWebGLContext(true, idStr));
        if (webGl2RenderingContext == nullptr) {
            webGl2RenderingContext = new WebGL2RenderingContext(env, exports);
            if (webGl2RenderingContext == nullptr) {
                return nullptr;
            }
            ObjectManager::GetInstance().AddWebGLObject(true, idStr, webGl2RenderingContext);
        }

        webGl2RenderingContext->SetWebGLContextAttributes(vec);
        webGl2RenderingContext->CreateSurface();
        if (!webGl2RenderingContext->Export(env, exports)) {
            return nullptr;
        }
    } else if (webgl1Str == "webgl") {
        WebGLRenderingContext *webGlRenderingContext =
            static_cast<WebGLRenderingContext *>(ObjectManager::GetInstance().GetWebGLContext(false, idStr));
        if (webGlRenderingContext == nullptr) {
            webGlRenderingContext = new WebGLRenderingContext(env, exports);
            if (webGlRenderingContext == nullptr) {
                return nullptr;
            }
            ObjectManager::GetInstance().AddWebGLObject(false, idStr, webGlRenderingContext);
        }
        webGlRenderingContext->SetWebGLContextAttributes(vec);
        webGlRenderingContext->CreateSurface();
        if (!webGlRenderingContext->Export(env, exports)) {
            return nullptr;
        }
    } else {
        return nullptr;
    }

    return ExportWebGlObj(env, exports);
}

NAPI_MODULE(libwebglnapi, Export)
} // namespace Rosen
} // namespace OHOS
