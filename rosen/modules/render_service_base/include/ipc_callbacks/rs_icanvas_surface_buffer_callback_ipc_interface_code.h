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

#ifndef RENDER_SERVICE_BASE_IPC_CALLBACKS_RS_ICANVAS_SURFACE_BUFFER_CALLBACK_IPC_INTERFACE_CODE_H
#define RENDER_SERVICE_BASE_IPC_CALLBACKS_RS_ICANVAS_SURFACE_BUFFER_CALLBACK_IPC_INTERFACE_CODE_H

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "ipc_security/rs_ipc_interface_code_underlying_type.h"

namespace OHOS {
namespace Rosen {
enum class RSICanvasSurfaceBufferCallbackInterfaceCode : CodeUnderlyingType {
    ON_CANVAS_SURFACE_BUFFER_CHANGED,
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK
#endif // RENDER_SERVICE_BASE_IPC_CALLBACKS_RS_ICANVAS_SURFACE_BUFFER_CALLBACK_IPC_INTERFACE_CODE_H
