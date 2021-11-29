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

#include <option_parser.h>

#include <sstream>

int32_t OptionParser::ParseArgument(const char *arg)
{
    if (arguments.empty()) {
        return -1;
    }

    std::stringstream ss(arg);
    switch (arguments.front().type) {
        case Argument::ValueType::i32:
            ss >> arguments.front().result->i32;
            break;
        case Argument::ValueType::i64:
            ss >> arguments.front().result->i64;
            break;
        case Argument::ValueType::f64:
            ss >> arguments.front().result->f64;
            break;
        case Argument::ValueType::str:
            ss >> arguments.front().result->str;
            break;
    }

    if (!ss.eof() || !ss) {
        error = "parse ";
        error = error + arg + " error";
        return 1;
    }

    arguments.pop_front();
    return 0;
}

int32_t OptionParser::ParseShortOption(const char *arg1, const char *arg2)
{
    for (const auto &option : options) {
        if (option.so == arg1) {
            if (option.type == Option::ValueType::bol) {
                option.result->bl = !option.result->bl;
                return -1;
            } else if (arg2 == nullptr) {
                error = option.so + " need argument";
                return 1;
            }

            std::stringstream ss(arg2);
            switch (option.type) {
                case Option::ValueType::i32:
                    ss >> option.result->i32;
                    break;
                case Option::ValueType::i64:
                    ss >> option.result->i64;
                    break;
                case Option::ValueType::f64:
                    ss >> option.result->f64;
                    break;
                case Option::ValueType::str:
                    ss >> option.result->str;
                    break;
                default:
                    assert(!"no way");
                    break;
            }

            if (!ss.eof() || !ss) {
                error = "parse ";
                error = error + arg1 + " error, " + arg2;
                return 1;
            }
        }
    }
    return 0;
}

int32_t OptionParser::ParseLongOption(const char *arg1, const char *arg2)
{
    for (const auto &option : options) {
        if (option.lo == arg1) {
            if (option.type == Option::ValueType::bol) {
                option.result->bl = !option.result->bl;
                return -1;
            } else if (arg2 == nullptr) {
                error = option.lo + " need argument";
                return 1;
            }

            std::stringstream ss(arg2);
            switch (option.type) {
                case Option::ValueType::i32:
                    ss >> option.result->i32;
                    break;
                case Option::ValueType::i64:
                    ss >> option.result->i64;
                    break;
                case Option::ValueType::f64:
                    ss >> option.result->f64;
                    break;
                case Option::ValueType::str:
                    ss >> option.result->str;
                    break;
                default:
                    assert(!"no way");
                    break;
            }

            if (!ss.eof() || !ss) {
                error = "parse ";
                error = error + arg1 + " error, " + arg2;
                return 1;
            }
        }
    }
    return 0;
}

int32_t OptionParser::ParseArgc(const char *arg)
{
    if (arg[0] == 0) {
        return 1;
    }

    if (arg[0] != '-') {
        skipped.push_back(arg);
        return 1;
    }

    if (arg[1] == 0) {
        return 1;
    }
    return 0;
}

int32_t OptionParser::Parse(int32_t argc, const char **argv)
{
    for (int32_t i = 0; i < argc; i++) {
        auto ret = ParseArgument(argv[i]);
        if (ret > 0) {
            return ret;
        } else if (ret == 0) {
            continue;
        }

        if (ParseArgc(argv[i])) {
            continue;
        }

        if (argv[i][1] != '-') {
            ret = ParseShortOption(argv[i] + 1, argv[i + 1]);
            if (ret > 0) {
                return ret;
            } else if (ret == 0) {
                i++;
            }
            continue;
        }

        bool parsed = false;
        for (const char *c = argv[i]; *c; c++) {
            if (*c == '=') {
                std::string arg1(argv[i], c - argv[i]);
                std::string arg2(c + 1);
                ret = ParseLongOption(arg1.c_str() + 0x2, arg2.c_str());
                if (ret) {
                    return ret;
                }
                parsed = true;
                break;
            }
        }

        if (parsed) {
            continue;
        }

        ret = ParseLongOption(argv[i] + 0x2, argv[i + 1]);
        if (ret > 0) {
            return ret;
        } else if (ret == 0) {
            i++;
        }
    }

    if (!arguments.empty()) {
        error = "need more arguments";
        return 1;
    } else {
        skipped.push_back(nullptr);
    }

    return 0;
}

int32_t OptionParser::AddOption(const std::string &shortOpt,
    const std::string &longOpt, void *result, Option::ValueType type)
{
    struct Option option = {
        .so = shortOpt,
        .lo = longOpt,
        .result = reinterpret_cast<union Option::Value *>(result),
        .type = type,
    };
    options.emplace_back(std::move(option));
    return 0;
}

int32_t OptionParser::AddArguments(void *result, Argument::ValueType type)
{
    struct Argument argument = {
        .result = reinterpret_cast<union Argument::Value *>(result),
        .type = type,
    };
    arguments.emplace_back(std::move(argument));
    return 0;
}

std::string OptionParser::GetErrorString()
{
    return error;
}

int32_t OptionParser::GetSkippedArgc()
{
    return skipped.size() - 1;
}

const char **OptionParser::GetSkippedArgv()
{
    return skipped.data();
}
