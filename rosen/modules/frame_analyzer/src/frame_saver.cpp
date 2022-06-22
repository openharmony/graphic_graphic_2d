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

#include <cinttypes>
#include <filesystem>
#include <map>
#include <sstream>
#include <unistd.h>

#include <hilog/log.h>

#include "frame_info.h"

namespace fs = std::filesystem;

namespace OHOS {
namespace Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FrameSaver" };
} // namespace

FrameSaver::FrameSaver()
{
    std::error_code ec;
    bool isExist = fs::exists(saveDirectory, ec);
    if (ec) {
        ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
            "get stat '%{public}s' failed: %{public}s",
            saveDirectory, ec.message().c_str());
        return;
    }

    if (isExist == false) {
        fs::create_directory(saveDirectory, ec);
        if (ec) {
            ::OHOS::HiviewDFX::HiLog::Warn(LABEL,
                "create directory '%{public}s' failed: %{public}s",
                saveDirectory, ec.message().c_str());
            return;
        }
    }

    std::stringstream ss;
    ss << saveDirectory << "/" << getpid() << ".log";
    ofs_.open(ss.str(), ofs_.out | ofs_.app);
}

FrameSaver::~FrameSaver()
{
    ofs_.close();
}

void FrameSaver::SaveFrameEvent(const FrameEventType &type, int64_t timeNs)
{
    static std::map<FrameEventType, const char *> frameEventTypeStringMap = {
        {FrameEventType::HandleInputStart, "HandleInputStart"},
        {FrameEventType::HandleInputEnd,   "HandleInputEnd  "},
        {FrameEventType::UploadStart,      "UploadStart     "},
        {FrameEventType::UploadEnd,        "UploadEnd       "},
        {FrameEventType::AnimateStart,     "AnimateStart    "},
        {FrameEventType::AnimateEnd,       "AnimateEnd      "},
        {FrameEventType::LayoutStart,      "LayoutStart     "},
        {FrameEventType::LayoutEnd,        "LayoutEnd       "},
        {FrameEventType::DrawStart,        "DrawStart       "},
        {FrameEventType::DrawEnd,          "DrawEnd         "},
        {FrameEventType::WaitVsyncStart,   "WaitVsyncStart  "},
        {FrameEventType::WaitVsyncEnd,     "WaitVsyncEnd    "},
        {FrameEventType::ReleaseStart,     "ReleaseStart    "},
        {FrameEventType::ReleaseEnd,       "ReleaseEnd      "},
        {FrameEventType::FlushStart,       "FlushStart      "},
        {FrameEventType::FlushEnd,         "FlushEnd        "},
    };

    if (ofs_.is_open() == false) {
        ::OHOS::HiviewDFX::HiLog::Info(LABEL, "%{public}s %{public}" PRIi64, frameEventTypeStringMap[type], timeNs);
        return;
    }

    ofs_ << frameEventTypeStringMap[type] << " " << timeNs << std::endl;
}
} // namespace Rosen
} // namespace OHOS
