/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_NODE_VISITOR_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_NODE_VISITOR_H

#include <memory>

namespace OHOS {
namespace Rosen {

class RSBaseRenderNode;
class RSDisplayRenderNode;
class RSRenderNode;
class RSSurfaceRenderNode;
class RSRootRenderNode;

class RSNodeVisitor : public std::enable_shared_from_this<RSNodeVisitor> {
public:
    RSNodeVisitor() {}
    virtual ~RSNodeVisitor() {}
    virtual void PrepareBaseRenderNode(RSBaseRenderNode& node) = 0;
    virtual void PrepareDisplayRenderNode(RSDisplayRenderNode& node) = 0;
    virtual void PrepareRenderNode(RSRenderNode& node) = 0;
    virtual void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) = 0;
    virtual void PrepareRootRenderNode(RSRootRenderNode& node) = 0;

    virtual void ProcessBaseRenderNode(RSBaseRenderNode& node) = 0;
    virtual void ProcessDisplayRenderNode(RSDisplayRenderNode& node) = 0;
    virtual void ProcessRenderNode(RSRenderNode& node) = 0;
    virtual void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) = 0;
    virtual void ProcessRootRenderNode(RSRootRenderNode& node) = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_NODE_VISITOR_H
