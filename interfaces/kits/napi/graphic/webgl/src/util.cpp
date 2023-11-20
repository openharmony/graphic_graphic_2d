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

#include "util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;
void Util::SplitString(const string& str, vector<string>& vec, const string& pattern)
{
    string::size_type pos1 = 0;
    string::size_type pos2 = str.find(pattern);
    while (string::npos != pos2) {
        vec.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + pattern.size();
        pos2 = str.find(pattern, pos1);
    }
    if (pos1 != str.length()) {
        vec.push_back(str.substr(pos1));
    }
}

bool Util::GetContextInfo(napi_env env, napi_value thisVar, std::string &contextId, std::vector<std::string> &info)
{
    napi_status status;
    bool succ = false;
    napi_value id = nullptr;
    status = napi_get_named_property(env, thisVar, "id", &id);
    if (status != napi_ok) {
        return false;
    }
    unique_ptr<char[]> idRev;
    tie(succ, idRev, ignore) = NVal(env, id).ToUTF8String();
    if (!succ) {
        return false;
    }
    contextId = idRev.get();

    napi_value param = nullptr;
    status = napi_get_named_property(env, thisVar, "param", &param);
    if (status != napi_ok) {
        return false;
    }
    unique_ptr<char[]> strRev;
    tie(succ, strRev, ignore) = NVal(env, param).ToUTF8String();
    if (!succ) {
        return false;
    }

    string str = strRev.get();
    Util::SplitString(str, info, ",");
    if (info.size() == 0) {
        return false;
    }
    return true;
}

WebGLRenderingContextBasicBase *Util::GetContextObject(napi_env env, napi_value thisVar)
{
    string contextId;
    std::vector<std::string> info;
    bool succ = GetContextInfo(env, thisVar, contextId, info);
    if (!succ) {
        return nullptr;
    }
    size_t webglItem = info[0].find("webgl");
    string webgl2Str = info[0].substr(webglItem, 6); // length of webgl2
    return ObjectManager::GetInstance().GetWebGLContext(webgl2Str == "webgl2", contextId);
}
} // namespace Rosen
} // namespace OHOS
