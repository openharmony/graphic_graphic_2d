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

#include "webgl/webgl_sync.h"

#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "napi/n_val.h"

namespace OHOS {
namespace Rosen {
using namespace std;
napi_value WebGLSync::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLSync> webGlSync = make_unique<WebGLSync>();
    if (!NClass::SetEntityFor<WebGLSync>(env, funcArg.GetThisVar(), move(webGlSync))) {
        LOGE("SetEntityFor webGlSync failed.");
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLSync::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLSync::Constructor, std::move(props));
    if (!succ) {
        LOGE("WebGLSync defineClass failed.");
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        LOGE("WebGLSync saveClass failed.");
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLSync::GetClassName()
{
    return WebGLSync::className;
}
} // namespace Rosen
} // namespace OHOS
