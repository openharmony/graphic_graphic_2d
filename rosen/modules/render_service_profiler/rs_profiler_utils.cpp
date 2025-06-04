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
#include <cstdarg>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#ifndef RENDER_PROFILER_APPLICATION
#include <dirent.h>
#include <sched.h>
#include <securec.h>
#include <unistd.h>

#include "directory_ex.h"
#include "rs_profiler_log.h"
#else
#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {

float Utils::Kilobytes(size_t bytes)
{
    constexpr float factor = 1024;
    return static_cast<float>(bytes) / factor;
}

float Utils::Megabytes(size_t bytes)
{
    constexpr float factor = 1024 * 1024;
    return static_cast<float>(bytes) / factor;
}

float Utils::Gigabytes(size_t bytes)
{
    constexpr float factor = 1024 * 1024 * 1024;
    return static_cast<float>(bytes) / factor;
}

// Time routines
uint64_t Utils::Now()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

double Utils::ToSeconds(uint64_t nano)
{
    constexpr double nanoToSeconds = 1e-9;
    return nano * nanoToSeconds;
}

uint64_t Utils::ToNanoseconds(double seconds)
{
    constexpr double secondsToNano = 1e9;
    return seconds * secondsToNano;
}

#ifdef RENDER_PROFILER_APPLICATION
// Cpu routines
int32_t Utils::GetCpuId()
{
    return 0;
}

void Utils::SetCpuAffinity(uint32_t cpu) {}

bool Utils::GetCpuAffinity(uint32_t cpu)
{
    return false; // NOLINT
}

// Process routines
pid_t Utils::GetPid()
{
    return _getpid();
}
#else
// Cpu routines
int32_t Utils::GetCpuId()
{
    return sched_getcpu();
}

void Utils::SetCpuAffinity(uint32_t cpu)
{
    cpu_set_t set = {};
    CPU_ZERO(&set);
    CPU_SET(cpu, &set); // NOLINT
    sched_setaffinity(getpid(), sizeof(set), &set);
}

bool Utils::GetCpuAffinity(uint32_t cpu)
{
    cpu_set_t mask = {};
    return (sched_getaffinity(0, sizeof(cpu_set_t), &mask) != -1) && CPU_ISSET(cpu, &mask); // NOLINT
}

// Process routines
pid_t Utils::GetPid()
{
    return getpid();
}
#endif

// String routines
std::string Utils::Format(const char* format, va_list args)
{
    if (!format) {
        return {};
    }

    va_list temporary;
    va_copy(temporary, args);
    const auto length = vsnprintf(nullptr, 0, format, temporary);
    va_end(temporary);

    if (length <= 0) {
        return {};
    }

    std::string out(length + 1, 0);
    const auto size = vsnprintf_s(out.data(), length + 1, length, format, args);
    if ((size > 0) && (size <= length)) {
        out.resize(size);
    } else {
        out.clear();
    }
    return out;
}

std::string Utils::Format(const char* format, ...)
{
    if (!format) {
        return {};
    }

    va_list args;
    va_start(args, format);
    auto out = Format(format, args);
    va_end(args);
    return out;
}

std::vector<std::string> Utils::Split(const std::string& string)
{
    std::istringstream stream(string);
    std::vector<std::string> parts { std::istream_iterator<std::string> { stream },
        std::istream_iterator<std::string> {} };
    return parts;
}

void Utils::Replace(const std::string& susbtring, std::string& string)
{
    std::string::size_type position = string.find(susbtring);
    while (position != string.npos) {
        string.replace(position, 1, "");
        position = string.find(susbtring);
    }
}

std::string Utils::ExtractNumber(const std::string& string)
{
    return std::regex_replace(string, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
}

int8_t Utils::ToInt8(const std::string& string)
{
    return static_cast<int8_t>(ToInt32(string));
}

int16_t Utils::ToInt16(const std::string& string)
{
    return static_cast<int16_t>(ToInt32(string));
}

int32_t Utils::ToInt32(const std::string& string)
{
    return static_cast<int32_t>(std::atol(string.data()));
}

int64_t Utils::ToInt64(const std::string& string)
{
    return std::atoll(string.data());
}

uint8_t Utils::ToUint8(const std::string& string)
{
    return ToUint32(string);
}

uint16_t Utils::ToUint16(const std::string& string)
{
    return ToUint32(string);
}

uint32_t Utils::ToUint32(const std::string& string)
{
    return ToUint64(string);
}

uint64_t Utils::ToUint64(const std::string& string)
{
    constexpr int32_t base = 10;
    char* end = const_cast<char*>(string.data()) + string.size();
    return std::strtoull(string.data(), &end, base);
}

float Utils::ToFp32(const std::string& string)
{
    return static_cast<float>(ToFp64(string));
}

double Utils::ToFp64(const std::string& string)
{
    char* end = const_cast<char*>(string.data()) + string.size();
    return std::strtod(string.data(), &end);
}

void Utils::ToNumber(const std::string& string, int8_t& number)
{
    number = ToInt8(string);
}

void Utils::ToNumber(const std::string& string, int16_t& number)
{
    number = ToInt16(string);
}

void Utils::ToNumber(const std::string& string, int32_t& number)
{
    number = ToInt32(string);
}

void Utils::ToNumber(const std::string& string, int64_t& number)
{
    number = ToInt64(string);
}

void Utils::ToNumber(const std::string& string, uint8_t& number)
{
    number = ToUint8(string);
}

void Utils::ToNumber(const std::string& string, uint16_t& number)
{
    number = ToUint16(string);
}

void Utils::ToNumber(const std::string& string, uint32_t& number)
{
    number = ToUint32(string);
}

void Utils::ToNumber(const std::string& string, uint64_t& number)
{
    number = ToUint64(string);
}

void Utils::ToNumber(const std::string& string, float& number)
{
    number = ToFp32(string);
}

void Utils::ToNumber(const std::string& string, double& number)
{
    number = ToFp64(string);
}

// Memory routines
bool Utils::Move(void* destination, size_t destinationSize, const void* source, size_t size)
{
    return memmove_s(destination, destinationSize, source, size) == EOK;
}

bool Utils::Set(void* data, size_t size, int32_t value, size_t count)
{
    return memset_s(data, size, value, count) == EOK;
}

// File system routines
std::string Utils::GetRealPath(const std::string& path)
{
    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        HRPD("PathToRealPath fails on %s !", path.data());
        realPath.clear();
    }
    return realPath;
}

std::string Utils::MakePath(const std::string& directory, const std::string& file)
{
    return NormalizePath(directory) + file;
}

std::string Utils::NormalizePath(const std::string& path)
{
    return (path.rfind('/') != path.size() - 1) ? path + "/" : path;
}

std::string Utils::GetFileName(const std::string& path)
{
#ifdef RENDER_PROFILER_APPLICATION
    return std::filesystem::path(path).filename().string();
#else
    std::string filename;
    const size_t lastSlashIdx = path.rfind('/');
    if (std::string::npos != lastSlashIdx) {
        filename = path.substr(lastSlashIdx + 1);
    }
    return filename;
#endif
}

std::string Utils::GetDirectory(const std::string& path)
{
#ifdef RENDER_PROFILER_APPLICATION
    return std::filesystem::path(path).parent_path().string();
#else
    std::string directory;
    const size_t lastSlashIdx = path.rfind('/');
    if (std::string::npos != lastSlashIdx) {
        directory = path.substr(0, lastSlashIdx);
    }
    return directory;
#endif
}

bool Utils::IsDirectory(const std::string& path)
{
#ifdef RENDER_PROFILER_APPLICATION
    return std::filesystem::is_directory(path);
#else
    struct stat st {};
    return (stat(path.data(), &st) == 0) && S_ISDIR(st.st_mode);
#endif
}

void Utils::IterateDirectory(const std::string& path, std::vector<std::string>& files)
{
    const std::string realPath = GetRealPath(path);
    if (realPath.empty()) {
        return;
    }

#ifdef RENDER_PROFILER_APPLICATION
    for (auto const& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (entry.is_directory()) {
            IterateDirectory(entry.path().string(), files);
        } else {
            files.push_back(entry.path().string());
        }
    }
#else
    DIR* directory = opendir(realPath.data());
    if (!directory) {
        return;
    }

    while (struct dirent* entry = readdir(directory)) {
        const std::string entryName(entry->d_name);
        if ((entryName == ".") || (entryName == "..")) {
            continue;
        }
        const std::string entryPath = NormalizePath(realPath) + entryName;
        if (entry->d_type == DT_DIR) {
            IterateDirectory(entryPath, files);
        } else {
            files.push_back(entryPath);
        }
    }
    closedir(directory);
#endif
}

void Utils::LoadLine(const std::string& path, std::string& line)
{
    line.clear();

    const std::string realPath = GetRealPath(path);
    if (realPath.empty()) {
        return;
    }

    std::ifstream file(realPath);
    if (file) {
        std::getline(file, line);
    }
}

void Utils::LoadLines(const std::string& path, std::vector<std::string>& lines)
{
    lines.clear();

    const std::string realPath = GetRealPath(path);
    if (realPath.empty()) {
        return;
    }

    std::ifstream file(realPath);
    if (file) {
        std::string line;
        while (std::getline(file, line)) {
            lines.emplace_back(line);
        }
    }
}

void Utils::LoadContent(const std::string& path, std::string& content)
{
    content.clear();

    const std::string realPath = GetRealPath(path);
    if (realPath.empty()) {
        return;
    }

    std::ifstream file(realPath);
    if (file) {
        copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(content));
        Replace("\r", content);
        Replace("\n", content);
    }
}

static std::stringstream g_recordInMemory(std::ios::in | std::ios::out | std::ios::binary);
static FILE* g_recordInMemoryFile = reinterpret_cast<FILE*>(1);

static bool IsRecordInMemoryFile(const std::string& path)
{
    return path == "RECORD_IN_MEMORY";
}

static bool HasWriteFlag(const std::string& options)
{
    return options.find('w') != std::string::npos;
}

static bool HasAppendFlag(const std::string& options)
{
    return options.find('a') != std::string::npos;
}

