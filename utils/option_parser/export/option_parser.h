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

#ifndef UTILS_OPTION_PARSER_EXPORT_OPTION_PARSER_H
#define UTILS_OPTION_PARSER_EXPORT_OPTION_PARSER_H

#include <list>
#include <string>
#include <vector>

class OptionParser {
public:
    int32_t Parse(int32_t argc, const char **argv);
    std::string GetErrorString();

    template<typename T>
    int32_t AddOption(const std::string &shortOpt, const std::string &longOpt, T &result)
    {
        return 0;
    }

    template<>
    int32_t AddOption<int32_t>(const std::string &shortOpt, const std::string &longOpt, int32_t &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::i32);
    }

    template<>
    int32_t AddOption<uint32_t>(const std::string &shortOpt, const std::string &longOpt, uint32_t &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::u32);
    }

    template<>
    int32_t AddOption<int64_t>(const std::string &shortOpt, const std::string &longOpt, int64_t &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::i64);
    }

    template<>
    int32_t AddOption<double>(const std::string &shortOpt, const std::string &longOpt, double &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::f64);
    }

    template<>
    int32_t AddOption<std::string>(const std::string &shortOpt, const std::string &longOpt, std::string &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::str);
    }

    template<>
    int32_t AddOption<bool>(const std::string &shortOpt, const std::string &longOpt, bool &result)
    {
        return AddOption(shortOpt, longOpt, &result, Option::ValueType::bol);
    }

    template<typename T>
    int32_t AddArguments(T &result)
    {
        return 0;
    }

    template<>
    int32_t AddArguments<int32_t>(int32_t &result)
    {
        return AddArguments(&result, Argument::ValueType::i32);
    }

    template<>
    int32_t AddArguments<uint32_t>(uint32_t &result)
    {
        return AddArguments(&result, Argument::ValueType::u32);
    }

    template<>
    int32_t AddArguments<int64_t>(int64_t &result)
    {
        return AddArguments(&result, Argument::ValueType::i64);
    }

    template<>
    int32_t AddArguments<double>(double &result)
    {
        return AddArguments(&result, Argument::ValueType::f64);
    }

    template<>
    int32_t AddArguments<std::string>(std::string &result)
    {
        return AddArguments(&result, Argument::ValueType::str);
    }

    int32_t GetSkippedArgc();
    const char **GetSkippedArgv();

private:
    int32_t ParseArgument(const char *arg, const char *arg2);
    int32_t ParseArgc(const char *arg, const char *arg2);
    int32_t ParseShortOption(const char *arg1, const char *arg2);
    int32_t ParseLongEqualOption(const char *arg, const char *arg2);
    int32_t ParseLongOption(const char *arg1, const char *arg2);
    int32_t AddSkipped(const char *arg, const char *arg2);

    struct Option {
        const std::string so;
        const std::string lo;
        union Value {
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            double f64;
            std::string str;
            bool bl;
        } *result;
        enum class ValueType {
            i32,
            u32,
            i64,
            f64,
            str,
            bol,
        } type;
    };

    int32_t AddOption(const std::string &shortOpt,
                      const std::string &longOpt, void *result, Option::ValueType type);

    struct Argument {
        union Value {
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            double f64;
            std::string str;
        } *result;
        enum class ValueType {
            i32,
            u32,
            i64,
            f64,
            str,
        } type;
    };

    int32_t AddArguments(void *result, Argument::ValueType type);

    std::list<struct Argument> arguments;
    std::vector<struct Option> options;
    std::vector<const char *> skipped;
    std::string error = "";
};

#endif // UTILS_OPTION_PARSER_EXPORT_OPTION_PARSER_H
