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

#ifndef RS_MULTI_DISPLAY_FEATURE_PARAM_H
#define RS_MULTI_DISPLAY_FEATURE_PARAM_H

#include "rs_multi_display_feature_common.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string>

namespace OHOS::Rosen {
class RSMultiDisplayFeatureParam {
public:
    RSMultiDisplayFeatureParam() = delete;
    ~RSMultiDisplayFeatureParam() = delete;
    static bool Load(const std::string& filePath = {});
    static bool IsSplitScreenFeatureEnable();
    static bool IsInterpolationFeatureEnable();
    static bool IsCrossDomainFeatureEnable();
    static bool IsScreenInCrossDomain(ScreenId screenId);
    static std::optional<SplitScreenParams> GetSplitScreenParams(ScreenId screenId);
    static std::optional<InterpolationParams> GetInterpolationParams(ScreenId screenId);

private:
    static bool Parse(xmlDoc& doc);
    static void ParseFeatureParam(xmlNode& featuresNode);
    inline static SplitScreenFeature splitFeature_;
    inline static InterpolationFeature interpolationFeature_;
    inline static CrossDomainFeature crossDomainFeature_;
};
} // OHOS rosen
#endif // RS_MULTI_DISPLAY_FEATURE_PARAM_H