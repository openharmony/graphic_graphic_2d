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

#ifndef RS_APP_STATE_LISTENER_H
#define RS_APP_STATE_LISTENER_H
#include "app_state_subscriber.h"

namespace OHOS::Rosen {
class RSAppStateListener : public Memory::AppStateSubscriber {
public:
    RSAppStateListener();
    ~RSAppStateListener();

    void OnTrim(Memory::SystemMemoryLevel level) override;
    void ForceReclaim(int32_t pid, int32_t uid) override;
};

} // namespace OHOS::Rosen
#endif