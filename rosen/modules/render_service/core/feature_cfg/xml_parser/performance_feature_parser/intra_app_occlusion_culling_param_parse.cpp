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

#include <sstream>
#include "intra_app_occlusion_culling_param_parse.h"

namespace OHOS::Rosen {

int32_t IntraAppOcclusionCullingParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("IntraAppOcclusionCullingParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("IntraAppOcclusionCullingParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseIntraAppOcclusionCullingInternal(featureMap, *currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("IntraAppOcclusionCullingParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t IntraAppOcclusionCullingParamParse::ParseIntraAppOcclusionCullingInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;

    auto iter = featureMap.find(FEATURE_CONFIGS[INTRA_APP_OCCLUSION_CULLING]);
    if (iter == featureMap.end()) {
        RS_LOGD("IntraAppOcclusionCullingParamParse stop parsing, no initializing param map");
        return PARSE_INTERNAL_FAIL;
    }
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool parseFeatireSwitchResult = ParseFeatureSwitch(val);
        if (name == "IntraAppOcclusionCullingEnable") {
            IntraAppOcclusionCullingParam::SetIntraAppOcclusionCullingEnable(parseFeatireSwitchResult);
            RS_LOGI("IntraAppOcclusionCullingParamParse parse IntraAppOcclusionCullingEnable %{public}d",
                IntraAppOcclusionCullingParam::IsIntraAppOcclusionCullingEnable());
        }
    }

    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen