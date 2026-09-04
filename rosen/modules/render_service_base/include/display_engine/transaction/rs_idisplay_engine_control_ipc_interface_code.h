/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_H

#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {
enum class RSIDisplayEngineControlInterfaceCode : CodeUnderlyingType {
    NOTIFY_DE_STATUS_CHANGE = 0,
    REGISTER_DE_STATUS_CHANGE_CALLBACK = 1,
    UNREGISTER_DE_STATUS_CHANGE_CALLBACK = 2,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_DISPLAY_ENGINE_TRANSACTION_RS_IDISPLAY_ENGINE_CONTROL_IPC_INTERFACE_CODE_H