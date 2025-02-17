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

namespace OHOS::Rosen {

int32_t HWCParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("HWCParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("HWCParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseHwcInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("HWCParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t HWCParamParse::ParseHwcInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(FEATURE_CONFIGS[HWC]);
    if (iter != featureMap.end()) {
        hwcParam_ = std::static_pointer_cast<HWCParam>(iter->second);
    } else {
        RS_LOGD("HWCParamParse stop parsing, no initializing param map");
    }

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "HwcEnabled") {
            hwcParam_->SetHwcEnable(isEnabled);
            RS_LOGI("HWCParamParse parse HwcEnabled %{public}d", hwcParam_->IsHwcEnable());
        } else if (name == "HwcMirrorEnabled") {
            hwcParam_->SetHwcMirrorEnable(isEnabled);
            RS_LOGI("HWCParamParse parse HwcMirrorEnabled %{public}d", hwcParam_->IsHwcMirrorEnable());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        if (ParseFeatureMultiParamForApp(*currNode, name) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("HWCParamParse parse MultiParam fail");
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t HWCParamParse::ParseFeatureMultiParamForApp(xmlNode &node, std::string &name)
{
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("HWCParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto appName = ExtractPropertyValue("name", *currNode);
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            return PARSE_ERROR;
        }
        if (name == "SourceTuningForYuv420") {
            hwcParam_->SetSourceTuningForApp(appName, val);
        } else if (name == "RsSolidColorLayerConfig") {
            hwcParam_->SetSolidColorLayerForApp(appName, val);
        } else {
            RS_LOGD("HWCParamParse ParseFeatureMultiParam cannot find name");
            return PARSE_NO_PARAM;
        }
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen