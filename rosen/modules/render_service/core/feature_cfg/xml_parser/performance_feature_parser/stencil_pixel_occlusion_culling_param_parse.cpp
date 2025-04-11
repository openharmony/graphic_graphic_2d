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

#include "stencil_pixel_occlusion_culling_param_parse.h"

namespace OHOS::Rosen {

int32_t StencilPixelOcclusionCullingParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("StencilPixelOcclusionCullingParamParse start");
    xmlNode *curNode = &node;
    if (curNode->xmlChildrenNode == nullptr) {
        RS_LOGE("StencilPixelOcclusionCullingParamParse stop parsing, no children nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    curNode = curNode->xmlChildrenNode;
    for (; curNode; curNode = curNode->next) {
        if (curNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseStencilPixelOcclusionCullingInternal(*curNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGE("StencilPixelOcclusionCullingParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t StencilPixelOcclusionCullingParamParse::ParseStencilPixelOcclusionCullingInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    if (xmlParamType == PARSE_XML_FEATURE_SWITCH && name == "SpocEnabled") {
        auto val = ExtractPropertyValue("value", node);
        bool isEnabled = ParseFeatureSwitch(val);
        StencilPixelOcclusionCullingParam::SetStencilPixelOcclusionCullingEnable(isEnabled);
        RS_LOGI("StencilPixelOcclusionCullingParamParse parse SpocEnabled %{public}d",
            StencilPixelOcclusionCullingParam::IsStencilPixelOcclusionCullingEnable());
    }
    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen