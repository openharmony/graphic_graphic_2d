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
 * @file rs_property_modifier.h
 *
 * @brief Defines the properties and methods of all derived modifier classes.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H

#include "modifier/rs_modifier.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSBoundsModifier
 *
 * @brief The class for bounds modifiers.
 */
class RSC_EXPORT RSBoundsModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSBoundsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBoundsModifier.
     */
    virtual ~RSBoundsModifier() = default;
protected:

    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSBoundsModifier
 *
 * @brief The class for bounds modifiers.
 */
class RSC_EXPORT RSBoundsSizeModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSBoundsSizeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBoundsSizeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBoundsSizeModifier.
     */
    virtual ~RSBoundsSizeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSBoundsPositionModifier
 *
 * @brief The class for bounds position modifiers.
 */
class RSC_EXPORT RSBoundsPositionModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSBoundsPositionModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBoundsPositionModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBoundsPositionModifier.
     */
    virtual ~RSBoundsPositionModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSFrameModifier
 *
 * @brief The class for frame modifiers.
 */
class RSC_EXPORT RSFrameModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSFrameModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFrameModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFrameModifier.
     */
    virtual ~RSFrameModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPositionZModifier
 *
 * @brief The class for position Z modifiers.
 */
class RSC_EXPORT RSPositionZModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPositionZModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPositionZModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPositionZModifier.
     */
    virtual ~RSPositionZModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPositionZApplicableCamera3DModifier
 *
 * @brief The class for position Z applicable camera 3D modifiers.
 */
class RSC_EXPORT RSPositionZApplicableCamera3DModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPositionZApplicableCamera3DModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPositionZApplicableCamera3DModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPositionZApplicableCamera3DModifier.
     */
    virtual ~RSPositionZApplicableCamera3DModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSSandBoxModifier
 *
 * @brief The class for sandbox modifiers.
 */
class RSC_EXPORT RSSandBoxModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSSandBoxModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSandBoxModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSandBoxModifier.
     */
    virtual ~RSSandBoxModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPivotModifier
 *
 * @brief The class for pivot modifiers.
 */
class RSC_EXPORT RSPivotModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPivotModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPivotModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPivotModifier.
     */
    virtual ~RSPivotModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSPivotModifier
 *
 * @brief The class for pivot modifiers.
 */
class RSC_EXPORT RSPivotZModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPivotZModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPivotZModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPivotZModifier.
     */
    virtual ~RSPivotZModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSQuaternionModifier
 *
 * @brief The class for quaternion modifiers.
 */
class RSC_EXPORT RSQuaternionModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSQuaternionModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSQuaternionModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSQuaternionModifier.
     */
    virtual ~RSQuaternionModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSRotationModifier
 *
 * @brief The class for rotation modifiers.
 */
class RSC_EXPORT RSRotationModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSRotationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSRotationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSRotationModifier.
     */
    virtual ~RSRotationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSRotationXModifier
 *
 * @brief The class for rotation X modifiers.
 */
class RSC_EXPORT RSRotationXModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSRotationXModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSRotationXModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSRotationXModifier.
     */
    virtual ~RSRotationXModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSRotationYModifier
 *
 * @brief The class for rotation Y modifiers.
 */
class RSC_EXPORT RSRotationYModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSRotationYModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSRotationYModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSRotationYModifier.
     */
    virtual ~RSRotationYModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSCameraDistanceModifier
 *
 * @brief The class for camera distance modifiers.
 */
class RSC_EXPORT RSCameraDistanceModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSCameraDistanceModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSCameraDistanceModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSCameraDistanceModifier.
     */
    virtual ~RSCameraDistanceModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSScaleModifier
 *
 * @brief The class for scale modifiers.
 */
class RSC_EXPORT RSScaleModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSScaleModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSScaleModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSScaleModifier.
     */
    virtual ~RSScaleModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSScaleZModifier
 *
 * @brief The class for scale Z modifiers.
 */
class RSC_EXPORT RSScaleZModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSScaleZModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSScaleZModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSScaleZModifier.
     */
    virtual ~RSScaleZModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSSkewModifier
 *
 * @brief The class for skew modifiers.
 */
class RSC_EXPORT RSSkewModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSSkewModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSkewModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSkewModifier.
     */
    virtual ~RSSkewModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSPerspModifier
 *
 * @brief The class for perspective modifiers.
 */
class RSC_EXPORT RSPerspModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPerspModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPerspModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPerspModifier.
     */
    virtual ~RSPerspModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSTranslateModifier
 *
 * @brief The class for translate modifiers.
 */
class RSC_EXPORT RSTranslateModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSTranslateModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSTranslateModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSTranslateModifier.
     */
    virtual ~RSTranslateModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSTranslateZModifier
 *
 * @brief The class for translate Z modifiers.
 */
class RSC_EXPORT RSTranslateZModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSTranslateZModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSTranslateZModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSTranslateZModifier.
     */
    virtual ~RSTranslateZModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;

    /**
     * @brief Apply the modifier to the specified geometry.
     *
     * @param geometry A shared pointer to the RSObjAbsGeometry object.
     */
    void Apply(const std::shared_ptr<RSObjAbsGeometry>& geometry) override;
};

/**
 * @class RSCornerRadiusModifier
 *
 * @brief The class for corner radius modifiers.
 */
