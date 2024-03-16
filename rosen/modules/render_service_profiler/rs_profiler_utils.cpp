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

#include "rs_profiler_utils.h"

#include <chrono>
#include <fcntl.h>
#include <filesystem>
#include <sstream>
#include <unistd.h>

#ifndef REPLAY_TOOL_CLIENT
#include "rs_profiler.h"

#include "platform/common/rs_log.h"
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
    return RSProfiler::PatchTime(RawNowNano());
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

static std::stringstream g_recordInMemory(std::ios::in | std::ios::out | std::ios::binary);
static FILE* g_recordInMemoryFile = reinterpret_cast<FILE*>(1);

FILE* FileOpen(const std::string& path, const std::string& openOptions)
{
    if (path == "RECORD_IN_MEMORY") {
        if (openOptions == "wbe") {
            g_recordInMemory.str("");
        }
        g_recordInMemory.seekg(0);
        g_recordInMemory.seekp(0);
        return g_recordInMemoryFile;
    }
    auto file = fopen(path.c_str(), openOptions.c_str());
    if (!IsFileValid(file)) {
        RS_LOGE("Cant open file '%s'!", path.c_str()); // NOLINT
    }
    return file;
}

void FileClose(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        return;
    }
    if (fclose(file) != 0) {
        RS_LOGE("File close failed"); // NOLINT
    }
}

bool IsFileValid(FILE* file)
{
    return file;
}

size_t FileSize(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        const int64_t position = g_recordInMemory.tellg();
        g_recordInMemory.seekg(0, std::ios_base::end);
        const int64_t size = g_recordInMemory.tellg();
        g_recordInMemory.seekg(position, std::ios_base::beg);
        return size;
    }
    if (!IsFileValid(file)) {
        return 0;
    }

    const int64_t position = ftell(file);
    if (position == -1) {
        return 0;
    }
    FileSeek(file, 0, SEEK_END);
    const size_t size = ftell(file);
    FileSeek(file, position, SEEK_SET);
    return static_cast<size_t>(size);
}

size_t FileTell(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        return g_recordInMemory.tellg();
    }
    if (!IsFileValid(file)) {
        return 0;
    }

    return ftell(file);
}

void FileSeek(FILE* file, int64_t offset, int origin)
{
    if (file == g_recordInMemoryFile) {
        if (origin == SEEK_SET) {
            g_recordInMemory.seekg(offset, std::ios_base::beg);
            g_recordInMemory.seekp(offset, std::ios_base::beg);
        } else if (origin == SEEK_CUR) {
            g_recordInMemory.seekg(offset, std::ios_base::cur);
            g_recordInMemory.seekp(offset, std::ios_base::cur);
        } else if (origin == SEEK_END) {
            g_recordInMemory.seekg(offset, std::ios_base::end);
            g_recordInMemory.seekp(offset, std::ios_base::end);
        }
        return;
    }
    if (fseek(file, offset, origin) != 0) {
        RS_LOGE("Failed fseek in file"); // NOLINT
    }
}

void FileRead(FILE* file, void* data, size_t size)
{
    if (file == g_recordInMemoryFile) {
        g_recordInMemory.read(reinterpret_cast<char*>(data), size);
        g_recordInMemory.seekp(g_recordInMemory.tellg());
        return;
    }
    if (fread(data, size, 1, file) < 1) {
        RS_LOGE("Error while reading from file"); // NOLINT
    }
}

void FileWrite(FILE* file, const void* data, size_t size)
{
    if (file == g_recordInMemoryFile) {
        g_recordInMemory.write(reinterpret_cast<const char*>(data), size);
        g_recordInMemory.seekg(g_recordInMemory.tellp());
        return;
    }
    if (fwrite(data, size, 1, file) < 1) {
        RS_LOGE("Error while writing to file"); // NOLINT
    }
}

// deprecated
void FileRead(void* data, size_t size, size_t count, FILE* file)
{
    FileRead(file, data, size * count);
}

// deprecated
void FileWrite(const void* data, size_t size, size_t count, FILE* file)
{
    FileWrite(file, data, size * count);
}

} // namespace OHOS::Rosen::Utils