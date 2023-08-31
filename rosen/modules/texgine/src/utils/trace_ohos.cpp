/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"

#include <hitrace_meter.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void Trace::Start(const std::string &proc)
{
    StartTrace(HITRACE_TAG_GRAPHIC_AGP, proc);
}

void Trace::Finish()
{
    FinishTrace(HITRACE_TAG_GRAPHIC_AGP);
}

void Trace::Count(const std::string &key, int val)
{
    CountTrace(HITRACE_TAG_GRAPHIC_AGP, key, val);
}

void Trace::Disable()
{
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
#endif