class RSC_EXPORT RSCornerRadiusModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSCornerRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSCornerRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSCornerRadiusModifier.
     */
    virtual ~RSCornerRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAlphaModifier
 *
 * @brief The class for alpha modifiers.
 */
class RSC_EXPORT RSAlphaModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSAlphaModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAlphaModifier.
     */
    virtual ~RSAlphaModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAlphaOffscreenModifier
 *
 * @brief The class for alpha offscreen modifiers.
 */
class RSC_EXPORT RSAlphaOffscreenModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSAlphaOffscreenModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAlphaOffscreenModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAlphaOffscreenModifier.
     */
    virtual ~RSAlphaOffscreenModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSEnvForegroundColorModifier
 *
 * @brief The class for environment foreground color modifiers.
 */
class RS_EXPORT RSEnvForegroundColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSEnvForegroundColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSEnvForegroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSEnvForegroundColorModifier.
     */
    virtual ~RSEnvForegroundColorModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSEnvForegroundColorStrategyModifier
 *
 * @brief The class for environment foreground color strategy modifiers.
 */
class RS_EXPORT RSEnvForegroundColorStrategyModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSEnvForegroundColorStrategyModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSEnvForegroundColorStrategyModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSEnvForegroundColorStrategyModifier.
     */
    virtual ~RSEnvForegroundColorStrategyModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundColorModifier
 *
 * @brief The class for foreground color modifiers.
 */
