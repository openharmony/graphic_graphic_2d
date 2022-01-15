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

#include <memory>
#include <vector>

#include "common/napi/n_val.h"
#include "include/context/webgl_rendering_context.h"
#include "include/context/webgl2_rendering_context.h"
#include "include/webgl/webgl_active_info.h"
#include "include/webgl/webgl_buffer.h"
#include "include/webgl/webgl_framebuffer.h"
#include "include/webgl/webgl_program.h"
#include "include/webgl/webgl_query.h"
#include "include/webgl/webgl_renderbuffer.h"
#include "include/webgl/webgl_sampler.h"
#include "include/webgl/webgl_sync.h"
#include "include/webgl/webgl_shader.h"
#include "include/webgl/webgl_texture.h"
#include "include/webgl/webgl_transform_feedback.h"
#include "include/webgl/webgl_uniform_location.h"
#include "include/webgl/webgl_vertex_array_object.h"
#include "include/webgl/webgl_shader_precision_format.h"
#include "include/util/log.h"
#include "include/util/object_manager.h"
#include "include/util/egl_manager.h"
#include "include/util/util.h"
#include "include/context/webgl_context_attributes.h"

using namespace std;

namespace OHOS {
namespace Rosen {
static napi_value Export(napi_env env, napi_value exports)
{
    napi_status status;
    bool succ = false;
    napi_value id = nullptr;
    status = napi_get_named_property(env, exports, "id", &id);
    if (status != napi_ok) {
        return nullptr;
    }
    unique_ptr<char[]> idRev;
    tie(succ, idRev, ignore) = NVal(env, id).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    string idStr = idRev.get();

    napi_value param = nullptr;
    status = napi_get_named_property(env, exports, "param", &param);
    if (status != napi_ok) {
        return nullptr;
    }
    unique_ptr<char[]> strRev;
    tie(succ, strRev, ignore) = NVal(env, param).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    string str = strRev.get();

    vector<string> vec;
    Util::SplitString(str, vec, ",");
    if (vec.size() == 0) {
        return nullptr;
    }

    int webglItem = vec[0].find("webgl");
    string webgl2Str = vec[0].substr(webglItem, 6);
    if (webgl2Str == "webgl2") {
        auto& webgl2Objects = ObjectManager::GetInstance().GetWebgl2ObjectMap();
        WebGL2RenderingContext *webGl2RenderingContext = nullptr;
        auto it = webgl2Objects.find(idStr);
        if (it == webgl2Objects.end()) {
            webGl2RenderingContext = new WebGL2RenderingContext(env, exports);
            webgl2Objects.insert({idStr, webGl2RenderingContext});
        } else {
            webGl2RenderingContext = reinterpret_cast<WebGL2RenderingContext *>(it->second);
        }

        webGl2RenderingContext->mEGLSurface = EglManager::GetInstance().CreateSurface(
            webGl2RenderingContext->mEglWindow);

        if (!webGl2RenderingContext->Export(env, exports)) {
            return nullptr;
        }
    } else {
        string webgl1Str = vec[0].substr(webglItem, 5);
        if (webgl1Str == "webgl") {
            auto& webgl1Objects = ObjectManager::GetInstance().GetWebgl1ObjectMap();
            WebGLRenderingContext *webGlRenderingContext = nullptr;
            auto it = webgl1Objects.find(idStr);
            if (it == webgl1Objects.end()) {
                webGlRenderingContext = new WebGLRenderingContext(env, exports);
                webgl1Objects.insert({idStr, webGlRenderingContext});
            } else {
                webGlRenderingContext = reinterpret_cast<WebGLRenderingContext *>(it->second);
            }
            if (vec.size() > 1) {
                WebGLContextAttributes *webGlContextAttributes = new WebGLContextAttributes();
                Util::SetContextAttr(vec, webGlContextAttributes);
                webGlRenderingContext->webGlContextAttributes = webGlContextAttributes;
                webGlContextAttributes = nullptr;
            }
            webGlRenderingContext->mEGLSurface = EglManager::GetInstance().CreateSurface(
                webGlRenderingContext->mEglWindow);

            if (!webGlRenderingContext->Export(env, exports)) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }
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

NAPI_MODULE(libwebglnapi, Export)
} // namespace Rosen
} // namespace OHOS
