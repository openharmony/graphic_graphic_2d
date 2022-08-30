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

#include "sandbox_utils.h"
#include <cstdlib>
#include <cstring>

namespace OHOS {
#ifndef OHOS_LITE
const int PID_STR_SIZE = 4;
const int STATUS_LINE_SIZE = 1024;

static int FindAndConvertPid(char *buf)
{
    int count = 0;
    char pidBuf[11] = {0}; /* 11: 32 bits to the maximum length of a string */
    char *str = buf;
    while (*str != '\0') {
        if ((*str >= '0') && (*str <= '9')) {
            pidBuf[count] = *str;
            count++;
            str++;
            continue;
        }

        if (count > 0) {
            break;
        }
        str++;
    }
    return atoi(pidBuf);
}
#endif

pid_t GetRealPid(void)
{
#ifdef OHOS_LITE
    return getpid();
#else
    const char *path = "/proc/self/status";
    char buf[STATUS_LINE_SIZE] = {0};
    FILE *fp = fopen(path, "r");
    if (fp == nullptr) {
        return -1;
    }

    while (!feof(fp)) {
        if (fgets(buf, STATUS_LINE_SIZE, fp) == nullptr) {
            fclose(fp);
            return -1;
        }
        if (strncmp(buf, "Pid:", PID_STR_SIZE) == 0) {
            break;
        }
    }
    (void)fclose(fp);

    return static_cast<pid_t>(FindAndConvertPid(buf));
#endif
}
} // namespace OHOS
