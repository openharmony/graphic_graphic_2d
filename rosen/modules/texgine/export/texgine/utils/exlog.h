/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_EXLOG_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_EXLOG_H

#include "logger.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
void ExTime(Logger &logger, enum Logger::LOG_PHASE phase);

#define LOGNEX(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::NoReturn, Logger::Continue, Logger::FileLog, "exlog", NULL)
#define LOGCEX(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Continue, Logger::FileLog, "exlog", NULL)
#define LOGEX_FILE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileLog, "exlog", NULL)
#define LOGEX_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::Func, Logger::FileLog, "exlog", NULL)
#define LOGEX_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::FuncLine, Logger::FileLog, "exlog", NULL)
#define LOGEX_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::FileLine, Logger::FileLog, "exlog", NULL)
#define LOGEX_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::FileFuncLine, Logger::FileLog, "exlog", NULL)
#define LOGEX_PT_FILE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileLog, "exlog", NULL)
#define LOGEX_PT_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::PidTid, Logger::Func, Logger::FileLog, "exlog", NULL)
#define LOGEX_PT_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::PidTid, Logger::FuncLine, Logger::FileLog, "exlog", NULL)
#define LOGEX_PT_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::PidTid, Logger::FileLine, Logger::FileLog, "exlog", NULL)
#define LOGEX_PT_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    ExTime, Logger::PidTid, Logger::FileFuncLine, Logger::FileLog, "exlog", NULL)

#ifdef TEXGINE_ENABLE_DEBUGLOG
#define LOGNEX_DEBUG(...) LOGNEX(DEBUG, ##__VA_ARGS__)
#define LOGCEX_DEBUG(...) LOGCEX(DEBUG, ##__VA_ARGS__)
#define LOGEX_FILE_DEBUG(...) LOGEX_FILE(DEBUG, ##__VA_ARGS__)
#define LOGEX_FUNC_DEBUG(...) LOGEX_FUNC(DEBUG, ##__VA_ARGS__)
#define LOGEX_FUNC_LINE_DEBUG(...) LOGEX_FUNC_LINE(DEBUG, ##__VA_ARGS__)
#define LOGEX_FILE_LINE_DEBUG(...) LOGEX_FILE_LINE(DEBUG, ##__VA_ARGS__)
#define LOGEX_FFL_DEBUG(...) LOGEX_FFL(DEBUG, ##__VA_ARGS__)
#define LOGEX_PT_FILE_DEBUG(...) LOGEX_PT_FILE(DEBUG, ##__VA_ARGS__)
#define LOGEX_PT_FUNC_DEBUG(...) LOGEX_PT_FUNC(DEBUG, ##__VA_ARGS__)
#define LOGEX_PT_FUNC_LINE_DEBUG(...) LOGEX_PT_FUNC_LINE(DEBUG, ##__VA_ARGS__)
#define LOGEX_PT_FILE_LINE_DEBUG(...) LOGEX_PT_FILE_LINE(DEBUG, ##__VA_ARGS__)
#define LOGEX_PT_FFL_DEBUG(...) LOGEX_PT_FFL(DEBUG, ##__VA_ARGS__)
#else
#define LOGNEX_DEBUG(...) NoLogger()
#define LOGCEX_DEBUG(...) NoLogger()
#define LOGEX_FILE_DEBUG(...) NoLogger()
#define LOGEX_FUNC_DEBUG(...) NoLogger()
#define LOGEX_FUNC_LINE_DEBUG(...) NoLogger()
#define LOGEX_FILE_LINE_DEBUG(...) NoLogger()
#define LOGEX_FFL_DEBUG(...) NoLogger()
#define LOGEX_PT_FILE_DEBUG(...) NoLogger()
#define LOGEX_PT_FUNC_DEBUG(...) NoLogger()
#define LOGEX_PT_FUNC_LINE_DEBUG(...) NoLogger()
#define LOGEX_PT_FILE_LINE_DEBUG(...) NoLogger()
#define LOGEX_PT_FFL_DEBUG(...) NoLogger()
#endif
} // namespace TextEngine
} // namespace Roesen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_EXLOG_H
