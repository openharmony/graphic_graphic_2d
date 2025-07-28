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

#ifndef DRM_PARAM_PARSE_H
#define DRM_PARAM_PARSE_H

#include "xml_parser_base.h"
#include "drm_param.h"

namespace OHOS::Rosen {
class DRMParamParse : public XMLParserBase {
public:
    DRMParamParse() = default;
    ~DRMParamParse() = default;

    int32_t ParseFeatureParam(FeatureParamMapType &featureMap, xmlNode &node) override;

private:
    int32_t ParseDrmInternal(xmlNode &node);
};
} // namespace OHOS::Rosen
#endif // DRM_PARAM_PARSE_H