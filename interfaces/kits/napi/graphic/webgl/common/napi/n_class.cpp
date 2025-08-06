/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/n_class.h"

namespace OHOS {
namespace Rosen {
using namespace std;
napi_env NClass::environment = nullptr;
NClass &NClass::GetInstance()
{
    static NClass nClass;
    return nClass;
}
NClass::~NClass()
{
    lock_guard<std::mutex> lock(exClassMapLock);
    auto env = NClass::environment;
    if (env && !exClassMap.empty()) {
        auto it = exClassMap.begin();
        while (it != exClassMap.end()) {
            napi_delete_reference(env, it->second);
            it = exClassMap.erase(it);
        }
    }
}

tuple<bool, napi_value> NClass::DefineClass(napi_env env, string className, napi_callback constructor,
    vector<napi_property_descriptor> &&properties)
{
    napi_value classVal = nullptr;
    napi_status stat = napi_define_class(env, className.c_str(), className.length(), constructor,
        nullptr, properties.size(), properties.data(), &classVal);
    return { stat == napi_ok, classVal };
}

bool NClass::SaveClass(napi_env env, string className, napi_value exClass)
{
    NClass &nClass = NClass::GetInstance();
    lock_guard(nClass.exClassMapLock);

    if (nClass.exClassMap.find(className) != nClass.exClassMap.end()) {
        return true;
    }

    napi_ref constructor;
    napi_status res = napi_create_reference(env, exClass, 2, &constructor);
    if (res == napi_ok) {
	    NClass::environment = env;
        nClass.exClassMap.insert({ className, constructor });
    }
    return res == napi_ok;
}

napi_value NClass::InstantiateClass(napi_env env, string className, vector<napi_value> args)
{
    NClass &nClass = NClass::GetInstance();
    lock_guard(nClass.exClassMapLock);

    auto it = nClass.exClassMap.find(className);
    if (it == nClass.exClassMap.end()) {
        return nullptr;
    }
    napi_value cons = nullptr;
    napi_status status = napi_get_reference_value(env, it->second, &cons);
    if (status != napi_ok) {
        return nullptr;
    }

    napi_value instance = nullptr;
    status = napi_new_instance(env, cons, args.size(), args.data(), &instance);
    if (status != napi_ok) {
        return nullptr;
    }
    return instance;
}
} // namespace Rosen
} // namespace OHOS