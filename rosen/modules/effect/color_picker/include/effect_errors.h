/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef EFFECT_ERRORS_H
#define EFFECT_ERRORS_H

#include <cstdint>
#include "errors.h"

namespace OHOS {
namespace Rosen {
/* Effect defined errors */
const uint32_t ERR_EFFECT_INVALID_VALUE = 2;                   // Invalid value
const uint32_t ERROR = 1;                                      // Operation error
const uint32_t SUCCESS = 0;                                    // Operation success
const uint32_t ERR_INVALID_PARAM = 401;                        // the value do not change. It is defined on all system
}  // namespace Rosen
}  // namespace OHOS
#endif  // EFFECT_ERRORS_H
