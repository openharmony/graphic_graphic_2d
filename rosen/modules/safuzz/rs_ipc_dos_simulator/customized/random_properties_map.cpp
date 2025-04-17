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

#include "customized/random_properties_map.h"

#include <utility>

#include "common/rs_common_def.h"
#include "customized/random_rs_render_property_base.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
namespace {
std::vector<uint64_t> GetRandomUInt64SmallVector()
{
    std::vector<uint64_t> data;
    int length = RandomEngine::GetRandomSmallVectorLength();
    for (int i = 0; i < length; i++) {
        data.push_back(RandomData::GetRandomUint64());
    }
    return data;
}
} // namespace

PropertiesMap RandomPropertiesMap::GetRandomPropertiesMap()
{
    PropertiesMap propertiesMap;
    int vectorLength = RandomEngine::GetRandomMiddleVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        auto renderPropertiesBase = RandomRSRenderPropertyBase::GetRandomRSRenderPropertyBase();
        if (renderPropertiesBase == nullptr) {
            continue;
        }
        PropertyId propertyId = renderPropertiesBase->GetId();
        NodeId nodeId = RandomData::GetRandomUint64();
        propertiesMap[std::make_pair(nodeId, propertyId)] =
            std::make_pair(renderPropertiesBase, GetRandomUInt64SmallVector());
    }
    return propertiesMap;
}
} // namespace Rosen
} // namespace OHOS
