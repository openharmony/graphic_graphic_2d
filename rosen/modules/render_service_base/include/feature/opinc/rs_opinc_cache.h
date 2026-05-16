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

#ifndef RS_OPINC_CACHE_H
#define RS_OPINC_CACHE_H

#include "common/rs_macros.h"
#include "common/rs_rect.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSOpincCache {
public:
    RSOpincCache();
    ~RSOpincCache();

    bool GetSubTreeSupportFlag() const;

    void SetSubTreeSupportFlag(bool supportFlag);

    void UpdateSubTreeSupportFlag(bool childSupportFlag, bool childRootFlag, bool groupTypeIsNone);

    bool GetCurNodeTreeSupportFlag() const;

    void SetCurNodeTreeSupportFlag(bool curNodeTreeSupportFlag);

    bool HasUnstableOpincNode() const;

    void SetHasUnstableOpincNode(bool hasUnstableOpincNode);

    void UpdateSubTreeHasUnstableOpincNode(RSOpincCache& childOpincCache,
        bool childRootHasUnstableOpincNode);

    void OpincSetInAppStateStart(bool& unchangeMarkInApp);

    void OpincSetInAppStateEnd(bool& unchangeMarkInApp);

    void MarkMaterialNode(bool isMaterialNode);

    bool IsMaterialNode() const;

    void SetLayerPartRenderDirtyFlag(bool dirtyFlag);

    bool GetLayerPartRenderDirtyFlag() const;

    void SetLayerPartRenderOldAbsDrawRect(const RectI& oldAbsDrawRect);

    const RectI& GetLayerPartRenderOldAbsDrawRect() const;
private:
    // opinc state
    bool subTreeSupportFlag_ = true;
    bool curNodeTreeSupportFlag_ = false;
    bool hasUnstableOpincNode_ = false;

    // layer part render
    bool isUnchangeMarkInApp_ = false;
    bool isMaterialNode_ = false;
    bool layerPartRenderDirtyFlag_ = false;
    RectI oldAbsDrawRect_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_OPINC_CACHE_H