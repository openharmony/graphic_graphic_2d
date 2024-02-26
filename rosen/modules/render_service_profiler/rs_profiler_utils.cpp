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

#include <chrono>
#include <filesystem>
#include <sstream>

#include "rs_profiler_utils.h"

#ifndef REPLAY_TOOL_CLIENT
#include "rs_profiler_base.h"

#include "platform/common/rs_log.h"
#else
#include "../rs_adapt.h"
#endif

namespace OHOS::Rosen::Utils {

#ifndef REPLAY_TOOL_CLIENT

uint64_t RawNowNano()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

uint64_t NowNano()
{
    uint64_t curTimeNano = RawNowNano();
    curTimeNano = RSProfilerBase::TimePauseApply(curTimeNano);
    return curTimeNano;
}

#else

uint64_t RawNowNano()
{
    return 0;
}

uint64_t NowNano()
{
    return 0;
}

#endif

double Now()
{
    constexpr int nanosecInSec = 1'000'000'000;
    return (static_cast<double>(NowNano())) / nanosecInSec;
}

std::vector<std::string> Split(const std::string& input)
{
    std::istringstream iss(input);
    std::vector<std::string> parts { std::istream_iterator<std::string> { iss },
        std::istream_iterator<std::string> {} };
    return parts;
}

void FileRead(void* data, size_t size, size_t count, FILE* file)
{
    if (fread(data, size, count, file) < count) {
        RS_LOGE("Error while reading from file"); // NOLINT
    }
}

void FileWrite(const void* data, size_t size, size_t count, FILE* file)
{
    if (fwrite(data, size, count, file) < count) {
        RS_LOGE("Error while writing to file"); // NOLINT
    }
}

void FileSeek(FILE* stream, int64_t offset, int origin)
{
    if (fseek(stream, offset, origin) != 0) {
        RS_LOGE("Failed fseek in file"); // NOLINT
    }
}

FILE* FileOpen(const std::string& path, const std::string& openOptions)
{
    const std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(std::filesystem::path(path));
    if (std::filesystem::exists(canonicalPath)) {
        auto file = fopen(canonicalPath.c_str(), openOptions.c_str());
        if (file == nullptr) {
            RS_LOGE("Cant open file '%s'!", path.c_str()); // NOLINT
        }
        return file;
    }
    RS_LOGE("File '%s' doesn't exists!", path.c_str()); // NOLINT
    return nullptr;
}

void FileClose(FILE* file)
{
    if (fclose(file) != 0) {
        RS_LOGE("File close failed"); // NOLINT
    }
}

} // namespace OHOS::Rosen::Utils