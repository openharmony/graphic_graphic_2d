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
ObjectMgr& ObjectMgr::GetInstance()
{
    static ObjectMgr instance;
    return instance;
}

void ObjectMgr::AddObject(void* obj)
{
    std::unique_lock lock(mutex_);
    objects_.insert(obj);
}

bool ObjectMgr::HasObject(void* obj)
{
    std::shared_lock lock(mutex_);
    return objects_.find(obj) != objects_.end();
}

bool ObjectMgr::RemoveObject(void* obj)
{
    std::unique_lock lock(mutex_);
    return objects_.erase(obj) > 0;
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
