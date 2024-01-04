/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ring_array.h"

namespace OHOS {
namespace Rosen {

using namespace std;
const int DEFAULT_CAPABILITY = 101;
RingArray::RingArray()
    : arrayLen_(0),
    arrayCapability_(DEFAULT_CAPABILITY),
    arrayHead_(0),
    arrayTail_(0)
{
    Init();
}

void RingArray::Init()
{
    ringArray_ = new std::variant<int64_t, float, bool>[arrayCapability_];
}

RingArray::RingArray(int arrayCapability)
    : arrayLen_(0),
    arrayCapability_(arrayCapability + 1),
    arrayHead_(0),
    arrayTail_(0)
{
    Init();
}

RingArray::~RingArray()
{
    delete[] ringArray_;
    ringArray_ = nullptr;
}

void RingArray::ClearArray()
{
    arrayHead_ = 0;
    arrayTail_ = 0;
    arrayLen_ = 0;
}

bool RingArray::IsArrayEmpty() const
{
    if (arrayLen_ == 0) {
        return true;
    }
    return false;
}

bool RingArray::IsArrayFull() const
{
    if (arrayLen_ == arrayCapability_ - 1) {
        return true;
    }
    return false;
}

int RingArray::ArraySize() const
{
    return arrayLen_;
}

int RingArray::ArrayCapability() const
{
    return arrayCapability_ - 1;
}

void RingArray::PushElement(std::variant<int64_t, float, bool> element)
{
    if (IsArrayFull()) {
        arrayHead_++;
        arrayHead_ %= arrayCapability_;
        arrayLen_--;
    }
    ringArray_[arrayTail_] = element;
    arrayTail_++;
    arrayTail_ %= arrayCapability_;
    arrayLen_++;
}

bool RingArray::PopElement(std::variant<int64_t, float, bool> element)
{
    if (IsArrayEmpty()) {
        return false;
    } else {
        element = ringArray_[arrayHead_];
        arrayHead_++;
        arrayHead_ %= arrayCapability_;
        arrayLen_--;
    }
    return true;
}

std::variant<int64_t, float, bool> RingArray::GetElement(const int& pos) const
{
    return ringArray_[(arrayHead_ + pos) % arrayCapability_];
}

std::variant<int64_t, float, bool> RingArray::GetElementR(const int& pos) const
{
    return ringArray_[(arrayHead_ + (arrayLen_ - pos) - 1) % arrayCapability_];
}

std::variant<int64_t, float, bool> RingArray::GetHeadElement() const
{
    return ringArray_[arrayHead_];
}

std::variant<int64_t, float, bool> RingArray::GetTailElement() const
{
    return ringArray_[(arrayHead_ + arrayLen_ - 1) % arrayCapability_];
}

} // namespace Rosen
} // namespace OHOS