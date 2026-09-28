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
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include "common/rs_common_def.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
enum class ReclaimState : uint8_t {
    PENDING = 0,
    RECLAIMING = 1,
    RECLAIMED = 2,
    RECLAIMING_RESUME = 3,
}

class RSB_EXPORT RSBufferReclaim {
public:
    static RSBufferReclaim& GetInstance();
    RSBufferReclaim() = default;
    ~RSBufferReclaim() = default;

    bool DoBufferReclaim(sptr<SurfaceBuffer> buffer);
    bool DoBufferResume(sptr<SurfaceBuffer> buffer);
    bool CheckBufferReclaim();
    void RemoveBufferReclaim(uint64_t bufferId);
    void AddUICaptureNode(NodeId nodeId);
    void RemoveUICaptureNode(NodeId nodeId);
    bool CheckSameProcessUICaptureNode(NodeId nodeId);

private:
    static void BufferDestructorCallback(uint64_t bufferId);
    void ReclaimBufferOnFfrt(sptr<SurfaceBuffer> buffer);
    bool TryResumeBuffer(sptr<SurfaceBuffer> buffer, uint64_t bufferId);

    std::mutex uiCaptureNodeMapMutex_;
    std::map<uint32_t, std::map<NodeId, uint32_t>> uiCaptureNodeMap_;
    mutable std::mutex resumeMutex_;
    std::condition_variable resumeCv_;
    mutable std::mutex bufferReclaimMapMutex_;
    std::map<uint64_t, ReclaimState> bufferReclaimMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_CROSS_PLATFORM
#endif // RS_BUFFER_RECLAIM_H