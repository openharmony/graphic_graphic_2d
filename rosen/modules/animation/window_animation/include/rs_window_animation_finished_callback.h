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

#ifndef WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISHED_CALLBACK_H
#define WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISHED_CALLBACK_H

#include <functional>

#include "rs_window_animation_finished_callback_stub.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationFinishedCallback : public RSWindowAnimationFinishedCallbackStub {
public:
    explicit RSWindowAnimationFinishedCallback(const std::function<void(void)>& callback);
    virtual ~RSWindowAnimationFinishedCallback() = default;

private:
    void OnAnimationFinished() override;

    std::function<void(void)> callback_;
};
} // namespace Rosen
} // namespace OHOS

#endif // WINDOW_ANIMATION_RS_WINDOW_ANIMATION_FINISHED_CALLBACK_H
