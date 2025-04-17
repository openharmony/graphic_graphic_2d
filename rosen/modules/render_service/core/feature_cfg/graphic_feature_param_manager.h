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

#ifndef GRAPHIC_FEATURE_PARAM_MANAGER_H
#define GRAPHIC_FEATURE_PARAM_MANAGER_H

#include <list>
#include <mutex>
#include <unordered_map>

#include "refbase.h"
#include "xml_parser_base.h"

#include "accessibility_param_parse.h"
#include "accessibility_param.h"
#include "capture_base_param_parse.h"
#include "capture_base_param.h"
#include "dirtyregion_param.h"
#include "dirtyregion_param_parse.h"
#include "drm_param_parse.h"
#include "drm_param.h"
#include "color_gamut_param_parse.h"
#include "color_gamut_param.h"
#include "hard_cursor_param.h"
#include "hard_cursor_param_parse.h"
#include "hfbc_param_parse.h"
#include "hfbc_param.h"
#include "hwc_param_parse.h"
#include "hwc_param.h"
#include "mem_param_parse.h"
#include "mem_param.h"
#include "multiscreen_param_parse.h"
#include "multiscreen_param.h"
#include "occlusion_culling_param.h"
#include "occlusion_culling_param_parse.h"
#include "opinc_param_parse.h"
#include "opinc_param.h"
#include "prevalidate_param_parse.h"
#include "prevalidate_param.h"
#include "rotateoffscreen_param_parse.h"
#include "rotateoffscreen_param.h"
#include "speciallayer_param.h"
#include "speciallayer_param_parse.h"
#include "uifirst_param_parse.h"
#include "uifirst_param.h"
#include "vrate_param_parse.h"
#include "vrate_param.h"
#include "filter_param_parse.h"
#include "filter_param.h"
#include "dvsync_param_parse.h"
#include "dvsync_param.h"
#include "socperf_param_parse.h"
#include "socperf_param.h"
#include "surface_capture_param_parse.h"
#include "surface_capture_param.h"
#include "ui_capture_param_parse.h"
#include "ui_capture_param.h"

