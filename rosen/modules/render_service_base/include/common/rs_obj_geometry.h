/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H

#include <cmath>

#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
namespace OHOS {
namespace Rosen {
class RSTransform {
public:
    RSTransform() = default;
    RSTransform(const RSTransform& other) = default;
    ~RSTransform() = default;
    float pivotX_ { 0.5f };
    float pivotY_ { 0.5f };
    float pivotZ_ { 0.0f };
    float scaleX_ { 1.0f };
    float scaleY_ { 1.0f };
    float scaleZ_ { 1.0f };
    float skewX_ {0.0f};
    float skewY_ {0.0f};
    float skewZ_ {0.0f};
    float perspX_ {0.0f};
    float perspY_ {0.0f};
    float perspZ_ {0.0f};
    float perspW_ {1.0f};
    float rotation_ { 0.0f };
    float rotationX_ { 0.0f };
    float rotationY_ { 0.0f };
    float translateX_ { 0.0f };
    float translateY_ { 0.0f };
    float translateZ_ { 0.0f };
    float cameraDistance_ { 0.0f };
    Quaternion quaternion_ { 0.0f, 0.0f, 0.0f, 1.0f };
};

class RSObjGeometry {
public:
    RSObjGeometry() : x_(-INFINITY), y_(-INFINITY), z_(0.0f), width_(-INFINITY), height_(-INFINITY),
        isZApplicableCamera3D_(true) {}

    virtual ~RSObjGeometry() = default;

    void SetX(float x)
    {
        x_ = x;
    }
    void SetY(float y)
    {
        y_ = y;
    }
    void SetZ(float z)
    {
        z_ = z;
    }
    void SetWidth(float w)
    {
        width_ = w;
    }
    void SetHeight(float h)
    {
        height_ = h;
    }
    void SetPosition(float x, float y)
    {
        SetX(x);
        SetY(y);
    }
    void SetSize(float w, float h)
    {
        SetWidth(w);
        SetHeight(h);
    }
    void SetRect(float x, float y, float w, float h)
    {
        SetPosition(x, y);
        SetSize(w, h);
    }
    void SetZApplicableCamera3D(bool isApplicable)
    {
        isZApplicableCamera3D_ = isApplicable;
    }

    float GetX() const
    {
        return x_;
    }
    float GetWidth() const
    {
        return width_;
    }
    float GetY() const
    {
        return y_;
    }
    float GetHeight() const
    {
        return height_;
    }
    float GetZ() const
    {
        return z_;
    }
    bool IsEmpty() const
    {
        return width_ <= 0 && height_ <= 0;
    }
    bool GetZApplicableCamera3D() const
    {
        return isZApplicableCamera3D_;
    }
    void Round()
    {
        x_ = std::floor(x_);
        y_ = std::floor(y_);
    }
    RSObjGeometry& operator=(const RSObjGeometry& geo)
    {
        if (&geo != this) {
            SetRect(geo.x_, geo.y_, geo.width_, geo.height_);
            SetZ(geo.z_);
        }
        return *this;
    }

protected:
    float x_;
    float y_;
    float z_;
    float width_;
    float height_;
    bool isZApplicableCamera3D_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_OBJ_GEOMETRY_H
