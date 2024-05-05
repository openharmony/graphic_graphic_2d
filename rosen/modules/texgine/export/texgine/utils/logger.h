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
namespace Rosen {
namespace TextEngine {
class Logger : public std::stringstream {
public:
    enum class LOG_LEVEL { DEBUG, INFO, WARN, ERROR, FATAL };
    enum class LOG_PHASE { BEGIN, END };

    using LoggerWrapperFunc = void(*)(Logger &, enum LOG_PHASE phase);

    /*
     * @brief output LoggerWrapperFunc
     */
    static void SetToNoReturn(Logger &logger, enum LOG_PHASE phase);
    static void SetToContinue(Logger &logger, enum LOG_PHASE phase);
    static void OutputByStdout(Logger &logger, enum LOG_PHASE phase);
    static void OutputByStderr(Logger &logger, enum LOG_PHASE phase);
    static void OutputByHilog(Logger &logger, enum LOG_PHASE phase);
    static void OutputByFileLog(Logger &logger, enum LOG_PHASE phase);

    /*
     * @brief wrapper LoggerWrapperFunc
     */
    static void AppendFunc(Logger &logger, enum LOG_PHASE phase);
    static void AppendFuncLine(Logger &logger, enum LOG_PHASE phase);
    static void AppendFileLine(Logger &logger, enum LOG_PHASE phase);
    static void AppendFileFuncLine(Logger &logger, enum LOG_PHASE phase);
    static void AppendPidTid(Logger &logger, enum LOG_PHASE phase);

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
    ScopedLogger(const ScopedLogger &) = default;
    ScopedLogger(NoLogger &&logger);
    ScopedLogger(NoLogger &&logger, const std::string &name);
    ScopedLogger(Logger &&logger);
    ScopedLogger(Logger &&logger, const std::string &name);
    ScopedLogger &operator=(const ScopedLogger &) = default;
    ~ScopedLogger();

    void Finish();

private:
    Logger *logger_ = nullptr;
};

#define LOGGER_ARG(level) __FILE__, __func__, __LINE__, (Logger::LOG_LEVEL::level)
#define LOGSYSERR " failed, because " << strerror(errno)

#define LOGSCOPED(name, logger, ...) ScopedLogger name(logger, ##__VA_ARGS__)
#define LOGSCOPED_FINISH(name) (name).Finish()
#define LOGENTER() Logger::EnterScope()
#define LOGEXIT() Logger::ExitScope()

/*
 * @brief stdout
 */
#define LOGNSO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToNoReturn, Logger::SetToContinue, Logger::OutputByStdout, NULL)
#define LOGCSO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToContinue, Logger::OutputByStdout, NULL)
#define LOGSO(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::OutputByStdout, NULL)
#define LOGSO_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFunc, Logger::OutputByStdout, NULL)
#define LOGSO_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFuncLine, Logger::OutputByStdout, NULL)
#define LOGSO_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileLine, Logger::OutputByStdout, NULL)
#define LOGSO_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileFuncLine, Logger::OutputByStdout, NULL)
#define LOGSO_PT(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::OutputByStdout, NULL)
#define LOGSO_PT_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFunc, Logger::OutputByStdout, NULL)
#define LOGSO_PT_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFuncLine, Logger::OutputByStdout, NULL)
#define LOGSO_PT_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileLine, Logger::OutputByStdout, NULL)
#define LOGSO_PT_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileFuncLine, Logger::OutputByStdout, NULL)

/*
 * @brief stderr
 */
#define LOGNSE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToNoReturn, Logger::SetToContinue, Logger::OutputByStderr, NULL)
#define LOGCSE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToContinue, Logger::OutputByStderr, NULL)
#define LOGSE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::OutputByStderr, NULL)
#define LOGSE_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFunc, Logger::OutputByStderr, NULL)
#define LOGSE_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFuncLine, Logger::OutputByStderr, NULL)
#define LOGSE_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileLine, Logger::OutputByStderr, NULL)
#define LOGSE_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileFuncLine, Logger::OutputByStderr, NULL)
#define LOGSE_PT(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::OutputByStderr, NULL)
#define LOGSE_PT_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFunc, Logger::OutputByStderr, NULL)
#define LOGSE_PT_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFuncLine, Logger::OutputByStderr, NULL)
#define LOGSE_PT_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileLine, Logger::OutputByStderr, NULL)
#define LOGSE_PT_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileFuncLine, Logger::OutputByStderr, NULL)

/*
 * @brief filelog
 */
#define DEFINE_FILE_LABEL(str) namespace { constexpr const char *FILE_LABEL = str; }
#define LOGNF(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToNoReturn, Logger::SetToContinue, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGCF(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::SetToContinue, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFunc, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFuncLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendFileFuncLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_PT(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_PT_FUNC(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFunc, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_PT_FUNC_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFuncLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_PT_FILE_LINE(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
#define LOGF_PT_FFL(level, ...) Logger(LOGGER_ARG(level), ##__VA_ARGS__, \
    Logger::AppendPidTid, Logger::AppendFileFuncLine, Logger::OutputByFileLog, FILE_LABEL, NULL)
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_LOGGER_H
