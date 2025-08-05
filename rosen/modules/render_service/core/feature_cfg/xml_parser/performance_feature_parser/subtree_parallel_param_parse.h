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

#ifndef SUBTREE_PARAM_PARSE_H
#define SUBTREE_PARAM_PARSE_H

#include <map>
#include "subtree_parallel_param.h"
#include "xml_parser_base.h"

namespace OHOS::Rosen {
enum class SubtreeSwitchType {
    SUBTREE_ENABLED,
    MULTIWIN_POLICY_ENABLED,
    MULTIWIN_POLICY_SURFACE_NUMBER,
    RB_POLICY_ENABLED,
    RB_POLICY_CHILDREN_WEIGHT,
    RB_POLICY_SUBTREE_WEIGHT,
    SUBTREE_PARALLEL_ENABLE_SCENE,
    UNKNOWN
};

class SubtreeParallelParamParse : public XMLParserBase {
public:
    SubtreeParallelParamParse() = default;
    ~SubtreeParallelParamParse() = default;

    int32_t ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node) override;

private:
    SubtreeSwitchType GetSubtreeSwitchType(const std::string &input);
    int32_t ParseFeatureMultiParamForApp(xmlNode &node, std::string &name);
    int32_t ParseFeatureSingleParam(std::string name, std::string val);

    int32_t ParseSubtreeInternal(xmlNode &node);
};
} // namespace OHOS::Rosen
#endif // SUBTREE_PARAM_PARSE_H