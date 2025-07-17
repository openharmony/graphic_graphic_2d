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

#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {
#ifndef ROSEN_CROSS_PLATFORM
BufferSelfDrawingData* RSGpuDirtyCollector::GpuDirtyRegionCompute(const sptr<SurfaceBuffer> &buffer)
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
    if (buffer == nullptr) {
        return false;
    }
    auto src = RSGpuDirtyCollector::GpuDirtyRegionCompute(buffer);
    if (src == nullptr || !src->curFrameDirtyEnable) {
        return false;
    }
    // when layer don't have dirty region, the right and bottom of Gpu Dirty Region are initialized to minimum values
    // the left and top are initialized to maximum values, Gpu Dirty Region can't set to (0, 0, 0, 0)
    bool isNotDirty = src->left == buffer->GetWidth() &&
        src->top == buffer->GetHeight() && src->right == 0 && src->bottom == 0;
    if (isNotDirty) {
        rect = { 0, 0, 0, 0 };
        return true;
    }
    bool isDirtyRegionInvalid = src->left < 0 || src->left > buffer->GetWidth() || src->right < 0 ||
        src->right > buffer->GetWidth() || src->top < 0 || src->top > buffer->GetHeight() ||
        src->bottom < 0 || src->bottom > buffer->GetHeight() || src->left >= src->right ||
        src->top >= src->bottom;
    if (isDirtyRegionInvalid) {
        return false;
    }
    rect = { src->left, src->top, src->right - src->left, src->bottom - src->top };
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS
