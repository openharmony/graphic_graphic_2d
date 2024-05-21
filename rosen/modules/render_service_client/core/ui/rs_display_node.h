/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "ui/rs_node.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSDisplayNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSDisplayNode>;
    using SharedPtr = std::shared_ptr<RSDisplayNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::DISPLAY_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSDisplayNode() override;

    void ClearChildren() override;

    static SharedPtr Create(const RSDisplayNodeConfig& displayNodeConfig);

    bool Marshalling(Parcel& parcel) const;
    static SharedPtr Unmarshalling(Parcel& parcel);

    void SetScreenId(uint64_t screenId);

    void SetDisplayOffset(int32_t offsetX, int32_t offsetY);

    void SetSecurityDisplay(bool isSecurityDisplay);

    void SetScreenRotation(const uint32_t& rotation);

    void SetDisplayNodeMirrorConfig(const RSDisplayNodeConfig& displayNodeConfig);

    bool GetSecurityDisplay() const;

    bool IsMirrorDisplay() const;

    void SetBootAnimation(bool isBootAnimation);
    bool GetBootAnimation() const;

    void SetScbNodePid(const std::vector<int32_t>& oldPids, int32_t currentPid);

protected:
    explicit RSDisplayNode(const RSDisplayNodeConfig& config);
    RSDisplayNode(const RSDisplayNodeConfig& config, NodeId id);
    RSDisplayNode(const RSDisplayNode&) = delete;
    RSDisplayNode(const RSDisplayNode&&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&) = delete;
    RSDisplayNode& operator=(const RSDisplayNode&&) = delete;
    void OnBoundsSizeChanged() const override;

private:
    uint64_t screenId_;
    int32_t offsetX_;
    int32_t offsetY_;
    bool isSecurityDisplay_ = false;
    bool isMirroredDisplay_ = false;
    bool isBootAnimation_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_DISPLAY_NODE_H
