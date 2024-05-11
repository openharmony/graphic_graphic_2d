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

#ifndef SKIA_REGION_H
#define SKIA_REGION_H

#include "include/core/SkRegion.h"
#include "impl_interface/region_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRegion : public RegionImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaRegion() noexcept;
    ~SkiaRegion() override {};

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool Contains(int32_t x, int32_t y) const override;
    bool SetRect(const RectI& rectI) override;
    bool SetPath(const Path& path, const Region& clip) override;
    bool GetBoundaryPath(Path* path) const override;
    bool IsIntersects(const Region& other) const override;
    bool IsEmpty() const override;
    bool IsRect() const override;
    bool Op(const Region& region, RegionOp op) override;
    void Clone(const Region& other) override;
    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

    /*
     * @brief   Export Skia member variables for use by the adaptation layer.
     * @return  A shared pointer to SkRegion that its mutable.
     */
    std::shared_ptr<SkRegion> GetSkRegion() const;

private:
    std::shared_ptr<SkRegion> skRegion_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
