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

#ifndef GRAPHIC_CCM_FEATURE_PARAM_MANAGER_H
#define GRAPHIC_CCM_FEATURE_PARAM_MANAGER_H

#include <list>
#include <mutex>
#include <unordered_map>

#include "refbase.h"
#include "xml_parser_base.h"

#include "drm_param_parse.h"
#include "drm_param.h"
#include "dvsync_param_parse.h"
#include "dvsync_param.h"
#include "hdr_param_parse.h"
#include "hdr_param.h"
#include "hwc_param_parse.h"
#include "hwc_param.h"

namespace OHOS::Rosen {
class GraphicCcmFeatureParamManager : public RefBase {
public:
    static sptr<GraphicCcmFeatureParamManager> GetInstance() noexcept;

    // non-copyable
    GraphicCcmFeatureParamManager(const GraphicCcmFeatureParamManager &) = delete;
    GraphicCcmFeatureParamManager &operator=(const GraphicCcmFeatureParamManager &) = delete;

    void Init();
    void FeatureParamParseEntry();
    std::shared_ptr<FeatureParam> GetFeatureParam(std::string featureName);

    // [featureName, parseFunc]
    std::unordered_map<std::string, std::shared_ptr<XMLParserBase>> featureParseMap;
    // [featureName, featureParam]
    std::unordered_map<std::string, std::shared_ptr<FeatureParam>> featureParamMap;

private:
    GraphicCcmFeatureParamManager();
    ~GraphicCcmFeatureParamManager() noexcept override;
    static std::once_flag createFlag_;
    static sptr<GraphicCcmFeatureParamManager> instance_;

    static constexpr char GRAPHIC_CCM_CONFIG_FILE_PRODUCT[] = "/sys_prod/etc/graphic/graphic_ccm_config.xml";

    std::unique_ptr<XMLParserBase> featureParser_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // GRAPHIC_CCM_FEATURE_PARAM_MANAGER_H