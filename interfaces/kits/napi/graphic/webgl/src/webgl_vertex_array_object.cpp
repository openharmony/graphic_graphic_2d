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

#include "webgl/webgl_vertex_array_object.h"

#include "napi/n_class.h"
#include "napi/n_func_arg.h"

namespace OHOS {
namespace Rosen {
using namespace std;
napi_value WebGLVertexArrayObject::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLVertexArrayObject> webGlVertexArrayObject = make_unique<WebGLVertexArrayObject>();
    if (!NClass::SetEntityFor<WebGLVertexArrayObject>(env, funcArg.GetThisVar(), move(webGlVertexArrayObject))) {
        LOGE("SetEntityFor webGlVertexArrayObject failed.");
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLVertexArrayObject::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) =
        NClass::DefineClass(exports_.env_, className, WebGLVertexArrayObject::Constructor, std::move(props));
    if (!succ) {
        LOGE("WebGLVertexArrayObject defineClass failed.");
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        LOGE("WebGLVertexArrayObject saveClass failed.");
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLVertexArrayObject::GetClassName()
{
    return WebGLVertexArrayObject::className;
}
} // namespace Rosen
} // namespace OHOS
