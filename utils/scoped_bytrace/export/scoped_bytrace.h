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

#ifndef UTILS_TRACE_SCOPED_BYTRACE_H
#define UTILS_TRACE_SCOPED_BYTRACE_H

#include <string>

class ScopedBytrace {
public:
    ScopedBytrace(const std::string &proc);
    ~ScopedBytrace();

    void End();

private:
    std::string proc_;
    bool isEnd = false;
};

class ScopedDebugTrace {
public:
    ScopedDebugTrace(const std::string &traceStr);
    ~ScopedDebugTrace();
    static bool debugTraceEnabled_;
};

#endif // UTILS_TRACE_SCOPED_BYTRACE_H
