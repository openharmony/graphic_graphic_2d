/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "behind_window_filter_param_parse.h"
#include "feature/behind_window_filter/rs_behind_window_filter_manager.h"

#define XML_KEY_NAME "name"
#define XML_KEY_VALUE "value"
#define XML_PARAM_BEHIND_WINDOW_FILTER_ENABLED "BehindWindowBlur"

namespace OHOS::Rosen {

int32_t BehindWindowFilterParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("BehindWindowFilterParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("BehindWindowFilterParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        ParseBehindWindowFilterInternal(*currNode);
    }

    BehindWindowFilterCCMInit();
    return PARSE_EXEC_SUCCESS;
}

int32_t BehindWindowFilterParamParse::ParseBehindWindowFilterInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == XML_PARAM_BEHIND_WINDOW_FILTER_ENABLED) {
            BehindWindowFilterParam::SetBehindWindowFilterEnable(isEnabled);
            RS_LOGI("BehindWindowFilterParamParse parse BehindWindowBlur %{public}d",
                BehindWindowFilterParam::IsBehindWindowFilterEnable());
        } else {
            RS_LOGD("BehindWindowFilterParamParse unknown switch feature name");
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        RS_LOGD("BehindWindowFilterParamParse unknown single param feature name");
    }

    return PARSE_EXEC_SUCCESS;
}

void BehindWindowFilterParamParse::BehindWindowFilterCCMInit()
{
    RSBehindWindowFilterManager::Instance().SetBehindWindowFilterEnabledByCCM(
        BehindWindowFilterParam::IsBehindWindowFilterEnable());
}
} // namespace OHOS::Rosen