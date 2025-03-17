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

#ifndef UIFIRST_PARAM_PARSE_H
#define UIFIRST_PARAM_PARSE_H

#include <map>
#include "xml_parser_base.h"
#include "uifirst_param.h"

namespace OHOS::Rosen {
enum class UIFirstSwitchType {
    UIFIRST_ENABLED,
    CARD_UIFIRST_ENABLED,
    ROTATE_ENABLED,
    FREE_MULTI_WINDOW_ENABLED,
    UNKNOWN
};

class UIFirstParamParse : public XMLParserBase {
public:
    UIFirstParamParse() = default;
    ~UIFirstParamParse() = default;

    UIFirstSwitchType GetUIFirstSwitchType(const std::string& input);
    int32_t ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node) override;

private:
    int32_t ParseUIFirstInternal(FeatureParamMapType &featureMap, xmlNode &node);
    int32_t ParseUIFirstSingleParam(const std::string& name, const std::string& value);

    std::shared_ptr<UIFirstParam> uifirstParam_;
};
} // namespace OHOS::Rosen
#endif // UIFIRST_PARAM_PARSE_H