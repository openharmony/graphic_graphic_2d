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

#ifndef RS_RENDER_GROUP_CACHE_ADAPTER_H
#define RS_RENDER_GROUP_CACHE_ADAPTER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {

struct FilterNodeInfo {
    FilterNodeInfo(NodeId nodeId, Drawing::Matrix matrix, std::vector<Drawing::RectI> rectVec)
        : nodeId_(nodeId), matrix_(matrix), rectVec_(rectVec) {}
    NodeId nodeId_ = 0;
    Drawing::Matrix matrix_;
    std::vector<Drawing::RectI> rectVec_;
};

class RSRenderGroupCacheAdapter {
public:
    RSRenderGroupCacheAdapter() = default;
    ~RSRenderGroupCacheAdapter() = default;

    RSRenderGroupCacheAdapter(const RSRenderGroupCacheAdapter&) = delete;
    RSRenderGroupCacheAdapter(RSRenderGroupCacheAdapter&&) = delete;
    RSRenderGroupCacheAdapter& operator=(const RSRenderGroupCacheAdapter&) = delete;
    RSRenderGroupCacheAdapter& operator=(RSRenderGroupCacheAdapter&&) = delete;

    std::vector<FilterNodeInfo>& GetFilterInfoVec();
    void ClearFilterInfoVec();

    std::unordered_map<NodeId, Drawing::Matrix>& GetWithoutFilterMatrixMap();

    void SetFilterNodeSize(size_t size);
    size_t GetFilterNodeSize() const;
    void ReduceFilterNodeSize();

    void SetLastDrawnFilterNodeId(NodeId nodeId);
    NodeId GetLastDrawnFilterNodeId() const;

private:
    std::vector<FilterNodeInfo> filterInfoVec_;
    std::unordered_map<NodeId, Drawing::Matrix> withoutFilterMatrixMap_;
    size_t filterNodeSize_ = 0;
    NodeId lastDrawnFilterNodeId_ = 0;
};

} // namespace Rosen
} // namespace OHOS
#endif // RS_RENDER_GROUP_CACHE_ADAPTER_H