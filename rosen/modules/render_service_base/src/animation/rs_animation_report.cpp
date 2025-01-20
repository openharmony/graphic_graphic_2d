/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "animation/rs_animation_report.h"

#ifdef ROSEN_OHOS
#include "hisysevent.h"
#endif

namespace OHOS {
namespace Rosen {

void RSAnimationReport::ReportFinishCallbackMissing(int type, float duration)
{
#ifdef ROSEN_OHOS
    auto reportName = "ANIMATION_CALLBACK_MISSING";
    HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::GRAPHIC, reportName,
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC, "CURVE_TYPE", type, "DURATION", duration);
#endif
}
} // namespace Rosen
} // namespace OHOS
