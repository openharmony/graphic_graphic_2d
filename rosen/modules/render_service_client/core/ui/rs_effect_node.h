/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H

#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSEffectNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSEffectNode>;
    using SharedPtr = std::shared_ptr<RSEffectNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::EFFECT_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSEffectNode() override = default;

    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false);
    void SetFreeze(bool isFreeze) override;

protected:
    RSEffectNode(bool isRenderServiceNode, bool isTextureExportNode = false);
    RSEffectNode(const RSEffectNode&) = delete;
    RSEffectNode(const RSEffectNode&&) = delete;
    RSEffectNode& operator=(const RSEffectNode&) = delete;
    RSEffectNode& operator=(const RSEffectNode&&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_EFFECT_NODE_H
