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
#include <string>

#define RANGE_MAX_REFRESHRATE 144

namespace OHOS {
namespace Rosen {
// different ltpo frame rate vote type
constexpr uint32_t RS_ANIMATION_FRAME_RATE_TYPE = 1;
constexpr uint32_t UI_ANIMATION_FRAME_RATE_TYPE = 2;
constexpr uint32_t DISPLAY_SYNC_FRAME_RATE_TYPE = 3;
constexpr uint32_t ACE_COMPONENT_FRAME_RATE_TYPE = 4;
constexpr uint32_t DISPLAY_SOLOIST_FRAME_RATE_TYPE = 5;
constexpr uint32_t DRAG_SCENE_FRAME_RATE_TYPE = 6;
constexpr uint32_t NATIVE_VSYNC_FRAME_RATE_TYPE = 7;
// extent info of ltpo frame rate vote: it indicates the frame contains the first frame of animation,
// its value should be independent from above types
constexpr uint32_t ANIMATION_STATE_FIRST_FRAME = 0x1000;

enum ComponentScene : int32_t {
    UNKNOWN_SCENE = 0,
    SWIPER_FLING = 1,
};

class FrameRateRange {
public:
    FrameRateRange() : min_(0), max_(0), preferred_(0), type_(0), isEnergyAssurance_(false),
        componentScene_(ComponentScene::UNKNOWN_SCENE) {}

    FrameRateRange(int min, int max, int preferred) : min_(min), max_(max), preferred_(preferred) {}

    FrameRateRange(int min, int max, int preferred, uint32_t type) : min_(min), max_(max),
        preferred_(preferred), type_(type) {}

    FrameRateRange(int min, int max, int preferred, uint32_t type, ComponentScene componentScene)
        : min_(min), max_(max), preferred_(preferred), type_(type), componentScene_(componentScene) {}

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
        this->type_ = 0;
        this->isEnergyAssurance_ = false;
        this->componentScene_ = ComponentScene::UNKNOWN_SCENE;
    }

    void Set(int min, int max, int preferred)
    {
        this->min_ = min;
        this->max_ = max;
        this->preferred_ = preferred;
    }

    void Set(int min, int max, int preferred, uint32_t type)
    {
        this->min_ = min;
        this->max_ = max;
        this->preferred_ = preferred;
        this->type_ = type;
    }

    bool Merge(const FrameRateRange& other)
    {
        if (this->preferred_ < other.preferred_) {
            this->Set(other.min_, other.max_, other.preferred_, other.type_);
            this->isEnergyAssurance_ = other.isEnergyAssurance_;
            this->componentScene_ = other.componentScene_;
            return true;
        }
        return false;
    }

    std::string GetExtInfo() const
    {
        auto componentName = GetComponentName();
        if (isEnergyAssurance_ && componentName != "UNKNOWN_SCENE") {
            return std::string("COMPONENT_") + componentName + "_ASSURANCE";
        }
        std::string extInfo = "";
        switch (type_ & ~ANIMATION_STATE_FIRST_FRAME) {
            case RS_ANIMATION_FRAME_RATE_TYPE:
                extInfo = "RS_ANIMATION";
                break;
            case UI_ANIMATION_FRAME_RATE_TYPE:
                extInfo = "UI_ANIMATION";
                break;
            case DISPLAY_SYNC_FRAME_RATE_TYPE:
                extInfo = "DISPLAY_SYNC";
                break;
            case ACE_COMPONENT_FRAME_RATE_TYPE:
                extInfo = "ACE_COMPONENT";
                break;
            case DISPLAY_SOLOIST_FRAME_RATE_TYPE:
                extInfo = "DISPLAY_SOLOIST";
                break;
            default:
                return "";
        }
        if ((type_ & ANIMATION_STATE_FIRST_FRAME) != 0) {
            extInfo += "_FIRST_UI_ANIMATION_FRAME";
        }
        return extInfo + (isEnergyAssurance_ ? "_ENERGY_ASSURANCE" : "");
    }

    std::string GetComponentName() const
    {
        switch (componentScene_) {
            case ComponentScene::SWIPER_FLING:
                return "SWIPER_FLING";
            default:
                return "UNKNOWN_SCENE";
        }
    }

    bool operator==(const FrameRateRange& other) const
    {
        return this->min_ == other.min_ && this->max_ == other.max_ &&
            this->preferred_ == other.preferred_ && this->type_ == other.type_;
    }

    bool operator!=(const FrameRateRange& other) const
    {
        return !operator==(other);
    }

    int min_ = 0;
    int max_ = 0;
    int preferred_ = 0;
    uint32_t type_ = 0;
    bool isEnergyAssurance_ = false;
    ComponentScene componentScene_ = ComponentScene::UNKNOWN_SCENE;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_RENDER_SERVICE_BASE_ANIMATION_RS_FRAME_RATE_RANGE_H