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
#include <unistd.h>

#ifdef BUILD_NON_SDK_VER
#include <sys/syscall.h>
#define GET_TID() syscall(__NR_gettid)
#else
#ifdef _WIN32
#include <windows.h>
#define GET_TID GetCurrentThreadId
#endif

#if defined(BUILD_SDK_MAC) || defined(BUILD_SDK_IOS)
#include <stdlib.h>
#include <sys/syscall.h>
#define GET_TID() syscall(SYS_thread_selfid)
#elif defined(BUILD_SDK_ANDROID)
#define GET_TID() gettid()
#else
#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define GET_TID() syscall(SYS_gettid)
#endif
#endif

#ifdef __APPLE__
#define getpid getpid
#endif

#ifdef ERROR
#undef ERROR
#endif
#endif

#ifdef LOGGER_NO_COLOR
#define IF_COLOR(x)
#else
#define IF_COLOR(x) x
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace {
const char *GetLevelStr(enum Logger::LOG_LEVEL level)
{
    switch (level) {
        case Logger::LOG_LEVEL::DEBUG:
            return IF_COLOR("\033[37m") "D" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::INFO:
            return IF_COLOR("\033[36m") "I" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::WARN:
            return IF_COLOR("\033[33m") "W" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::ERROR:
            return IF_COLOR("\033[31m") "E" IF_COLOR("\033[0m");
        case Logger::LOG_LEVEL::FATAL:
            return IF_COLOR("\033[4;31m") "F" IF_COLOR("\033[0m");
    }
    return "?";
}
} // namespace

void Logger::SetToNoReturn(Logger &logger, enum LOG_PHASE phase)
{
    logger.return_ = false;
}

void Logger::SetToContinue(Logger &logger, enum LOG_PHASE phase)
{
    logger.continue_ = true;
}

void Logger::OutputByStdout(Logger &logger, enum LOG_PHASE phase)
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

void Logger::OutputByStderr(Logger &logger, enum LOG_PHASE phase)
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

void Logger::OutputByFileLog(Logger &logger, enum LOG_PHASE phase)
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
    ofs.close();
}

void Logger::AppendFunc(Logger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m");
        logger.AlignFunc();
        logger << logger.GetFunc() << IF_COLOR("\033[0m") " ";
    }
}

void Logger::AppendFuncLine(Logger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m");
        logger.AlignFunc();
        logger << logger.GetFunc() << " ";
        logger.AlignLine();
        logger << IF_COLOR("\033[35m") "+" << logger.GetLine() << IF_COLOR("\033[0m") " ";
    }
}

void Logger::AppendFileLine(Logger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m") << logger.GetFile() << " ";
        logger.AlignLine();
        logger << IF_COLOR("\033[35m") "+" << logger.GetLine() << IF_COLOR("\033[0m") " ";
    }
}

void Logger::AppendFileFuncLine(Logger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << IF_COLOR("\033[34m") << logger.GetFile() << " ";
        logger.AlignLine();
        logger << IF_COLOR("\033[35m") "+" << logger.GetLine() << " ";
        logger.AlignFunc();
        logger << logger.GetFunc() << IF_COLOR("\033[0m") " ";
    }
}

void Logger::AppendPidTid(Logger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << getpid() << ":" << GET_TID() << " ";
    }
}

void Logger::SetScopeParam(int func, int line)
{
    alignFunc = func;
    alignLine = line;
}

void Logger::EnterScope()
{
    std::lock_guard<std::mutex> lock(scopeMutex_);
    scope_++;
}

void Logger::ExitScope()
{
    std::lock_guard<std::mutex> lock(scopeMutex_);
    scope_--;
}

Logger::Logger(const std::string &file, const std::string &func, int line, enum LOG_LEVEL level, ...)
{
    *this << std::boolalpha;
    file_ = file;
    func_ = func;
    line_ = line;
    level_ = level;
    va_start(vl_, level);

    while (true) {
        LoggerWrapperFunc f = va_arg(vl_, LoggerWrapperFunc);
        if (f == nullptr) {
            break;
        }

        f(*this, LOG_PHASE::BEGIN);
        wrappers_.push_back(f);
    }

#ifdef LOGGER_ENABLE_SCOPE
    {
        std::lock_guard<std::mutex> lock(scopeMutex_);
        // The number of space if enable scope
        Align(scope_ * 2);  // 2 means multiple
    }
#endif
}

Logger::Logger(const Logger &logger)
{
    file_ = logger.file_;
    func_ = logger.func_;
    line_ = logger.line_;
    level_ = logger.level_;
    data_ = logger.data_;
    wrappers_ = logger.wrappers_;
    *this << logger.str();
}

Logger::Logger(Logger &&logger)
{
    file_ = logger.file_;
    func_ = logger.func_;
    line_ = logger.line_;
    level_ = logger.level_;
    data_ = logger.data_;
    wrappers_ = logger.wrappers_;
    *this << logger.str();

    logger.wrappers_.clear();
}

Logger::~Logger()
{
    for (const auto &wrapper : wrappers_) {
        wrapper(*this, LOG_PHASE::END);
    }
}

const std::string &Logger::GetFile() const
{
    return file_;
}

const std::string &Logger::GetFunc() const
{
    return func_;
}

int Logger::GetLine() const
{
    return line_;
}

enum Logger::LOG_LEVEL Logger::GetLevel() const
{
    return level_;
}

va_list &Logger::GetVariousArgument()
{
    return vl_;
}

void Logger::Align(int num)
{
    if (continue_) {
        return;
    }

    for (int32_t i = 0; i < num; i++) {
        *this << " ";
    }
}

void Logger::AlignLine()
{
    if (alignLine) {
        auto line = GetLine();
        auto num = line == 0 ? 1 : 0;
        while (line) {
            // 10 is to calculate the number of bits in the row where the function is located
            line /= 10;
            num++;
        }
        Align(alignLine - num);
    }
}

void Logger::AlignFunc()
{
    if (alignFunc) {
        Align(alignFunc - GetFunc().size());
    }
}

ScopedLogger::ScopedLogger(NoLogger &&logger)
{
}

ScopedLogger::ScopedLogger(NoLogger &&logger, const std::string &name)
{
}

ScopedLogger::ScopedLogger(Logger &&logger)
    : ScopedLogger(std::move(logger), "")
{
}

ScopedLogger::ScopedLogger(Logger &&logger, const std::string &name)
{
#ifdef LOGGER_ENABLE_SCOPE
    logger_ = new Logger(logger);
    *logger_ << "} " << name;
    logger << "{ ";
#endif
    logger << name;
    Logger::EnterScope();
}

ScopedLogger::~ScopedLogger()
{
    Finish();
}

void ScopedLogger::Finish()
{
    if (logger_) {
        Logger::ExitScope();
        delete logger_;
        logger_ = nullptr;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
