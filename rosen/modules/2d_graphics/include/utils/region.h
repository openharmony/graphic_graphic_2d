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

#ifndef REGION_H
#define REGION_H

#include "impl_interface/region_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class RegionOp {
    DIFFERENCE,
    INTERSECT,
    UNION,
    XOR,
    REVERSE_DIFFERENCE,
    REPLACE,
};
class DRAWING_API Region {
public:
    Region();
    Region(const Region& other);
    Region& operator=(const Region& other);
    bool operator==(const Region& other) const;
    virtual ~Region() = default;

    void Clone(const Region& other)
    {
        impl_->Clone(other);
    }

    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    virtual bool Contains(int32_t x, int32_t y) const;

    /**
     * @brief Sets to empty bounds at (0, 0) with zero width and height.
     */
    virtual void SetEmpty();

    /**
     * @brief Constructs a rectangular Region matching the bounds of rect.
     * @param rectI Bounds of constructed Region.
     * @return If rectI is empty, constructs empty and returns false.
     */
    virtual bool SetRect(const RectI& rectI);

    /**
     * @brief Constructs a copy of an existing region.
     * @param region Region to copy by value.
     * @return Return true if constructed region is not empty.
     */
    virtual bool SetRegion(const Region& region);

    /**
     * @brief Constructs Region to match outline of path within clip.
     * @param path Providing outline.
     * @param clip Containing path.
     * @return Return true if constructed Region is not empty.
     */
    virtual bool SetPath(const Path& path, const Region& clip);

    /**
     * @brief Appends outline of Region to path.
     * @param path Path to append to.
     * @return Return true if path changed.
     */
    bool GetBoundaryPath(Path* path) const;

    /**
     * @brief Get the external rect with the smallest region.
     * @return Return the external rect with the smallest region.
     */
    RectI GetBounds() const;

    /**
     * @brief Determine if the area contains multiple rectangles.
     * @return If true indicates that the Region contains multiple rectangles.
     */
    bool IsComplex() const;

    /**
     * @brief Determines whether it intersects other.
     * @param other Other Region object.
     * @return If true indicates that other and Region have area in common.
     */
    bool IsIntersects(const Region& other) const;

    /**
     * @brief Determines whether Region is empty.
     * @return If true indicates that bounds has no width or height.
     */
    bool IsEmpty() const;

    /**
     * @brief Determines whether Region is one Rect with positive dimensions.
     * @return If true indicates that Region contains one Rect.
     */
    bool IsRect() const;

    /**
     * @brief Determines whether other region is in the region.
     * @param other Other region object.
     * @return If true indicates that other and region have area in common.
     */
    virtual bool IsRegionContained(const Region& other) const;

    /**
     * @brief Replaces Region with the result of Region op region.
     * @param region Operand.
     * @param op     Operation type.
     * @return Returns true if replaced Region is not empty.
     */
    virtual bool Op(const Region& region, RegionOp op);

     /**
     * @brief Determines whether rect and region does not intersect.
     * @param rectI RectI to intersect.
     * @return Returns true if rect and region is not intersect.
     */
    virtual bool QuickReject(const RectI& rectI) const;

    /**
     * @brief Determines whether two regions does not intersect.
     * @param region Region to intersect.
     * @return Returns true if two regions are not intersect.
     */
    virtual bool QuickReject(const Region& region) const;

    /**
     * @brief Translate the region by (x, y).
     * @param x horizontal translation.
     * @param y vertical translation.
     */
    void Translate(int32_t x, int32_t y);

    inline void Dump(std::string& out) const;

    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);

    /**
     * @brief Get the adaptation layer instance, called in the adaptation layer.
     * @return Adaptation Layer instance.
     */
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }

private:
    std::shared_ptr<RegionImpl> impl_;
};

inline void Region::Dump(std::string& out) const
{
    out += '[';
    if (impl_ != nullptr) {
        out += "empty:" + std::string(impl_->IsEmpty() ? "true" : "false");
        out += " isRect:" + std::string(impl_->IsRect() ? "true" : "false");
        out += " BoundaryPath";
        Path path;
        impl_->GetBoundaryPath(&path);
        path.Dump(out);
    }
    out += ']';
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
