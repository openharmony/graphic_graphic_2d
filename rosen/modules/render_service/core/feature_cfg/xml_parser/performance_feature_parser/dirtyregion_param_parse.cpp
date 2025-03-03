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
        RS_LOGD("DirtyRegionParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    curNode = curNode->xmlChildrenNode;
    for (; curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseDirtyRegionInternal(featureMap, *curNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("DirtyRegionParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t DirtyRegionParamParse::ParseDirtyRegionInternal(FeatureParamMapType &featureMap, xmlNode &node)
{
    auto iter = featureMap.find(FEATURE_CONFIGS[DIRTYREGION]);
    if (iter != featureMap.end()) {
        dirtyRegionParam_ = std::static_pointer_cast<DirtyRegionParam>(iter->second);
    } else {
        RS_LOGD("DirtyRegionParamParse stop parsing, no initializing param map");
        return PARSE_NO_PARAM;
    }

    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "DirtyRegionEnabled") {
            dirtyRegionParam_->SetDirtyRegionEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetDirtyRegionEnable %{public}d", isEnabled);
        } else if (name == "ExpandScreenDirtyRegionEnabled") {
            dirtyRegionParam_->SetExpandScreenDirtyRegionEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetExpandScreenDirtyRegionEnable %{public}d", isEnabled);
        } else if (name == "ExpandScreenDirtyRegionEnabled") {
            dirtyRegionParam_->SetExpandScreenDirtyRegionEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetExpandScreenDirtyRegionEnable %{public}d", isEnabled);
        } else if (name == "MirrorScreenDirtyRegionEnabled") {
            dirtyRegionParam_->SetMirrorScreenDirtyRegionEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetMirrorScreenDirtyRegionEnable %{public}d", isEnabled);
        } else if (name == "AdvancedDirtyRegionEnabled") {
            dirtyRegionParam_->SetAdvancedDirtyRegionEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetAdvancedDirtyRegionEnabled %{public}d", isEnabled);
        } else if (name == "TileBasedAlignEnabled") {
            dirtyRegionParam_->SetTileBasedAlignEnable(isEnabled);
            RS_LOGI("DirtyRegionParamParse parse SetTileBasedAlignEnable %{public}d", isEnabled);
        } else {
            RS_LOGE("DirtyRegionParamParse error occured during param parsing.");
            return PARSE_ERROR;
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "TileBasedAlignBits" && IsNumber(val)) {
            int num;
            std::istringstream iss(val);
            if (iss >> num) {
                dirtyRegionParam_->SetTileBasedAlignBits(num);
                RS_LOGI("DirtyRegionParamParse parse SetTileBasedAlignBits %{public}d", num);
            } else {
                dirtyRegionParam_->SetTileBasedAlignEnable(false);
                RS_LOGE("DirtyRegionParamParse error occured during param parsing.");
                return PARSE_ERROR;
            }
        } else {
            dirtyRegionParam_->SetTileBasedAlignEnable(false);
            RS_LOGE("DirtyRegionParamParse error occured during param parsing.");
            return PARSE_ERROR;
        }
    } else {
        RS_LOGE("DirtyRegionParamParse error occured during param parsing.");
        return PARSE_ERROR;
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen