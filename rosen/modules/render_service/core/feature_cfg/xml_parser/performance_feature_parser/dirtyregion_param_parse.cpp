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

#include "dirtyregion_param_parse.h"

#include <sstream>

namespace OHOS::Rosen {

int32_t DirtyRegionParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("DirtyRegionParamParse start");
    xmlNode *curNode = &node;
    if (curNode->xmlChildrenNode == nullptr) {
        RS_LOGE("DirtyRegionParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    curNode = curNode->xmlChildrenNode;
    for (; curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseDirtyRegionInternal(*curNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("DirtyRegionParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t DirtyRegionParamParse::ParseDirtyRegionInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "DirtyRegionEnabled") {
            DirtyRegionParam::SetDirtyRegionEnable(isEnabled);
        } else if (name == "ExpandScreenDirtyRegionEnabled") {
            DirtyRegionParam::SetExpandScreenDirtyRegionEnable(isEnabled);
        } else if (name == "ExpandScreenDirtyRegionEnabled") {
            DirtyRegionParam::SetExpandScreenDirtyRegionEnable(isEnabled);
        } else if (name == "MirrorScreenDirtyRegionEnabled") {
            DirtyRegionParam::SetMirrorScreenDirtyRegionEnable(isEnabled);
        } else if (name == "AdvancedDirtyRegionEnabled") {
            DirtyRegionParam::SetAdvancedDirtyRegionEnable(isEnabled);
        } else if (name == "ComposeDirtyRegionEnableInPartialDisplay") {
            DirtyRegionParam::SetComposeDirtyRegionEnableInPartialDisplay(isEnabled);
        } else if (name == "TileBasedAlignEnabled") {
            DirtyRegionParam::SetTileBasedAlignEnable(isEnabled);
        } else {
            return PARSE_ERROR;
        }
        RS_LOGI("DirtyRegionParamParse Set %{public}s with value: %{public}d", name.c_str(), isEnabled);
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "TileBasedAlignBits" && IsNumber(val)) {
            int num;
            std::istringstream iss(val);
            if (iss >> num) {
                DirtyRegionParam::SetTileBasedAlignBits(num);
                RS_LOGI("DirtyRegionParamParse parse SetTileBasedAlignBits %{public}d", num);
            } else {
                DirtyRegionParam::SetTileBasedAlignEnable(false);
                return PARSE_ERROR;
            }
        } else {
            DirtyRegionParam::SetTileBasedAlignEnable(false);
            return PARSE_ERROR;
        }
    } else {
        return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen