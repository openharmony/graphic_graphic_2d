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

#include "graphic_ccm_feature_param_manager.h"

#include "hgm_log.h"

namespace OHOS::Rosen {
std::once_flag GraphicCcmFeatureParamManager::createFlag_;
sptr<GraphicCcmFeatureParamManager> GraphicCcmFeatureParamManager::instance_ = nullptr;

sptr<GraphicCcmFeatureParamManager> GraphicCcmFeatureParamManager::GetInstance() noexcept
{
    std::call_once(createFlag_, []() {
        instance_ = new GraphicCcmFeatureParamManager();
    });
    return instance_;
}

GraphicCcmFeatureParamManager::GraphicCcmFeatureParamManager()
{
}

GraphicCcmFeatureParamManager::~GraphicCcmFeatureParamManager() noexcept
{
}

void GraphicCcmFeatureParamManager::Init()
{
    HGM_LOGD("GraphicCcmFeatureParamManager %{public}s : Init feature map", __func__);
    // parse map init
    featureParseMap["HdrConfig"] = std::make_unique<HDRParamParse>();
    featureParseMap["DvsyncConfig"] = std::make_unique<DvsyncParamParse>();
    featureParseMap["DrmConfig"] = std::make_unique<DRMParamParse>();
    featureParseMap["HwcConfig"] = std::make_unique<HWCParamParse>();

    // param map init
    featureParamMap["HdrConfig"] = std::make_unique<HDRParam>();
    featureParamMap["DvsyncConfig"] = std::make_unique<DvsyncParam>();
    featureParamMap["DrmConfig"] = std::make_unique<DRMParam>();
    featureParamMap["HwcConfig"] = std::make_unique<HWCParam>();
}

void GraphicCcmFeatureParamManager::FeatureParamParseEntry()
{
    HGM_LOGD("GraphicCcmFeatureParamManager %{public}s : In", __func__);
    if (!featureParser_) {
        featureParser_ = std::make_unique<XMLParserBase>();
    }

    if (featureParser_->LoadGraphicConfiguration(GRAPHIC_CCM_CONFIG_FILE_PRODUCT) != EXEC_SUCCESS) {
        HGM_LOGW("GraphicCcmFeatureParamManager failed to load prod xml configuration file");
    }

    if (featureParser_->Parse() != EXEC_SUCCESS) {
        HGM_LOGW("GraphicCcmFeatureParamManager failed to parse prod xml configuration");
    }
}

std::shared_ptr<FeatureParam> GraphicCcmFeatureParamManager::GetFeatureParam(std::string featureName)
{
    HGM_LOGI("GraphicCcmFeatureParamManager %{public}s : getFeatureParam %{public}s", __func__, featureName.c_str());

    auto iter = featureParamMap.find(featureName);
    if (iter == featureParamMap.end()) {
        HGM_LOGE("GraphicCcmFeatureParamManager %{public}s : getFeatureParam featureName: %{public}s failed", __func__, featureName.c_str());
        return nullptr;
    }

    return iter->second;
}

} // namespace OHOS::Rosen