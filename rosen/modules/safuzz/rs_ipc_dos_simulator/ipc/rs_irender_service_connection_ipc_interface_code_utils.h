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

#ifndef SAFUZZ_RS_IRENDER_SERVICE_CONNECTION_IPC_INTERFACE_CODE_UTILS_H
#define SAFUZZ_RS_IRENDER_SERVICE_CONNECTION_IPC_INTERFACE_CODE_UTILS_H

#include <string>

#include "platform/ohos/rs_irender_service_connection_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
class RSIRenderServiceConnectionInterfaceCodeUtils {
public:
    static int GetCodeFromName(const std::string& interfaceName);
    static bool IsParcelSplitEnabled(const std::string& interfaceName);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RS_IRENDER_SERVICE_CONNECTION_IPC_INTERFACE_CODE_UTILS_H
