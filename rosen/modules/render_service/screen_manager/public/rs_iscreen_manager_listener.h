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

#ifndef RS_ISCREEN_MANAGER_LISTENER_H
#define RS_ISCREEN_MANAGER_LISTENER_H

#include <iremote_object.h>
#include <refbase.h>

#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_property.h"

namespace OHOS {
namespace Rosen {
class RSIScreenManagerListener : public RefBase {
public:
    virtual ~RSIScreenManagerListener() = default;
    virtual sptr<IRemoteObject> OnScreenConnected(ScreenId id, const std::shared_ptr<HdiOutput>& output,
        const sptr<RSScreenProperty>& property) = 0;
    virtual void OnScreenDisconnected(ScreenId id) = 0;
    virtual void OnHwcRestored(ScreenId id, const std::shared_ptr<HdiOutput>& output,
        const sptr<RSScreenProperty>& property) = 0;
    virtual void OnHwcDead(ScreenId id) = 0;
    virtual void OnScreenPropertyChanged(ScreenId id, const sptr<RSScreenProperty>& property) = 0;
    virtual void OnScreenRefresh(ScreenId id) = 0;
    virtual void OnVBlankIdle(ScreenId id, uint64_t ns) = 0;
    virtual void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property) = 0;
    virtual void OnVirtualScreenDisconnected(ScreenId id) = 0;
    virtual void OnHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData) = 0;
    virtual void OnActiveScreenIdChanged(ScreenId activeScreenId) = 0;
    virtual void OnScreenBacklightChanged(ScreenId id, uint32_t level) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_ISCREEN_MANAGER_LISTENER_H