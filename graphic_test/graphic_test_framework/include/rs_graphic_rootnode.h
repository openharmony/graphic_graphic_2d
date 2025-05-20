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

#ifndef RS_GRAPHIC_ROOTNODE_H
#define RS_GRAPHIC_ROOTNODE_H

#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
class RSGraphicRootNode final {
public:
    RSGraphicRootNode();

    void AddChild(std::shared_ptr<RSNode> child, int index = -1);
    void RemoveChild(std::shared_ptr<RSNode> child);
    void ClearChildren();
    void RegisterNode(std::shared_ptr<RSNode> node);

private:
    void SetTestSurface(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> node);
    void ResetTestSurface();

    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> screenSurfaceNode_;
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> testSurfaceNode_;

    std::vector<std::shared_ptr<OHOS::Rosen::RSSurfaceNode>> testSurfaceNodes_;
    std::vector<std::shared_ptr<RSNode>> nodes_;

    friend class RSGraphicTestDirector;
    friend class RSGraphicTest;
    friend class RSGraphicTestProfiler;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_ROOTNODE_H
