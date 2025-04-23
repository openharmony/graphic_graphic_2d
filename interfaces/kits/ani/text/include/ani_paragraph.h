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

#ifndef OHOS_ROSEN_ANI_PARAGRAPH_H
#define OHOS_ROSEN_ANI_PARAGRAPH_H

#include <ani.h>

#include "typography.h"

namespace OHOS::Rosen {
class AniParagraph final {
public:
    AniParagraph();
    void setTypography(std::unique_ptr<OHOS::Rosen::Typography>& typography);
    static ani_status AniInit(ani_vm* vm, uint32_t* result);

private:
    static void Constructor(ani_env* env, ani_object object);
    static void LayoutSync(ani_env* env, ani_object object, ani_double width);
    static ani_double GetLongestLine(ani_env* env, ani_object object);
    static ani_ref GetLineMetrics(ani_env* env, ani_object object);
    static ani_object GetLineMetricsAt(ani_env* env, ani_object object, ani_double lineNumber);
    std::shared_ptr<Typography> paragraph_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PARAGRAPH_H