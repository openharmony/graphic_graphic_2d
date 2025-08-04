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
#ifndef ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_SOUND_WAVE_FILTER_H
#define ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_SOUND_WAVE_FILTER_H
#include <memory>
#include "ui_effect/property/include/rs_ui_filter_para_base.h"

#include "common/rs_vector2.h"
#include "modifier/rs_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

class RSUISoundWaveFilterPara : public RSUIFilterParaBase {
public:
    RSUISoundWaveFilterPara() : RSUIFilterParaBase(RSUIFilterType::SOUND_WAVE) {}
    virtual ~RSUISoundWaveFilterPara() = default;

    bool Equals(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void Dump(std::string& out) const override;

    virtual void SetProperty(const std::shared_ptr<RSUIFilterParaBase>& other) override;

    void SetColors(Vector4f colorA, Vector4f colorB, Vector4f colorC);
    void SetColorProgress(float progress);
    void SetSoundIntensity(float intensity);
    void SetShockWaveAlphaA(float alpha);
    void SetShockWaveAlphaB(float alpha);
    void SetShockWaveProgressA(float progress);
    void SetShockWaveProgressB(float progress);
    void SetShockWaveTotalAlpha(float alpha);

    virtual std::shared_ptr<RSRenderFilterParaBase> CreateRSRenderFilter() override;

    virtual std::vector<std::shared_ptr<RSPropertyBase>> GetLeafProperties() override;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_CLIENT_CORE_UI_EFFECT_SOUND_WAVE_FILTER_H