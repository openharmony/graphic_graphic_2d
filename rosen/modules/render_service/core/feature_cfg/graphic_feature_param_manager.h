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

#include "drm_param_parse.h"
#include "drm_param.h"
#include "hdr_param_parse.h"
#include "hdr_param.h"
#include "hwc_param_parse.h"
#include "hwc_param.h"
#include "hfbc_param_parse.h"
#include "hfbc_param.h"

namespace OHOS::Rosen {
#define DEFINE_MOD(x) {FEATURE_CONFIGS[x], [] {return std::make_unique<x##ParamParse>(); }, \
    [] {return std::make_unique<x##Param>(); }}

struct ModuleConfig {
    std::string name;
    std::function<std::unique_ptr<XMLParserBase>()> xmlParser;
    std::function<std::unique_ptr<FeatureParam>()> featureParam;
};

// add new module here
const std::vector<ModuleConfig> FEATURE_MODULES = {
    DEFINE_MOD(HDR),
    DEFINE_MOD(DRM),
    DEFINE_MOD(HWC),
    DEFINE_MOD(HFBC),
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

    static constexpr char GRAPHIC_CONFIG_FILE_PRODUCT[] = "/sys_prod/etc/graphic/graphic_config.xml";

    std::unique_ptr<XMLParserBase> featureParser_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // GRAPHIC_FEATURE_PARAM_MANAGER_H