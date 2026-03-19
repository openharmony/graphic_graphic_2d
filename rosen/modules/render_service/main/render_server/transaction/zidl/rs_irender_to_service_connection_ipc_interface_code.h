/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_IPC_INTERFACE_CODE_H
#define RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_IPC_INTERFACE_CODE_H

#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {

enum class RSIRenderToServiceConnectionInterfaceCode : CodeUnderlyingType {
    NOTIFY_RENDER_PROCESS_READY = 0,
    REPLY_DUMP_RESULT_TO_SERVICE = 1,
    NOTIFY_PROCESS_FRAME_RATE = 2,
    NOTIFY_SCREEN_SWITCH_FINISHED = 3,
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_MAIN_RENDER_SERVER_TRANSACTION_ZIDL_RS_IRENDER_TO_SERVICE_CONNECTION_IPC_INTERFACE_CODE_H