class RSC_EXPORT RSForegroundColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundColorModifier.
     */
    virtual ~RSForegroundColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSComplexShaderParamModifier : public RSBackgroundModifier {
public:
    explicit RSComplexShaderParamModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSComplexShaderParamModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBackgroundUIFilterModifier : public RSBackgroundModifier {
public:
    explicit RSBackgroundUIFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSBackgroundUIFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
private:
    void OnAttachToNode(const std::weak_ptr<RSNode>& target) override;
    void OnDetachFromNode() override;
};

class RSC_EXPORT RSForegroundUIFilterModifier : public RSForegroundModifier {
public:
    explicit RSForegroundUIFilterModifier(const std::shared_ptr<RSPropertyBase>& property);
    virtual ~RSForegroundUIFilterModifier() = default;
protected:
    RSModifierType GetModifierType() const override;
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
private:
    void OnAttachToNode(const std::weak_ptr<RSNode>& target) override;
    void OnDetachFromNode() override;
};

/**
 * @class RSUseShadowBatchingModifier
 *
 * @brief The class for shadow batching modifiers.
 */
class RSC_EXPORT RSUseShadowBatchingModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSUseShadowBatchingModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSUseShadowBatchingModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSUseShadowBatchingModifier.
     */
    virtual ~RSUseShadowBatchingModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundColorModifier
 *
 * @brief The class for background color modifiers.
 */
class RSC_EXPORT RSBackgroundColorModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundColorModifier.
     */
    virtual ~RSBackgroundColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundShaderModifier
 *
 * @brief The class for background shader modifiers.
 */
class RSC_EXPORT RSBackgroundShaderModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundShaderModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundShaderModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundShaderModifier.
     */
    virtual ~RSBackgroundShaderModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundShaderProgressModifier
 *
 * @brief The class for background shader progress modifiers.
 */
class RSC_EXPORT RSBackgroundShaderProgressModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundShaderProgressModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundShaderProgressModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundShaderProgressModifier.
     */
    virtual ~RSBackgroundShaderProgressModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgImageModifier
 *
 * @brief The class for background image modifiers.
 */
class RSC_EXPORT RSBgImageModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImageModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImageModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImageModifier.
     */
    virtual ~RSBgImageModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgImageInnerRectModifier
 *
 * @brief The class for background image inner rectangle modifiers.
 */
class RSC_EXPORT RSBgImageInnerRectModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImageInnerRectModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImageInnerRectModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImageInnerRectModifier.
     */
    virtual ~RSBgImageInnerRectModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgImageWidthModifier
 *
 * @brief The class for background image width modifiers.
 */
class RSC_EXPORT RSBgImageWidthModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImageWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImageWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImageWidthModifier.
     */
    virtual ~RSBgImageWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RSC_EXPORT RSBgImageHeightModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImageHeightModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImageHeightModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImageHeightModifier.
     */
    virtual ~RSBgImageHeightModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgImagePositionXModifier
 *
 * @brief The class for background image position X modifiers.
 */
class RSC_EXPORT RSBgImagePositionXModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImagePositionXModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImagePositionXModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImagePositionXModifier.
     */
    virtual ~RSBgImagePositionXModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgImagePositionYModifier
 *
 * @brief The class for background image position Y modifiers.
 */
class RSC_EXPORT RSBgImagePositionYModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgImagePositionYModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgImagePositionYModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgImagePositionYModifier.
     */
    virtual ~RSBgImagePositionYModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBorderColorModifier
 *
 * @brief The class for border color modifiers.
 */
class RSC_EXPORT RSBorderColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBorderColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBorderColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBorderColorModifier.
     */
    virtual ~RSBorderColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBorderWidthModifier
 *
 * @brief The class for border width modifiers.
 */
class RSC_EXPORT RSBorderWidthModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBorderWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBorderWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBorderWidthModifier.
     */
    virtual ~RSBorderWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBorderStyleModifier
 *
 * @brief The class for border style modifiers.
 */
class RSC_EXPORT RSBorderStyleModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBorderStyleModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBorderStyleModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBorderStyleModifier.
     */
    virtual ~RSBorderStyleModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBorderDashWidthModifier
 *
 * @brief The class for border dash width modifiers.
 */
class RSC_EXPORT RSBorderDashWidthModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBorderDashWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBorderDashWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBorderDashWidthModifier.
     */
    virtual ~RSBorderDashWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBorderDashGapModifier
 *
 * @brief The class for border dash gap modifiers.
 */
class RSC_EXPORT RSBorderDashGapModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBorderDashGapModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBorderDashGapModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBorderDashGapModifier.
     */
    virtual ~RSBorderDashGapModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineColorModifier
 *
 * @brief The class for outline color modifiers.
 */
class RSC_EXPORT RSOutlineColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineColorModifier.
     */
    virtual ~RSOutlineColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineWidthModifier
 *
 * @brief The class for outline width modifiers.
 */
class RSC_EXPORT RSOutlineWidthModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineWidthModifier.
     */
    virtual ~RSOutlineWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineDashWidthModifier
 *
 * @brief The class for outline dash width modifiers.
 */
class RSC_EXPORT RSOutlineDashWidthModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineDashWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineDashWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineDashWidthModifier.
     */
    virtual ~RSOutlineDashWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineDashGapModifier
 *
 * @brief The class for outline dash gap modifiers.
 */
class RSC_EXPORT RSOutlineDashGapModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineDashGapModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineDashGapModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineDashGapModifier.
     */
    virtual ~RSOutlineDashGapModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineStyleModifier
 *
 * @brief The class for outline style modifiers.
 */
class RSC_EXPORT RSOutlineStyleModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineStyleModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineStyleModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineStyleModifier.
     */
    virtual ~RSOutlineStyleModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSOutlineRadiusModifier
 *
 * @brief The class for outline radius modifiers.
 */
class RSC_EXPORT RSOutlineRadiusModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSOutlineRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSOutlineRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSOutlineRadiusModifier.
     */
    virtual ~RSOutlineRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundEffectRadiusModifier
 *
 * @brief The class for foreground effect radius modifiers.
 */
class RSC_EXPORT RSForegroundEffectRadiusModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundEffectRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundEffectRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundEffectRadiusModifier.
     */
    virtual ~RSForegroundEffectRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFilterModifier
 *
 * @brief The class for filter modifiers.
 */
class RSC_EXPORT RSFilterModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFilterModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFilterModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFilterModifier.
     */
    virtual ~RSFilterModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurRadiusModifier
 *
 * @brief The class for background blur radius modifiers.
 */
class RSC_EXPORT RSBackgroundBlurRadiusModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurRadiusModifier.
     */
    virtual ~RSBackgroundBlurRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurSaturationModifier
 *
 * @brief The class for background blur saturation modifiers.
 */
class RSC_EXPORT RSBackgroundBlurSaturationModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurSaturationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurSaturationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurSaturationModifier.
     */
    virtual ~RSBackgroundBlurSaturationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurBrightnessModifier
 *
 * @brief The class for background blur brightness modifiers.
 */
class RSC_EXPORT RSBackgroundBlurBrightnessModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurBrightnessModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurBrightnessModifier.
     */
    virtual ~RSBackgroundBlurBrightnessModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurMaskColorModifier
 *
 * @brief The class for background blur mask color modifiers.
 */
class RSC_EXPORT RSBackgroundBlurMaskColorModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurMaskColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurMaskColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurMaskColorModifier.
     */
    virtual ~RSBackgroundBlurMaskColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurColorModeModifier
 *
 * @brief The class for background blur color mode modifiers.
 */
class RSC_EXPORT RSBackgroundBlurColorModeModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurColorModeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurColorModeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurColorModeModifier.
     */
    virtual ~RSBackgroundBlurColorModeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurRadiusXModifier
 *
 * @brief The background blur radius modifier in the X direction
 */
class RSC_EXPORT RSBackgroundBlurRadiusXModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurRadiusXModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurRadiusXModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurRadiusXModifier.
     */
    virtual ~RSBackgroundBlurRadiusXModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundBlurRadiusYModifier
 *
 * @brief The background blur radius modifier in the Y direction
 */
class RSC_EXPORT RSBackgroundBlurRadiusYModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundBlurRadiusYModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundBlurRadiusYModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundBlurRadiusYModifier.
     */
    virtual ~RSBackgroundBlurRadiusYModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBlurDisableSystemAdaptationModifier
 *
 * @brief The class for background blur disable system adaptation modifiers.
 */
class RSC_EXPORT RSBgBlurDisableSystemAdaptationModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBlurDisableSystemAdaptationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBlurDisableSystemAdaptationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBlurDisableSystemAdaptationModifier.
     */
    virtual ~RSBgBlurDisableSystemAdaptationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAlwaysSnapshotModifier
 *
 * @brief The class for always snapshot modifiers.
 */
class RSC_EXPORT RSAlwaysSnapshotModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSAlwaysSnapshotModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAlwaysSnapshotModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAlwaysSnapshotModifier.
     */
    virtual ~RSAlwaysSnapshotModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurRadiusModifier
 *
 * @brief The class for foreground blur radius modifiers.
 */
class RSC_EXPORT RSForegroundBlurRadiusModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurRadiusModifier.
     */
    virtual ~RSForegroundBlurRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurSaturationModifier
 *
 * @brief The class for foreground blur saturation modifiers.
 */
class RSC_EXPORT RSForegroundBlurSaturationModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurSaturationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurSaturationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurSaturationModifier.
     */
    virtual ~RSForegroundBlurSaturationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurBrightnessModifier
 *
 * @brief The class for foreground blur brightness modifiers.
 */
class RSC_EXPORT RSForegroundBlurBrightnessModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurBrightnessModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurBrightnessModifier.
     */
    virtual ~RSForegroundBlurBrightnessModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurMaskColorModifier
 *
 * @brief The class for foreground blur mask color modifiers.
 */
class RSC_EXPORT RSForegroundBlurMaskColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurMaskColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurMaskColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurMaskColorModifier.
     */
    virtual ~RSForegroundBlurMaskColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurColorModeModifier
 *
 * @brief The class for foreground blur color mode modifiers.
 */
class RSC_EXPORT RSForegroundBlurColorModeModifier : public RSForegroundModifier {
 public:
    /**
     * @brief Construct a new RSForegroundBlurColorModeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurColorModeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurColorModeModifier.
     */
    virtual ~RSForegroundBlurColorModeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurRadiusXModifier
 *
 * @brief The foreground blur radius modifier in the X direction
 */
class RSC_EXPORT RSForegroundBlurRadiusXModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurRadiusXModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurRadiusXModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurRadiusXModifier.
     */
    virtual ~RSForegroundBlurRadiusXModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSForegroundBlurRadiusYModifier
 *
 * @brief The foreground blur radius modifier in the Y direction
 */
class RSC_EXPORT RSForegroundBlurRadiusYModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSForegroundBlurRadiusYModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSForegroundBlurRadiusYModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSForegroundBlurRadiusYModifier.
     */
    virtual ~RSForegroundBlurRadiusYModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBlurDisableSystemAdaptationModifier
 *
 * @brief The class for figure blur disable system adaptation modifiers.
 */
class RSC_EXPORT RSFgBlurDisableSystemAdaptationModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBlurDisableSystemAdaptationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBlurDisableSystemAdaptationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBlurDisableSystemAdaptationModifier.
     */
    virtual ~RSFgBlurDisableSystemAdaptationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSLightUpEffectModifier
 *
 * @brief The class for light up effect modifiers.
 */
class RSC_EXPORT RSLightUpEffectModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSLightUpEffectModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSLightUpEffectModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSLightUpEffectModifier.
     */
    virtual ~RSLightUpEffectModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSDynamicLightUpRateModifier
 *
 * @brief The class for dynamic light up rate modifiers.
 */
class RSC_EXPORT RSDynamicLightUpRateModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSDynamicLightUpRateModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSDynamicLightUpRateModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSDynamicLightUpRateModifier.
     */
    virtual ~RSDynamicLightUpRateModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSDynamicLightUpDegreeModifier
 *
 * @brief The class for dynamic light up degree modifiers.
 */ 
class RSC_EXPORT RSDynamicLightUpDegreeModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSDynamicLightUpDegreeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSDynamicLightUpDegreeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSDynamicLightUpDegreeModifier.
     */
    virtual ~RSDynamicLightUpDegreeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSWaterRippleParamsModifier
 *
 * @brief The class for water ripple parameters modifiers.
 */
class RSC_EXPORT RSWaterRippleParamsModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSWaterRippleParamsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSWaterRippleParamsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSWaterRippleParamsModifier.
     */
    virtual ~RSWaterRippleParamsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSWaterRippleProgressModifier
 *
 * @brief The class for water ripple progress modifiers.
 */
class RSC_EXPORT RSWaterRippleProgressModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSWaterRippleProgressModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSWaterRippleProgressModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSWaterRippleProgressModifier.
     */
    virtual ~RSWaterRippleProgressModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFlyOutParamsModifier
 *
 * @brief The class for fly out parameters modifiers.
 */
class RSC_EXPORT RSFlyOutParamsModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFlyOutParamsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFlyOutParamsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFlyOutParamsModifier.
     */
    virtual ~RSFlyOutParamsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFlyOutDegreeModifier
 *
 * @brief The class for fly out degree modifiers.
 */
class RSC_EXPORT RSFlyOutDegreeModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFlyOutDegreeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFlyOutDegreeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFlyOutDegreeModifier.
     */
    virtual ~RSFlyOutDegreeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSDistortionKModifier
 *
 * @brief The class for distortion K modifiers.
 */
class RSC_EXPORT RSDistortionKModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSDistortionKModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSDistortionKModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSDistortionKModifier.
     */
    virtual ~RSDistortionKModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBrightnessRatesModifier
 *
 * @brief The class for foreground brightness rates modifiers.
 */
class RSC_EXPORT RSFgBrightnessRatesModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBrightnessRatesModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBrightnessRatesModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBrightnessRatesModifier.
     */
    virtual ~RSFgBrightnessRatesModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBrightnessSaturationModifier
 *
 * @brief The class for foreground brightness saturation modifiers.
 */
class RSC_EXPORT RSFgBrightnessSaturationModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBrightnessSaturationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBrightnessSaturationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBrightnessSaturationModifier.
     */
    virtual ~RSFgBrightnessSaturationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBrightnessPosCoeffModifier
 *
 * @brief The class for figure brightness positive coefficient modifiers.
 */
class RSC_EXPORT RSFgBrightnessPosCoeffModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBrightnessPosCoeffModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBrightnessPosCoeffModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBrightnessPosCoeffModifier.
     */
    virtual ~RSFgBrightnessPosCoeffModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBrightnessNegCoeffModifier
 *
 * @brief The class for figure brightness negative coefficient modifiers.
 */
class RSC_EXPORT RSFgBrightnessNegCoeffModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBrightnessNegCoeffModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBrightnessNegCoeffModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBrightnessNegCoeffModifier.
     */
    virtual ~RSFgBrightnessNegCoeffModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFgBrightnessFractModifier
 *
 * @brief The class for figure brightness fraction modifiers.
 */
class RSC_EXPORT RSFgBrightnessFractModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSFgBrightnessFractModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFgBrightnessFractModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFgBrightnessFractModifier.
     */
    virtual ~RSFgBrightnessFractModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBrightnessRatesModifier
 *
 * @brief The class for background brightness rates modifiers.
 */
class RSC_EXPORT RSBgBrightnessRatesModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBrightnessRatesModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBrightnessRatesModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBrightnessRatesModifier.
     */
    virtual ~RSBgBrightnessRatesModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBrightnessSaturationModifier
 *
 * @brief The class for background brightness saturation modifiers.
 */
class RSC_EXPORT RSBgBrightnessSaturationModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBrightnessSaturationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBrightnessSaturationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBrightnessSaturationModifier.
     */
    virtual ~RSBgBrightnessSaturationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBrightnessPosCoeffModifier
 *
 * @brief The class for background brightness positive coefficient modifiers.
 */
class RSC_EXPORT RSBgBrightnessPosCoeffModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBrightnessPosCoeffModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBrightnessPosCoeffModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBrightnessPosCoeffModifier.
     */
    virtual ~RSBgBrightnessPosCoeffModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBrightnessNegCoeffModifier
 *
 * @brief The class for background brightness negative coefficient modifiers.
 */
class RSC_EXPORT RSBgBrightnessNegCoeffModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBrightnessNegCoeffModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBrightnessNegCoeffModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBrightnessNegCoeffModifier.
     */
    virtual ~RSBgBrightnessNegCoeffModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBgBrightnessFractModifier
 *
 * @brief The class for background brightness fraction modifiers.
 */
class RSC_EXPORT RSBgBrightnessFractModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBgBrightnessFractModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBgBrightnessFractModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBgBrightnessFractModifier.
     */
    virtual ~RSBgBrightnessFractModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSDynamicDimDegreeModifier
 *
 * @brief The class for dynamic dim degree modifiers.
 */
class RSC_EXPORT RSDynamicDimDegreeModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSDynamicDimDegreeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSDynamicDimDegreeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSDynamicDimDegreeModifier.
     */
    virtual ~RSDynamicDimDegreeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSEmitterUpdaterModifier
 *
 * @brief The class for emitter updater modifiers.
 */
class RSC_EXPORT RSEmitterUpdaterModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSEmitterUpdaterModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSEmitterUpdaterModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSEmitterUpdaterModifier.
     */
    virtual ~RSEmitterUpdaterModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSParticleNoiseFieldsModifier
 *
 * @brief The class for particle noise fields modifiers.
 */
class RSC_EXPORT RSParticleNoiseFieldsModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSParticleNoiseFieldsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSParticleNoiseFieldsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSParticleNoiseFieldsModifier.
     */
    virtual ~RSParticleNoiseFieldsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSLinearGradientBlurParaModifier
 *
 * @brief The class for linear gradient blur parameter modifiers.
 */
class RSC_EXPORT RSLinearGradientBlurParaModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSLinearGradientBlurParaModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSLinearGradientBlurParaModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSLinearGradientBlurParaModifier.
     */
    virtual ~RSLinearGradientBlurParaModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSMotionBlurParaModifier
 *
 * @brief The class for motion blur parameter modifiers.
 */
class RSC_EXPORT RSMotionBlurParaModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSMotionBlurParaModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSMotionBlurParaModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSMotionBlurParaModifier.
     */
    virtual ~RSMotionBlurParaModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSMagnifierParamsModifier
 *
 * @brief The class for magnifier parameter modifiers.
 */
class RSC_EXPORT RSMagnifierParamsModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSMagnifierParamsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSMagnifierParamsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSMagnifierParamsModifier.
     */
    virtual ~RSMagnifierParamsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBackgroundFilterModifier
 *
 * @brief The class for background filter modifiers.
 */
class RSC_EXPORT RSBackgroundFilterModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBackgroundFilterModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBackgroundFilterModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBackgroundFilterModifier.
     */
    virtual ~RSBackgroundFilterModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSFrameGravityModifier
 *
 * @brief The class for frame gravity modifiers.
 */
class RSC_EXPORT RSFrameGravityModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSFrameGravityModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSFrameGravityModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSFrameGravityModifier.
     */
    virtual ~RSFrameGravityModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSClipRRectModifier
 *
 * @brief The class for clip rounded rectangle modifiers.
 */
class RSC_EXPORT RSClipRRectModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSClipRRectModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSClipRRectModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSClipRRectModifier.
     */
    virtual ~RSClipRRectModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSClipBoundsModifier
 *
 * @brief The class for clip bounds modifiers.
 */
class RSC_EXPORT RSClipBoundsModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSClipBoundsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSClipBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSClipBoundsModifier.
     */
    virtual ~RSClipBoundsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSClipToBoundsModifier
 *
 * @brief The class for clip to bounds modifiers.
 */
class RSC_EXPORT RSClipToBoundsModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSClipToBoundsModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSClipToBoundsModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSClipToBoundsModifier.
     */
    virtual ~RSClipToBoundsModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSClipToFrameModifier
 *
 * @brief The class for clip to frame modifiers.
 */
class RSC_EXPORT RSClipToFrameModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSClipToFrameModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSClipToFrameModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSClipToFrameModifier.
     */
    virtual ~RSClipToFrameModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSCustomClipToFrameModifier
 *
 * @brief The class for custom clip to frame modifiers.
 */
class RS_EXPORT RSCustomClipToFrameModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSCustomClipToFrameModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSCustomClipToFrameModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSCustomClipToFrameModifier.
     */
    virtual ~RSCustomClipToFrameModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSHDRBrightnessFactorModifier
 *
 * @brief The class for HDR brightness factor modifiers.
 */
class RS_EXPORT RSHDRBrightnessFactorModifier : public RSForegroundModifier {
    public:
        /**
         * @brief Construct a new RSHDRBrightnessFactorModifier instance.
         *
         * @param property A shared pointer to the RSPropertyBase object.
         */
        explicit RSHDRBrightnessFactorModifier(const std::shared_ptr<RSPropertyBase>& property);
    
        /**
         * @brief Destructor for RSHDRBrightnessFactorModifier.
         */
        virtual ~RSHDRBrightnessFactorModifier() = default;
    
    protected:
        /**
         * @brief Get the type of the modifier.
         *
         * @return The type of the modifier.
         */
        RSModifierType GetModifierType() const override;
    
        /**
         * @brief Create a new render modifier.
         *
         * @return A shared pointer to the created RSRenderModifier.
         */
        std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
    };

/**
 * @class RSHDRBrightnessModifier
 *
 * @brief The class for HDR brightness modifiers.
 */
class RS_EXPORT RSHDRBrightnessModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSHDRBrightnessModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSHDRBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSHDRBrightnessModifier.
     */
    virtual ~RSHDRBrightnessModifier() = default;

protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

class RS_EXPORT RSHDRUIBrightnessModifier : public RSForegroundModifier {
    public:
        /**
         * @brief Construct a new RSHDRUIBrightnessModifier instance.
         *
         * @param property A shared pointer to the RSPropertyBase object.
         */
        explicit RSHDRUIBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);
    
        /**
         * @brief Destructor for RSHDRUIBrightnessModifier.
         */
        virtual ~RSHDRUIBrightnessModifier() = default;
    
    protected:
        /**
         * @brief Get the type of the modifier.
         *
         * @return The type of the modifier.
         */
        RSModifierType GetModifierType() const override;
    
        /**
         * @brief Create a new render modifier.
         *
         * @return A shared pointer to the created RSRenderModifier.
         */
        std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
    };

/**
 * @class RSVisibleModifier
 *
 * @brief The class for visibility modifiers.
 */
class RSC_EXPORT RSVisibleModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSVisibleModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSVisibleModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSVisibleModifier.
     */
    virtual ~RSVisibleModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSSpherizeModifier
 *
 * @brief The class for spherize modifiers.
 */
class RSC_EXPORT RSSpherizeModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSSpherizeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSpherizeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSpherizeModifier.
     */
    virtual ~RSSpherizeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAttractionFractionModifier
 *
 * @brief The class for attraction fraction modifiers.
 */
class RSC_EXPORT RSAttractionFractionModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSAttractionFractionModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAttractionFractionModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAttractionFractionModifier.
     */
    virtual ~RSAttractionFractionModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAttractionDstPointModifier
 *
 * @brief The class for attraction destination point modifiers.
 */
class RSC_EXPORT RSAttractionDstPointModifier : public RSAppearanceModifier {
public:
    /**
     * @brief Construct a new RSAttractionDstPointModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAttractionDstPointModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAttractionDstPointModifier.
     */
    virtual ~RSAttractionDstPointModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowColorModifier
 *
 * @brief The class for shadow color modifiers.
 */
class RSC_EXPORT RSShadowColorModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowColorModifier.
     */
    virtual ~RSShadowColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowOffsetXModifier
 *
 * @brief The class for shadow offset X modifiers.
 */
class RSC_EXPORT RSShadowOffsetXModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowOffsetXModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowOffsetXModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowOffsetXModifier.
     */
    virtual ~RSShadowOffsetXModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowOffsetYModifier
 *
 * @brief The class for shadow offset Y modifiers.
 */
class RSC_EXPORT RSShadowOffsetYModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowOffsetYModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowOffsetYModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowOffsetYModifier.
     */
    virtual ~RSShadowOffsetYModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowAlphaModifier
 *
 * @brief The class for shadow alpha modifiers.
 */
class RSC_EXPORT RSShadowAlphaModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowAlphaModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowAlphaModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowAlphaModifier.
     */
    virtual ~RSShadowAlphaModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowElevationModifier
 *
 * @brief The class for shadow elevation modifiers.
 */
class RSC_EXPORT RSShadowElevationModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowElevationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowElevationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowElevationModifier.
     */
    virtual ~RSShadowElevationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowRadiusModifier
 *
 * @brief The class for shadow radius modifiers.
 */
class RSC_EXPORT RSShadowRadiusModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowRadiusModifier.
     */
    virtual ~RSShadowRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowPathModifier
 *
 * @brief The class for shadow path modifiers.
 */
class RSC_EXPORT RSShadowPathModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowPathModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowPathModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowPathModifier.
     */
    virtual ~RSShadowPathModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowMaskModifier
 *
 * @brief The class for shadow mask modifiers.
 */
class RSC_EXPORT RSShadowMaskModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowMaskModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowMaskModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowMaskModifier.
     */
    virtual ~RSShadowMaskModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowIsFilledModifier
 *
 * @brief The class for shadow filled modifiers.
 */
class RSC_EXPORT RSShadowIsFilledModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowIsFilledModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowIsFilledModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowIsFilledModifier.
     */
    virtual ~RSShadowIsFilledModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSGreyCoefModifier
 *
 * @brief The class for grey coefficient modifiers.
 */
class RSC_EXPORT RSGreyCoefModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSGreyCoefModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSGreyCoefModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSGreyCoefModifier.
     */
    virtual ~RSGreyCoefModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSMaskModifier
 *
 * @brief The class for mask modifiers.
 */
class RSC_EXPORT RSMaskModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSMaskModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSMaskModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSMaskModifier.
     */
    virtual ~RSMaskModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPixelStretchModifier
 *
 * @brief The class for pixel stretch modifiers.
 */
class RSC_EXPORT RSPixelStretchModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPixelStretchModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPixelStretchModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPixelStretchModifier.
     */
    virtual ~RSPixelStretchModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPixelStretchPercentModifier
 *
 * @brief The class for pixel stretch percent modifiers.
 */
class RSC_EXPORT RSPixelStretchPercentModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPixelStretchPercentModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPixelStretchPercentModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPixelStretchPercentModifier.
     */
    virtual ~RSPixelStretchPercentModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSPixelStretchTileModeModifier
 *
 * @brief The class for pixel stretch tile mode modifiers.
 */
class RSC_EXPORT RSPixelStretchTileModeModifier : public RSGeometryModifier {
public:
    /**
     * @brief Construct a new RSPixelStretchTileModeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSPixelStretchTileModeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSPixelStretchTileModeModifier.
     */
    virtual ~RSPixelStretchTileModeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSGrayScaleModifier
 *
 * @brief The class for gray scale modifiers.
 */
class RSC_EXPORT RSGrayScaleModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSGrayScaleModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSGrayScaleModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSGrayScaleModifier.
     */
    virtual ~RSGrayScaleModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSLightIntensityModifier
 *
 * @brief The class for light intensity modifiers.
 */
class RSC_EXPORT RSLightIntensityModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSLightIntensityModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSLightIntensityModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSLightIntensityModifier.
     */
    virtual ~RSLightIntensityModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSLightColorModifier
 *
 * @brief The class for light color modifiers.
 */
class RSC_EXPORT RSLightColorModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSLightColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSLightColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSLightColorModifier.
     */
    virtual ~RSLightColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSIlluminatedTypeModifier
 *
 * @brief The class for illuminated type modifiers.
 */
class RSC_EXPORT RSIlluminatedTypeModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSIlluminatedTypeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSIlluminatedTypeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSIlluminatedTypeModifier.
     */
    virtual ~RSIlluminatedTypeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSLightPositionModifier
 *
 * @brief The class for light position modifiers.
 */
class RSC_EXPORT RSLightPositionModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSLightPositionModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSLightPositionModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSLightPositionModifier.
     */
    virtual ~RSLightPositionModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSIlluminatedBorderWidthModifier
 *
 * @brief The class for illuminated border width modifiers.
 */
class RSC_EXPORT RSIlluminatedBorderWidthModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSIlluminatedBorderWidthModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSIlluminatedBorderWidthModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSIlluminatedBorderWidthModifier.
     */
    virtual ~RSIlluminatedBorderWidthModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBloomModifier
 *
 * @brief The class for bloom modifiers.
 */
class RSC_EXPORT RSBloomModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBloomModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBloomModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBloomModifier.
     */
    virtual ~RSBloomModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBrightnessModifier
 *
 * @brief The class for brightness modifiers.
 */
class RSC_EXPORT RSBrightnessModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSBrightnessModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBrightnessModifier.
     */
    virtual ~RSBrightnessModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSContrastModifier
 *
 * @brief The class for contrast modifiers.
 */
class RSC_EXPORT RSContrastModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSContrastModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSContrastModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSContrastModifier.
     */
    virtual ~RSContrastModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSSaturateModifier
 *
 * @brief The class for saturate modifiers.
 */
class RSC_EXPORT RSSaturateModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSSaturateModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSaturateModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSaturateModifier.
     */
    virtual ~RSSaturateModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSSepiaModifier
 *
 * @brief The class for sepia modifiers.
 */
class RSC_EXPORT RSSepiaModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSSepiaModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSepiaModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSepiaModifier.
     */
    virtual ~RSSepiaModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSInvertModifier
 *
 * @brief The class for invert modifiers.
 */
class RSC_EXPORT RSInvertModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSInvertModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSInvertModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSInvertModifier.
     */
    virtual ~RSInvertModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSAiInvertModifier
 *
 * @brief The class for AI invert modifiers.
 */
class RSC_EXPORT RSAiInvertModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSAiInvertModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSAiInvertModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSAiInvertModifier.
     */
    virtual ~RSAiInvertModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSSystemBarEffectModifier
 *
 * @brief The class for system bar effect modifiers.
 */
class RSC_EXPORT RSSystemBarEffectModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSSystemBarEffectModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSSystemBarEffectModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSSystemBarEffectModifier.
     */
    virtual ~RSSystemBarEffectModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSHueRotateModifier
 *
 * @brief The class for hue rotate modifiers.
 */
class RSC_EXPORT RSHueRotateModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSHueRotateModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSHueRotateModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSHueRotateModifier.
     */
    virtual ~RSHueRotateModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSColorBlendModifier
 *
 * @brief The class for color blend modifiers.
 */
class RSC_EXPORT RSColorBlendModifier : public RSForegroundModifier {
public:
    /**
     * @brief Construct a new RSColorBlendModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSColorBlendModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSColorBlendModifier.
     */
    virtual ~RSColorBlendModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSUseEffectModifier
 *
 * @brief The class for use effect modifiers.
 */
class RSC_EXPORT RSUseEffectModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSUseEffectModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSUseEffectModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSUseEffectModifier.
     */
    virtual ~RSUseEffectModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSUseEffectTypeModifier
 *
 * @brief The class for use effect type modifiers.
 */
class RSC_EXPORT RSUseEffectTypeModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSUseEffectTypeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSUseEffectTypeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSUseEffectTypeModifier.
     */
    virtual ~RSUseEffectTypeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBehindWindowFilterRadiusModifier
 *
 * @brief The class for behind window filter radius modifiers.
 */
class RSC_EXPORT RSBehindWindowFilterRadiusModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBehindWindowFilterRadiusModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBehindWindowFilterRadiusModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBehindWindowFilterRadiusModifier.
     */
    virtual ~RSBehindWindowFilterRadiusModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBehindWindowFilterSaturationModifier
 *
 * @brief The class for behind window filter saturation modifiers.
 */
class RSC_EXPORT RSBehindWindowFilterSaturationModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBehindWindowFilterSaturationModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBehindWindowFilterSaturationModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBehindWindowFilterSaturationModifier.
     */
    virtual ~RSBehindWindowFilterSaturationModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBehindWindowFilterBrightnessModifier
 *
 * @brief The class for behind window filter brightness modifiers.
 */
class RSC_EXPORT RSBehindWindowFilterBrightnessModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBehindWindowFilterBrightnessModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBehindWindowFilterBrightnessModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBehindWindowFilterBrightnessModifier.
     */
    virtual ~RSBehindWindowFilterBrightnessModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSBehindWindowFilterMaskColorModifier
 *
 * @brief The class for behind window filter mask color modifiers.
 */
class RSC_EXPORT RSBehindWindowFilterMaskColorModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSBehindWindowFilterMaskColorModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSBehindWindowFilterMaskColorModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSBehindWindowFilterMaskColorModifier.
     */
    virtual ~RSBehindWindowFilterMaskColorModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSColorBlendModeModifier
 *
 * @brief The class for color blend mode modifiers.
 */
class RSC_EXPORT RSColorBlendModeModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSColorBlendModeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSColorBlendModeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSColorBlendModeModifier.
     */
    virtual ~RSColorBlendModeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSColorBlendApplyTypeModifier
 *
 * @brief The class for color blend apply type modifiers.
 */
class RSC_EXPORT RSColorBlendApplyTypeModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSColorBlendApplyTypeModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSColorBlendApplyTypeModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSColorBlendApplyTypeModifier.
     */
    virtual ~RSColorBlendApplyTypeModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};

/**
 * @class RSShadowColorStrategyModifier
 *
 * @brief The class for shadow color strategy modifiers.
 */
class RSC_EXPORT RSShadowColorStrategyModifier : public RSBackgroundModifier {
public:
    /**
     * @brief Construct a new RSShadowColorStrategyModifier instance.
     *
     * @param property A shared pointer to the RSPropertyBase object.
     */
    explicit RSShadowColorStrategyModifier(const std::shared_ptr<RSPropertyBase>& property);

    /**
     * @brief Destructor for RSShadowColorStrategyModifier.
     */
    virtual ~RSShadowColorStrategyModifier() = default;
protected:
    /**
     * @brief Get the type of the modifier.
     *
     * @return The type of the modifier.
     */
    RSModifierType GetModifierType() const override;

    /**
     * @brief Create a new render modifier.
     *
     * @return A shared pointer to the created RSRenderModifier.
     */
    std::shared_ptr<RSRenderModifier> CreateRenderModifier() const override;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_PROPERTY_MODIFIER_H
