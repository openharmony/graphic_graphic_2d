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

#ifndef INTERFACES_INNERKITS_SURFACE_IBUFFER_CONSUMER_LISTENER_H
#define INTERFACES_INNERKITS_SURFACE_IBUFFER_CONSUMER_LISTENER_H

#include <refbase.h>
#include "surface_type.h"

namespace OHOS {
class IBufferConsumerListener : public RefBase {
public:
    virtual ~IBufferConsumerListener() = default;
    virtual void OnBufferAvailable() = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_IBUFFER_CONSUMER_LISTENER_H
