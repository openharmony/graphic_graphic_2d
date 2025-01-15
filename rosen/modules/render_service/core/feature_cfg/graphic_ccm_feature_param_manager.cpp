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
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
GraphicCcmFeatureParamManager& GraphicCcmFeatureParamManager::GetInstance()
{
    static GraphicCcmFeatureParamManager instance;
    return instance;
}

GraphicCcmFeatureParamManager::GraphicCcmFeatureParamManager()
{
}

GraphicCcmFeatureParamManager::~GraphicCcmFeatureParamManager() noexcept
{
}

void GraphicCcmFeatureParamManager::Init()
{
    RS_LOGD("GraphicCcmFeatureParamManager %{public}s : Init feature map", __func__);
    // parse map init
    featureParseMap_[paramVec[featureParamCode::HDR]] = std::make_unique<HDRParamParse>();
    featureParseMap_[paramVec[featureParamCode::DRM]] = std::make_unique<DRMParamParse>();
    featureParseMap_[paramVec[featureParamCode::HWC]] = std::make_unique<HWCParamParse>();

    // param map init
    featureParamMap_[paramVec[featureParamCode::HDR]] = std::make_unique<HDRParam>();
    featureParamMap_[paramVec[featureParamCode::DRM]] = std::make_unique<DRMParam>();
    featureParamMap_[paramVec[featureParamCode::HWC]] = std::make_unique<HWCParam>();
}

void GraphicCcmFeatureParamManager::FeatureParamParseEntry()
{
    RS_LOGD("GraphicCcmFeatureParamManager %{public}s : In", __func__);
    if (!featureParser_) {
        featureParser_ = std::make_unique<XMLParserBase>();
    }

    if (featureParser_->LoadGraphicConfiguration(GRAPHIC_CCM_CONFIG_FILE_PRODUCT) != CCM_EXEC_SUCCESS) {
        RS_LOGW("GraphicCcmFeatureParamManager failed to load prod xml configuration file");
        return;
    }

    if (featureParser_->Parse() != CCM_EXEC_SUCCESS) {
        RS_LOGW("GraphicCcmFeatureParamManager failed to parse prod xml configuration");
    }
}

std::shared_ptr<FeatureParam> GraphicCcmFeatureParamManager::GetFeatureParam(std::string featureName)
{
    RS_LOGI("GraphicCcmFeatureParamManager %{public}s : getFeatureParam %{public}s", __func__, featureName.c_str());

    auto iter = featureParamMap_.find(featureName);
    if (iter == featureParamMap_.end()) {
        RS_LOGE("GraphicCcmFeatureParamManager %{public}s : getFeatureParam featureName: %{public}s failed", __func__, featureName.c_str());
        return nullptr;
    }

    return iter->second;
}
} // namespace OHOS::Rosen