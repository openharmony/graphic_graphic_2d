/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef TAIHE_INCLUDE_UIEFFECT_BLENDER_TAIHE_H
#define TAIHE_INCLUDE_UIEFFECT_BLENDER_TAIHE_H

#include <memory>
#include "ohos.graphics.uiEffect.uiEffect.proj.hpp"
#include "ohos.graphics.uiEffect.uiEffect.impl.hpp"
#include "taihe/runtime.hpp"

namespace OHOS::Rosen {
class Blender;
class BrightnessBlender;
class HdrDarkenBlender;
}

namespace ANI::UIEffect {
using namespace taihe;
using namespace ohos::graphics::uiEffect;
using namespace ohos::graphics::uiEffect::uiEffect;

class BlenderImpl {
public:
    BlenderImpl();
    explicit BlenderImpl(std::shared_ptr<OHOS::Rosen::Blender> blender);
    virtual ~BlenderImpl();

    virtual int64_t getNativePtr();

protected:
    bool IsBlenderValid() const;

    std::shared_ptr<OHOS::Rosen::Blender> nativeBlender_;
};

class BrightnessBlenderImpl : public BlenderImpl {
public:
    BrightnessBlenderImpl();
    explicit BrightnessBlenderImpl(std::shared_ptr<OHOS::Rosen::BrightnessBlender> blender);
    ~BrightnessBlenderImpl() override;

    double getCubicRate();
    void setCubicRate(double cubicRate);
    double getQuadraticRate();
    void setQuadraticRate(double quadraticRate);
    double getLinearRate();
    void setLinearRate(double linearRate);
    double getDegree();
    void setDegree(double degree);
    double getSaturation();
    void setSaturation(double saturation);
    Tuple3F getPositiveCoefficient();
    void setPositiveCoefficient(Tuple3F const& positiveCoefficient);
    Tuple3F getNegativeCoefficient();
    void setNegativeCoefficient(Tuple3F const& negativeCoefficient);
    double getFraction();
    void setFraction(double fraction);

protected:
    OHOS::Rosen::BrightnessBlender* GetBrightnessBlender();
};

class HdrBrightnessBlenderImpl : public BrightnessBlenderImpl {
public:
    HdrBrightnessBlenderImpl();
    explicit HdrBrightnessBlenderImpl(std::shared_ptr<OHOS::Rosen::BrightnessBlender> blender);
    ~HdrBrightnessBlenderImpl() override;
};

class HdrDarkenBlenderImpl : public BlenderImpl {
public:
    HdrDarkenBlenderImpl();
    explicit HdrDarkenBlenderImpl(std::shared_ptr<OHOS::Rosen::HdrDarkenBlender> blender);
    ~HdrDarkenBlenderImpl() override;

    double getHdrBrightnessRatio();
    void setHdrBrightnessRatio(double hdrBrightnessRatio);
    taihe::optional<Tuple3F> getGrayscaleFactor();
    void setGrayscaleFactor(taihe::optional<Tuple3F> const& grayscaleFactor);

private:
    OHOS::Rosen::HdrDarkenBlender* GetHdrDarkenBlender();
};

} // namespace ANI::UIEffect

#endif