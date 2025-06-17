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

#include "mem_param_parse.h"
#include "memory/rs_memory_snapshot.h"

namespace OHOS::Rosen {

int32_t MEMParamParse::ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node)
{
    RS_LOGI("MEMParamParse start");
    xmlNode *currNode = &node;
    if (currNode == nullptr || currNode->xmlChildrenNode == nullptr) {
        RS_LOGD("MEMParamParse stop parsing, no nodes");
        return PARSE_GET_CHILD_FAIL;
    }

    currNode = currNode->xmlChildrenNode;
    for (; currNode; currNode = currNode->next) {
        if (currNode->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (ParseMemInternal(*currNode) != PARSE_EXEC_SUCCESS) {
            RS_LOGD("MEMParamParse stop parsing, parse internal fail");
            return PARSE_INTERNAL_FAIL;
        }
    }

    return PARSE_EXEC_SUCCESS;
}

int32_t MEMParamParse::ParseMemInternal(xmlNode &node)
{
    // Start Parse Feature Params
    int xmlParamType = GetXmlNodeAsInt(node);
    auto name = ExtractPropertyValue("name", node);
    auto val = ExtractPropertyValue("value", node);
    if (xmlParamType == PARSE_XML_FEATURE_SINGLEPARAM) {
        if (name == "RsWatchPoint") {
            MEMParam::SetRSWatchPoint(val);
            RS_LOGI("MEMParamParse parse RSWatchPoint %{public}s", MEMParam::GetRSWatchPoint().c_str());
        } else if (name == "RSCacheLimitsResourceSize" && IsNumber(val)) {
            MEMParam::SetRSCacheLimitsResourceSize(stoi(val));
            RS_LOGI("RSCacheLimitsResourceSize %{public}d", MEMParam::GetRSCacheLimitsResourceSize());
        } else if (name =="MemSnapshotPrintHilogLimit" && IsNumber(val)) {
            MemorySnapshot::Instance().SetMemSnapshotPrintHilogLimit(stoi(val));
            RS_LOGI("memSnapshotPrintHilogLimit %{public}d",
                MemorySnapshot::Instance().GetMemSnapshotPrintHilogLimit());
        }
    } else if (xmlParamType == PARSE_XML_FEATURE_SWITCH) {
        bool isEnabled = ParseFeatureSwitch(val);
        if (name == "ReclaimEnabled") {
            MEMParam::SetReclaimEnabled(isEnabled);
            RS_LOGI("MEMParamParse parse ReclaimEnabled %{public}d", MEMParam::IsReclaimEnabled());
        } else if (name == "DeeplyReleaseGpuResourceEnabled") {
            MEMParam::SetDeeplyRelGpuResEnable(isEnabled);
            RS_LOGI("MEMParamParse parse DeeplyReleaseGpuResourceEnabled %{public}d",
                MEMParam::IsDeeplyRelGpuResEnable());
        }
    }

    return PARSE_EXEC_SUCCESS;
}
} // namespace OHOS::Rosen