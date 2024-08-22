/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef UTILS_INCLUDE_TEST_HEADER_H
#define UTILS_INCLUDE_TEST_HEADER_H

#include "hilog/log.h"

namespace OHOS {
static inline ::OHOS::HiviewDFX::HiLogLabel TEST_HEADER_HILOG_LABEL = { LOG_CORE, 0, "TEST" };
#define WMT_CPRINTF(color, func, fmt, ...) \
    func (TEST_HEADER_HILOG_LABEL, "\033[" #color "m" "<%{public}d>" fmt "\033[0m", __LINE__, ##__VA_ARGS__)
#define WMTLOGI(color, fmt, ...) \
    WMT_CPRINTF(color, HiviewDFX::HiLog::Info, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#define PART(part) WMTLOGI(33, part); if (const char *strPart = part)
#define STEP(desc) WMTLOGI(34, desc); if (const char *strDesc = desc)

#define STEP_CONDITION(condition) strPart << ": " << strDesc << " (" << condition << ")"

#define STEP_ASSERT_(l, r, func, opstr) ASSERT_##func(l, r) <<  STEP_CONDITION(#l " " opstr " " #r)

#define STEP_ASSERT_EQ(l, r) STEP_ASSERT_(l, r, EQ, "==")
#define STEP_ASSERT_NE(l, r) STEP_ASSERT_(l, r, NE, "!=")
#define STEP_ASSERT_GE(l, r) STEP_ASSERT_(l, r, GE, ">=")
#define STEP_ASSERT_LE(l, r) STEP_ASSERT_(l, r, LE, "<=")
#define STEP_ASSERT_GT(l, r) STEP_ASSERT_(l, r, GT, ">")
#define STEP_ASSERT_LT(l, r) STEP_ASSERT_(l, r, LT, "<")
} // namespace OHOS

#endif // UTILS_INCLUDE_TEST_HEADER_H
