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

#include "gslogger.h"

#include <fstream>
#include <iostream>
#include <unistd.h>

#include <hilog/log.h>

namespace {
const char *GetLevelStr(enum Gslogger::LOG_LEVEL level)
{
    switch (level) {
        case Gslogger::LOG_LEVEL::DEBUG: return "D";
        case Gslogger::LOG_LEVEL::INFO: return "I";
        case Gslogger::LOG_LEVEL::WARN: return "W";
        case Gslogger::LOG_LEVEL::ERROR: return "E";
        case Gslogger::LOG_LEVEL::FATAL: return "F";
    }
    return "?";
}
} // namespace

void Gslogger::Stdout(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        return;
    }

    // LOG_PHASE::END
    std::cout << "[" << GetLevelStr(logger.GetLevel()) << "] " << logger.str() << std::endl;
}

void Gslogger::Stderr(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        return;
    }

    // LOG_PHASE::END
    std::cerr << "[" << GetLevelStr(logger.GetLevel()) << "] " << logger.str() << std::endl;
}

void Gslogger::Hilog(Gslogger& logger, enum LOG_PHASE phase)
{
    struct HilogData {
        OHOS::HiviewDFX::HiLogLabel hiLogLabel;
    };
    auto data = logger.GetData<struct HilogData>();
    if (phase == LOG_PHASE::BEGIN) {
        auto label = va_arg(logger.GetVariousArgument(), const char *);
        data->hiLogLabel = { LOG_CORE, 0, label };
        return;
    }

    // LOG_PHASE::END
    auto fn = OHOS::HiviewDFX::HiLog::Debug;
    switch (logger.GetLevel()) {
        case LOG_LEVEL::DEBUG:
            fn = OHOS::HiviewDFX::HiLog::Debug;
            break;
        case LOG_LEVEL::INFO:
            fn = OHOS::HiviewDFX::HiLog::Info;
            break;
        case LOG_LEVEL::WARN:
            fn = OHOS::HiviewDFX::HiLog::Warn;
            break;
        case LOG_LEVEL::ERROR:
            fn = OHOS::HiviewDFX::HiLog::Error;
            break;
        case LOG_LEVEL::FATAL:
            fn = OHOS::HiviewDFX::HiLog::Fatal;
            break;
    }
    fn(data->hiLogLabel, "%{public}s", logger.str().c_str());
}

void Gslogger::FileLog(Gslogger& logger, enum LOG_PHASE phase)
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

    if (ofs) {
        ofs << "[" << GetLevelStr(logger.GetLevel()) << "] " << logger.str() << std::endl;
    }
}

void Gslogger::Func(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << "[" << logger.GetFunc() << "] ";
    }
}

void Gslogger::FuncLine(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << "[" << logger.GetFunc() << ":" << logger.GetLine() << "] ";
    }
}

void Gslogger::FileLine(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << "[" << logger.GetFile() << " +" << logger.GetLine() << "] ";
    }
}

void Gslogger::FileFuncLine(Gslogger& logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << "[" << logger.GetFile() << " +" << logger.GetLine() << ":" << logger.GetFunc() << "] ";
    }
}

void Gslogger::PidTid(Gslogger &logger, enum LOG_PHASE phase)
{
    if (phase == LOG_PHASE::BEGIN) {
        logger << "[" << getpid() << "][" << gettid() << "]";
    }
}

Gslogger::Gslogger(const std::string &file, const std::string &func, int line, enum LOG_LEVEL level, ...)
{
    file_ = file;
    func_ = func;
    line_ = line;
    level_ = level;
    va_start(vl_, level);

    while (true) {
        GsloggerWrapperFunc f = va_arg(vl_, GsloggerWrapperFunc);
        if (f == nullptr) {
            break;
        }

        f(*this, LOG_PHASE::BEGIN);
        wrappers_.push_back(f);
    }
}

Gslogger::~Gslogger()
{
    for (const auto &wrapper : wrappers_) {
        wrapper(*this, LOG_PHASE::END);
    }
}

const std::string &Gslogger::GetFile() const
{
    return file_;
}

const std::string &Gslogger::GetFunc() const
{
    return func_;
}

int Gslogger::GetLine() const
{
    return line_;
}

enum Gslogger::LOG_LEVEL Gslogger::GetLevel() const
{
    return level_;
}

va_list &Gslogger::GetVariousArgument()
{
    return vl_;
}
