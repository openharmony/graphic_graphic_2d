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

#ifndef ROSEN_RENDER_SERVICE_BASE_RENDER_UI_EFFECTRS_FILTER_H
#define ROSEN_RENDER_SERVICE_BASE_RENDER_UI_EFFECTRS_FILTER_H

#include <memory>
#include <stdint.h>

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "image/gpu_context.h"
#ifdef USE_M133_SKIA
#include "src/core/SkChecksum.h"
#else
#include "src/core/SkOpts.h"
#endif

namespace OHOS {
namespace Rosen {
constexpr float BLUR_SIGMA_SCALE = 0.57735f;
enum BLUR_COLOR_MODE : int {
    PRE_DEFINED = 0,           // use the pre-defined mask color
    AVERAGE     = 1,           // use the average color of the blurred area as mask color
    FASTAVERAGE = 2,
    DEFAULT     = PRE_DEFINED
};

class RSB_EXPORT RSFilter : public std::enable_shared_from_this<RSFilter> {
public:
    virtual ~RSFilter();
    RSFilter(const RSFilter&) = delete;
    RSFilter(const RSFilter&&) = delete;
    RSFilter& operator=(const RSFilter&) = delete;
    RSFilter& operator=(const RSFilter&&) = delete;
    virtual std::string GetDescription();
    virtual std::string GetDetailedDescription();
    static std::shared_ptr<RSFilter> CreateBlurFilter(float blurRadiusX, float blurRadiusY,
        bool disableSystemAdaptation = true);
    static std::shared_ptr<RSFilter> CreateMaterialFilter(
        int style, float dipScale, BLUR_COLOR_MODE mode = DEFAULT, float ratio = 1.0,
        bool disableSystemAdaptation = true);
    static std::shared_ptr<RSFilter> CreateMaterialFilter(
        float radius, float saturation, float brightness, uint32_t colorValue,
        BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT,
        bool disableSystemAdaptation = true);
    static std::shared_ptr<RSFilter> CreateLightUpEffectFilter(float lightUpDegree);
    static float RadiusVp2Sigma(float radiusVp, float dipScale);

    enum FilterType {
        NONE = 0,
        BLUR,
        MATERIAL,
        LIGHT_UP_EFFECT,
        AIBAR,
        LINEAR_GRADIENT_BLUR,
        FOREGROUND_EFFECT,
        MOTION_BLUR,
        SPHERIZE_EFFECT,
        COLORFUL_SHADOW,
        ATTRACTION_EFFECT,
        WATER_RIPPLE,
        COMPOUND_EFFECT,
        MAGNIFIER,
        FLY_OUT,
        DISTORT,
        ALWAYS_SNAPSHOT,
        DISPLACEMENT_DISTORT,
        COLOR_GRADIENT,
        SOUND_WAVE,
        EDGE_LIGHT,
        HDR_UI_BRIGHTNESS,
        BEZIER_WARP,
        DISPERSION,
        CONTENT_LIGHT,
    };
    FilterType GetFilterType() const
    {
        return type_;
    }

    void SetFilterType(FilterType type)
    {
        type_ = type;
#ifdef USE_M133_SKIA
        const auto hashFunc = SkChecksum::Hash32;
#else
        const auto hashFunc = SkOpts::hash;
#endif
        hash_ = hashFunc(&type_, sizeof(type_), hash_);
    }

    virtual bool IsValid() const
    {
        return type_ != FilterType::NONE;
    }

    float DecreasePrecision(float value)
    {
        // preserve two digital precision when calculating hash, this can reuse filterCache as much as possible.
        return 0.01 * round(value * 100);
    }

    virtual uint32_t Hash() const
    {
        return hash_;
    }

    virtual void OnSync() {}

    bool NeedSnapshotOutset() const
    {
        return needSnapshotOutset_;
    }

    void SetSnapshotOutset(bool needSnapshotOutset)
    {
        needSnapshotOutset_ = needSnapshotOutset;
    }

    bool GetHpaeCallback() const {
        return hasHpae_;
    }

protected:
    bool hasHpae_ = false;
    FilterType type_;
    uint32_t hash_ = 0;
    bool needSnapshotOutset_ = true;
    RSFilter();
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RENDER_UI_EFFECTRS_FILTER_H
