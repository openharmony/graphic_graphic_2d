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

#include "texgine/utils/logger.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>
#define gettid() syscall(__NR_gettid)

#ifdef LOGGER_NO_COLOR
#define IF_COLOR(x)
#else
#define IF_COLOR(x) x
#endif

namespace OHOS {
namespace TextEngine {
namespace {
const char *GetLevelStr(enum Logger::LOG_LEVEL level)
{
    switch (level) {
        case Logger::LOG_LEVEL::DEBUG: return IF_COLOR("\033[37m") "D" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::INFO: return IF_COLOR("\033[36m") "I" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::WARN: return IF_COLOR("\033[33m") "W" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::ERROR: return IF_COLOR("\033[31m") "E" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::FATAL: return IF_COLOR("\033[4;31m") "F" IF_COLOR("\033[0m");
    }
    return "?";
}
} // namespace

void Logger::NoReturn(Logger& logger, enum LOG_PHASE phase)
{
    logger.return_ = false;
}

void Logger::Continue(Logger& logger, enum LOG_PHASE phase)
{
    logger.continue_ = true;
}

void Logger::Stdout(Logger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        return;
    }

    // LOG_PHASE::END
    if (logger.continue_ == false) {
        std::cout << GetLevelStr(logger.GetLevel()) << " ";
    }

    std::cout << logger.str();
    if (logger.return_) {
        std::cout << std::endl;
    }
}

void Logger::Stderr(Logger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        return;
    }

    // LOG_PHASE::END
    if (logger.continue_ == false) {
        std::cerr << GetLevelStr(logger.GetLevel()) << " ";
    }

    std::cerr << logger.str();
    if (logger.return_) {
        std::cerr << std::endl;
    }
}

void Logger::FileLog(Logger& logger, enum LOG_PHASE phase)
{
    struct FileLogData {
        const char *filename;
    };
    auto data = logger.GetData<struct FileLogData>();
    if (phase == LOG_PHASE::BEGIN) {
        auto filename = va_arg(logger.GetVariousArgument(), const char *);
        data->filename = filename;
        return;
    }

    // LOG_PHASE::END
    std::ofstream ofs(data->filename, std::ofstream::out | std::ofstream::app);
    if (!ofs) {
        // open failed, errno
        return;
    }

    if (logger.continue_ == false) {
        ofs << GetLevelStr(logger.GetLevel()) << " ";
    }

    ofs << logger.str();
    if (logger.return_) {
        ofs << std::endl;
    }
}

void Logger::Func(Logger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m");
        logger.AlignFunc();
        logger << logger.GetFunc() << IF_COLOR("\033[0m") " ";
    }
}

void Logger::FuncLine(Logger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m");
        logger.AlignFunc();
        logger << logger.GetFunc() << " ";
        logger.AlignLine();
        logger << IF_COLOR("\033[35m") "+" << logger.GetLine() << IF_COLOR("\033[0m") " ";
    }
}
} // namespace TextEngine
} // namespace OHOS
