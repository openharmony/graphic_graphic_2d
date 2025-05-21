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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_showing_properties_freezer.h
 *
 * @brief Defines the properties and methods for RSShowingPropertiesFreezer class.
 *
 * Used to cancel the animation and get the value of the property.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H

#include "common/rs_common_def.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
/**
 * @class RSNode
 *
 * @brief The class for RSNode(render service node).
 */
class RSNode;

/**
 * @class RSUIContext
 *
 * @brief The class for RSUIContext.
 */
class RSUIContext;

/**
 * @class RSShowingPropertiesFreezer
 *
 * @brief The class for freezing properties of a node.
 *
 * Used to cancel the animation and get the value of the property.
 */
class RSC_EXPORT RSShowingPropertiesFreezer {
public:
    /**
     * @brief Constructor for RSShowingPropertiesFreezer.
     *
     * @param id The ID of the node.
     * @param rsUIContext The RSUIContext to use for freezing properties. Default is nullptr.
     */
    RSShowingPropertiesFreezer(NodeId id, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Destructor for RSShowingPropertiesFreezer.
     */
    virtual ~RSShowingPropertiesFreezer() = default;

    /**
     * @brief Gets the bounds of modifier.
     * 
     * @return A Vector4f object representing the bounds.
     */
    std::optional<Vector4f> GetBounds() const;

    /**
     * @brief Gets the frame of modifier.
     * 
     * @return A Vector4f object representing the frame.
     */
    std::optional<Vector4f> GetFrame() const;

    /**
     * @brief Gets the positionZ of modifier.
     * 
     * @return A float representing the positionZ.
     */
    std::optional<float> GetPositionZ() const;

    /**
     * @brief Gets the pivot of modifier.
     * 
     * @return A Vector2f object representing the pivot of x and y axes.
     */
    std::optional<Vector2f> GetPivot() const;

    /**
     * @brief Gets the pivotZ of modifier.
     * 
     * @return A float representing the pivot of z-axis.
     */
    std::optional<float> GetPivotZ() const;

    /**
     * @brief Gets the Quaternion of modifier.
     * 
     * @return A Quaternion object.
     */
    std::optional<Quaternion> GetQuaternion() const;

    /**
     * @brief Gets the rotation of modifier.
     * 
     * @return A float representing the rotation.
     */
    std::optional<float> GetRotation() const;

    /**
     * @brief Gets the rotationX of modifier.
     * 
     * @return A float representing the rotation of x-axis.
     */
    std::optional<float> GetRotationX() const;

    /**
     * @brief Gets the rotationY of modifier.
     * 
     * @return A float representing the rotation of y-axis.
     */
    std::optional<float> GetRotationY() const;

    /**
     * @brief Gets the distance of the camera.
     * 
     * @return The distance of the camera.
     */
    std::optional<float> GetCameraDistance() const;

    /**
     * @brief Gets the translate of modifier on the x and y axes.
     * 
     * @return A Vector2f object representing the translation distance.
     */
    std::optional<Vector2f> GetTranslate() const;

    /**
     * @brief Gets the translate of modifier on the z-axis.
     * 
     * @return A float representing the translation distance on the z-axis.
     */
    std::optional<float> GetTranslateZ() const;

    /**
     * @brief Gets the scale of modifier.
     * 
     * @return A Vector2f object representing the scale on the x and y axes.
     */
    std::optional<Vector2f> GetScale() const;

    /**
     * @brief Gets the scale of modifier on the z-axis.
     * 
     * @return A float representing the scale on the z-axis.
     */
    std::optional<float> GetScaleZ() const;

    /**
     * @brief Gets the skew of modifier.
     * 
     * @return A Vector3f object representing the skew.
     */
    std::optional<Vector3f> GetSkew() const;

    /**
     * @brief Gets the perspective of modifier.
     * 
     * @return A Vector4f object representing the perspective.
     */
    std::optional<Vector4f> GetPersp() const;

    /**
     * @brief Gets the alpha of modifier.
     * 
     * @return The alpha of the modifier.
     */
    std::optional<float> GetAlpha() const;

    /**
     * @brief Gets the corner radius of modifier.
     *
     * @return A Vector4f representing the corner radius for each corner.
     */
    std::optional<Vector4f> GetCornerRadius() const;

    /**
     * @brief Gets the foreground color of modifier.
     *
     * @return The foreground color.
     */
    std::optional<Color> GetForegroundColor() const;

    /**
     * @brief Gets the background color of modifier.
     *
     * @return The background color.
     */
    std::optional<Color> GetBackgroundColor() const;

    /**
     * @brief Gets the background color of surface.
     *
     * @return The background color of the surface.
     */
    std::optional<Color> GetSurfaceBgColor() const;

    /**
     * @brief Gets the background image width.
     *
     * @return The width of the background image.
     */
    std::optional<float> GetBgImageWidth() const;

    /**
     * @brief Gets the background image height.
     *
     * @return The height of the background image.
     */
    std::optional<float> GetBgImageHeight() const;

    /**
     * @brief Gets the background image position on the x-axis.
     *
     * @return The x-coordinate of the background image position.
     */
    std::optional<float> GetBgImagePositionX() const;

    /**
     * @brief Gets the background image position on the y-axis.
     *
     * @return The y-coordinate of the background image position.
     */
    std::optional<float> GetBgImagePositionY() const;

    /**
     * @brief Gets the border color.
     *
     * @return A Vector4 containing the colors of the border.
     */
    std::optional<Vector4<Color>> GetBorderColor() const;

    /**
     * @brief Gets the border width.
     *
     * @return A Vector4f containing the widths of the border.
     */
    std::optional<Vector4f> GetBorderWidth() const;

    /**
     * @brief Gets the background filter.
     *
     * @return The pointer to background filter.
     */
    std::optional<std::shared_ptr<RSFilter>> GetBackgroundFilter() const;

    /**
     * @brief Gets the filter.
     *
     * @return The pointer to filter.
     */
    std::optional<std::shared_ptr<RSFilter>> GetFilter() const;

    /**
     * @brief Gets the color of shadow.
     *
     * @return The color of shadow.
     */
    std::optional<Color> GetShadowColor() const;

    /**
     * @brief Gets the X-axis offset of shadow.
     *
     * @return The X-axis offset of the shadow.
     */
    std::optional<float> GetShadowOffsetX() const;

    /**
     * @brief Gets the Y-axis offset of shadow.
     *
     * @return The Y-axis offset of the shadow.
     */
    std::optional<float> GetShadowOffsetY() const;

    /**
     * @brief Gets the alpha of shadow.
     *
     * @return The alpha value of the shadow.
     */
    std::optional<float> GetShadowAlpha() const;

    /**
     * @brief Gets the elevation of shadow.
     *
     * @return The elevation of the shadow.
     */
    std::optional<float> GetShadowElevation() const;

    /**
     * @brief Gets the radius of shadow.
     *
     * @return The radius of the shadow.
     */
    std::optional<float> GetShadowRadius() const;

    /**
     * @brief Gets the degree of sphericity.
     *
     * @return The spherize degree.
     */
    std::optional<float> GetSpherizeDegree() const;

    /**
     * @brief Gets the brightness ratio of HDR UI component.
     *
     * @return The HDR UI component brightness ratio.
     */
    std::optional<float> GetHDRUIBrightness() const;

    /**
     * @brief Gets the degree of light up effect.
     *
     * @return The light up effect degree.
     */
    std::optional<float> GetLightUpEffectDegree() const;

    /**
     * @brief Gets the degree of dynamic dimension.
     *
     * @return The dynamic dimension degree.
     */
    std::optional<float> GetDynamicDimDegree() const;

    /**
     * @brief Gets the transformation progress of the meteor animation effect.
     *
     * @return The transformation progress of the meteor animation effect.
     */
    std::optional<float> GetAttractionFractionValue() const;

    /**
     * @brief Gets the destination point of the meteor animation effect.
     *
     * @return The destination point of the meteor animation effect.
     */
    std::optional<Vector2f> GetAttractionDstPointValue() const;

private:
    NodeId id_;
    std::weak_ptr<RSUIContext> rsUIContext_;
    template<typename T, RSModifierType Type>
    std::optional<T> GetPropertyImpl() const;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_SHOWING_PROPERTIES_FREEZER_H
