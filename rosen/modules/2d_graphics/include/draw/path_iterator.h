/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef PATH_ITERATOR_H
#define PATH_ITERATOR_H

#include <memory>
#include <vector>

#include "common/rs_macros.h"
#include "draw/path.h"
#include "drawing/engine_adapter/impl_interface/path_iterator_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class PathVerb {
    UNINIT = -1,
    MOVE,
    LINE,
    QUAD,
    CONIC,
    CUBIC,
    CLOSE,
    DONE  = CLOSE + 1
};

class DRAWING_API PathIterator {
public:
    PathIterator(const Path &path) noexcept;
    virtual ~PathIterator();
    scalar ConicWeight() const;
    PathVerb Next(Point* points);
    PathVerb Peek();

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
private:
    std::shared_ptr<PathIteratorImpl> impl_;
};

class DRAWING_API PathIter {
public:
    PathIter(const Path& path, bool forceClose) noexcept;
    virtual ~PathIter();
    scalar ConicWeight() const;
    PathVerb Next(Point* points);

    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<PathIterImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
