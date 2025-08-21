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

#include "dirty_region/rs_optimize_canvas_dirty_collector.h"

#include "common/rs_optional_trace.h"
#include "common/rs_rect.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSOptimizeCanvasDirtyCollector& RSOptimizeCanvasDirtyCollector::GetInstance()
{
    static RSOptimizeCanvasDirtyCollector instance;
    return instance;
}

RSOptimizeCanvasDirtyCollector::RSOptimizeCanvasDirtyCollector()
{
}

RSOptimizeCanvasDirtyCollector::~RSOptimizeCanvasDirtyCollector() noexcept
{
}

RectF RSOptimizeCanvasDirtyCollector::CalcCmdlistDrawRegionFromOpItem(
    const std::shared_ptr<ModifierNG::RSRenderModifier>& modifier)
{
    auto propertyType = ModifierNG::ModifierTypeConvertor::GetPropertyType(modifier->GetType());
    auto propertyPtr =
        std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(modifier->GetProperty(propertyType));
    auto drawCmdlistPtr = propertyPtr ? propertyPtr->Get() : nullptr;
    if (drawCmdlistPtr == nullptr) {
        RS_OPTIONAL_TRACE_NAME_FMT(
            "RSOptimizeCanvasDirtyCollector::CalcCmdlistDrawRegionFromOpItem modifier:%llu drawCmdlistPtr is nullptr",
            modifier->GetId());
        return RectF {};
    }

    const auto& rect = drawCmdlistPtr->GetCmdlistDrawRegion();
    RectF cmdlistDrawRegion = RectF { rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight() };
    RS_OPTIONAL_TRACE_NAME_FMT("RSRenderNode::CalcCmdlistDrawRegionFromOpItem modifier:%llu cmdlistDrawRegion is %s",
        modifier->GetId(), cmdlistDrawRegion.ToString().c_str());
    return cmdlistDrawRegion;
}

void RSOptimizeCanvasDirtyCollector::SetOptimizeCanvasDirtyPidList(const std::vector<pid_t>& pidList)
{
    std::lock_guard<std::mutex> lock(optimizeCanvasDirtyPidListMutex_);
    optimizeCanvasDirtyRegionPidList_.clear();
    optimizeCanvasDirtyRegionPidList_.insert(pidList.begin(), pidList.end());
}

bool RSOptimizeCanvasDirtyCollector::IsOptimizeCanvasDirtyEnabled(NodeId nodeId) const
{
    std::lock_guard<std::mutex> lock(optimizeCanvasDirtyPidListMutex_);
    int32_t pid = ExtractPid(nodeId);
    return optimizeCanvasDirtyRegionPidList_.find(pid) != optimizeCanvasDirtyRegionPidList_.end();
}
} // namespace Rosen
} // namespace OHOS
