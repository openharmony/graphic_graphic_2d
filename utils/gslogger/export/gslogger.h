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

#ifndef UTILS_INCLUDE_LOGGER_EXPORT_GSLOGGER_H
#define UTILS_INCLUDE_LOGGER_EXPORT_GSLOGGER_H

#include <any>
#include <cstdarg>
#include <sstream>
#include <vector>

class Gslogger : public std::stringstream {
public:
    enum class LOG_LEVEL { DEBUG, INFO, WARN, ERROR, FATAL };
    enum class LOG_PHASE { BEGIN, END };

    using GsloggerWrapperFunc = void(*)(Gslogger &, enum LOG_PHASE phase);
    // output GsloggerWrapperFunc
    static void Stdout(Gslogger &logger, enum LOG_PHASE phase);
    static void Stderr(Gslogger &logger, enum LOG_PHASE phase);
    static void Hilog(Gslogger &logger, enum LOG_PHASE phase);
    static void FileLog(Gslogger &logger, enum LOG_PHASE phase);

    // wrapper GsloggerWrapperFunc
    static void Func(Gslogger &logger, enum LOG_PHASE phase); // 1
    static void FuncLine(Gslogger &logger, enum LOG_PHASE phase); // 2
    static void FileLine(Gslogger &logger, enum LOG_PHASE phase); // 3
    static void FileFuncLine(Gslogger &logger, enum LOG_PHASE phase); // 4
    static void PidTid(Gslogger &logger, enum LOG_PHASE phase); // +5

    Gslogger(const std::string &file, const std::string &func, int line, enum LOG_LEVEL level, ...);
    virtual ~Gslogger() override;

    const std::string &GetFile() const;
    const std::string &GetFunc() const;
    int GetLine() const;
    enum LOG_LEVEL GetLevel() const;
    va_list &GetVariousArgument();

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
    std::string file_;
    std::string func_;
    int line_;
    enum LOG_LEVEL level_;
    va_list vl_;
    std::any data_;
    std::vector<GsloggerWrapperFunc> wrappers_;
};

#define LOGGER_ARG(level) __FILE__, __func__, __LINE__, (Gslogger::LOG_LEVEL::level)

// hilog
#define DEFINE_HILOG_LABEL(str) \
    namespace { constexpr const char *HILOG_LABEL = str; }
#define GSLOG0HI(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Hilog, HILOG_LABEL, NULL)
#define GSLOG1HI(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Func, Gslogger::Hilog, HILOG_LABEL, NULL)
#define GSLOG2HI(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FuncLine, Gslogger::Hilog, HILOG_LABEL, NULL)
#define GSLOG3HI(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileLine, Gslogger::Hilog, HILOG_LABEL, NULL)
#define GSLOG4HI(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileFuncLine, Gslogger::Hilog, HILOG_LABEL, NULL)

// stdout
#define GSLOG0SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Stdout, NULL)
#define GSLOG1SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Func, Gslogger::Stdout, NULL)
#define GSLOG2SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FuncLine, Gslogger::Stdout, NULL)
#define GSLOG3SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileLine, Gslogger::Stdout, NULL)
#define GSLOG4SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileFuncLine, Gslogger::Stdout, NULL)
#define GSLOG5SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::Stdout, NULL)
#define GSLOG6SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::Func, Gslogger::Stdout, NULL)
#define GSLOG7SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FuncLine, Gslogger::Stdout, NULL)
#define GSLOG8SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileLine, Gslogger::Stdout, NULL)
#define GSLOG9SO(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileFuncLine, Gslogger::Stdout, NULL)

// stderr
#define GSLOG0SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Stderr, NULL)
#define GSLOG1SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Func, Gslogger::Stderr, NULL)
#define GSLOG2SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FuncLine, Gslogger::Stderr, NULL)
#define GSLOG3SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileLine, Gslogger::Stderr, NULL)
#define GSLOG4SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileFuncLine, Gslogger::Stderr, NULL)
#define GSLOG5SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::Stderr, NULL)
#define GSLOG6SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::Func, Gslogger::Stderr, NULL)
#define GSLOG7SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FuncLine, Gslogger::Stderr, NULL)
#define GSLOG8SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileLine, Gslogger::Stderr, NULL)
#define GSLOG9SE(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileFuncLine, Gslogger::Stderr, NULL)

// filelog
#define DEFINE_FILE_LABEL(str) \
    namespace { constexpr const char *FILE_LABEL = str; }
#define GSLOG0F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG1F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::Func, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG2F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FuncLine, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG3F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileLine, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG4F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::FileFuncLine, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG5F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG6F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::Func, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG7F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FuncLine, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG8F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileLine, Gslogger::FileLog, FILE_LABEL, NULL)
#define GSLOG9F(level) Gslogger(LOGGER_ARG(level), \
                                 Gslogger::PidTid, Gslogger::FileFuncLine, Gslogger::FileLog, FILE_LABEL, NULL)

#endif // UTILS_INCLUDE_LOGGER_EXPORT_GSLOGGER_H
