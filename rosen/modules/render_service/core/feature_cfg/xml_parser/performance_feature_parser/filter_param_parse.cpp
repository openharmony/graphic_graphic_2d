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

#include "filter_param_parse.h"
#define XML_KEY_NAME "name"
#define XML_KEY_VALUE "value"
#define XML_PARAM_FILTER_CACHE_ENABLED "FilterCacheEnabled"
#define XML_PARAM_EFFECT_MERGE_ENABLED "EffectMergeEnabled"
#define XML_PARAM_BLUR_ADAPTIVE_ADJUST "BlurAdaptiveAdjust"

namespace OHOS::Rosen {

int32_t FilterParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("FilterParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("FilterParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseFilterCacheInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("FilterParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t FilterParamParse::ParseFilterCacheInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue(XML_KEY_NAME, node);
    auto val = ExtractPropertyValue(XML_KEY_VALUE, node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == XML_PARAM_FILTER_CACHE_ENABLED) {
            FilterParam::SetFilterCacheEnable(isEnabled);
            RS_LOGI("FilterParamParse parse FilterCacheEnabled %{public}d", FilterParam::IsFilterCacheEnable());
        } else if (name == XML_PARAM_EFFECT_MERGE_ENABLED) {
            FilterParam::SetEffectMergeEnable(isEnabled);
            RS_LOGI("FilterParamParse parse EffectMergeEnabled %{public}d", FilterParam::IsEffectMergeEnable());
        } else if (name == XML_PARAM_BLUR_ADAPTIVE_ADJUST) {
            FilterParam::SetBlurAdaptiveAdjust(isEnabled);
            RS_LOGI("FilterParamParse parse BlurAdaptiveAdjust %{public}d", FilterParam::IsBlurAdaptiveAdjust());
        } else {
            RS_LOGD("FilterParamParse unknown feature name");
            return PARSE_ERROR;
        }
    }

    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen