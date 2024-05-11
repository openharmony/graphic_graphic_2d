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

#ifndef REGION_IMPL_H
#define REGION_IMPL_H

#include "base_impl.h"
#include "utils/rect.h"
#include "draw/path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Region;
enum class RegionOp;

class RegionImpl : public BaseImpl {
public:
    RegionImpl() {}
    ~RegionImpl() override {}
    virtual bool Contains(int32_t x, int32_t y) const = 0;
    virtual bool SetRect(const RectI& rectI) = 0;
    virtual bool SetPath(const Path& path, const Region& clip) = 0;
    virtual bool GetBoundaryPath(Path* path) const = 0;
    virtual bool IsIntersects(const Region& other) const = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsRect() const = 0;
    virtual bool Op(const Region& region, const RegionOp op) = 0;
    virtual void Clone(const Region& other) = 0;
    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
