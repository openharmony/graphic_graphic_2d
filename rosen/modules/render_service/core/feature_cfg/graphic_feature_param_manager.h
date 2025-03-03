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
#include "background_drawable_param_parse.h"
#include "background_drawable_param.h"
#include "capture_base_param_parse.h"
#include "capture_base_param.h"
#include "drm_param_parse.h"
#include "drm_param.h"
#include "hdr_param_parse.h"
#include "hdr_param.h"
#include "hfbc_param_parse.h"
#include "hfbc_param.h"
#include "hwc_param_parse.h"
#include "hwc_param.h"
#include "mem_param_parse.h"
#include "mem_param.h"
#include "node_group_param_parse.h"
#include "node_group_param.h"
#include "opinc_param_parse.h"
#include "opinc_param.h"
#include "prevalidate_param_parse.h"
#include "prevalidate_param.h"
#include "speciallayer_param.h"
#include "speciallayer_param_parse.h"
#include "uifirst_param_parse.h"
#include "uifirst_param.h"
#include "filter_param_parse.h"
#include "filter_param.h"
#include "dvsync_param_parse.h"
#include "dvsync_param.h"
#include "socperf_param_parse.h"
#include "socperf_param.h"
#include "surface_watermark_param_parse.h"
#include "surface_watermark_param.h"
#include "surface_capture_param_parse.h"
#include "surface_capture_param.h"
#include "gpu_resource_release_param_parse.h"
#include "gpu_resource_release_param.h"
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
    {FEATURE_CONFIGS[HDR], [] {return std::make_unique<HDRParamParse>(); }, [] {return std::make_unique<HDRParam>(); }},
    {FEATURE_CONFIGS[DRM], [] {return std::make_unique<DRMParamParse>(); }, [] {return std::make_unique<DRMParam>(); }},
    {FEATURE_CONFIGS[HWC], [] {return std::make_unique<HWCParamParse>(); }, [] {return std::make_unique<HWCParam>(); }},
    {FEATURE_CONFIGS[HFBC], [] {return std::make_unique<HFBCParamParse>(); },
        [] {return std::make_unique<HFBCParam>(); }},
    {FEATURE_CONFIGS[MEM], [] { return std::make_unique<MEMParamParse>(); },
        [] { return std::make_unique<MEMParam>(); }},
    {FEATURE_CONFIGS[SPECIALLAYER], [] {return std::make_unique<SpecialLayerParamParse>(); },
        [] {return std::make_unique<SpecialLayerParam>(); }},
    {FEATURE_CONFIGS[OPInc], [] {return std::make_unique<OPIncParamParse>(); },
        [] {return std::make_unique<OPIncParam>(); }},
    {FEATURE_CONFIGS[PREVALIDATE], [] {return std::make_unique<PrevalidateParamParse>(); },
        [] {return std::make_unique<PrevalidateParam>(); }},
    {FEATURE_CONFIGS[UIFirst], [] {return std::make_unique<UIFirstParamParse>(); },
        [] {return std::make_unique<UIFirstParam>(); }},
    {FEATURE_CONFIGS[FILTER], [] { return std::make_unique<FilterParamParse>(); },
        [] { return std::make_unique<FilterParam>(); }},
    {FEATURE_CONFIGS[DVSYNC], [] { return std::make_unique<DVSyncParamParse>(); },
        [] { return std::make_unique<DVSyncParam>(); }},
    {FEATURE_CONFIGS[SOC_PERF], [] { return std::make_unique<SOCPerfParamParse>(); },
        [] { return std::make_unique<SOCPerfParam>(); }},
    {FEATURE_CONFIGS[DEEPLY_REL_GPU_RES], [] { return std::make_unique<DeeplyRelGpuResParamParse>(); },
        [] { return std::make_unique<DeeplyRelGpuResParam>(); }},
    {FEATURE_CONFIGS[NODE_GROUP], [] {return std::make_unique<NodeGroupParamParse>(); },
        [] {return std::make_unique<NodeGroupParam>(); }},
    {FEATURE_CONFIGS[SURFACE_WATERMARK], [] {return std::make_unique<SurfaceWatermarkParamParse>(); },
        [] {return std::make_unique<SurfaceWatermarkParam>(); }},
    {FEATURE_CONFIGS[SURFACE_CAPTURE], [] {return std::make_unique<SurfaceCaptureParamParse>(); },
        [] {return std::make_unique<SurfaceCaptureParam>(); }},
    {FEATURE_CONFIGS[UI_CAPTURE], [] {return std::make_unique<UICaptureParamParse>(); },
        [] {return std::make_unique<UICaptureParamParam>(); }},
    {FEATURE_CONFIGS[CAPTURE], [] {return std::make_unique<CaptureBaseParamParse>(); },
        [] {return std::make_unique<CaptureBaseParam>(); }},
    {FEATURE_CONFIGS[BACKGROUND_DRAWABLE], [] {return std::make_unique<BackgroundDrawableParamParse>(); },
        [] {return std::make_unique<BackgroundDrawableParam>(); }},
    {FEATURE_CONFIGS[Accessibility], [] {return std::make_unique<AccessibilityParamParse>(); },
        [] {return std::make_unique<AccessibilityParam>(); }},
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

template<class Ty>
bool GetFeatureParam(const std::string& featureName, Ty& param)
{
    static_assert(std::is_base_of_v<FeatureParam, Ty>, "Invalid Param Type");
    auto pParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(featureName);
    if (pParam == nullptr) {
        return false; 
    }
    param = *std::static_pointer_cast<Ty>(pParam);
    return true;
}

template<class Ret, class Cls, class... Args>
std::optional<Ret> GetFeatureParamValue(const std::string& featureName,
    Ret (Cls::*func)(Args...), Args&&... args)
{
    static_assert(std::is_base_of_v<FeatureParam, Cls>, "Invalid Param Type");
    auto pParam = GraphicFeatureParamManager::GetInstance().GetFeatureParam(featureName);
    if (pParam == nullptr) {
        return std::nullopt;
    }
    auto pCls = std::static_pointer_cast<Cls>(pParam);
    return (pCls->*func)(std::forward<Args>(args)...);
}
} // namespace OHOS::Rosen
#endif // GRAPHIC_FEATURE_PARAM_MANAGER_H