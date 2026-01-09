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

#ifndef ROSEN_RENDER_SERVICE_LPP_LAYER_COLLECTOR_H
#define ROSEN_RENDER_SERVICE_LPP_LAYER_COLLECTOR_H
#include <set>

#include "feature/lpp/render_server/lpp_layer_collector.h"
#include "hdi_layer.h"
namespace OHOS::Rosen {
constexpr uint32_t LPP_LAYER_PROPERTY =
    TunnelLayerProperty::TUNNEL_PROP_BUFFER_ADDR | TunnelLayerProperty::TUNNEL_PROP_DEVICE_COMMIT;
class LppLayerColletor {
private:
    std::set<uint64_t> lppLayerId_;

public:
    void AddLppLayerId(const std::vector<RSLayerPtr>& layers);
    void RemoveLayerId(const std::vector<RSLayerPtr>& layers);
    const std::set<uint64_t>& GetLppLayerId();
};
} // namespace OHOS::Rosen

#endif