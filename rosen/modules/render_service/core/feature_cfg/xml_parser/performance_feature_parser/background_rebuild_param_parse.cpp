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

#include "background_rebuild_param_parse.h"
#include "parameters.h"
#include "pipeline/rs_background_rebuild_param.h"

namespace OHOS::Rosen {

int32_t BackgroundRebuildParamParse::ParseFeatureParam(FeatureParamMapType& featureMap, xmlNode& node)
{
    RS_LOGI("BackgroundRebuildParamParse start");
    xmlNode* currNode = &node;
    if (currNode == nullptr || currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("BackgroundRebuildParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseBackgroundRebuildInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("BackgroundRebuildParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t BackgroundRebuildParamParse::ParseBackgroundRebuildInternal(xmlNode& node)
{
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "BackgroundRebuildEnabled") {
            RSBackgroundRebuildParam::Instance().SetBackgroundRebuildEnabled(isEnabled);
            // Sync GoStop node map mode so legacy configs keep their original GoStop behavior.
            // Parsing is sequential, so an explicit GoStopNodeMapMode must appear after
            // BackgroundRebuildEnabled in the xml, otherwise it gets overwritten here.
            RSBackgroundRebuildParam::Instance().SetGoStopNodeMapMode(
                isEnabled ? GoStopNodeMapMode::DESTROY_TOKEN_NODE : GoStopNodeMapMode::REMOVE_SURFACE_NODE_MAP);
            RS_LOGI("BackgroundRebuildParamParse parse BackgroundRebuildEnabled %{public}d",
                RSBackgroundRebuildParam::Instance().IsBackgroundRebuildEnabled());
        } else {
            RS_LOGI("BackgroundRebuildParamParse parse %{public}s is not support!", name.c_str());
            return PARSE_ERROR;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        return ParseGoStopNodeMapMode(name, val);
    } else {
        RS_LOGI("BackgroundRebuildParamParse fail! The xmlParamType is not support");
        return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}

int32_t BackgroundRebuildParamParse::ParseGoStopNodeMapMode(const std::string& name, const std::string& val)
{
    if (name != "GoStopNodeMapMode") {
        RS_LOGI("BackgroundRebuildParamParse parse %{public}s is not support!", name.c_str());
        return PARSE_ERROR;
    }
    int mode = -1;
    std::istringstream iss(val);
    if (!IsNumber(val) || !(iss >> mode) || mode < static_cast<int>(GoStopNodeMapMode::NONE) ||
        mode > static_cast<int>(GoStopNodeMapMode::DESTROY_TOKEN_NODE)) {
        RS_LOGE("BackgroundRebuildParamParse invalid GoStopNodeMapMode: %{public}s", val.c_str());
        return PARSE_ERROR;
    }
    RSBackgroundRebuildParam::Instance().SetGoStopNodeMapMode(static_cast<GoStopNodeMapMode>(mode));
    RS_LOGI("BackgroundRebuildParamParse parse GoStopNodeMapMode %{public}d",
        static_cast<int>(RSBackgroundRebuildParam::Instance().GetGoStopNodeMapMode()));
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen