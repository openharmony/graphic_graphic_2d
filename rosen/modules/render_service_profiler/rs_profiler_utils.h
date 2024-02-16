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

#ifndef RS_PROFILER_UTILS_H
#define RS_PROFILER_UTILS_H

#include <chrono>
#include <string>
#include <vector>

#define HGM_EFUNC OHOS::HiviewDFX::HiLog::Error
#define HGM_CPRINTF(func, fmt, ...) func({ LOG_CORE, 0xD001400, "RRI2D" }, fmt, ##__VA_ARGS__)
#define HGM_LOGE(fmt, ...) HGM_CPRINTF(HGM_EFUNC, fmt, ##__VA_ARGS__)

namespace OHOS::Rosen::Utils {

uint64_t RawNowNano();
uint64_t NowNano();
double Now();
std::vector<std::string> Split(const std::string& input);

constexpr float milli = 1e-3f;
constexpr float micro = 1e-6f;
constexpr float nano = 1e-9f;

void FileRead(void* data, size_t size, size_t count, FILE* file);

template <typename T>
void FileRead(T* data, size_t size, size_t count, FILE* file)
{
    FileRead(reinterpret_cast<void*>(data), size, count, file);
}

void FileWrite(const void* data, size_t size, size_t count, FILE* file);

template <typename T>
void FileWrite(const T* data, size_t size, size_t count, FILE* file)
{
    FileWrite(reinterpret_cast<const void*>(data), size, count, file);
}

void FileSeek(FILE *stream, int64_t offset, int origin);

FILE* FileOpen(const std::string& path, const std::string& openOptions);

void FileClose(FILE *file);

} // namespace OHOS::Rosen::Utils

#endif // RS_PROFILER_UTILS_H