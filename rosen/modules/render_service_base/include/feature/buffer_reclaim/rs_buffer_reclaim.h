/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef RS_BUFFER_RECLAIM_H
#define RS_BUFFER_RECLAIM_H

#ifndef ROSEN_CROSS_PLATFORM
#include <mutex>
#include <set>
#include "surface_buffer.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSBufferReclaim {
public:
    static RSBufferReclaim& GetInstance();
    RSBufferReclaim() = default;
    ~RSBufferReclaim() = default;

    bool DoBufferReclaim(sptr<SurfaceBuffer> buffer);
    bool DoBufferResume(sptr<SurfaceBuffer> buffer);
    bool CheckBufferReclaim();
    void RemoveBufferReclaim(uint64_t bufferId);

    static void BufferDestructorCallBack(uint64_t bufferId);

private:
    mutable std::mutex mutex_;
    std::set<uint64_t> bufferReclaimNumsSet_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif // RS_BUFFER_RECLAIM_H