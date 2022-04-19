/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H

#include <vector>

#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"
#include "render/rs_border.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"
#include "render/rs_mask.h"

namespace OHOS {
namespace Rosen {
class RSProperties final {
public:
    RSProperties(bool inRenderNode);
    virtual ~RSProperties();

    // geometry properties
    void SetBounds(Vector4f bounds);
    void SetBoundsSize(Vector2f size);
    void SetBoundsWidth(float width);
    void SetBoundsHeight(float height);
    void SetBoundsPosition(Vector2f position);
    void SetBoundsPositionX(float positionX);
    void SetBoundsPositionY(float positionY);
    Vector4f GetBounds() const;
    Vector2f GetBoundsSize() const;
    float GetBoundsWidth() const;
    float GetBoundsHeight() const;
    Vector2f GetBoundsPosition() const;
    float GetBoundsPositionX() const;
    float GetBoundsPositionY() const;

    void SetFrame(Vector4f frame);
    void SetFrameSize(Vector2f size);
    void SetFrameWidth(float width);
    void SetFrameHeight(float height);
    void SetFramePosition(Vector2f position);
    void SetFramePositionX(float positionX);
    void SetFramePositionY(float positionY);
    Vector4f GetFrame() const;
    Vector2f GetFrameSize() const;
    float GetFrameWidth() const;
    float GetFrameHeight() const;
    Vector2f GetFramePosition() const;
    float GetFramePositionX() const;
    float GetFramePositionY() const;
    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;

    void SetPositionZ(float positionZ);
    float GetPositionZ() const;

    void SetPivot(Vector2f pivot);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);
    Vector2f GetPivot() const;
    float GetPivotX() const;
    float GetPivotY() const;

    void SetCornerRadius(Vector4f cornerRadius);
    Vector4f GetCornerRadius() const;

    void SetQuaternion(Quaternion quaternion);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);
    Quaternion GetQuaternion() const;
    float GetRotation() const;
    float GetRotationX() const;
    float GetRotationY() const;

    void SetTranslate(Vector2f translate);
    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);
    Vector2f GetTranslate() const;
    float GetTranslateX() const;
    float GetTranslateY() const;
    float GetTranslateZ() const;

    void SetScale(Vector2f scale);
    void SetScaleX(float sx);
    void SetScaleY(float sy);
    Vector2f GetScale() const;
    float GetScaleX() const;
    float GetScaleY() const;

    void SetAlpha(float alpha);
    float GetAlpha() const;

    void SetSublayerTransform(Matrix3f sublayerTransform);
    Matrix3f GetSublayerTransform() const;

    // foreground properties
    void SetForegroundColor(Color color);
    Color GetForegroundColor() const;

    // background properties
    void SetBackgroundColor(Color color);
    Color GetBackgroundColor() const;
    void SetBackgroundShader(std::shared_ptr<RSShader> shader);
    std::shared_ptr<RSShader> GetBackgroundShader() const;
    void SetBgImage(std::shared_ptr<RSImage> image);
    std::shared_ptr<RSImage> GetBgImage() const;
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;

    // border properties
    void SetBorderColor(Vector4<Color> color);
    void SetBorderWidth(Vector4f width);
    void SetBorderStyle(Vector4<BorderStyle> style);
    Vector4<Color> GetBorderColor() const;
    Vector4f GetBorderWidth() const;
    Vector4<BorderStyle> GetBorderStyle() const;
    std::shared_ptr<RSBorder> GetBorder() const;

    // filter properties
    void SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter);
    void SetFilter(std::shared_ptr<RSFilter> filter);
    std::shared_ptr<RSFilter> GetBackgroundFilter() const;
    std::shared_ptr<RSFilter> GetFilter() const;

    // shadow properties
    void SetShadowColor(Color color);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float radius);
    void SetShadowRadius(float radius);
    void SetShadowPath(std::shared_ptr<RSPath> shadowpath);
    Color GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;

    void SetFrameGravity(Gravity gravity);
    Gravity GetFrameGravity() const;

    void SetClipBounds(std::shared_ptr<RSPath> path);
    std::shared_ptr<RSPath> GetClipBounds() const;
    void SetClipToBounds(bool clipToBounds);
    bool GetClipToBounds() const;
    void SetClipToFrame(bool clipToFrame);
    bool GetClipToFrame() const;

    void SetVisible(bool visible);
    bool GetVisible() const;
    bool SetId(NodeId id);
    std::string Dump() const;

    void SetMask(std::shared_ptr<RSMask> mask);
    std::shared_ptr<RSMask> GetMask() const;

    const std::shared_ptr<RSObjGeometry>& GetBoundsGeometry() const;
    const std::shared_ptr<RSObjGeometry>& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag);

private:
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;
    void ResetBounds();

    RectF GetBoundsRect() const;
    RectF GetFrameRect() const;
    RectF GetBgImageRect() const;
    RRect GetRRect() const;
    RRect GetInnerRRect() const;
    RectI GetDirtyRect() const;

    bool NeedFilter() const;
    bool NeedClip() const;

    bool visible_ = true;
    bool clipToBounds_ = false;
    bool clipToFrame_ = false;
    bool isDirty_ = false;
    bool geoDirty_ = false;

    bool hasBounds_ = false;

    Gravity frameGravity_ = Gravity::DEFAULT;

    float alpha_ = 1.f;

    std::unique_ptr<Matrix3f> sublayerTransform_ = nullptr;
    std::unique_ptr<Decoration> decoration_ = nullptr;
    std::unique_ptr<Vector4f> cornerRadius_ = nullptr;
    std::unique_ptr<RSShadow> shadow_ = nullptr;

    std::shared_ptr<RSObjGeometry> boundsGeo_;
    std::shared_ptr<RSObjGeometry> frameGeo_;
    std::shared_ptr<RSBorder> border_ = nullptr;
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;
    std::shared_ptr<RSMask> mask_ = nullptr;

    friend class RSPropertiesPainter;
    friend class RSRenderNode;
    friend class RSCanvasRenderNode;
    friend class RSCanvasNode;
    friend class RSPropertiesUtils;
    friend class RSRenderTransitionEffect;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
