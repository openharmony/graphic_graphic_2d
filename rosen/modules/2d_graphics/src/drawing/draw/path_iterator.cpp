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

#include "draw/path_iterator.h"

#include "impl_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
PathIterator::PathIterator(const Path& path) noexcept : impl_(ImplFactory::CreatePathIteratorImpl(path)) {}

scalar PathIterator::ConicWeight() const
{
    return impl_->ConicWeight();
}

PathVerb PathIterator::Next(Point* points)
{
    if (points == nullptr) {
        LOGE("PathIterator::Next, points is nullptr");
        return PathVerb::DONE;
    }
    return impl_->Next(points);
}

PathVerb PathIterator::Peek()
{
    return impl_->Peek();
}

PathIterator::~PathIterator() {}

PathIter::PathIter(const Path& path, bool forceClose) noexcept
    : impl_(ImplFactory::CreatePathIterImpl(path, forceClose)) {}

scalar PathIter::ConicWeight() const
{
    return impl_->ConicWeight();
}

PathVerb PathIter::Next(Point* points)
{
    if (points == nullptr) {
        LOGE("PathIter::Next, points is nullptr");
        return PathVerb::DONE;
    }
    return impl_->Next(points);
}

PathIter::~PathIter() {}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
