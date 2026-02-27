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

#include <limits>
#include <unordered_map>

#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
using SubWindowId = uint32_t;
constexpr SubWindowId INVALID_SUB_WINDOW_ID = std::numeric_limits<SubWindowId>::max();

enum class SubWindowOrder : uint8_t {
    BELOW_MAIN = 0,
    ABOVE_MAIN,
    ABSOLUTE,
};

struct SubWindowOptions {
    Vector4f bounds;
    SubWindowOrder order = SubWindowOrder::ABOVE_MAIN;
    float absoluteZ = 0.0f;
    uint32_t contentBgColor = 0x00000000;
};

struct SubWindowEntry {
    std::shared_ptr<RSSurfaceNode> leashNode;
    std::shared_ptr<RSSurfaceNode> contentNode;
};

class RSGraphicRootNode final {
public:
    RSGraphicRootNode();

    void AddChild(std::shared_ptr<RSNode> child, int index = -1);
    void RemoveChild(std::shared_ptr<RSNode> child);
    void ClearChildren();
    void RegisterNode(std::shared_ptr<RSNode> node);

    // Sub-window management
    void SetScreenId(ScreenId id);
    void SetMainWindowZ(float mainZ);
    float GetMainWindowZ() const;

    SubWindowId CreateSubWindow(const SubWindowOptions& options);
    std::shared_ptr<RSSurfaceNode> GetSubWindow(SubWindowId id) const;
    std::shared_ptr<RSSurfaceNode> GetSubWindowTestSurface(SubWindowId id) const;
    bool AddChildToSubWindow(SubWindowId id, std::shared_ptr<RSNode> child, int childIndex = -1);
    bool RemoveSubWindow(SubWindowId id);
    void ResetSubWindows();
    bool HasSubWindows() const;

private:
    void SetTestSurface(std::shared_ptr<OHOS::Rosen::RSSurfaceNode> node);
    void ResetTestSurface();
    std::shared_ptr<RSSurfaceNode> CreateContentNode(
        SubWindowId id, const SubWindowOptions& options);

    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> screenSurfaceNode_;
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> testSurfaceNode_;

    std::vector<std::shared_ptr<OHOS::Rosen::RSSurfaceNode>> testSurfaceNodes_;
    std::vector<std::shared_ptr<RSNode>> nodes_;

    std::unordered_map<SubWindowId, SubWindowEntry> subWindows_;
    SubWindowId nextSubWindowId_ = 0;
    ScreenId screenId_ = 0;
    float mainWindowZ_ = 0.0f;

    friend class RSGraphicTestDirector;
    friend class RSGraphicTest;
    friend class RSGraphicTestProfiler;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_GRAPHIC_ROOTNODE_H