namespace OHOS::Rosen {
struct ModuleConfig {
    std::string name;
    std::function<std::unique_ptr<XMLParserBase>()> xmlParser;
    std::function<std::unique_ptr<FeatureParam>()> featureParam;
};
// add new module here
const std::vector<ModuleConfig> FEATURE_MODULES = {
    {FEATURE_CONFIGS[DIRTYREGION], [] { return std::make_unique<DirtyRegionParamParse>(); },
        [] { return std::make_unique<DirtyRegionParam>(); }},
    {FEATURE_CONFIGS[COLOR_GAMUT], [] {return std::make_unique<ColorGamutParamParse>(); },
        [] {return std::make_unique<ColorGamutParam>(); }},
    {FEATURE_CONFIGS[DRM], [] { return std::make_unique<DRMParamParse>(); },
        [] { return std::make_unique<DRMParam>(); }},
    {FEATURE_CONFIGS[HWC], [] {return std::make_unique<HWCParamParse>(); }, [] {return std::make_unique<HWCParam>(); }},
    {FEATURE_CONFIGS[HFBC], [] {return std::make_unique<HFBCParamParse>(); },
        [] {return std::make_unique<HFBCParam>(); }},
    {FEATURE_CONFIGS[MEM], [] { return std::make_unique<MEMParamParse>(); },
        [] { return std::make_unique<MEMParam>(); }},
    {FEATURE_CONFIGS[SPECIALLAYER], [] { return std::make_unique<SpecialLayerParamParse>(); },
        [] { return std::make_unique<SpecialLayerParam>(); }},
    {FEATURE_CONFIGS[OCCLUSION_CULLING], [] { return std::make_unique<OcclusionCullingParamParse>(); },
        [] {return std::make_unique<OcclusionCullingParam>(); }},
    {FEATURE_CONFIGS[OPInc], [] {return std::make_unique<OPIncParamParse>(); },
        [] {return std::make_unique<OPIncParam>(); }},
    {FEATURE_CONFIGS[MULTISCREEN], [] { return std::make_unique<MultiScreenParamParse>(); },
        [] {return std::make_unique<MultiScreenParam>(); }},
    {FEATURE_CONFIGS[PREVALIDATE], [] { return std::make_unique<PrevalidateParamParse>(); },
        [] { return std::make_unique<PrevalidateParam>(); }},
    {FEATURE_CONFIGS[UIFirst], [] {return std::make_unique<UIFirstParamParse>(); },
        [] {return std::make_unique<UIFirstParam>(); }},
    {FEATURE_CONFIGS[FILTER], [] { return std::make_unique<FilterParamParse>(); },
        [] { return std::make_unique<FilterParam>(); }},
    {FEATURE_CONFIGS[DVSYNC], [] { return std::make_unique<DVSyncParamParse>(); },
        [] { return std::make_unique<DVSyncParam>(); }},
    {FEATURE_CONFIGS[HARDCURSOR], [] { return std::make_unique<HardCursorParamParse>(); },
        [] { return std::make_unique<HardCursorParam>(); }},
    {FEATURE_CONFIGS[SOC_PERF], [] { return std::make_unique<SOCPerfParamParse>(); },
        [] { return std::make_unique<SOCPerfParam>(); }},
    {FEATURE_CONFIGS[CAPTURE_BASE], [] {return std::make_unique<CaptureBaseParamParse>(); },
        [] {return std::make_unique<CaptureBaseParam>(); }},
    {FEATURE_CONFIGS[SURFACE_CAPTURE], [] {return std::make_unique<SurfaceCaptureParamParse>(); },
        [] {return std::make_unique<SurfaceCaptureParam>(); }},
    {FEATURE_CONFIGS[UI_CAPTURE], [] {return std::make_unique<UICaptureParamParse>(); },
        [] {return std::make_unique<UICaptureParam>(); }},
    {FEATURE_CONFIGS[ACCESSIBILITY], [] { return std::make_unique<AccessibilityParamParse>(); },
        [] { return std::make_unique<AccessibilityParam>(); }},
    {FEATURE_CONFIGS[VRATE], [] { return std::make_unique<VRateParamParse>(); },
        [] { return std::make_unique<VRateParam>(); }},
    {FEATURE_CONFIGS[ROTATEOFFSCREEN], [] { return std::make_unique<RotateOffScreenParamParse>(); },
        [] { return std::make_unique<RotateOffScreenParam>(); }},
};

class GraphicFeatureParamManager : public RefBase {
public:
    static GraphicFeatureParamManager& GetInstance();

    void Init();
    void FeatureParamParseEntry();
    std::shared_ptr<FeatureParam> GetFeatureParam(std::string featureName);

    // [featureName, parseFunc]
    std::unordered_map<std::string, std::shared_ptr<XMLParserBase>> featureParseMap_;
    // [featureName, featureParam]
    std::unordered_map<std::string, std::shared_ptr<FeatureParam>> featureParamMap_;

private:
    GraphicFeatureParamManager();
    ~GraphicFeatureParamManager() noexcept override;

    std::string graphicConfigPath_ = "etc/graphic/graphic_config.xml";

    std::unique_ptr<XMLParserBase> featureParser_ = nullptr;
};

template<class Ret, class Cls, class... FuncArgs, class... Args>
std::optional<Ret> GetFeatureParamValue(const std::string& featureName,
    Ret (Cls::*func)(FuncArgs&&...) const, Args&&... args)
{
    static_assert(std::is_base_of_v<FeatureParam, Cls>, "Invalid Param Type");
    auto pParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(featureName);
    if (pParam == nullptr || func == nullptr) {
        return std::nullopt;
    }
    auto pCls = std::static_pointer_cast<Cls>(pParam);
    return ((pCls.get())->*func)(std::forward<Args>(args)...);
}
} // namespace OHOS::Rosen
#endif // GRAPHIC_FEATURE_PARAM_MANAGER_H