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

#include "hwc_param_parse.h"
#include "graphic_ccm_feature_param_manager.h"

namespace OHOS::Rosen {

int32_t HWCParamParse::ParseFeatureParam(xmlNode &node)
{
    HGM_LOGI("HWCParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("HWCParamParse stop parsing, no children nodes");
        return HGM_ERROR;
    }
    auto featureMap = GraphicCcmFeatureParamManager::GetInstance()->featureParamMap;
    auto iter = featureMap.find("HwcConfig");
    if (iter != featureMap.end()) {
        hwcParam_ = std::static_pointer_cast<HWCParam>(iter->second);
    } else {
        HGM_LOGD("HWCParamParse stop parsing, no initializing param map");
        return HGM_ERROR;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        // Start Parse Feature Params
        int xmlParamType = GetCcmXmlNodeAsInt(*currNode);
        auto name = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (xmlParamType == CCM_XML_FEATURE_SWITCH) {
            bool isEnabled = ParseFeatureSwitch(val);
            if (name == "HwcEnabled") {
                hwcParam_->SetHwcEnable(isEnabled);
                HGM_LOGI("HWCParamParse parse HwcEnabled %{public}d", hwcParam_->IsHwcEnable());
            } else if (name == "HwcMirrorEnabled"){
                hwcParam_->SetHwcMirrorEnable(isEnabled);
                HGM_LOGI("HWCParamParse parse HwcMirrorEnabled %{public}d", hwcParam_->IsHwcMirrorEnable());
            }
        } else if (xmlParamType == CCM_XML_FEATURE_MULTIPARAM) {
            if (ParseFeatureMultiParamForApp(*currNode, name) != EXEC_SUCCESS) {
                HGM_LOGI("HWCParamParse parse MultiParam fail");
            }
        }
    }
    return EXEC_SUCCESS;
}

int32_t HWCParamParse::ParseFeatureMultiParamForApp(xmlNode &node, std::string name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        HGM_LOGD("HWCParamParse stop parsing, no children nodes");
        return HGM_ERROR;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto appName = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (name == "SourceTuningForYuv420") {
            hwcParam_->SetSourceTuningForApp(appName, val);
        } else if (name == "RsSolidColorLayerConfig") {
            hwcParam_->SetSolidColorLayerForApp(appName, val);
        } else {
            HGM_LOGI("HWCParamParse ParseFeatureMultiParam cannot find name");
            return HGM_ERROR;
        }
    }
    return EXEC_SUCCESS;
}
} // namespace OHOS::Rosen