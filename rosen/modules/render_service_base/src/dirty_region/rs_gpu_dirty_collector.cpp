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

#include "dirty_region/rs_gpu_dirty_collector.h"

#include "common/rs_optional_trace.h"
#include "common/rs_rect.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
RSGpuDirtyCollector& RSGpuDirtyCollector::GetInstance()
{
    static RSGpuDirtyCollector instance;
    return instance;
}

void RSGpuDirtyCollector::SetSelfDrawingGpuDirtyPidList(const std::vector<int32_t>& pidList)
{
    std::lock_guard<std::mutex> lock(pidListMutex_);
    selfDrawingGpuDirtyPidList_.clear();
    selfDrawingGpuDirtyPidList_.insert(pidList.begin(), pidList.end());
}

bool RSGpuDirtyCollector::IsGpuDirtyEnable(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(pidListMutex_);
    int32_t pid = ExtractPid(nodeId);
    return selfDrawingGpuDirtyPidList_.find(pid) != selfDrawingGpuDirtyPidList_.end();
}

#ifndef ROSEN_CROSS_PLATFORM
BufferSelfDrawingData* RSGpuDirtyCollector::GetBufferSelfDrawingData(const sptr<SurfaceBuffer> &buffer)
{
    if (buffer == nullptr) {
        return nullptr;
    }
    BlobDataType* data = nullptr;
    BufferSelfDrawingData* src = nullptr;
    std::vector<uint8_t> rec;
    int32_t ret = buffer->GetMetadata(RSGpuDirtyCollectorConst::ATTRKEY_GPU_DIRTY_REGION, rec);
    if (ret == GSERROR_OK && rec.size() == sizeof(BlobDataType)) {
        data = reinterpret_cast<BlobDataType*>(rec.data());
        src = reinterpret_cast<BufferSelfDrawingData*>(data->vaddr + data->offset);
    }
    return src;
}

bool RSGpuDirtyCollector::DirtyRegionCompute(const sptr<SurfaceBuffer> &buffer, Rect &rect)
{
    if (!RSSystemProperties::GetSelfDrawingDirtyRegionEnabled()) {
        return false;
    }

    if (buffer == nullptr) {
        return false;
    }
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    if (src == nullptr) {
        return false;
    }
    // Determine whether current frame dirty region is valid and application enable self drawning dirty region
    bool isSelfDrawingDirtyRegionEnable = src->curFrameDirtyEnable && src->gpuDirtyEnable;
    if (!isSelfDrawingDirtyRegionEnable) {
        RS_OPTIONAL_TRACE_NAME_FMT("RSGpuDirtyCollector::DirtyRegionCompute curFrameDirtyEnable:%d, gpuDirtyEnable:%d",
            src->curFrameDirtyEnable, src->gpuDirtyEnable);
        return false;
    }
    // when layer don't have dirty region, the right and bottom of Gpu Dirty Region are initialized to minimum values
    // the left and top are initialized to maximum values, Gpu Dirty Region can't set to (0, 0, 0, 0)
    bool isNotDirty = src->left == static_cast<uint32_t>(buffer->GetWidth()) &&
        src->top == static_cast<uint32_t>(buffer->GetHeight()) && src->right == 0 && src->bottom == 0;
    if (isNotDirty) {
        rect = { 0, 0, 0, 0 };
        return true;
    }
    bool isDirtyRegionInvalid = src->left < 0 || src->left > static_cast<uint32_t>(buffer->GetWidth()) ||
        src->right < 0 || src->right > static_cast<uint32_t>(buffer->GetWidth()) || src->top < 0 ||
        src->top > static_cast<uint32_t>(buffer->GetHeight()) || src->bottom < 0 || src->bottom >
        static_cast<uint32_t>(buffer->GetHeight()) || src->left >= src->right || src->top >= src->bottom;
    if (isDirtyRegionInvalid) {
        RS_OPTIONAL_TRACE_NAME_FMT("invalidBufferSelfDrawingData:[%d, %d, %d, %d]",
            src->left, src->top, src->right, src->bottom);
        return false;
    }
    rect = { src->left, src->top, src->right - src->left, src->bottom - src->top };
    return true;
}

void RSGpuDirtyCollector::SetGpuDirtyEnabled(const sptr<SurfaceBuffer> &buffer, bool gpuDirtyEnable)
{
    if (buffer == nullptr) {
        return;
    }
    auto src = RSGpuDirtyCollector::GetBufferSelfDrawingData(buffer);
    if (src != nullptr) {
        src->gpuDirtyEnable = RSSystemProperties::GetGpuDirtyApsEnabled() && gpuDirtyEnable;
    }
}
#endif
} // namespace Rosen
} // namespace OHOS
