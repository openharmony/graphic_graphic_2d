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

#ifndef FILTER_FACTORY_H
#define FILTER_FACTORY_H

#include <memory>
#include <unordered_set>
#include <string>
#include "cJSON.h"
#include "filter.h"

namespace OHOS {
namespace Rosen {
class FilterFactory {
public:
    std::shared_ptr<Filter> GetFilter(cJSON* type);

private:
    std::unordered_set<std::string> algoFilterSet_ {
        "Input",
        "Output",
        "GaussianBlur",
        "HorizontalBlur",
        "VerticalBlur",
        "Scale",
        "Saturation",
        "Contrast",
        "Brightness"
    };
};
} // namespcae Rosen
} // namespace OHOS
#endif // FILTER_FACTORY_H