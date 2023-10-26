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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_TRACE_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_TRACE_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class ScopedTrace;

class Trace {
public:
    /*
     * @brief This method will be called when the scope trace starts.
     * @param proc The process name for scope tracking.
     */
    static void Start(const std::string &proc);

    /*
     * @brief End the scope tracking
     */
    static void Finish();

    /*
     * @brief Append the count of key to trace content.
     * @param key The proc what you what appned that counts to trace content
     * @param val The count of the key
     */
    static void Count(const std::string &key, int val);

    /*
     * @brief Disable the trace
     */
    static void Disable();
};

class ScopedTrace {
public:
    ScopedTrace(const std::string &proc)
    {
#ifdef LOGGER_ENABLE_SCOPE
        Trace::Start(proc);
#endif
    }

    ~ScopedTrace()
    {
        Finish();
    }

    /*
     * @brief End the scope tracking
     */
    void Finish()
    {
        if (isFinished == false) {
            isFinished = true;
#ifdef LOGGER_ENABLE_SCOPE
            Trace::Finish();
#endif
        }
    }

private:
    bool isFinished = false;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_TRACE_H
