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

#include "platform/common/rs_system_properties.h"

#include <sstream>
#include <vector>

#include <parameters.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
template <typename Out>
void SplitHelper(const std::string &s, char delimiter, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delimiter)) {
        *result++ = item;
    }
}

std::vector<std::string> GetSplitResult(const std::string &s, char delimiter) {
    std::vector<std::string> elems;
    SplitHelper(s, delimiter, std::back_inserter(elems));
    return elems;
}

UniRenderEnabledType RSSystemProperties::GetUniRenderEnabledType()
{
    return static_cast<UniRenderEnabledType>(std::atoi((system::GetParameter("rosen.unirender.enabled", "0")).c_str()));
}

const std::set<std::string>& RSSystemProperties::GetUniRenderEnabledList()
{
    uniRenderEnabledList_.clear();
    std::string paramUniLayers = system::GetParameter("rosen.unirender.layers", "clock0");
    auto uniLayers = GetSplitResult(paramUniLayers, ',');
    for (auto& layer: uniLayers) {
        RS_LOGI("RSSystemProperties::GetUniRenderEnabledList uniRender for:%s", layer.c_str());
        uniRenderEnabledList_.insert(layer);
    }
    return uniRenderEnabledList_;
}
} // namespace Rosen
} // namespace OHOS
