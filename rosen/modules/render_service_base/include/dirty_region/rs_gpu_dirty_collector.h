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
#ifndef RENDER_SERVICE_BASE_RS_GPU_DIRTY_COLLECTOR_H
#define RENDER_SERVICE_BASE_RS_GPU_DIRTY_COLLECTOR_H

#include "common/rs_rect.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
namespace RSGpuDirtyCollectorConst {
    constexpr int ATTRKEY_GPU_DIRTY_REGION = 17;
}
enum CacheOption : int32_t {
    CACHE_NOOP = 0x0,   // do nothing
    CACHE_INVALIDATE,   // hadware writing aux buffer to disable cache
    CACHE_FLUSH,        // write aux buffer on CPU and flush cache to DDR
};

struct BlobDataType {
    unsigned long vaddr;         // vitual address, direct accessed w/o copy
    unsigned int offset;         // field offset in dmabuf, support dma access
    unsigned int length;         // data length set by previous SetMetadate(), default to 0
    unsigned int capacity;       // reserved data size
    CacheOption cacheop;         // effective in SetMetadate()
};

struct BufferSelfDrawingData {
    unsigned int left;
    unsigned int top;
    unsigned int right;
    unsigned int bottom;
    bool gpuDirtyEnable;        // the result of APS
    bool curFrameDirtyEnable;   // current frame dirty region
};

class RSB_EXPORT RSGpuDirtyCollector {
public:
#ifndef ROSEN_CROSS_PLATFORM
    static BufferSelfDrawingData *GetBufferSelfDrawingData(const sptr<SurfaceBuffer> &buffer);
    static bool DirtyRegionCompute(const sptr<SurfaceBuffer> &buffer, Rect &rect);
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_RS_GPU_DIRTY_COLLECTOR_H
