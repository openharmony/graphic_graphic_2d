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

#include "webgl/webgl_transform_feedback.h"

#include "napi/n_class.h"
#include "napi/n_func_arg.h"

namespace OHOS {
namespace Rosen {
using namespace std;
napi_value WebGLTransformFeedback::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLTransformFeedback> webGlTransformFeedback = make_unique<WebGLTransformFeedback>();
    if (!NClass::SetEntityFor<WebGLTransformFeedback>(env, funcArg.GetThisVar(), move(webGlTransformFeedback))) {
        LOGE("SetEntityFor webGlTransformFeedback failed.");
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLTransformFeedback::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) =
        NClass::DefineClass(exports_.env_, className, WebGLTransformFeedback::Constructor, std::move(props));
    if (!succ) {
        LOGE("WebGLTransformFeedback defineClass failed.");
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        LOGE("WebGLTransformFeedback saveClass failed.");
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLTransformFeedback::GetClassName()
{
    return WebGLTransformFeedback::className;
}
} // namespace Rosen
} // namespace OHOS
