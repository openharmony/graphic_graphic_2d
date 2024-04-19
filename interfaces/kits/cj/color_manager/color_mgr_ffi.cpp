/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "color_mgr_ffi.h"
#include "cj_color_manager.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace ColorManager {
extern "C" {
const int32_t OK = 0;
RetDataCString CJ_ColorMgrCreateByColorSpace(uint32_t colorSpaceName, int64_t* id)
{
    CMLOGI("[ColorMgr] CJ_ColorMgrCreateByColorSpace start");
    RetDataCString ret = { .code = OK, .data = nullptr };
    auto&& [errCode, errMsg, ptr]= CjColorManager::create(ApiColorSpaceType(colorSpaceName));
    ret.code = errCode;
    ret.data = ::Utils::MallocCString(errMsg);
    if (errCode != OK) {
        *id = 0;
        return ret;
    }
    
    auto native = FFIData::Create<CjColorManager>(ptr);
    CMLOGI("[ColorMgr] CJ_ColorMgrCreateByColorSpace success");
    *id = native->GetID();
    return ret;
}

int64_t CJ_ColorMgrCreate(ColorSpacePrimaries primaries, float gamma, int32_t* errCode)
{
    CMLOGI("[ColorMgr] CJ_ColorMgrCreate start");
    std::shared_ptr<ColorSpace> ptr = CjColorManager::create(primaries, gamma, errCode);
    if (*errCode != OK) {
        return 0;
    }
    auto native = FFIData::Create<CjColorManager>(ptr);
    CMLOGI("[ColorMgr] CJ_ColorMgrCreate success");
    return native->GetID();
}

uint32_t CJ_ColorMgrGetColorSpaceName(int64_t id, int32_t* errCode)
{
    CMLOGI("[ColorMgr] CJ_ColorMgrGetColorSpaceName start");
    auto native = FFIData::GetData<CjColorManager>(id);
    CMLOGI("[ColorMgr] CJ_ColorMgrGetColorSpaceName success");
    return native->GetColorSpaceName(errCode);
}

float* CJ_ColorMgrGetWhitePoint(int64_t id, int32_t* errCode)
{
    CMLOGI("[ColorMgr] CJ_ColorMgrGetWhitePoint start");
    auto native = FFIData::GetData<CjColorManager>(id);
    CMLOGI("[ColorMgr] CJ_ColorMgrGetWhitePoint success");
    std::array<float, DIMES_2> arr = native->GetWhitePoint(errCode);
    float* res = static_cast<float*>(malloc(2 * sizeof(float)));
    if (res == nullptr) {
        return res;
    }
    for (size_t i = 0; i < DIMES_2; i++) {
        CMLOGI("[ColorMgr] CJ_ColorMgrGetWhitePoint process %{public}f", arr[i]);
        *(res + i) = arr[i];
    }
    return res;
}

float CJ_ColorMgrGetGamma(int64_t id, int32_t* errCode)
{
    CMLOGI("[ColorMgr] CJ_ColorMgrGetGamma start");
    auto native = FFIData::GetData<CjColorManager>(id);
    CMLOGI("[ColorMgr] CJ_ColorMgrGetGamma success");
    return native->GetGamma(errCode);
}
}
}
}