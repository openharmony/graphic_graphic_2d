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

#ifndef CJ_COLOR_MGR_FFI_H
#define CJ_COLOR_MGR_FFI_H

#include "cj_color_mgr_utils.h"
#include "cj_common_ffi.h"
#include "color_space.h"

extern "C" {
    FFI_EXPORT RetDataCString CJ_ColorMgrCreateByColorSpace(uint32_t colorSpaceName, int64_t* id);
    FFI_EXPORT int64_t CJ_ColorMgrCreate(OHOS::ColorManager::ColorSpacePrimaries primaries,
        float gamma, int32_t* errCode);
    FFI_EXPORT uint32_t CJ_ColorMgrGetColorSpaceName(int64_t id, int32_t* errCode);
    FFI_EXPORT float* CJ_ColorMgrGetWhitePoint(int64_t id, int32_t* errCode);
    FFI_EXPORT float CJ_ColorMgrGetGamma(int64_t id, int32_t* errCode);
}

#endif