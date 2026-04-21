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
#include "hwc_param_parse.h"
#include "hwc_param.h"
#include "mem_param_parse.h"
#include "mem_param.h"
#include "multiscreen_param_parse.h"
#include "multiscreen_param.h"
#include "node_mem_release_param_parse.h"
#include "node_mem_release_param.h"
#include "occlusion_culling_param.h"
#include "occlusion_culling_param_parse.h"
#include "opinc_param_parse.h"
#include "opinc_param.h"
#include "rotateoffscreen_param_parse.h"
#include "rotateoffscreen_param.h"
#include "speciallayer_param.h"
#include "speciallayer_param_parse.h"
#include "subtree_parallel_param.h"
#include "subtree_parallel_param_parse.h"
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
#include "gpu_resource_release_param_parse.h"
#include "gpu_resource_release_param.h"
#include "ui_capture_param_parse.h"
#include "ui_capture_param.h"
#include "image_enhance_param_parse.h"
#include "image_enhance_param.h"
#include "video_metadata_param_parse.h"
#include "video_metadata_param.h"
#include "smart_cache_param_parse.h"
#include "smart_cache_param.h"
#include "gpu_cache_param_parse.h"
#include "vma_block_param_parse.h"
#include "vma_block_param.h"
#include "spirv_cache_param_parse.h"
#include "spirv_cache_param.h"
#include "buffer_reclaim_param_parse.h"
#include "buffer_reclaim_param.h"

namespace OHOS::Rosen {
struct ModuleConfig {
    std::string name;
    std::function<std::unique_ptr<XMLParserBase>()> xmlParser;
    std::function<std::unique_ptr<FeatureParam>()> featureParam;
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