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

#ifndef ROSEN_MODULE_FRAME_ANALYZER_EXPORT_RING_QUEUE_H
#define ROSEN_MODULE_FRAME_ANALYZER_EXPORT_RING_QUEUE_H

#include <array>

namespace OHOS {
namespace Rosen {
template<class T, int cap>
class RingQueue {
public:
    static constexpr int realcap = cap + 1;
    using Array = std::array<T, realcap>;

    int GetSize() const
    {
        return (back_ - front_ + realcap) % realcap;
    }

    T &Push(const T& t)
    {
        if (front_ == (back_ + 1) % realcap) {
            front_ = (front_ + 1) % realcap;
        }
        queue_[back_++] = t;
        back_ %= realcap;
        return queue_[(back_ + cap) % realcap];
    }

    void Clear()
    {
        front_ = 0;
        back_ = 0;
    }

    class Iter {
    public:
        Iter(Array *ptr, int offset)
            : ptr_(ptr), offset_(offset)
        {
        }

        Iter &operator++()
        {
            offset_ = (offset_ + 1) % realcap;
            return *this;
        }

        Iter operator++(int)
        {
            auto ret = *this;
            offset_ = (offset_ + 1) % realcap;
            return ret;
        }

        T &operator*()
        {
            return (*ptr_)[offset_];
        }

        T *operator->()
        {
            return &(*ptr_)[offset_];
        }

        bool operator==(const Iter &rhs)
        {
            return offset_ == rhs.offset_;
        }

        bool operator!=(const Iter &rhs)
        {
            return offset_ != rhs.offset_;
        }

    private:
        Array *ptr_;
        int offset_;
    };

    class RevertIter {
    public:
        RevertIter(Array *ptr, int offset)
            : ptr_(ptr), offset_(offset)
        {
        }

        RevertIter &operator++()
        {
            offset_ = (offset_ + cap) % realcap;
            return *this;
        }

        RevertIter operator++(int)
        {
            auto ret = *this;
            offset_ = (offset_ + cap) % realcap;
            return ret;
        }

        T &operator*()
        {
            return (*ptr_)[offset_];
        }

        T *operator->()
        {
            return &(*ptr_)[offset_];
        }

        bool operator==(const RevertIter &rhs)
        {
            return offset_ == rhs.offset_;
        }

        bool operator!=(const RevertIter &rhs)
        {
            return offset_ != rhs.offset_;
        }

    private:
        Array *ptr_;
        int offset_;
    };

    Iter begin()
    {
        return Iter(&queue_, front_);
    }

    Iter end()
    {
        return Iter(&queue_, back_);
    }

    RevertIter rbegin()
    {
        return RevertIter(&queue_, (back_ + cap) % realcap);
    }

    RevertIter rend()
    {
        return RevertIter(&queue_, (front_ + cap) % realcap);
    }

private:
    Array queue_;
    int front_ = 0;
    int back_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULE_FRAME_ANALYZER_EXPORT_RING_QUEUE_H
