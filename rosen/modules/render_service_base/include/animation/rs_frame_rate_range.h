/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H
#define ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H

#define RANGE_MAX_REFRESHRATE 144

namespace OHOS {
namespace Rosen {
class FrameRateRange {
public:
    FrameRateRange() : min_(0), max_(0), preferred_(0) {}

    FrameRateRange(int min, int max, int preferred) : min_(min), max_(max), preferred_(preferred) {}

    bool IsZero() const
    {
        return this->preferred_ == 0;
    }

    bool IsValid() const
    {
        return !this->IsZero() && this->min_ <= this->preferred_ && this->preferred_ <= this->max_ &&
            this->min_ >= 0 && this->max_ <= RANGE_MAX_REFRESHRATE;
    }

    bool IsDynamic() const
    {
        return IsValid() && this->min_ != this->max_;
    }

    void Reset()
    {
        this->min_ = 0;
        this->max_ = 0;
        this->preferred_ = 0;
    }

    void Set(int min, int max, int preferred)
    {
        this->min_ = min;
        this->max_ = max;
        this->preferred_ = preferred;
    }

    bool Merge(const FrameRateRange& other)
    {
        if (this->preferred_ < other.preferred_) {
            this->Set(other.min_, other.max_, other.preferred_);
            return true;
        }
        return false;
    }

    bool operator==(const FrameRateRange& other) const
    {
        return this->min_ == other.min_ && this->max_ == other.max_ &&
            this->preferred_ == other.preferred_;
    }

    bool operator!=(const FrameRateRange& other) const
    {
        return this->min_ != other.min_ || this->max_ != other.max_ ||
            this->preferred_ != other.preferred_;
    }

    int min_ = 0;
    int max_ = 0;
    int preferred_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H