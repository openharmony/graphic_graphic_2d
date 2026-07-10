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

#ifndef OBJECT_MGR_H
#define OBJECT_MGR_H

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ObjectMgr {
public:
    ObjectMgr(const ObjectMgr&) = delete;
    ObjectMgr& operator=(const ObjectMgr&) = delete;

    static ObjectMgr& GetInstance();
    void AddObject(void* obj);
    bool HasObject(void* obj);
    bool RemoveObject(void* obj);

    template<typename Func>
    bool WithObject(void* obj, Func&& func)
    {
        std::shared_lock lock(mutex_);
        auto iter = objects_.find(obj);
        if (iter != objects_.end()) {
            func(*iter);
            return true;
        }
        return false;
    }

private:
    ObjectMgr() = default;

    std::unordered_set<void*> objects_;
    std::shared_mutex mutex_;
};

class TypefaceMgr {
public:
    TypefaceMgr(const TypefaceMgr&) = delete;
    TypefaceMgr& operator=(const TypefaceMgr&) = delete;

    static TypefaceMgr& GetInstance();

    void Insert(void* key, std::shared_ptr<Typeface> typeface);
    std::shared_ptr<Typeface> Find(void* key);
    bool Remove(void* key);

private:
    TypefaceMgr() {}

    std::unordered_map<void*, std::shared_ptr<Typeface>> typeface_;
    std::mutex mutex_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
