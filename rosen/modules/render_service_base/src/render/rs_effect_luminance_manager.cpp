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
#include "render/rs_effect_luminance_manager.h"

#include <algorithm>
#include <mutex>
#include <vector>

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

namespace {

// for type inference
using ColorGradientEffectColorRenderTags = std::tuple<
    ColorGradientEffectColor0RenderTag,
    ColorGradientEffectColor1RenderTag,
    ColorGradientEffectColor2RenderTag,
    ColorGradientEffectColor3RenderTag,
    ColorGradientEffectColor4RenderTag,
    ColorGradientEffectColor5RenderTag,
    ColorGradientEffectColor6RenderTag,
    ColorGradientEffectColor7RenderTag,
    ColorGradientEffectColor8RenderTag,
    ColorGradientEffectColor9RenderTag,
    ColorGradientEffectColor10RenderTag,
    ColorGradientEffectColor11RenderTag>;

using FilterEDRChecker = std::function<bool(std::shared_ptr<RSNGRenderFilterBase>)>;
using ShaderEDRChecker = std::function<bool(std::shared_ptr<RSNGRenderShaderBase>)>;

constexpr uint32_t COLOR_PROPS_NUM = 4;

bool GetEnableEDREffectEdgeLight(std::shared_ptr<RSNGRenderFilterBase> renderFilter)
{
    auto filter = std::static_pointer_cast<RSNGRenderEdgeLightFilter>(renderFilter);
    if (!filter) {
        return false;
    }
    const auto& color = filter->Getter<EdgeLightColorRenderTag>()->Get();
    return ROSEN_GNE(color.x_, 1.0f) || ROSEN_GNE(color.y_, 1.0f) || ROSEN_GNE(color.z_, 1.0f);
}

bool GetEnableEDREffectColorGradient(std::shared_ptr<RSNGRenderFilterBase> renderFilter)
{
    auto filter = std::static_pointer_cast<RSNGRenderColorGradientFilter>(renderFilter);
    if (!filter) {
        return false;
    }
    const auto& colors = filter->Getter<ColorGradientColorsRenderTag>()->Get();
    for (size_t i = 0; i < colors.size(); i++) {
        if ((i + 1) % COLOR_PROPS_NUM == 0) {
            continue;
        }
        if (ROSEN_GNE(colors[i], 1.0f)) {
            return true;
        }
    }
    return false;
}

bool GetEnableEDREffectSoundWave(std::shared_ptr<RSNGRenderFilterBase> renderFilter)
{
    auto filter = std::static_pointer_cast<RSNGRenderSoundWaveFilter>(renderFilter);
    if (!filter) {
        return false;
    }
    const auto& colorA = filter->Getter<SoundWaveColorARenderTag>()->Get();
    const auto& colorB = filter->Getter<SoundWaveColorBRenderTag>()->Get();
    const auto& colorC = filter->Getter<SoundWaveColorCRenderTag>()->Get();
    return ROSEN_GNE(colorA.x_, 1.0f) || ROSEN_GNE(colorA.y_, 1.0f) || ROSEN_GNE(colorA.z_, 1.0f) ||
           ROSEN_GNE(colorB.x_, 1.0f) || ROSEN_GNE(colorB.y_, 1.0f) || ROSEN_GNE(colorB.z_, 1.0f) ||
           ROSEN_GNE(colorC.x_, 1.0f) || ROSEN_GNE(colorC.y_, 1.0f) || ROSEN_GNE(colorC.z_, 1.0f);
}

bool GetEnableEDRShaderColorGradient(std::shared_ptr<RSNGRenderShaderBase> renderShader)
{
    auto shader = std::static_pointer_cast<RSNGRenderColorGradientEffect>(renderShader);
    if (!shader) {
        return false;
    }
    bool isUseEDR = false;
    auto lamba = [&isUseEDR](const Vector4f& v4) {
        if (ROSEN_GNE(v4.x_, 1.0f) || ROSEN_GNE(v4.y_, 1.0f) || ROSEN_GNE(v4.z_, 1.0f)) {
            isUseEDR = true;
        }
    };
    ColorGradientEffectColorRenderTags colorTag{};
    std::apply([&shader, &lamba](auto&&... args) {
        (lamba(shader->Getter<std::decay_t<decltype(args)>>()->Get()), ...);
        }, colorTag);
    return isUseEDR;
}

bool GetEnableEDRShaderRoundedRectFlowlight(std::shared_ptr<RSNGRenderShaderBase> renderFilter)
{
    auto shader = std::static_pointer_cast<RSNGRenderRoundedRectFlowlight>(renderShader);
    if (!shader) {
        return false;
    }
    const auto& color = shader->Getter<RoundedRectFlowlightColorRenderTag>()->Get();
    const auto& brightness = shader->Getter<RoundedRectFlowlightBrightnessRenderTag>()->Get();
    return ROSEN_GNE(color.x_, 1.0f) || ROSEN_GNE(color.y_, 1.0f) || ROSEN_GNE(color.z_, 1.0f) ||
           ROSEN_GNE(brightness.x_, 1.0f);
}

bool GetEnableEDRShaderAIBarGlow(std::shared_ptr<RSNGRenderShaderBase> renderFilter)
{
    auto shader = std::static_pointer_cast<RSNGRenderAIBarGlow>(renderShader);
    if (!shader) {
        return false;
    }
    const auto& color0 = shader->Getter<AIBarGlowColor0RenderTag>()->Get();
    const auto& color1 = shader->Getter<AIBarGlowColor1RenderTag>()->Get();
    const auto& color2 = shader->Getter<AIBarGlowColor2RenderTag>()->Get();
    const auto& color3 = shader->Getter<AIBarGlowColor3RenderTag>()->Get();
    const auto& brightness = shader->Getter<AIBarGlowBrightnessRenderTag>()->Get();
    return ROSEN_GNE(color0.x_, 1.0f) || ROSEN_GNE(color0.y_, 1.0f) || ROSEN_GNE(color0.z_, 1.0f) ||
           ROSEN_GNE(color1.x_, 1.0f) || ROSEN_GNE(color1.y_, 1.0f) || ROSEN_GNE(color1.z_, 1.0f) ||
           ROSEN_GNE(color2.x_, 1.0f) || ROSEN_GNE(color2.y_, 1.0f) || ROSEN_GNE(color2.z_, 1.0f) ||
           ROSEN_GNE(color3.x_, 1.0f) || ROSEN_GNE(color3.y_, 1.0f) || ROSEN_GNE(color3.z_, 1.0f) ||
           ROSEN_GNE(brightness.x_, 1.0f);
}

static std::unordered_map<RSNGEffectType, FilterEDRChecker> edrFilterCheckerLUT = {
    {RSNGEffectType::EDGE_LIGHT, GetEnableEDREffectEdgeLight},
    {RSNGEffectType::COLOR_GRADIENT, GetEnableEDREffectColorGradient},
    {RSNGEffectType::SOUND_WAVE, GetEnableEDREffectSoundWave},
};
 
static std::unordered_map<RSNGEffectType, ShaderEDRChecker> edrShaderCheckerLUT = {
    {RSNGEffectType::COLOR_GRADIENT_EFFECT, GetEnableEDRShaderColorGradient},
    {RSNGEffectType::ROUNDED_RECT_FLOWLIGHT, GetEnableEDRShaderRoundedRectFlowlight},
    {RSNGEffectType::AIBAR_GLOW, GetEnableEDRShaderAIBarGlow},
};

std::mutex g_dataMutex;
}  // namespace

