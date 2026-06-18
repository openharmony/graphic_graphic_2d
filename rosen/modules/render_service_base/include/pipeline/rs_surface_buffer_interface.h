/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_BUFFER_INTERFACE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_BUFFER_INTERFACE_H

#include <memory>
#include <set>
#include <string>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSSurfaceRenderNode;

class RSB_EXPORT RSSurfaceBufferInterface {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceBufferInterface>;
    using SharedPtr = std::shared_ptr<RSSurfaceBufferInterface>;

    virtual ~RSSurfaceBufferInterface() = default;

    virtual NodeId GetId() const = 0;

    virtual const std::string& GetName() const = 0;

    virtual RSSurfaceRenderNode* AsRSSurfaceRenderNode()
    {
        return nullptr;
    }

    virtual bool OnBufferAvailable()
    {
        return false;
    }

    virtual void OnTunnelHandleChange() {}

    virtual void OnCleanCache(std::set<uint64_t>& bufferCacheSet) {}

    virtual void OnSurfaceGoBackground() {}

    virtual void OnTransformChange() {}

    virtual void NeedClearBufferCache(std::set<uint64_t>& bufferCacheSet) {}
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_SURFACE_BUFFER_INTERFACE_H