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

#ifndef HGM_LRU_CACHE_H
#define HGM_LRU_CACHE_H

#include <list>
#include <unordered_map>

namespace OHOS::Rosen {
template<typename ValueType>
class HgmLRUCache {
public:
    using Value = ValueType;
    explicit HgmLRUCache(uint32_t capacity) : capacity_(capacity) {}
    virtual ~HgmLRUCache() = default;

    bool Existed(Value value) const
    {
        return searchHelper_.find(value) != searchHelper_.end();
    }

    void Put(Value value)
    {
        if (capacity_ == 0) {
            return;
        }
        if (auto iter = searchHelper_.find(value); iter != searchHelper_.end()) {
            // exist: move to head
            valueCache_.erase(iter->second);
            valueCache_.push_front(value);
            searchHelper_[value] = valueCache_.begin();
        } else if (valueCache_.size() >= capacity_) {
            // full: remove least recently used
            searchHelper_.erase(valueCache_.back());
            valueCache_.pop_back();

            valueCache_.push_front(value);
            searchHelper_[value] = valueCache_.begin();
        } else {
            // add
            valueCache_.push_front(value);
            searchHelper_[value] = valueCache_.begin();
        }
    }

    void Erase(Value value)
    {
        if (auto iter = searchHelper_.find(value); iter != searchHelper_.end()) {
            valueCache_.erase(iter->second);
            searchHelper_.erase(iter);
        }
    }

    void Clear()
    {
        valueCache_.clear();
        searchHelper_.clear();
    }

private:
    uint32_t capacity_;
    typename std::list<Value> valueCache_;
    std::unordered_map<Value, typename std::list<Value>::iterator> searchHelper_;
};
}
#endif // HGM_LRU_CACHE_H