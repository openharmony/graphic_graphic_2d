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

#include "frame_saver.h"

#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "hilog/log.h"

#include "frame_info.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001400

#undef LOG_TAG
#define LOG_TAG "FramePainter"
#define LOGW(fmt, ...) HILOG_WARN(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)

FrameSaver::FrameSaver()
{
    struct stat saveDirectoryStat = {};
    if (stat(saveDirectory, &saveDirectoryStat) && errno != ENOENT) {
        LOGW("get stat '%{public}s' failed: %{public}s", saveDirectory, strerror(errno));
        return;
    }

    if (errno == ENOENT) {
        constexpr int directoryPermission = 0777; // drwxrwxrwx
        if (mkdir(saveDirectory, directoryPermission)) {
            LOGW("create directory '%{public}s' failed: %{public}s", saveDirectory, strerror(errno));
            return;
        }
    }

    std::stringstream ss;

    char tmpPath[PATH_MAX] = {0};
    if (realpath(ss.str().c_str(), tmpPath) == nullptr) {
        return;
    }

    ss << saveDirectory << "/" << GetRealPid() << ".log";
    ofs_.open(ss.str(), ofs_.out | ofs_.app);
}

FrameSaver::~FrameSaver()
{
    ofs_.close();
}

void FrameSaver::SaveFrameEvent(const FrameEventType &type, int64_t timeNs)
{
    if (!ofs_.is_open()) {
        LOGI("%{public}s %{public}s", GetNameByFrameEventType(type).c_str(), std::to_string(timeNs).c_str());
        return;
    }

    ofs_ << GetNameByFrameEventType(type) << " " << timeNs << std::endl;
}
} // namespace Rosen
} // namespace OHOS
