/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISH_CALLBACK_STUB_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISH_CALLBACK_STUB_H

#include <iremote_stub.h>
#include <nocopyable.h>

#include "rs_iwindow_animation_finished_callback.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationFinishedCallbackStub : public IRemoteStub<RSIWindowAnimationFinishedCallback> {
public:
    RSWindowAnimationFinishedCallbackStub() = default;
    virtual ~RSWindowAnimationFinishedCallbackStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    DISALLOW_COPY_AND_MOVE(RSWindowAnimationFinishedCallbackStub);
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISH_CALLBACK_STUB_H
