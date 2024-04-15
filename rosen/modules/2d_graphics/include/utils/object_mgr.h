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

#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>

#include "text/typeface.h"

#ifndef USE_GRAPHIC_TEXT_GINE
namespace rosen {
class FontCollection;
}
#else
namespace OHOS {
namespace Rosen {
class FontCollection;
}
}
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ObjectMgr {
public:
    static std::shared_ptr<ObjectMgr> GetInstance() noexcept(true);
    void AddObject(void* obj);
    bool HasObject(void* obj);
    bool RemoveObject(void* obj);
    size_t ObjectCount();

private:
    static inline std::shared_ptr<ObjectMgr> objectMgr = nullptr;
    std::vector<void*> vector_;
    std::mutex mutex_;
};

class FontCollectionMgr {
public:
    FontCollectionMgr(const FontCollectionMgr&) = delete;
    FontCollectionMgr& operator=(const FontCollectionMgr&) = delete;

    static FontCollectionMgr& GetInstance();

#ifndef USE_GRAPHIC_TEXT_GINE
    using FontCollectionType = rosen::FontCollection;
#else
    using FontCollectionType = ::OHOS::Rosen::FontCollection;
#endif

    void Insert(void* key, std::shared_ptr<FontCollectionType> fontCollection);
    std::shared_ptr<FontCollectionType> Find(void* key);
    bool Remove(void* key);

private:
    FontCollectionMgr() {}

    std::unordered_map<void*, std::shared_ptr<FontCollectionType>> collections_;
    std::mutex mutex_;
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
