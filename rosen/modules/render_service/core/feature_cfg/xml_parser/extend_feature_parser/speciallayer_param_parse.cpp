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

#include "speciallayer_param_parse.h"

namespace OHOS::Rosen {

int32_t SpecialLayerParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("SpecialLayerParamParse start");
    xmlNode *curNode = &node;
    if (curNode->xmlChildrenNode == nullptr) {
        RS_LOGE("SpecialLayerParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    curNode = curNode->xmlChildrenNode;
    for (; curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseSpecialLayerInternal(featureMap, *curNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("SpecialLayerParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t SpecialLayerParamParse::ParseSpecialLayerInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    auto iter = featureMap.find(FEATURE_CONFIGS[SPECIALLAYER]);
    if (iter != featureMap.end()) {
        speciallayerParam_ = std::static_pointer_cast<SpecialLayerParam>(iter->second);
    } else {
        RS_LOGD("SpecialLayerParamParse stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH && name == "SpecialLayerEnabled") {
        auto val = ExtractPropertyValue("value", node);
        bool isEnabled = ParseFeatureSwitch(val);
        speciallayerParam_->SetSpecialLayerEnable(isEnabled);
        RS_LOGI("SpecialLayerParamParse parse SpecialLayerEnabled %{public}d",
            speciallayerParam_->IsSpecialLayerEnable());
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen