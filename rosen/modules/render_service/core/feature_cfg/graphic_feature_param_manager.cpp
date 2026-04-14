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

#include "graphic_feature_param_manager.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
GraphicFeatureParamManager& GraphicFeatureParamManager::GetInstance()
{
    static GraphicFeatureParamManager instance;
    return instance;
}

GraphicFeatureParamManager::GraphicFeatureParamManager()
{
}

GraphicFeatureParamManager::~GraphicFeatureParamManager() noexcept
{
}

// add new module here
const std::vector<ModuleConfig> FEATURE_MODULES = {
    {FEATURE_CONFIGS[DIRTYREGION], [] { return std::make_unique<DirtyRegionParamParse>(); },
        [] { return std::make_unique<DirtyRegionParam>(); }},
    {FEATURE_CONFIGS[COLOR_GAMUT], [] {return std::make_unique<ColorGamutParamParse>(); },
        [] {return std::make_unique<ColorGamutParam>(); }},
    {FEATURE_CONFIGS[DRM], [] { return std::make_unique<DRMParamParse>(); },
        [] { return std::make_unique<DRMParam>(); }},
    {FEATURE_CONFIGS[HWC], [] {return std::make_unique<HWCParamParse>(); }, [] {return std::make_unique<HWCParam>(); }},
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
    {FEATURE_CONFIGS[UIFirst], [] {return std::make_unique<UIFirstParamParse>(); },
        [] {return std::make_unique<UIFirstParam>(); }},
    {FEATURE_CONFIGS[FILTER], [] { return std::make_unique<FilterParamParse>(); },
        [] { return std::make_unique<FilterParam>(); }},
    {FEATURE_CONFIGS[DVSYNC], [] { return std::make_unique<DVSyncParamParse>(); },
        [] { return std::make_unique<DVSyncParam>(); }},
    {FEATURE_CONFIGS[SOC_PERF], [] { return std::make_unique<SOCPerfParamParse>(); },
        [] { return std::make_unique<SOCPerfParam>(); }},
    {FEATURE_CONFIGS[CAPTURE_BASE], [] {return std::make_unique<CaptureBaseParamParse>(); },
        [] {return std::make_unique<CaptureBaseParam>(); }},
    {FEATURE_CONFIGS[SURFACE_CAPTURE], [] {return std::make_unique<SurfaceCaptureParamParse>(); },
        [] {return std::make_unique<SurfaceCaptureParam>(); }},
    {FEATURE_CONFIGS[UI_CAPTURE], [] {return std::make_unique<UICaptureParamParse>(); },
        [] {return std::make_unique<UICaptureParam>(); }},
    {FEATURE_CONFIGS[DEEPLY_REL_GPU_RES], [] { return std::make_unique<DeeplyRelGpuResParamParse>(); },
        [] { return std::make_unique<DeeplyRelGpuResParam>(); }},
    {FEATURE_CONFIGS[ACCESSIBILITY], [] { return std::make_unique<AccessibilityParamParse>(); },
        [] { return std::make_unique<AccessibilityParam>(); }},
    {FEATURE_CONFIGS[VRATE], [] { return std::make_unique<VRateParamParse>(); },
        [] { return std::make_unique<VRateParam>(); }},
    {FEATURE_CONFIGS[ROTATEOFFSCREEN], [] { return std::make_unique<RotateOffScreenParamParse>(); },
        [] { return std::make_unique<RotateOffScreenParam>(); }},
    {FEATURE_CONFIGS[SUBTREEPARALLEL], [] { return std::make_unique<SubtreeParallelParamParse>(); },
        [] { return std::make_unique<SubtreeParallelParam>(); }},
    {FEATURE_CONFIGS[IMAGE_ENHANCE], [] { return std::make_unique<ImageEnhanceParamParse>(); },
        [] { return std::make_unique<ImageEnhanceParam>(); }},
    {FEATURE_CONFIGS[VIDEO_METADATA], [] { return std::make_unique<VideoMetadataParamParse>(); },
        [] { return std::make_unique<VideoMetadataParam>(); }},
    {FEATURE_CONFIGS[SMART_CACHE], [] { return std::make_unique<SmartCacheParamParse>(); },
        [] { return std::make_unique<SmartCacheParam>(); }},
    {FEATURE_CONFIGS[GPU_CACHE], [] { return std::make_unique<GpuCacheParamParse>(); },
        [] { return std::make_unique<GpuCacheParam>(); }},
    {FEATURE_CONFIGS[NODE_MEM_RELEASE], [] { return std::make_unique<NodeMemReleaseParamParse>(); },
        [] { return std::make_unique<NodeMemReleaseParam>(); }},
    {FEATURE_CONFIGS[VMA_BLOCK], [] { return std::make_unique<VMABlockParamParse>(); },
        [] { return std::make_unique<VMABlockParam>(); }},
    {FEATURE_CONFIGS[SPIRV_CACHE], [] { return std::make_unique<SpirvCacheParamParse>(); },
        [] { return std::make_unique<SpirvCacheParam>(); }},
    {FEATURE_CONFIGS[BUFFER_RECLAIM], [] { return std::make_unique<BufferReclaimParamParse>(); },
        [] { return std::make_unique<BufferReclaimParam>(); }},
};

void GraphicFeatureParamManager::Init()
{
    static std::once_flag flag;
    std::call_once(flag, [this] {
        RS_LOGI("GraphicFeatureParamManager %{public}s : Init feature map", __func__);
        // Init feature configs
        for (const auto& module : FEATURE_MODULES) {
            featureParseMap_.emplace(module.name, module.xmlParser());
            featureParamMap_.emplace(module.name, module.featureParam());
        }
    
        // Start parse feature
        FeatureParamParseEntry();
    });
}

void GraphicFeatureParamManager::FeatureParamParseEntry()
{
    RS_LOGI("GraphicFeatureParamManager %{public}s : In", __func__);
    if (!featureParser_) {
        featureParser_ = std::make_unique<XMLParserBase>();
    }

    if (featureParser_->LoadGraphicConfiguration(graphicConfigPath_) != PARSE_EXEC_SUCCESS) {
        RS_LOGD("GraphicFeatureParamManager failed to load xml configuration file");
        return;
    }

    if (featureParser_->ParseSysDoc() != PARSE_EXEC_SUCCESS) {
        RS_LOGD("GraphicFeatureParamManager failed to parse sys xml configuration");
    }

    if (featureParser_->ParseProdDoc() != PARSE_EXEC_SUCCESS) {
        RS_LOGD("GraphicFeatureParamManager failed to parse prod xml configuration");
    }
}

std::shared_ptr<FeatureParam> GraphicFeatureParamManager::GetFeatureParam(std::string featureName)
{
    RS_LOGD("GraphicFeatureParamManager %{public}s : %{public}s", __func__, featureName.c_str());

    auto iter = featureParamMap_.find(featureName);
    if (iter == featureParamMap_.end()) {
        RS_LOGD("GraphicFeatureParamManager %{public}s : %{public}s failed", __func__, featureName.c_str());
        return nullptr;
    }

    return iter->second;
}
} // namespace OHOS::Rosen