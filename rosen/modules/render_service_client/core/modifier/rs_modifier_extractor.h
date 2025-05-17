/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
 * @file rs_modifier_extractor.h
 *
 * @brief Defines the properties and methods for RSModifierExtractor class.
 *
 * Used to get the properties of the modifier.
 */
#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H

#include "common/rs_common_def.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

namespace OHOS {
namespace Rosen {
class RSNode;
class RSUIContext;

class RSC_EXPORT RSModifierExtractor {
public:
    /**
     * @brief Constructor for RSModifierExtractor.
     *
     * @param id The ID of the node to extract modifiers from.
     * @param rsUIContext The RSUIContext to use for extracting modifiers.Default is nullptr.
     */
    RSModifierExtractor(NodeId id, std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Destructor for RSModifierExtractor.
     */
    virtual ~RSModifierExtractor() = default;

    /**
     * @brief Gets the bounds of modifier.
     * 
     * @return A Vector4f object representing the bounds.
     */
    Vector4f GetBounds() const;

    /**
     * @brief Gets the frame of modifier.
     * 
     * @return A Vector4f object representing the frame.
     */
    Vector4f GetFrame() const;

    /**
     * @brief Gets the positionZ of modifier.
     * 
     * @return A float representing the positionZ.
     */
    float GetPositionZ() const;

    /**
     * @brief Gets the pivot of modifier.
     * 
     * @return A Vector2f object representing the pivot of x and y axes.
     */
    Vector2f GetPivot() const;

    /**
     * @brief Gets the pivotZ of modifier.
     * 
     * @return A float representing the pivot of z-axis.
     */
    float GetPivotZ() const;

    /**
     * @brief Gets the Quaternion of modifier.
     * 
     * @return A Quaternion object.
     */
    Quaternion GetQuaternion() const;

    /**
     * @brief Gets the rotation of modifier.
     * 
     * @return A float representing the rotation.
     */
    float GetRotation() const;

    /**
     * @brief Gets the rotationX of modifier.
     * 
     * @return A float representing the rotation of x-axis.
     */
    float GetRotationX() const;

    /**
     * @brief Gets the rotationY of modifier.
     * 
     * @return A float representing the rotation of y-axis.
     */
    float GetRotationY() const;

    /**
     * @brief Gets the distance of the camera.
     * 
     * @return The distance of the camera.
     */
    float GetCameraDistance() const;

    /**
     * @brief Gets the translate of modifier on the x and y axes.
     * 
     * @return A Vector2f object representing the translation distance.
     */
    Vector2f GetTranslate() const;

    /**
     * @brief Gets the translate of modifier on the z-axis.
     * 
     * @return A float representing the translation distance on the z-axis.
     */
    float GetTranslateZ() const;

    /**
     * @brief Gets the scale of modifier.
     * 
     * @return A Vector2f object representing the scale  on the x and y axes.
     */
    Vector2f GetScale() const;

    /**
     * @brief Gets the scale of modifier on the z-axis.
     * 
     * @return A float representing the scale on the z-axis.
     */
    float GetScaleZ() const;

    /**
     * @brief Gets the skew of modifier.
     * 
     * @return A Vector3f object representing the skew.
     */
    Vector3f GetSkew() const;

    /**
     * @brief Gets the perspective of modifier.
     * 
     * @return A Vector4f object representing the perspective.
     */
    Vector4f GetPersp() const;

    /**
     * @brief Gets the alpha of modifier.
     * 
     * @return The alpha of the modifier.
     */
    float GetAlpha() const;

    /**
     * @brief Gets the offscreen alpha of modifier.
     * 
     * @return true if offscreen alpha is enabled; false otherwise.
     */
    bool GetAlphaOffscreen() const;

    /**
     * @brief Gets the corner radius of modifier.
     *
     * @return A Vector4f representing the corner radius for each corner.
     */
    Vector4f GetCornerRadius() const;

    /**
     * @brief Gets the foreground color of modifier.
     *
     * @return The foreground color.
     */
    Color GetForegroundColor() const;

    /**
     * @brief Gets the background color of modifier.
     *
     * @return The backround color.
     */
    Color GetBackgroundColor() const;

    /**
     * @brief Gets the background color of surface.
     *
     * @return The background color of the surface.
     */
    Color GetSurfaceBgColor() const;

    /**
     * @brief Gets the background shader of modifier.
     *
     * @return The pointer to background shader.
     */
    std::shared_ptr<RSShader> GetBackgroundShader() const;


    /**
     * @brief Gets the background image.
     *
     * @return A shared pointer to the RSImage representing the background image.
     */
    std::shared_ptr<RSImage> GetBgImage() const;

    /**
     * @brief Gets the background image width.
     *
     * @return The width of the background image.
     */
    float GetBgImageWidth() const;

    /**
     * @brief Gets the background image height.
     *
     * @return The height of the background image.
     */
    float GetBgImageHeight() const;

    /**
     * @brief Gets the background image position on the x-axis.
     *
     * @return The x-coordinate of the background image position.
     */
    float GetBgImagePositionX() const;

    /**
     * @brief Gets the background image position on the y-axis.
     *
     * @return The y-coordinate of the background image position.
     */
    float GetBgImagePositionY() const;

    /**
     * @brief Gets the border color.
     *
     * @return A Vector4 containing the colors of the border.
     */
    Vector4<Color> GetBorderColor() const;

    /**
     * @brief Gets the border width.
     *
     * @return A Vector4f containing the widths of the border.
     */
    Vector4f GetBorderWidth() const;

    /**
     * @brief Gets the border style.
     *
     * @return A Vector4 containing the styles of the border.
     */
    Vector4<uint32_t> GetBorderStyle() const;

    /**
     * @brief Gets the width of border dash.
     *
     * @return A Vector4f containing the width of the border dash.
     */
    Vector4f GetBorderDashWidth() const;

    /**
     * @brief Gets the gap of border dash.
     *
     * @return A Vector4f containing the gap of the border dash.
     */
    Vector4f GetBorderDashGap() const;

    /**
     * @brief Gets the outline color.
     *
     * @return A Vector4 containing the colors of the Outline.
     */
    Vector4<Color> GetOutlineColor() const;

    /**
     * @brief Gets the outline width.
     *
     * @return A Vector4f containing the widths of the Outline
     */
    Vector4f GetOutlineWidth() const;

    /**
     * @brief Gets the outline style.
     *
     * @return A Vector4 containing the styles of the Outline.
     */
    Vector4<uint32_t> GetOutlineStyle() const;

    /**
     * @brief Gets the outline dash width.
     *
     * @return A Vector4f containing the width of the Outline dash.
     */
    Vector4f GetOutlineDashWidth() const;

    /**
     * @brief Gets the outline dash gap.
     *
     * @return A Vector4f containing the gap of the Outline dash.
     */
    Vector4f GetOutlineDashGap() const;

    /**
     * @brief Gets the outline radius.
     *
     * @return A Vector4f containing the radius of the Outline.
     */
    Vector4f GetOutlineRadius() const;

    /**
     * @brief Gets the radius of foreground effect.
     *
     * @return The radius of the foreground effect.
     */
    float GetForegroundEffectRadius() const;

    /**
     * @brief Gets the background filter.
     *
     * @return The pointer to background filter.
     */
    std::shared_ptr<RSFilter> GetBackgroundFilter() const;

    /**
     * @brief Gets the filter.
     *
     * @return The pointer to filter.
     */
    std::shared_ptr<RSFilter> GetFilter() const;

    /**
     * @brief Gets the color of shadow.
     *
     * @return The color of shadow.
     */
    Color GetShadowColor() const;

    /**
     * @brief Gets the X-axis offset of shadow.
     *
     * @return The X-axis offset of the shadow.
     */
    float GetShadowOffsetX() const;

    /**
     * @brief Gets the Y-axis offset of shadow.
     *
     * @return The Y-axis offset of the shadow.
     */
    float GetShadowOffsetY() const;

    /**
     * @brief Gets the alpha of shadow.
     *
     * @return The alpha value of the shadow.
     */
    float GetShadowAlpha() const;

    /**
     * @brief Gets the elevation of shadow.
     *
     * @return The elevation of the shadow.
     */
    float GetShadowElevation() const;

    /**
     * @brief Gets the radius of shadow.
     *
     * @return The radius of the shadow.
     */
    float GetShadowRadius() const;

    /**
     * @brief Gets the shadow path.
     *
     * @return A shared pointer to the RSPath representing the shadow path.
     */
    std::shared_ptr<RSPath> GetShadowPath() const;

    /**
     * @brief Gets the shadow mask.
     *
     * @return true if shadow mask is enabled; false otherwise.
     */
    bool GetShadowMask() const;

    /**
     * @brief Gets the shadow fill status.
     *
     * @return true if shadow is filled; false otherwise.
     */
    bool GetShadowIsFilled() const;

    /**
     * @brief Gets the strategy for shadow color.
     *
     * @return The strategy for shadow color.
     */
    int GetShadowColorStrategy() const;

    /**
     * @brief Gets the gravity of the frame.
     *
     * @return The gravity of the frame.
     */
    Gravity GetFrameGravity() const;

    /**
     * @brief Gets the bounds of the clip.
     *
     * @return A shared pointer to the RSPath representing the clip bounds.
     */
    std::shared_ptr<RSPath> GetClipBounds() const;

    /**
     * @brief Gets whether enabled to clip to bounds.
     *
     * @return true if clipping to bounds is enabled; false otherwise.
     */
    bool GetClipToBounds() const;

    /**
     * @brief Gets whether enabled to clip to frame.
     *
     * @return true if clipping to frame is enabled; false otherwise.
     */
    bool GetClipToFrame() const;

    /**
     * @brief Gets whether the modifier is visible.
     *
     * @return true if the modifier is visible; false otherwise.
     */
    bool GetVisible() const;

    std::shared_ptr<RSMask> GetMask() const;

    /**
     * @brief Gets the degree of sphericity.
     *
     * @return The spherize degree.
     */
    float GetSpherizeDegree() const;

    /**
     * @brief Gets the degree of light up effect.
     *
     * @return The light up effect degree.
     */
    float GetLightUpEffectDegree() const;

    /**
     * @brief Gets the degree of dynamic dimension.
     *
     * @return The dynamic dimension degree.
     */
    float GetDynamicDimDegree() const;

    float GetAttractionFractionValue() const;

    Vector2f GetAttractionDstPointValue() const;

    /**
     * @brief Gets the radius used for background blur.
     *
     * @return The radius used for background blur.
     */
    float GetBackgroundBlurRadius() const;
    
    /*
     * @brief Gets the saturation of background blur.
     *
     * @return The saturation of background blur.
     */
    float GetBackgroundBlurSaturation() const;
    
    /**
     * @brief Gets the brightness of background blur.
     *
     * @return The brightness of background blur.
     */
    float GetBackgroundBlurBrightness() const;

    /**
     * @brief Gets the color of background blur mask.
     *
     * @return The color of background blur mask.
     */
    Color GetBackgroundBlurMaskColor() const;

    /**
     * @brief Gets the color mode of background blur.
     *
     * @return The color mode of background blur.
     */
    int GetBackgroundBlurColorMode() const;

    /**
     * @brief Gets the radius of background blur on the x-axis.
     *
     * @return The radius of background blur on the x-axis.
     */
    float GetBackgroundBlurRadiusX() const;

    /**
     * @brief Gets the radius of background blur on the y-axis.
     *
     * @return The radius of background blur on the y-axis.
     */
    float GetBackgroundBlurRadiusY() const;

    /**
     * @brief Gets whether system adaptation is disabled for background blur.
     *
     * @return true if system adaptation is disabled; false otherwise.
     */
    bool GetBgBlurDisableSystemAdaptation() const;

    /**
     * @brief Gets whether the modifier is always in snapshot mode.
     *
     * @return true if always in snapshot mode; false otherwise.
     */
    bool GetAlwaysSnapshot() const;

    /**
     * @brief Gets the foreground blur radius.
     *
     * @return The foreground blur radius.
     */
    float GetForegroundBlurRadius() const;

    /**
     * @brief Gets the saturation of foreground blur.
     *
     * @return The saturation of foreground blur.
     */
    float GetForegroundBlurSaturation() const;

    /**
     * @brief Gets the brightness of foreground blur.
     *
     * @return The brightness of foreground blur.
     */
    float GetForegroundBlurBrightness() const;

    /**
     * @brief Gets the color of foreground blur mask.
     *
     * @return The color of foreground blur mask.
     */
    Color GetForegroundBlurMaskColor() const;

    /**
     * @brief Gets the color mode of foreground blur.
     *
     * @return The color mode of foreground blur.
     */
    int GetForegroundBlurColorMode() const;

    /**
     * @brief Gets the radius of foreground blur on the x-axis.
     *
     * @return The radius of foreground blur on the x-axis.
     */
    float GetForegroundBlurRadiusX() const;

    /**
     * @brief Gets the radius of foreground blur on the y-axis.
     *
     * @return The radius of foreground blur on the y-axis.
     */
    float GetForegroundBlurRadiusY() const;

    /**
     * @brief Gets whether system adaptation is disabled for foreground blur.
     *
     * @return true if system adaptation is disabled; false otherwise.
     */
    bool GetFgBlurDisableSystemAdaptation() const;
    
    /**
     * @brief Gets light intensity.
     *
     * @return The light intensity.
     */
    float GetLightIntensity() const;

    /**
     * @brief Gets light color.
     *
     * @return The light color.
     */
    Color GetLightColor() const;

    /**
     * @brief Gets light position.
     *
     * @return The light position.
     */
    Vector4f GetLightPosition() const;

    /**
     * @brief Gets the width of the illuminated border.
     *
     * @return The width of the illuminated border.
     */
    float GetIlluminatedBorderWidth() const;

    /**
     * @brief Gets the type of the illuminated.
     *
     * @return The type of the illuminated.
     */
    int GetIlluminatedType() const;

    /**
     * @brief Gets the bloom value.
     *
     * @return The bloom value.
     */
    float GetBloom() const;

    /**
     * @brief Gets the mode of color blend.
     *
     * @return The mode of color blend.
     */
    int GetColorBlendMode() const;

    /**
     * @brief Gets the application type of color blending.
     *
     * @return The application type of color blending.
     */
    int GetColorBlendApplyType() const;

    /**
     * @brief Generates debugging information.
     *
     * @return The debug information.
     */
    std::string Dump() const;
private:
    NodeId id_;
    std::weak_ptr<RSUIContext> rsUIContext_;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_MODIFIER_EXTRACTOR_H
