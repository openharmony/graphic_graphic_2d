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

#include "common/rs_exception_check.h"
#ifdef ROSEN_OHOS
#include "platform/common/rs_hisysevent.h"
#endif

namespace OHOS {
namespace Rosen {

int64_t RSTimer::GetDuration()
{
    return (GetNanoSeconds() - timestamp_) / MILLI;
}

void ExceptionCheck::UploadRenderExceptionData()
{
#ifdef ROSEN_OHOS
    if (isUpload_) {
        RSHiSysEvent::EventWrite(RSEventName::RS_RENDER_EXCEPTION, RSEventType::RS_FAULT,
            "PID", pid_, "UID", uid_, "PROCESS_NAME", processName_,
            "EXCEPTION_CNT", exceptionCnt_, "EXCEPTION_TIME", exceptionMoment_,
            "EXCEPTION_POINT", exceptionPoint_);
    }
#endif
}
} // namespace Rosen
} // namespace OHOS