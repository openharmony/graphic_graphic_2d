/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_UNI_RENDER_JUDGEMENT_H
#define RS_UNI_RENDER_JUDGEMENT_H

#include <set>
#include <string>

namespace OHOS {
namespace Rosen {
enum class UniRenderEnabledType {
    UNI_RENDER_DISABLED = 0,
    UNI_RENDER_ENABLED_FOR_ALL,
    UNI_RENDER_PARTIALLY_ENABLED,
};

// Judge the unified rendering strategy of RenderService.
class RSUniRenderJudgement final {
public:
    ~RSUniRenderJudgement() = default;

    // used by render server
    static UniRenderEnabledType GetUniRenderEnabledType();
    static const std::set<std::string>& GetUniRenderEnabledList();
    static bool QueryClientEnabled(const std::string &bundleName);
    static void InitUniRenderConfig();

private:
    RSUniRenderJudgement() = default;

    static void InitUniRenderWithConfigFile();

    static inline UniRenderEnabledType uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    static inline std::set<std::string> uniRenderEnabledList_ {};
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_RENDER_JUDGEMENT_H
