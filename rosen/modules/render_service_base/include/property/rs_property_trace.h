/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_PROPERTY_TRACE_H
#define RENDER_SERVICE_BASE_COMMON_RS_PROPERTY_TRACE_H

#include <set>
#include <unordered_map>
#include <sys/stat.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_obj_abs_geometry.h"
#include "modifier/rs_modifier_type.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPropertyTrace {
public:
    static RSPropertyTrace &GetInstance()
    {
        return instance_;
    };
    void PropertiesDisplayByTrace(const NodeId& id, const RSProperties& properties);
    void TracePropertiesByNodeName(const NodeId& id, const std::string& nodeName, const RSProperties& properties);
    void RefreshNodeTraceInfo();
private:
    RSPropertyTrace() = default;
    ~RSPropertyTrace() = default;

    void InitNodeAndPropertyInfo();

    void ClearNodeAndPropertyInfo();

    bool IsNeedPropertyTrace(const NodeId& id);
    bool IsNeedPropertyTrace(const std::string& nodeName);

    void DealConfigInputInfo(const std::string& info);
    bool DealNodeNameConfig(const std::string& info);

    bool IsNeedRefreshConfig();

    void AddTraceFlag(const std::string& str);

    bool needWriteAllNode_ {false};
    bool needTraceAllNode_ {false};
    static RSPropertyTrace instance_;
    std::set<NodeId> nodeIdSet_;
    std::set<std::string> nodeNameSet_;
    std::set<std::string> propertySet_;
    std::string propertyFileLastModifyTime;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_PROPERTY_TRACE_H
