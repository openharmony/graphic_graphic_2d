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

#ifndef UTILS_INCLUDE_RING_ARRAY_H
#define UTILS_INCLUDE_RING_ARRAY_H
#include <cstdint>
#include <variant>

namespace OHOS {
namespace Rosen {

class RingArray {
public:
    RingArray(); // default size of ringArray is 100
    explicit RingArray(int arrayCapability);
    ~RingArray();

    void Init();
    void ClearArray();
    bool IsArrayEmpty() const;
    bool IsArrayFull() const;
    int ArraySize() const;
    int ArrayCapability() const;
    void PushElement(std::variant<int64_t, float, bool> element);
    bool PopElement(std::variant<int64_t, float, bool> element);
    std::variant<int64_t, float, bool> GetElement(const int& pos) const;  // count elements from head
    std::variant<int64_t, float, bool> GetElementR(const int& pos) const; // count elements from tail
    std::variant<int64_t, float, bool> GetHeadElement() const;
    std::variant<int64_t, float, bool> GetTailElement() const;

private:
    std::variant<int64_t, float, bool>* ringArray_;
    int arrayLen_;
    int arrayCapability_;
    int arrayHead_;
    int arrayTail_;
};

} // namespace Rosen
} // namespace OHOS

#endif // UTILS_INCLUDE_RING_ARRAY_H