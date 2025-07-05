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

#ifndef RS_OPINC_MANAGER_H
#define RS_OPINC_MANAGER_H

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSOpincManager {
public:
    static RSOpincManager& Instance();

    bool GetOPIncSwitch() const
    {
        return isOPIncOn_;
    }

    void SetOPIncSwitch(bool opincSwitch)
    {
        isOPIncOn_ = opincSwitch;
    }

    bool OpincGetNodeSupportFlag(RSRenderNode& node);
    bool IsOpincSubTreeDirty(RSRenderNode& node, bool opincEnable);

private:
    RSOpincManager() = default;
    ~RSOpincManager() = default;
    RSOpincManager(const RSOpincManager&);
    RSOpincManager(const RSOpincManager&&);
    RSOpincManager& operator=(const RSOpincManager&);
    RSOpincManager& operator=(const RSOpincManager&&);

    bool OpincGetCanvasNodeSupportFlag(RSRenderNode& node);

    bool isOPIncOn_ = false;
};
}
#endif // RS_OPINC_MANAGER_H