RSEffectLuminanceManager& RSEffectLuminanceManager::GetInstance()
{
    static RSEffectLuminanceManager instance;
    return instance;
}

void RSEffectLuminanceManager::SetDisplayHeadroom(NodeId id, float headroom)
{
    std::lock_guard<std::mutex> lock(g_dataMutex);
    displayHeadroomMap_[static_cast<uint64_t>(id)] = headroom;
}

float RSEffectLuminanceManager::GetDisplayHeadroom(NodeId id) const
{
    std::lock_guard<std::mutex> lock(g_dataMutex);
    auto iter = displayHeadroomMap_.find(static_cast<uint64_t>(id));
    if (iter != displayHeadroomMap_.end()) {
        return iter->second;
    }
    return 1.0f;
}

bool RSEffectLuminanceManager::GetEnableHdrEffect(std::shared_ptr<RSNGRenderFilterBase> renderFilter)
{
    if (!renderFilter) {
        return false;
    }

    auto it = edrFilterCheckerLUT.find(renderFilter->GetType());
    return it != edrFilterCheckerLUT.end() ? it->second(renderFilter) : false;
}
 
bool RSEffectLuminanceManager::GetEnableHdrEffect(std::shared_ptr<RSNGRenderShaderBase> renderShader)
{
    if (!renderShader) {
        return false;
    }

    auto it = edrShaderCheckerLUT.find(renderShader->GetType());
    return it != edrShaderCheckerLUT.end() ? it->second(renderShader) : false;
}
}  // Rosen
} // OHOS