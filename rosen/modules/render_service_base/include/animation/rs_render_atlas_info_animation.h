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

#ifndef RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_ATLAS_INFO_ANIMATION_H
#define RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_ATLAS_INFO_ANIMATION_H

#include "animation/rs_render_property_animation.h"
#include "common/rs_atlas_info.h"

namespace OHOS {
namespace Rosen {

/**
 * AtlasInfo animation class (render side).
 *
 * Unlike RSRenderPropertyAnimation which uses standard value estimators,
 * this class implements OnAnimate() to do custom interpolation:
 *   fraction → interpolated currentFrame → ComputeRectInfo() → property_->Set()
 *
 * The animation operates on RSAnimatableProperty<AtlasInfo>, where only
 * currentFrame is interpolated and context fields (rows/cols/fps/etc) are
 * carried through from startValue_.
 */
class RSB_EXPORT RSRenderAtlasInfoAnimation : public RSRenderPropertyAnimation {
public:
    RSRenderAtlasInfoAnimation() = default;
    RSRenderAtlasInfoAnimation(AnimationId id, const PropertyId& propertyId,
        const std::shared_ptr<RSRenderPropertyBase>& originValue,
        const std::shared_ptr<RSRenderPropertyBase>& startValue,
        const std::shared_ptr<RSRenderPropertyBase>& endValue);
    ~RSRenderAtlasInfoAnimation() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static RSRenderAtlasInfoAnimation* Unmarshalling(Parcel& parcel);

protected:
    void OnSetPropertyOnAdd() override;
    bool OnAnimate(float fraction) override;
    void RebuildPropertyValue(float fraction) override;
    void InitValueEstimator() override;

private:
    bool ParseParam(Parcel& parcel) override;
    AtlasInfo startValue_;
    AtlasInfo endValue_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ANIMATION_RS_RENDER_ATLAS_INFO_ANIMATION_H
