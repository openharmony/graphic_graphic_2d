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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_STUB_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_STUB_H

#include <map>

#include <iremote_stub.h>
#include <nocopyable.h>

#include "rs_iwindow_animation_controller.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationStub;
using WindowAnimationStubFunc = int (RSWindowAnimationStub::*)(MessageParcel& data, MessageParcel& reply);

class RSWindowAnimationStub : public IRemoteStub<RSIWindowAnimationController> {
public:
    RSWindowAnimationStub() = default;
    virtual ~RSWindowAnimationStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    int Transition(MessageParcel& data, MessageParcel& reply);

    int MinimizeWindow(MessageParcel& data, MessageParcel& reply);

    int CloseWindow(MessageParcel& data, MessageParcel& reply);

    static const std::map<uint32_t, WindowAnimationStubFunc> stubFuncMap_;
    DISALLOW_COPY_AND_MOVE(RSWindowAnimationStub);
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_STUB_H
