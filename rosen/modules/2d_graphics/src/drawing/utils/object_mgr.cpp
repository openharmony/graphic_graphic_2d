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

#include "utils/object_mgr.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ObjectMgr> ObjectMgr::GetInstance() noexcept(true)
{
    if (objectMgr == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (objectMgr == nullptr) {
            objectMgr.reset(new ObjectMgr());
        }
    }
    return objectMgr;
}

void ObjectMgr::AddObject(void* obj)
{
    std::lock_guard<std::mutex> lock(mutex_);
    vector_.push_back(obj);
}

bool ObjectMgr::HasObject(void* obj)
{
    bool has = false;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = vector_.begin(); it != vector_.end(); it++) {
        if (*it == obj) {
            has = true;
            break;
        }
    }
    return has;
}

bool ObjectMgr::RemoveObject(void* obj)
{
    bool removed = false;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = vector_.begin(); it != vector_.end(); it++) {
        if (*it == obj) {
            vector_.erase(it);
            removed = true;
            break;
        }
    }
    return removed;
}

size_t ObjectMgr::ObjectCount()
{
    return vector_.size();
}

FontCollectionMgr& FontCollectionMgr::GetInstance()
{
    static FontCollectionMgr instance;
    return instance;
}

void FontCollectionMgr::Insert(void* key, std::shared_ptr<FontCollectionType> fontCollection)
{
    std::unique_lock lock(mutex_);
    collections_.insert({key, std::move(fontCollection)});
}

std::shared_ptr<FontCollectionMgr::FontCollectionType> FontCollectionMgr::Find(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = collections_.find(key);
    if (iter != collections_.end()) {
        return iter->second;
    } else {
        return {nullptr};
    }
}

bool FontCollectionMgr::Remove(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = collections_.find(key);
    if (iter != collections_.end()) {
        collections_.erase(iter);
        return true;
    } else {
        return false;
    }
}

TypefaceMgr& TypefaceMgr::GetInstance()
{
    static TypefaceMgr instance;
    return instance;
}

void TypefaceMgr::Insert(void* key, std::shared_ptr<Typeface> typeface)
{
    std::unique_lock lock(mutex_);
    typeface_.insert({key, std::move(typeface)});
}

std::shared_ptr<Typeface> TypefaceMgr::Find(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = typeface_.find(key);
    if (iter != typeface_.end()) {
        return iter->second;
    } else {
        return {nullptr};
    }
}

bool TypefaceMgr::Remove(void* key)
{
    std::unique_lock lock(mutex_);
    auto iter = typeface_.find(key);
    if (iter != typeface_.end()) {
        typeface_.erase(iter);
        return true;
    } else {
        return false;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
