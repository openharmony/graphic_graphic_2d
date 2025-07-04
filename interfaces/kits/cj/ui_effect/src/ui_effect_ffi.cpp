/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "cj_common_ffi.h"
#include "cj_filter.h"
#include "cj_ui_effect_log.h"
#include "cj_visual_effect.h"
#include "ffi_remote_data.h"

extern "C" {
FFI_EXPORT int64_t FfiOHOSUiEffectCreateEffect(int32_t* errCode)
{
    auto instance = OHOS::FFI::FFIData::Create<OHOS::Rosen::CJVisualEffect>(errCode);
    if (instance == nullptr) {
        *errCode = OHOS::Rosen::CJ_ERR_ILLEGAL_INSTANCE;
        return 0;
    }
    return instance->GetID();
}

FFI_EXPORT int64_t FfiOHOSUiEffectCreateFilter(int32_t* errCode)
{
    auto instance = OHOS::FFI::FFIData::Create<OHOS::Rosen::CJFilter>(errCode);
    if (instance == nullptr) {
        *errCode = OHOS::Rosen::CJ_ERR_ILLEGAL_INSTANCE;
        return 0;
    }
    return instance->GetID();
}

FFI_EXPORT void FfiOHOSUiEffectSetBlur(int64_t id, float blur, int32_t* errCode)
{
    auto instance = OHOS::FFI::FFIData::GetData<OHOS::Rosen::CJFilter>(id);
    if (instance == nullptr) {
        *errCode = OHOS::Rosen::CJ_ERR_ILLEGAL_INSTANCE;
        return;
    }
    instance->SetBlur(blur, errCode);
}
}