static bool ShouldFileBeCreated(const std::string& options)
{
    return HasWriteFlag(options) || HasAppendFlag(options);
}

bool Utils::FileExists(const std::string& path)
{
    if (IsRecordInMemoryFile(path)) {
        return true;
    }
#ifdef RENDER_PROFILER_APPLICATION
    return std::filesystem::exists(path);
#else
    struct stat st {};
    return (stat(path.data(), &st) == 0) && S_ISREG(st.st_mode);
#endif
}

bool Utils::FileDelete(const std::string& path)
{
    if (IsRecordInMemoryFile(path)) {
        g_recordInMemory = std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
        return true;
    }

    const std::string realPath = GetRealPath(path);
    if (!FileExists(realPath)) {
        return false;
    }

    return std::filesystem::remove(realPath);
}

FILE* Utils::FileOpen(const std::string& path, const std::string& options)
{
    if (IsRecordInMemoryFile(path)) {
        g_recordInMemory.seekg(0, std::ios_base::beg);
        g_recordInMemory.seekp(0, std::ios_base::beg);
        return g_recordInMemoryFile;
    }

    const std::string realPath = GetRealPath(path);
    if (realPath.empty()) {
        HRPE("FileOpen: '%s' is invalid!", path.data()); // NOLINT
        return nullptr;
    }

#ifndef RENDER_PROFILER_APPLICATION
    if (ShouldFileBeCreated(options) && !FileExists(realPath)) {
        auto file = open(realPath.data(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
        if (file != -1) {
            fdsan_exchange_owner_tag(file, 0, LOG_DOMAIN);
            fdsan_close_with_tag(file, LOG_DOMAIN);
        }
    }
#endif

    auto file = fopen(realPath.data(), options.data());
    if (!IsFileValid(file)) {
        HRPE("FileOpen: Cannot open '%s' with options '%s'!", realPath.data(), options.data()); // NOLINT
    }
    return file;
}

void Utils::FileClose(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        g_recordInMemory.seekg(0, std::ios_base::beg);
        g_recordInMemory.seekp(0, std::ios_base::beg);
        return;
    }
    if (fflush(file) != 0) {
        HRPE("File flush failed"); // NOLINT
    }
    if (fclose(file) != 0) {
        HRPE("File close failed"); // NOLINT
    }
}

bool Utils::IsFileValid(FILE* file)
{
    return file != nullptr;
}

size_t Utils::FileSize(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        const auto position = g_recordInMemory.tellg();
        g_recordInMemory.seekg(0, std::ios_base::end);
        const auto size = g_recordInMemory.tellg();
        if (size == -1) {
            g_recordInMemory.seekg(0, std::ios_base::beg);
            return 0;
        }
        g_recordInMemory.seekg(position, std::ios_base::beg);
        return static_cast<size_t>(size);
    }
    if (!IsFileValid(file)) {
        return 0;
    }

    const auto position = ftell(file);
    if (position == -1) {
        return 0;
    }
    FileSeek(file, 0, SEEK_END);
    const auto size = ftell(file);
    FileSeek(file, position, SEEK_SET);
    return static_cast<size_t>(size);
}

size_t Utils::FileTell(FILE* file)
{
    if (file == g_recordInMemoryFile) {
        return g_recordInMemory.tellg();
    }
    if (!IsFileValid(file)) {
        return 0;
    }

    return ftell(file);
}

void Utils::FileSeek(FILE* file, int64_t offset, int origin)
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
        HRPE("Failed fseek in file"); // NOLINT
    }
}

void Utils::FileRead(FILE* file, void* data, size_t size)
{
    if (size == 0) { // Avoid the frequent logging when size is zero
        return;
    }
    if (!data) {
        HRPE("FileRead: Data is null"); // NOLINT
        return;
    }

    if (file == g_recordInMemoryFile) {
        g_recordInMemory.read(reinterpret_cast<char*>(data), size);
        g_recordInMemory.seekp(g_recordInMemory.tellg());
        return;
    }
    if (fread(data, size, 1, file) < 1) {
        HRPE("FileRead: Error while reading from file"); // NOLINT
    }
}

void Utils::FileWrite(FILE* file, const void* data, size_t size)
{
    const size_t maxDataSize = 2'000'000'000; // To make sure size is a valid value
    if (!data || (size == 0) || (size > maxDataSize)) {
        HRPD("FileWrite: data or size is invalid, size %zu", size); // NOLINT
        return;
    }

    if (file == g_recordInMemoryFile) {
        g_recordInMemory.write(reinterpret_cast<const char*>(data), size);
        g_recordInMemory.seekg(g_recordInMemory.tellp());
        return;
    }
    if (fwrite(data, size, 1, file) < 1) {
        HRPE("FileWrite: Error while writing to file"); // NOLINT
    }
}

// deprecated
void Utils::FileRead(void* data, size_t size, size_t count, FILE* file)
{
    FileRead(file, data, size * count);
}

// deprecated
void Utils::FileWrite(const void* data, size_t size, size_t count, FILE* file)
{
    FileWrite(file, data, size * count);
}

} // namespace OHOS::Rosen