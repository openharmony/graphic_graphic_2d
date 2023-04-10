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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_LOGGER_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_LOGGER_H

#include <any>
#include <cerrno>
#include <cstdarg>
#include <cstring>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

namespace OHOS {
namespace TextEngine {
class Logger : public std::stringstream {
public:
    enum class LOG_LEVEL { DEBUG, INFO, WARN, ERROR, FATAL };
    enum class LOG_PHASE { BEGIN, END };

    using LoggerWrapperFunc = void(*)(Logger &, enum LOG_PHASE phase);

    // output LoggerWrapperFunc
    static void NoReturn(Logger &logger, enum LOG_PHASE phase);
    static void Continue(Logger &logger, enum LOG_PHASE phase);
    static void Stdout(Logger &logger, enum LOG_PHASE phase);
    static void Stderr(Logger &logger, enum LOG_PHASE phase);
    static void Hilog(Logger &logger, enum LOG_PHASE phase);
    static void FileLog(Logger &logger, enum LOG_PHASE phase);

    // wrapper LoggerWrapperFunc
    static void Func(Logger &logger, enum LOG_PHASE phase);
    static void FuncLine(Logger &logger, enum LOG_PHASE phase);
    static void FileLine(Logger &logger, enum LOG_PHASE phase);
    static void FileFuncLine(Logger &logger, enum LOG_PHASE phase);
    static void PidTid(Logger &logger, enum LOG_PHASE phase);

    static void SetScopeParam(int func, int line);
    static void EnterScope();
    static void ExitScope();

    Logger(const std::string &file, const std::string &func, int line, enum LOG_LEVEL level, ...);
    Logger(const Logger &logger);
    Logger(Logger &&logger);
    virtual ~Logger() override;

    const std::string &GetFile() const;
    const std::string &GetFunc() const;
    int GetLine() const;
    enum LOG_LEVEL GetLevel() const;
    va_list &GetVariousArgument();

    void Align(int num);
    void AlignLine();
    void AlignFunc();

    template<class T>
    std::shared_ptr<T> GetData()
    {
        using sptrT = std::shared_ptr<T>;
        sptrT ret = nullptr;
        auto pRet = std::any_cast<sptrT>(&data_);
        if (pRet != nullptr) {
            ret = *pRet;
        } else {
            ret = std::make_shared<T>();
            data_ = ret;
        }
        return ret;
    }

private:
    std::string file_ = "";
    std::string func_ = "";
    int line_ = 0;
    enum LOG_LEVEL level_ = LOG_LEVEL::DEBUG;
    bool return_ = true;
    bool continue_ = false;
    va_list vl_;
    std::any data_;
    std::vector<LoggerWrapperFunc> wrappers_;

    static inline int scope_ = 0;
    static inline std::mutex scopeMutex_;

#ifdef LOGGER_ENABLE_SCOPE
    static inline int alignFunc = 20;
    static inline int alignLine = 4;
#else
    static inline int alignFunc = 0;
    static inline int alignLine = 0;
#endif
};

class NoLogger {
public:
    template<class T>
    NoLogger &operator <<(const T &)
    {
        return *this;
    }
};

class ScopedLogger {
public:
    ScopedLogger(NoLogger &&logger);
    ScopedLogger(NoLogger &&logger, const std::string &name);
    ScopedLogger(Logger &&logger);
    ScopedLogger(Logger &&logger, const std::string &name);
    ~ScopedLogger();

    void Finish();

private:
    Logger *logger_ = nullptr;
};

#define LOGGER_ARG(level) __FILE__, __func__, __LINE__, (Logger::LOG_LEVEL::level)
#define LOGSYSERR " failed, because " << strerror(errno)

#define LOGSCOPED(name, logger, ...) ScopedLogger name(logger, ##__VA_ARGS__)
#define LOGSCOPED_FINISH(name) name.Finish()
#define LOGENTER() Logger::EnterScope()
#define LOGEXIT() Logger::ExitScope()

// stdout
#define LOGNSO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::NoReturn, Logger::Continue, Logger::Stdout, NULL)
#define LOGCSO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Continue, Logger::Stdout, NULL)
#define LOG0SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Stdout, NULL)
#define LOG1SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Func, Logger::Stdout, NULL)
#define LOG2SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FuncLine, Logger::Stdout, NULL)
#define LOG3SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileLine, Logger::Stdout, NULL)
#define LOG4SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileFuncLine, Logger::Stdout, NULL)
#define LOG5SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::Stdout, NULL)
#define LOG6SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::Func, Logger::Stdout, NULL)
#define LOG7SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FuncLine, Logger::Stdout, NULL)
#define LOG8SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileLine, Logger::Stdout, NULL)
#define LOG9SO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileFuncLine, Logger::Stdout, NULL)

// stderr
#define LOGNSE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::NoReturn, Logger::Continue, Logger::Stderr, NULL)
#define LOGCSE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Continue, Logger::Stderr, NULL)
#define LOG0SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Stderr, NULL)
#define LOG1SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Func, Logger::Stderr, NULL)
#define LOG2SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FuncLine, Logger::Stderr, NULL)
#define LOG3SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileLine, Logger::Stderr, NULL)
#define LOG4SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileFuncLine, Logger::Stderr, NULL)
#define LOG5SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::Stderr, NULL)
#define LOG6SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::Func, Logger::Stderr, NULL)
#define LOG7SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FuncLine, Logger::Stderr, NULL)
#define LOG8SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileLine, Logger::Stderr, NULL)
#define LOG9SE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileFuncLine, Logger::Stderr, NULL)

// filelog
#define DEFINE_FILE_LABEL(str) namespace { constexpr const char *FILE_LABEL = str; }
#define LOGNF(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::NoReturn, Logger::Continue, Logger::FileLog, FILE_LABEL, NULL)
#define LOGCF(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Continue, Logger::FileLog, FILE_LABEL, NULL)
#define LOG0F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileLog, FILE_LABEL, NULL)
#define LOG1F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::Func, Logger::FileLog, FILE_LABEL, NULL)
#define LOG2F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FuncLine, Logger::FileLog, FILE_LABEL, NULL)
#define LOG3F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileLine, Logger::FileLog, FILE_LABEL, NULL)
#define LOG4F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::FileFuncLine, Logger::FileLog, FILE_LABEL, NULL)
#define LOG5F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileLog, FILE_LABEL, NULL)
#define LOG6F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::Func, Logger::FileLog, FILE_LABEL, NULL)
#define LOG7F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FuncLine, Logger::FileLog, FILE_LABEL, NULL)
#define LOG8F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileLine, Logger::FileLog, FILE_LABEL, NULL)
#define LOG9F(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::PidTid, Logger::FileFuncLine, Logger::FileLog, FILE_LABEL, NULL)
} // namespace TextEngine
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_LOGGER_H
