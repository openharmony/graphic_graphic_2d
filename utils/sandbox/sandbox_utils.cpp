/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifdef _WIN32
#include <windows.h>
#endif

namespace OHOS {
#if !defined(OHOS_LITE) && !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
static pid_t g_appSandboxPid_ = -1;

const int PID_STR_SIZE = 5;
const int STATUS_LINE_SIZE = 1024;
const int PID_MAX_LEN = 11;

static int FindAndConvertPid(char *buf, int len)
{
    int count = 0;
    char pidBuf[PID_MAX_LEN] = {0};
    char *str = buf;

    if (len <= PID_STR_SIZE) {
        return -1;
    }

    while (*str != '\0') {
        if ((*str >= '0') && (*str <= '9') && (count < PID_MAX_LEN)) {
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

static pid_t ParseRealPid(void)
{
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
        if (strncmp(buf, "Tgid:", PID_STR_SIZE) == 0) {
            break;
        }
    }
    (void)fclose(fp);

    return static_cast<pid_t>(FindAndConvertPid(buf, strlen(buf)));
}
#endif

pid_t GetRealPid(void)
{
#ifdef _WIN32
    return GetCurrentProcessId();
#elif defined(OHOS_LITE) || defined(__APPLE__) || defined(__gnu_linux__)
    return getpid();
#else
    if (g_appSandboxPid_ < 0) {
        g_appSandboxPid_ = ParseRealPid();
    }
    return g_appSandboxPid_;
#endif
}
} // namespace OHOS
