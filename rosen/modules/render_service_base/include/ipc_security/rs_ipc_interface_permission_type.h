/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_PERMISSION_TYPE_H
#define ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_PERMISSION_TYPE_H

#include <unordered_map>

#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
enum class PermissionType : CodeUnderlyingType {
    CAPTURE_SCREEN,
    UPDATE_CONFIGURATION,
    GET_RUNNING_INFO,
};
extern const std::unordered_map<PermissionType, std::string> PERMISSION_MAP;

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_IPC_INTERFACE_PERMISSION_TYPE_H
