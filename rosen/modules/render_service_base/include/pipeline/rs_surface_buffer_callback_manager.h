/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H

#include <functional>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "common/rs_common_def.h"
#include "refbase.h"

namespace OHOS {
namespace Rosen {

class RSISurfaceBufferCallback;
namespace Drawing {
struct DrawSurfaceBufferFinishCbData;
struct DrawSurfaceBufferAfterAcquireCbData;
} // namespace Drawing
class RSB_EXPORT RSSurfaceBufferCallbackManager {
public:
    struct VSyncFuncs {
        std::function<void()> requestNextVsync;
        std::function<bool()> isRequestedNextVSync;
    };
 
#ifdef ROSEN_OHOS
    using OnFinishCb = std::function<void(const Drawing::DrawSurfaceBufferFinishCbData&)>;
    using OnAfterAcquireBufferCb = std::function<void(const Drawing::DrawSurfaceBufferAfterAcquireCbData&)>;
#endif

    void RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        sptr<RSISurfaceBufferCallback> callback);
    void UnregisterSurfaceBufferCallback(pid_t pid);
    void UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

#ifdef ROSEN_OHOS
    OnFinishCb GetOnFinishCb() const;
    OnAfterAcquireBufferCb GetOnAfterAcquireBufferCb() const;
#endif

    std::function<void(pid_t, uint64_t, uint32_t)> GetSurfaceBufferOpItemCallback() const;
    void SetRunPolicy(std::function<void(std::function<void()>)> runPolicy);
    void SetVSyncFuncs(VSyncFuncs vSyncFuncs);
    void SetIsUniRender(bool isUniRender);
#ifdef ROSEN_OHOS
    void SetReleaseFence(int releaseFenceFd, NodeId rootNodeId = INVALID_NODEID);
#endif
 
    void RunSurfaceBufferCallback();
#ifdef RS_ENABLE_VK
    void RunSurfaceBufferSubCallbackForVulkan(NodeId rootNodeId);
#endif

    static RSSurfaceBufferCallbackManager& Instance();
private:
    static constexpr size_t ROOTNODEIDS_POS = 3;
    struct BufferQueueData {
        std::vector<uint32_t> bufferIds;
        std::vector<uint8_t> isRenderedFlags;
#ifdef ROSEN_OHOS
        std::vector<sptr<SyncFence>> releaseFences;
#endif
        std::vector<NodeId> rootNodeIds;
    };
    RSSurfaceBufferCallbackManager() = default;
    ~RSSurfaceBufferCallbackManager() noexcept = default;

    RSSurfaceBufferCallbackManager(const RSSurfaceBufferCallbackManager&) = delete;
    RSSurfaceBufferCallbackManager(RSSurfaceBufferCallbackManager&&) = delete;
    RSSurfaceBufferCallbackManager& operator=(const RSSurfaceBufferCallbackManager&) = delete;
    RSSurfaceBufferCallbackManager& operator=(RSSurfaceBufferCallbackManager&&) = delete;

    sptr<RSISurfaceBufferCallback> GetSurfaceBufferCallback(pid_t pid, uint64_t uid) const;
    size_t GetSurfaceBufferCallbackSize() const;

#ifdef ROSEN_OHOS
    void OnAfterAcquireBuffer(const Drawing::DrawSurfaceBufferAfterAcquireCbData& data);
    void OnFinish(const Drawing::DrawSurfaceBufferFinishCbData& data);
#endif
 
#ifdef ROSEN_OHOS
    void EnqueueSurfaceBufferId(const Drawing::DrawSurfaceBufferFinishCbData& data);
#endif
    void RequestNextVSync();

    static std::string SerializeBufferIdVec(const std::vector<uint32_t>& bufferIdVec);

    std::map<std::pair<pid_t, uint64_t>, sptr<RSISurfaceBufferCallback>>
        surfaceBufferCallbacks_;
    std::map<std::pair<pid_t, uint64_t>, BufferQueueData> stagingSurfaceBufferIds_;
    mutable std::shared_mutex registerSurfaceBufferCallbackMutex_;
    std::mutex surfaceBufferOpItemMutex_;
    std::function<void(std::function<void()>)> runPolicy_ = [](auto task) {
        std::invoke(task);
    };
    VSyncFuncs vSyncFuncs_;
    bool isUniRender_ = {};
};

namespace RSSurfaceBufferCallbackMgrUtil {
namespace Detail {
template<class... Iters, class UnaryPred, size_t... Is>
std::tuple<Iters...> FindIf(std::tuple<Iters...> begin, std::tuple<Iters...> end,
    UnaryPred unaryPred, std::index_sequence<Is...>)
{
    for (; begin != end; (++std::get<Is>(begin), ...)) {
        if (unaryPred(begin)) {
            return begin;
        }
    }
    return end;
}
 
template<class... Iters, class UnaryPred, size_t... Is>
std::tuple<Iters...> RemoveIf(std::tuple<Iters...> begin, std::tuple<Iters...> end,
    UnaryPred unaryPred, std::index_sequence<Is...>)
{
    begin = Detail::FindIf(begin, end, unaryPred, std::make_index_sequence<sizeof...(Iters)>());
    if (begin != end) {
        for (auto i = begin; i != end; (++std::get<Is>(i), ...)) {
            if (!unaryPred(i)) {
                ((*(std::get<Is>(begin)++) = std::move(*std::get<Is>(i))), ...);
            }
        }
    }
    return begin;
}
 
template <class... InputIters, class... OutPutIters, class UnaryPred, size_t... Is>
std::tuple<OutPutIters...> CopyIf(std::tuple<InputIters...> inputBegin,
    std::tuple<InputIters...> inputEnd, std::tuple<OutPutIters...> outputBegin, UnaryPred unaryPred,
    std::index_sequence<Is...>)
{
    for (; inputBegin != inputEnd; (++std::get<Is>(inputBegin), ...)) {
        if (unaryPred(inputBegin))
        {
            ((*std::get<Is>(outputBegin) = *std::get<Is>(inputBegin)), ...);
            (++std::get<Is>(outputBegin), ...);
        }
    }
    return outputBegin;
}
} // namespace Detail
 
template<class... Iters, class UnaryPred>
std::tuple<Iters...> FindIf(std::tuple<Iters...> begin,
    std::tuple<Iters...> end, UnaryPred unaryPred)
{
    return Detail::FindIf(begin, end, unaryPred, std::make_index_sequence<sizeof...(Iters)>());
}
 
template<class... Iters, class UnaryPred>
std::tuple<Iters...> RemoveIf(std::tuple<Iters...> begin,
    std::tuple<Iters...> end, UnaryPred unaryPred)
{
    return Detail::RemoveIf(begin, end, unaryPred, std::make_index_sequence<sizeof...(Iters)>());
}
 
template<class... InputIters, class... OutPutIters, class UnaryPred>
std::tuple<OutPutIters...> CopyIf(std::tuple<InputIters...> inputBegin,
    std::tuple<InputIters...> inputEnd, std::tuple<OutPutIters...> outputBegin, UnaryPred unaryPred)
{
    static_assert(sizeof...(InputIters) == sizeof...(OutPutIters));
    return Detail::CopyIf(inputBegin, inputEnd, outputBegin, unaryPred,
        std::make_index_sequence<sizeof...(InputIters)>());
}
} // namespace RSSurfaceBufferCallbackMgrUtil
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PIPELINE_RS_SURFACE_BUFFER_CALLBACK_MANAGER_H