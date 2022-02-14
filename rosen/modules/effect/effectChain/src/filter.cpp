/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_trace.h"
#include "filter.h"

namespace OHOS {
namespace Rosen {
void Filter::Process(ProcessData& data)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "Filter::DoProcess");
    DoProcess(data);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    if (this->GetFilterType() == FILTER_TYPE::ALGOFILTER) {
        std::swap(data.srcTextureID, data.dstTextureID);
    }
    if (GetNextFilter() != nullptr) {
        GetNextFilter()->Process(data);
    }
}

void Filter::AddNextFilter(std::shared_ptr<Filter> next)
{
    if (nextNum_ < nextPtrMax_) {
        next_ = next;
        if (next != nullptr) {
            next->AddPreviousFilter(std::shared_ptr<Filter>(this));
        }
    }
    if (nextNum_ == nextPtrMax_) {
        return;
    }
    nextNum_++;
}

void Filter::AddPreviousFilter(std::shared_ptr<Filter> previous)
{
    if (preNum_ < prePtrMax_) {
        previous_ = previous;
    }
    if (preNum_ == prePtrMax_) {
        return;
    }
    preNum_++;
}

std::shared_ptr<Filter> Filter::GetNextFilter()
{
    return next_;
}

std::shared_ptr<Filter> Filter::GetPreviousFilter()
{
    return previous_;
}

int Filter::GetInputNumber()
{
    return preNum_;
}

int Filter::GetOutputNumber()
{
    return nextNum_;
}

int Filter::GetMaxInputNumber()
{
    return prePtrMax_;
}

int Filter::GetMaxOutputNumber()
{
    return nextPtrMax_;
}
} // namespcae Rosen
} // namespace OHOS