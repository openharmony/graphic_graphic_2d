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

#ifndef ROSEN_RENDER_THREAD_STATS_H
#define ROSEN_RENDER_THREAD_STATS_H

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <utility>

#include "common/rs_common_def.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties.h"
#include "surface_type.h"
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
using RenderFitSet = std::set<Gravity>;
using RSRenderFitInfo = std::map<std::string, RenderFitSet>;
using RSRecentUpdatedRenderFitInfo = std::unordered_map<std::string, bool>;

class RSRenderThreadStats {
public:
    static RSRenderThreadStats& GetInstance();
    void AddStaticInfo(const float frameWidth, const float frameHeight,
        const RSProperties& property, const ScalingMode scalingMode);
    void ReportRenderFitInfo();

private:
    RSRenderThreadStats() = default;
    ~RSRenderThreadStats() = default;
    DISALLOW_COPY_AND_MOVE(RSRenderThreadStats);
    std::string RenderFitToString(const Gravity& fit);
    void AddRenderFitInfo(RSRenderFitInfo& info,
        RSRecentUpdatedRenderFitInfo& recentUpdateInfo, const std::string& packageName,
        Gravity renderfit);
    bool IsRenderFitInfoFull(const RSRenderFitInfo& info) const;
    void ClearNodeRenderFitInfo();
    void ClearRecentUpdatedRenderFitInfo();
    std::string ConvertInfoToString(const RSRenderFitInfo& info,
        const RSRecentUpdatedRenderFitInfo& recentUpdateInfo);
    static constexpr int64_t RENDERFIT_INFO_MAP_LIMIT = 50;
    RSRenderFitInfo rsRenderFitRenderThreadInfo_;
    RSRecentUpdatedRenderFitInfo recentUpdatedRenderFitRenderThreadInfo_;
    std::mutex renderfitMutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_THREAD_STATS_H