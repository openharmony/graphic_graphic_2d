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

void GraphicFeatureParamManager::Init()
{
    RS_LOGI("GraphicFeatureParamManager %{public}s : Init feature map", __func__);
    // Init feature configs
    for (const auto& module : FEATURE_MODULES) {
        featureParseMap_.emplace(module.name, module.xmlParser());
        featureParamMap_.emplace(module.name, module.featureParam());
    }

    // Start parse feature
    FeatureParamParseEntry();
}

void GraphicFeatureParamManager::FeatureParamParseEntry()
{
    RS_LOGI("GraphicFeatureParamManager %{public}s : In", __func__);
    if (!featureParser_) {
        featureParser_ = std::make_unique<XMLParserBase>();
    }

    if (featureParser_->LoadGraphicConfiguration(GRAPHIC_CONFIG_FILE_PRODUCT) != PARSE_EXEC_SUCCESS) {
        RS_LOGD("GraphicFeatureParamManager failed to load prod xml configuration file");
        return;
    }

    if (featureParser_->Parse() != PARSE_EXEC_SUCCESS) {
        RS_LOGD("GraphicFeatureParamManager failed to parse prod xml configuration");
    }
}

std::shared_ptr<FeatureParam> GraphicFeatureParamManager::GetFeatureParam(std::string featureName)
{
    RS_LOGI("GraphicFeatureParamManager %{public}s : %{public}s", __func__, featureName.c_str());

    auto iter = featureParamMap_.find(featureName);
    if (iter == featureParamMap_.end()) {
        RS_LOGD("GraphicFeatureParamManager %{public}s : %{public}s failed", __func__, featureName.c_str());
        return nullptr;
    }

    return iter->second;
}
} // namespace OHOS::Rosen