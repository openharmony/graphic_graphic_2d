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

#ifndef SAFUZZ_RANDOM_PROPERTIES_MAP_H
#define SAFUZZ_RANDOM_PROPERTIES_MAP_H

#include <memory>
#include <vector>

#include "command/rs_node_showing_command.h"

namespace OHOS {
namespace Rosen {
using PropertiesMap = RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap;
class RandomPropertiesMap {
public:
    static PropertiesMap GetRandomPropertiesMap();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_PROPERTIES_MAP_H
