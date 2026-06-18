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
#include "pipeline/rs_render_node_gc.h"

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
            RSRenderNodeGC::Instance().SetNodeOffTreeMemReleaseEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse NodeOffTreeMemReleaseEnabled %{public}d",
                    RSRenderNodeGC::Instance().IsNodeOffTreeMemReleaseEnabled());
        } else if (name == "CanvasDrawingNodeDMAMemEnabled") {
            NodeMemReleaseParam::SetCanvasDrawingNodeDMAMemEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse CanvasDrawingNodeDMAMemEnabled %{public}d",
                    NodeMemReleaseParam::IsCanvasDrawingNodeDMAMemEnabled());
        } else if (name == "RsRenderNodeGCMemReleaseEnabled") {
            NodeMemReleaseParam::SetRsRenderNodeGCMemReleaseEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse RsRenderNodeGCMemReleaseEnabled %{public}d",
                    NodeMemReleaseParam::IsRsRenderNodeGCMemReleaseEnabled());
        } else if (name == "CanvasDrawingNodeBufferEnabled") {
            NodeMemReleaseParam::SetCanvasDrawingNodeBufferEnabled(isEnabled);
            RS_LOGI("NodeMemReleaseParamParse parse CanvasDrawingNodeBufferEnabled %{public}d",
                    NodeMemReleaseParam::IsCanvasDrawingNodeBufferEnabled());
        } else {
            RS_LOGI("NodeMemReleaseParamParse parse %{public}s is not support!", name.c_str());
            return PARSE_ERROR;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_MULTIPARAM) {
        if (name == "CanvasDrawingNodeBufferBlacklist") {
            ParseCanvasDrawingNodeBufferBlacklist(node, name);
        }
    } else {
        RS_LOGI("NodeMemReleaseParamParse fail! The xmlParamType is not support");
        return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}

void NodeMemReleaseParamParse::ParseCanvasDrawingNodeBufferBlacklist(xmlNode& node, std::string& name)
{
    xmlNode* currNode = &node;
    if (currNode->xmlChildrenNode == nullptr) {
        RS_LOGE("ParseCanvasDrawingNodeBufferBlacklist, stop parsing, no children nodes");
        return;
    }
    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }
        auto val = ExtractPropertyValue("value", *currNode);
        if (!IsNumber(val)) {
            continue;
        }
        auto appName = ExtractPropertyValue("name", *currNode);
        NodeMemReleaseParam::AddToCanvasBufferBlacklist(appName, val);
        RS_LOGI("ParseCanvasDrawingNodeBufferBlacklist, name=%{public}s, val=%{public}s", appName.c_str(), val.c_str());
    }
}
} // namespace OHOS::Rosen