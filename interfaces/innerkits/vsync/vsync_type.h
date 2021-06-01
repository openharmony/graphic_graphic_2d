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

#ifndef INTERFACES_INNERKITS_VSYNC_VSYNC_TYPE_H
#define INTERFACES_INNERKITS_VSYNC_VSYNC_TYPE_H

#include <string>

namespace OHOS {
#define DRM_MODULE_NAME "hisilicon"

using VsyncError = enum VsyncError {
    VSYNC_ERROR_OK = 0,
    VSYNC_ERROR_API_FAILED,
    VSYNC_ERROR_INVALID_OPERATING,
    VSYNC_ERROR_NULLPTR,
    VSYNC_ERROR_BINDER_ERROR,
    VSYNC_ERROR_SAMGR,
    VSYNC_ERROR_SERVICE_NOT_FOUND,
    VSYNC_ERROR_PROXY_NOT_INCLUDE,
    _VSYNC_ERROR_MAX,
};

static const std::string VsyncErrorStrs[] = {
    [VSYNC_ERROR_OK] = "<no error>",
    [VSYNC_ERROR_API_FAILED] = "<api failed>",
    [VSYNC_ERROR_INVALID_OPERATING] = "<operating is invalid>",
    [VSYNC_ERROR_NULLPTR] = "<param exists nullptr>",
    [VSYNC_ERROR_BINDER_ERROR] = "<binder SendRequest failed>",
    [VSYNC_ERROR_SAMGR] = "<get system ability failed>",
    [VSYNC_ERROR_SERVICE_NOT_FOUND] = "<service is not found>",
    [VSYNC_ERROR_PROXY_NOT_INCLUDE] = "<proxy header file is not included>",
};

static inline std::string VsyncErrorStr(VsyncError err)
{
    if (err >= _VSYNC_ERROR_MAX) {
        return "<VsyncErrorStr error index out of range>";
    }

    return VsyncErrorStrs[err];
}
} // namespace OHOS

#endif // INTERFACES_INNERKITS_VSYNC_VSYNC_TYPE_H
