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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_DEPTH_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_DEPTH_NODE_H

#include "property/rs_properties_def.h"
#include "render/rs_image.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
class RSNodeMap;

class RSC_EXPORT RSDepthNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSDepthNode>;
    using SharedPtr = std::shared_ptr<RSDepthNode>;
    RSUINodeType GetType() const override
    {
        return RSUINodeType::UNKNOWN;
    }

    static SharedPtr Create(bool isRenderServiceNode, bool isTextureExportNode,
        std::shared_ptr<RSUIContext> rsUIContext);

    void SetDepthSpaceType(DepthSpaceType spaceType);
    void SetDepthImage(const std::shared_ptr<RSImage>& depthImage);
    void SetDepthCameraPara(const DepthCameraPara& cameraPara);
    void SetDepthLightPara(const DepthLightPara& lightPara);

protected:
    RSDepthNode(bool isRenderServiceNode, bool isTextureExportNode, std::shared_ptr<RSUIContext> rsUIContext);
    RSDepthNode(const RSDepthNode&) = delete;
    RSDepthNode(const RSDepthNode&&) = delete;
    RSDepthNode& operator=(const RSDepthNode&) = delete;
    RSDepthNode& operator=(const RSDepthNode&&) = delete;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_DEPTH_NODE_H