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

#include "webgl/webgl_active_info.h"

#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/log.h"
#include "napi/n_val.h"

namespace OHOS {
namespace Rosen {
using namespace std;
WebGLActiveInfo* WebGLActiveInfo::GetWebGLActiveInfo(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }
    if (funcArg.GetThisVar() == nullptr) {
        LOGE("WebGLActiveInfo::GetWebGLActiveInfo GetThisVar failed.");
        return nullptr;
    }
    WebGLActiveInfo* webGLActiveInfo = nullptr;
    napi_status status = napi_unwrap(env, funcArg.GetThisVar(), (void**)&webGLActiveInfo);
    if (status != napi_ok) {
        LOGE("napi_unwrap webGLActiveInfo failed.");
        return nullptr;
    }
    return webGLActiveInfo;
}

napi_value WebGLActiveInfo::GetActiveName(napi_env env, napi_callback_info info)
{
    WebGLActiveInfo* webGLActiveInfo = GetWebGLActiveInfo(env, info);
    if (webGLActiveInfo == nullptr) {
        LOGE("webGLActiveInfo is nullptr.");
        return nullptr;
    }
    std::string name = webGLActiveInfo->GetActiveName();
    LOGD("WebGLActiveInfo::GetActiveName %s ", name.c_str());
    return NVal::CreateUTF8String(env, name).val_;
}

napi_value WebGLActiveInfo::GetActiveSize(napi_env env, napi_callback_info info)
{
    WebGLActiveInfo* webGLActiveInfo = GetWebGLActiveInfo(env, info);
    if (webGLActiveInfo == nullptr) {
        LOGE("webGLActiveInfo is nullptr.");
        return nullptr;
    }
    int size = webGLActiveInfo->GetActiveSize();
    LOGD("WebGLActiveInfo::GetActiveSize %{public}d ", size);
    napi_value result;
    return (napi_create_int32(env, size, &result) != napi_ok) ? nullptr : result;
}

napi_value WebGLActiveInfo::GetActiveType(napi_env env, napi_callback_info info)
{
    WebGLActiveInfo* webGLActiveInfo = GetWebGLActiveInfo(env, info);
    if (webGLActiveInfo == nullptr) {
        LOGE("webGLActiveInfo is nullptr.");
        return nullptr;
    }
    int type = webGLActiveInfo->GetActiveType();
    LOGD("WebGLActiveInfo::GetActiveType %{public}d", type);
    return NVal::CreateInt64(env, type).val_;
}

napi_value WebGLActiveInfo::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLActiveInfo> webGlActiveInfo = make_unique<WebGLActiveInfo>();
    if (!NClass::SetEntityFor<WebGLActiveInfo>(env, funcArg.GetThisVar(), move(webGlActiveInfo))) {
        LOGE("SetEntityFor webGlActiveInfo failed.");
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLActiveInfo::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {
        NVal::DeclareNapiGetter("name", WebGLActiveInfo::GetActiveName),
        NVal::DeclareNapiGetter("size", WebGLActiveInfo::GetActiveSize),
        NVal::DeclareNapiGetter("type", WebGLActiveInfo::GetActiveType)
    };

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLActiveInfo::Constructor, std::move(props));
    if (!succ) {
        LOGE("DefineClass webGLActiveInfo failed.");
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        LOGE("SaveClass webGLActiveInfo failed.");
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLActiveInfo::GetClassName()
{
    return WebGLActiveInfo::className;
}
} // namespace Rosen
} // namespace OHOS
