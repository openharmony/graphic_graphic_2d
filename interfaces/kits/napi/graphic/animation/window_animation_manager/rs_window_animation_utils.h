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

#ifndef INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_UTILS_H
#define INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_UTILS_H

#include <functional>

#include <native_engine/native_engine.h>

#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_target.h"

namespace OHOS {
namespace Rosen {
class RSWindowAnimationUtils {
public:
    static NativeValue* CreateJsWindowAnimationTarget(NativeEngine& engine, const RSWindowAnimationTarget& target);

    static NativeValue* CreateJsWindowAnimationFinishedCallback(
        NativeEngine& engine, const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback);

    static NativeValue* CreateJsRRect(NativeEngine& engine, const RRect& rrect);
};
} // namespace Rosen
} // namespace OHOS

#endif // INTERFACES_KITS_NAPI_GRAPHIC_ANIMATION_RS_WINDOW_ANIMATION_UTILS_H
