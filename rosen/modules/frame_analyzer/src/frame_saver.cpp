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
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FrameSaver" };
} // namespace

FrameSaver::FrameSaver()
{
    struct stat saveDirectoryStat = {};
    if (stat(saveDirectory, &saveDirectoryStat) && errno != ENOENT) {
        ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
            "get stat '%{public}s' failed: %{public}s",
            saveDirectory, strerror(errno));
        return;
    }

    if (errno == ENOENT) {
        constexpr int directoryPermission = 0777; // drwxrwxrwx
        if (mkdir(saveDirectory, directoryPermission)) {
            ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
                "create directory '%{public}s' failed: %{public}s",
                saveDirectory, strerror(errno));
            return;
        }
    }

    std::stringstream ss;
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
        ::OHOS::HiviewDFX::HiLog::Info(LABEL, "%{public}s %{public}s",
            GetNameByFrameEventType(type).c_str(), std::to_string(timeNs).c_str());
        return;
    }

    ofs_ << GetNameByFrameEventType(type) << " " << timeNs << std::endl;
}
} // namespace Rosen
} // namespace OHOS
