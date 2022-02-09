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

#ifndef BUILDER_H
#define BUILDER_H

#include <memory>
#include <unordered_map>
#include <vector>
#include "cJSON.h"
#include "image_chain.h"
#include "gaussian_blur_filter.h"
#include "input.h"
#include "output.h"
#include "saturation_filter.h"
#include "contrast_filter.h"
#include "brightness_filter.h"
#include "filter_factory.h"

namespace OHOS {
namespace Rosen {
class Builder {
public:
    ImageChain* CreateFromConfig(std::string path);

private:
    void AnalyseFilters(cJSON* filters);
    void ParseParams(std::shared_ptr<Filter> filter, cJSON* params);
    void ConnectPipeline(cJSON* connections);
    std::unordered_map<std::string, std::string> nameType_;
    std::unordered_map<std::string, std::shared_ptr<Filter>> nameFilter_;
    std::vector<std::shared_ptr<Input>> inputs_;
    cJSON* filters_ = nullptr;
    cJSON* connections_ = nullptr;
    std::shared_ptr<FilterFactory> algoFilterFactory = std::make_shared<FilterFactory>();
};
} // namespace Rosen
} // namespace OHOS
#endif // BUILDER_H
