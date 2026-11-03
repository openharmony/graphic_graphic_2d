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

#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_IPC_CALLBACKS_RS_IDE_STATUS_CHANGE_CALLBACK_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_IPC_CALLBACKS_RS_IDE_STATUS_CHANGE_CALLBACK_H

#include <iremote_broker.h>

namespace OHOS {
namespace Rosen {
class RSIDEStatusChangeCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.DEStatusChangeCallback");

    RSIDEStatusChangeCallback() = default;
    virtual ~RSIDEStatusChangeCallback() noexcept = default;

    virtual void OnNotifyDEStatusChangeDone(const uint32_t sceneKey, const std::vector<uint8_t>& result) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_IPC_CALLBACKS_RS_IDE_STATUS_CHANGE_CALLBACK_H