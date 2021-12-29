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

#ifndef ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_H
#define ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_H

#ifdef ROSEN_OHOS
#include <iremote_broker.h>
#include <screen_manager/screen_types.h>

namespace OHOS {
namespace Rosen {
class RSIScreenChangeCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.ScreenChangeListener");

    RSIScreenChangeCallback() = default;
    virtual ~RSIScreenChangeCallback() noexcept = default;

    enum {
        ON_SCREEN_CHANGED,
    };

    virtual void OnScreenChanged(ScreenId id, ScreenEvent event) = 0;
};

// normal callback functor for client users.
using ScreenChangeCallback = std::function<void(ScreenId, ScreenEvent)>;
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_ISCREEN_CHANGE_CALLBACK_H
