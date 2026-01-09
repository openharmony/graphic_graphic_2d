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
 
#include "node_mem_release_param_parse.h"
 
namespace OHOS::Rosen {
 
int32_t NodeMemReleaseParamParse::ParseFeatureParam(FeatureParamMapType& featureMap, xmlNode& node)
{
    RS_LOGI("NodeMemReleaseParamParse start");
    xmlNode* currNode = &node;
    currNode = currNode->xmlChildrenNode;
    if (currNode == nullptr) {
        RS_LOGD("NodeMemReleaseParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }
 
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
 
        if (ParseNodeMemReleaseInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("NodeMemReleaseParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}
 
int32_t NodeMemReleaseParamParse::ParseNodeMemReleaseInternal(xmlNode& node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "NodeOffTreeMemReleaseEnabled") {
            NodeMemReleaseParam::SetNodeOffTreeMemReleaseEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse NodeOffTreeMemReleaseEnabled %{public}d",
                    NodeMemReleaseParam::IsNodeOffTreeMemReleaseEnabled());
        } else if (name == "CanvasDrawingNodeDMAMemEnabled") {
            NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse CanvasDrawingNodeDMAMemEnabled %{public}d",
                    NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled());
        } else {
            RS_LOGI("NodeMemReleaseParamParse parse %{public}s is not support!", name.c_str());
        }
    } else {
        RS_LOGI("NodeMemReleaseParamParse fail! The xmlParamType is not support");
        return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}
 
} // namespace OHOS::Rosen