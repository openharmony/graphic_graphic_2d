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

#include "surface_capture_param_parse.h"

namespace OHOS::Rosen {
int32_t SurfaceCaptureParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("SurfaceCaptureParamParse start");
    xmlNode *currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("SurfaceCaptureParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type!= XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseSurfaceCaptureInternal(featureMap, *currNode)!= PARSE_EXEC_SUCCESS) {
            RS_LOGD("SurfaceCaptureParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t SurfaceCaptureParamParse::ParseSurfaceCaptureInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    xmlNode *currNode = &node;
    auto iter = featureMap.find(FEATURE_CONFIGS[SURFACE_CAPTURE]);
    if (iter!= featureMap.end()) {
        surfaceCaptureParam_ = std::static_pointer_cast<SurfaceCaptureParam>(iter->second);
    } else {
        RS_LOGD("SurfaceCaptureParamParse stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(*currNode);
    auto name = ExtractPropertyValue("name", *currNode);
    auto val = ExtractPropertyValue("value", *currNode);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "UseOptimizedFlushAndSubmit") {
            surfaceCaptureParam_->SetUseOptimizedFlushAndSubmitEnabled(isEnabled);
            RS_LOGI("SurfaceCaptureParamParse useOptimizedFlushAndSubmitEnabled is %{public}d", isEnabled);
        }
    }

    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen