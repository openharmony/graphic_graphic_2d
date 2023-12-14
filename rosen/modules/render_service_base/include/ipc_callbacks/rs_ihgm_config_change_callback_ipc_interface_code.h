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

#ifndef ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IHGM_CONFIG_CHANGE_CALLBACK_INTERFACE_CODE_H
#define ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IHGM_CONFIG_CHANGE_CALLBACK_INTERFACE_CODE_H

#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {
enum class RSIHgmConfigChangeCallbackInterfaceCode : CodeUnderlyingType {
    ON_HGM_CONFIG_CHANGED,
    ON_HGM_REFRESH_RATE_MODE_CHANGED,
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_TRANSACTION_RS_IHGM_CONFIG_CHANGE_CALLBACK_INTERFACE_CODE_H
