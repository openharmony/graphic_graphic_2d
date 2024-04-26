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

#include "skia_hm_symbol_config_ohos.h"

#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {

DrawingSymbolLayersGroups SkiaHmSymbolConfigOhos::GetSymbolLayersGroups(uint32_t glyphId)
{
#if !defined(ANDROID_PLATFORM) && !defined(IOS_PLATFORM)
    SymbolLayersGroups groups = HmSymbolConfig_OHOS::GetInstance()->GetSymbolLayersGroups(glyphId);

    DrawingSymbolLayersGroups drawingGroups;
    drawingGroups.symbolGlyphId = groups.symbolGlyphId;
    drawingGroups.layers = groups.layers;
    std::vector<DrawingAnimationSetting> drawingSettings;
    auto settings = groups.animationSettings;
    std::map<DrawingSymbolRenderingStrategy, std::vector<DrawingRenderGroup>> drawingRenderModeGroups;
    auto renderModeGroups = groups.renderModeGroups;
    for (size_t i = 0; i < settings.size(); i++) {
        drawingSettings.push_back(ConvertToDrawingAnimationSetting(settings.at(i)));
    }

    auto iter = renderModeGroups.begin();
    while (iter != renderModeGroups.end()) {
        auto renderGroups = iter->second;
        std::vector<DrawingRenderGroup> drawingRenderGroups;
        for (size_t j = 0; j < renderGroups.size(); j++) {
            drawingRenderGroups.push_back(ConvertToDrawingRenderGroup(renderGroups.at(j)));
        }
        auto key = static_cast<DrawingSymbolRenderingStrategy>(iter->first);
        drawingRenderModeGroups[key] = drawingRenderGroups;
        iter++;
    }

    drawingGroups.animationSettings = drawingSettings;
    drawingGroups.renderModeGroups = drawingRenderModeGroups;

    return drawingGroups;
#endif
    DrawingSymbolLayersGroups drawingGroups;
    return drawingGroups;
}

DrawingAnimationSetting SkiaHmSymbolConfigOhos::ConvertToDrawingAnimationSetting(AnimationSetting setting)
{
    DrawingAnimationSetting drawingSetting;
    for (size_t i = 0; i <setting.animationTypes.size(); i++) {
        DrawingAnimationType animationType = static_cast<DrawingAnimationType>(setting.animationTypes[i]);
        drawingSetting.animationTypes.push_back(animationType);
    }

    std::vector<DrawingGroupSetting> groupSettings;
    for (size_t i = 0; i < setting.groupSettings.size(); i++) {
        DrawingGroupSetting groupSetting;
        groupSetting.animationIndex = setting.groupSettings.at(i).animationIndex;
        groupSetting.groupInfos = ConvertToDrawingGroupInfo(setting.groupSettings.at(i).groupInfos);
        groupSettings.push_back(groupSetting);
    }
    drawingSetting.groupSettings = groupSettings;

    return drawingSetting;
}

DrawingRenderGroup SkiaHmSymbolConfigOhos::ConvertToDrawingRenderGroup(RenderGroup group)
{
    DrawingRenderGroup drawingRenderGroup;
    drawingRenderGroup.color.a = group.color.a;
    drawingRenderGroup.color.r = group.color.r;
    drawingRenderGroup.color.g = group.color.g;
    drawingRenderGroup.color.b = group.color.b;
    drawingRenderGroup.groupInfos = ConvertToDrawingGroupInfo(group.groupInfos);

    return drawingRenderGroup;
}

std::vector<DrawingGroupInfo> SkiaHmSymbolConfigOhos::ConvertToDrawingGroupInfo(std::vector<GroupInfo> infos)
{
    std::vector<DrawingGroupInfo> groupInfos;
    for (size_t i = 0; i < infos.size(); i++) {
        DrawingGroupInfo groupInfo;
        groupInfo.layerIndexes = infos.at(i).layerIndexes;
        groupInfo.maskIndexes = infos.at(i).maskIndexes;
        groupInfos.push_back(groupInfo);
    }
    return groupInfos;
}

static DrawingPiecewiseParameter ConvertPiecewiseParameter(const PiecewiseParameter& in)
{
    DrawingPiecewiseParameter out;
    out.curveType = static_cast<DrawingCurveType>(in.curveType);
    out.duration = in.duration;
    out.delay = in.delay;
    std::copy(in.curveArgs.begin(), in.curveArgs.end(),
        std::inserter(out.curveArgs, out.curveArgs.begin()));
    std::copy(in.properties.begin(), in.properties.end(),
        std::inserter(out.properties, out.properties.begin()));
    return out;
}

static std::vector<DrawingPiecewiseParameter> ConvertPiecewiseParametersVec(const std::vector<PiecewiseParameter>& in)
{
    std::vector<DrawingPiecewiseParameter> out;
    for (auto& tmp : in) {
        out.push_back(ConvertPiecewiseParameter(tmp));
    }
    return out;
}

#if !defined(ANDROID_PLATFORM) && !defined(IOS_PLATFORM)
std::vector<std::vector<DrawingPiecewiseParameter>> SkiaHmSymbolConfigOhos::GetGroupParameters(
    DrawingAnimationType type, uint16_t groupSum, uint16_t animationMode, DrawingCommonSubType commonSubType)
{
    auto animationType = static_cast<AnimationType>(type);
    auto subType = static_cast<CommonSubType>(commonSubType);
    auto parametersPtr = HmSymbolConfig_OHOS::GetInstance()->GetGroupParameters(
        animationType, groupSum, animationMode, subType);

    std::vector<std::vector<DrawingPiecewiseParameter>> parameters;
    for (auto& paraTmp : parametersPtr) {
        parameters.push_back(ConvertPiecewiseParametersVec(paraTmp));
    }
    return parameters;
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS