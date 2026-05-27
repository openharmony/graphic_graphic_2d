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

#ifndef RS_TUNNEL_ROUTE_ARBITER_H
#define RS_TUNNEL_ROUTE_ARBITER_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "common/rs_common_def.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;
class RSSurfaceRenderNode;

class RSTunnelRouteArbiter {
public:
    enum class MainThreadOutcome : uint32_t {
        NOT_TUNNEL_ACTIVE,
        GO_NORMAL,
        KEEP_DIRECT,
    };

    RSTunnelRouteArbiter() = default;
    ~RSTunnelRouteArbiter() = default;
    RSTunnelRouteArbiter(const RSTunnelRouteArbiter&) = delete;
    RSTunnelRouteArbiter& operator=(const RSTunnelRouteArbiter&) = delete;

    MainThreadOutcome ArbitrateAndClaim(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void AbandonNormalClaim(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void OnRenderCommitDone(ScreenId screenId);
    void AttachToRenderThread();

    static void RefreshGlobalTriggerSnapshot();
    static bool IsGlobalRouteForcedNormal();

    static const char* ComputeGlobalForbiddenCause(RSMainThread* mainThread);

private:
    // Pair the weak node with the screenId observed at claim time. If the surface migrates to
    // another screen before render commit, the original screen's commit-done still recognises
    // the entry and advances NORMAL_PREPARING -> NORMAL_COMMITTED, avoiding a permanent phase
    // stall that would lock out the listener path.
    std::vector<std::pair<std::weak_ptr<RSSurfaceRenderNode>, ScreenId>> normalRouteNodes_;
    static std::atomic<bool> globalRouteForcedNormal_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_TUNNEL_ROUTE_ARBITER_H
