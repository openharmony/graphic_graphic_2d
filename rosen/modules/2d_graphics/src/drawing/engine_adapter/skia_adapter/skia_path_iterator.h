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

#ifndef SKIA_PATH_ITERATOR_H
#define SKIA_PATH_ITERATOR_H

#include "include/core/SkPath.h"

#include "impl_interface/path_iterator_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API SkiaPathIterator : public PathIteratorImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    explicit SkiaPathIterator(const Path &path) noexcept;
    ~SkiaPathIterator() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    const SkPath::RawIter& ExportSkiaPathIterator() const;
    scalar ConicWeight() const override;
    PathVerb Next(Point* points) override;
    PathVerb Peek() override;
private:
    SkPath::RawIter skPathIterator_;
};

class DRAWING_API SkiaPathIter : public PathIterImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    ~SkiaPathIter() override {};
    explicit SkiaPathIter(const Path& path, bool forceClose) noexcept;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    const SkPath::Iter& ExportSkiaPathIter() const;
    scalar ConicWeight() const override;
    PathVerb Next(Point* points) override;

private:
    SkPath::Iter skPathIter_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
