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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H

#include <string>

#include <parcel.h>
#include <refbase.h>
#include "surface.h"

#include <transaction/rs_transaction_proxy.h>
#include "platform/drawing/rs_surface.h"
#include "ui/rs_node.h"
#include "surface_type.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {

using FirstTimeOnScreenCallback = std::function<void()>;
struct RSSurfaceNodeConfig {
    std::string SurfaceNodeName = "SurfaceNode";
};

class RS_EXPORT RSSurfaceNode : public RSNode, public Parcelable {
public:
    using WeakPtr = std::weak_ptr<RSSurfaceNode>;
    using SharedPtr = std::shared_ptr<RSSurfaceNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::SURFACE_NODE;

    virtual ~RSSurfaceNode();

    static SharedPtr Create(const RSSurfaceNodeConfig& surfaceNodeConfig, bool isWindow = true);

    void SetBounds(const Vector4f& bounds) override;
    void SetBounds(float positionX, float positionY, float width, float height) override;
    void SetBoundsSize(const Vector2f& size) override;
    void SetBoundsSize(float width, float height) override;
    void SetBoundsWidth(float width) override;
    void SetBoundsHeight(float height) override;
    void SetColorSpace(ColorGamut colorSpace);

    bool SetFirstTimeOnScreenCallback(FirstTimeOnScreenCallback callback);

    bool Marshalling(Parcel& parcel) const override;
    static RSSurfaceNode* Unmarshalling(Parcel& parcel);
    sptr<OHOS::Surface> GetSurface() const;
    RSUINodeType GetType() const override
    {
        return RSUINodeType::SURFACE_NODE;
    }
    ColorGamut GetColorSpace()
    {
        return colorSpace_;
    }
protected:
    bool NeedForcedSendToRemote() const override;
    explicit RSSurfaceNode(const RSSurfaceNodeConfig& config, bool isRenderServiceNode);
    RSSurfaceNode(const RSSurfaceNode&) = delete;
    RSSurfaceNode(const RSSurfaceNode&&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&) = delete;
    RSSurfaceNode& operator=(const RSSurfaceNode&&) = delete;

private:
    bool CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config);
    void UpdateSurfaceDefaultSize(float width, float height);
    std::shared_ptr<RSSurface> surface_;
    std::string name_;
    std::mutex mutex_;
    FirstTimeOnScreenCallback callback_;
    ColorGamut colorSpace_ = ColorGamut::COLOR_GAMUT_SRGB;

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    template<typename T>
    friend class RSPropertyAnimation;
    friend class RSSurfaceExtractor;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_NODE_H
