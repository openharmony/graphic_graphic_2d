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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H

#include "ui/rs_base_node.h"

class SkCanvas;
namespace OHOS {
namespace Rosen {

class RS_EXPORT RSDisplayNode : public RSBaseNode {
public:
    using WeakPtr = std::weak_ptr<RSDisplayNode>;
    using SharedPtr = std::shared_ptr<RSDisplayNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::DISPLAY_NODE;

    virtual ~RSDisplayNode();

    static SharedPtr Create(const RSDisplayNodeConfig& displayNodeConfig);

    RSUINodeType GetType() const override
    {
        return RSUINodeType::DISPLAY_NODE;
    }

protected:
    explicit RSDisplayNode(const RSDisplayNodeConfig& config);
    RSDisplayNode(const RSDisplayNode&) = delete;
    RSDisplayNode(const RSDisplayNode&&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&&) = delete;

private:
    uint64_t screenId_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H
