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

#include "texgine/utils/trace.h"

#include <chrono>
#include <cstdio>
#include <list>
#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TraceContent {
public:
    static TraceContent &GetInstance()
    {
        static TraceContent tc;
        return tc;
    }

    ~TraceContent()
    {
        auto fp = fopen("texgine.trace", "w");
        if (fp == nullptr) {
            return;
        }

        fprintf(fp, "# tracer: nop\n");
        for (const auto &[timeUs, content] : contents_) {
            fprintf(fp, "texgine-1 (1) [000] .... %.6lf: tracing_mark_write: %c|1|%s\n",
                    timeUs / 1e6, content[0], content.substr(1).c_str());
        }

        fclose(fp);
    }

    void Disable()
    {
        enable_ = false;
    }

    void AppendContent(char action, const std::string &content)
    {
        if (enable_ == false) {
            return;
        }

        auto nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        std::string c;
        c.push_back(action);
        c.insert(c.end(), content.begin(), content.end());
        contents_.push_back(std::make_pair(nowUs, std::move(c)));
    }

private:
    std::list<std::pair<uint64_t, std::string>> contents_;
    bool enable_ = true;
};

void Trace::Start(const std::string &proc)
{
    TraceContent::GetInstance().AppendContent('B', proc);
}

void Trace::Finish()
{
    TraceContent::GetInstance().AppendContent('E', "");
}

void Trace::Count(const std::string &key, int val)
{
    TraceContent::GetInstance().AppendContent('C', key + "|" + std::to_string(val));
}

void Trace::Disable()
{
    TraceContent::GetInstance().Disable();
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
