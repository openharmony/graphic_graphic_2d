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

#ifndef IMAGE_ENHANCE_PARAM_PARSE_H
#define IMAGE_ENHANCE_PARAM_PARSE_H

#include "common/rs_common_hook.h"
#include "image_enhance_param.h"
#include "securec.h"
#include "xml_parser_base.h"

namespace OHOS::Rosen {
class ImageEnhanceParamParse : public XMLParserBase {
public:
    ImageEnhanceParamParse() = default;
    ~ImageEnhanceParamParse() = default;

    int32_t ParseFeatureParam(FeatureParamMapType& featureMap, xmlNode& node) override;

private:
    int32_t ParseImageEnhanceInternal(xmlNode& node);
    int32_t ParseFeatureMultiParamForApp(xmlNode& node);

    int32_t ParseImageEnhanceParamInternal(xmlNode& node);
    int32_t ParseImageEnhanceParam(xmlNode& node);
    bool CheckParams();

    int32_t ParseImageEnhanceAlgoInternal(xmlNode& node);
    int32_t ParseImageEnhanceAlgoParam(xmlNode& node);
    bool CheckAlgoParams(RSImageDetailEnhanceAlgoParams& algoParams);

    template <typename T>
    bool ExtractValue(xmlNode& node, const std::string& format, T& value)
    {
        T parseValue;
        std::string nodeValueStr = ExtractPropertyValue("value", node);
        if (sscanf_s(nodeValueStr.c_str(), format.c_str(), &parseValue) <= 0) {
            RS_LOGE("Invalid input! node value = %{public}s", nodeValueStr.c_str());
            return false;
        }
        if (parseValue < 0) {
            return false;
        }
        value = parseValue;
        return true;
    }

    RSImageDetailEnhanceParams params_{};
    std::unordered_map<std::string, RSImageDetailEnhanceAlgoParams> algoParams_{};
};
} // namespace OHOS::Rosen
#endif // IMAGE_ENHANCE_PARAM_PARSE_H