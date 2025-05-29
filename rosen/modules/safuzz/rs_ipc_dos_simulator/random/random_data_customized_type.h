/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_RANDOM_DATA_CUSTOMIZED_TYPE_H
#define SAFUZZ_RANDOM_DATA_CUSTOMIZED_TYPE_H

#include "command/rs_animation_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_frame_rate_linker_command.h"
#include "command/rs_node_command.h"
#include "command/rs_node_showing_command.h"
#include "command/rs_surface_node_command.h"

namespace OHOS {
namespace Rosen {
class RandomDataCustomizedType {
public:
    // RSNodeCommand params
    static PropertyUpdateType GetRandomPropertyUpdateType();
    static RSUIFirstSwitch GetRandomRSUIFirstSwitch();
    static OutOfParentType GetRandomOutOfParentType();
    static Color GetRandomColor();
    static Gravity GetRandomGravity();
    static Matrix3f GetRandomMatrix3f();
    static RSDynamicBrightnessPara GetRandomRSDynamicBrightnessPara();
    static RSWaterRipplePara GetRandomRSWaterRipplePara();
    static RSFlyOutPara GetRandomRSFlyOutPara();
    static std::shared_ptr<MotionBlurParam> GetRandomMotionBlurParamSharedPtr();
    static std::shared_ptr<RSMagnifierParams> GetRandomRSMagnifierParamsSharedPtr();
    static std::vector<std::shared_ptr<EmitterUpdater>> GetRandomEmitterUpdaterSharedPtrVector(
        const std::string& sizeType = "normal");
    static ShapeType GetRandomShapeType();
    static std::shared_ptr<ParticleNoiseFields> GetRandomParticleNoiseFieldsSharedPtr(
        const std::string& sizeType = "normal");
    static Vector4<uint32_t> GetRandomUint32Vector4();
    static Vector4<Color> GetRandomColorVector4();
    static std::shared_ptr<RSRenderModifier> GetRandomRSRenderModifierSharedPtr();
    static std::shared_ptr<RSFilter> GetRandomRSFilterSharedPtr();
    static std::shared_ptr<RSImage> GetRandomRSImageSharedPtr();
    static std::shared_ptr<RSMask> GetRandomRSMaskSharedPtr();
    static std::shared_ptr<RSPath> GetRandomRSPathSharedPtr();
    static std::shared_ptr<RSLinearGradientBlurPara> GetRandomRSLinearGradientBlurParaSharedPtr();
    static std::shared_ptr<RSShader> GetRandomRSShaderSharedPtr();
    static Drawing::Matrix GetRandomDrawingMatrix();
    static std::shared_ptr<Drawing::DrawCmdList> GetRandomDrawingDrawCmdListPtr();

    // RSDisplayNodeCommand params
    static RSDisplayNodeConfig GetRandomRSDisplayNodeConfig();
    static ScreenRotation GetRandomScreenRotation();

    // RSSurfaceNodeCommand params
    static RSSurfaceNodeType GetRandomRSSurfaceNodeType();
    static SurfaceWindowType GetRandomSurfaceWindowType();
    static Drawing::Rect GetRandomDrawingRect();
    static std::optional<Drawing::Rect> GetRandomOptionalDrawingRect();
    static Vector2f GetRandomVector2f();
    static Vector3f GetRandomVector3f();
    static Vector4f GetRandomVector4f();
    static Quaternion GetRandomQuaternion();
    static RectF GetRandomRectF();
    static std::shared_ptr<RectF> GetRandomRectFSharedPtr();
    static RRect GetRandomRRect();
#ifndef ROSEN_CROSS_PLATFORM
    static GraphicColorGamut GetRandomGraphicColorGamut();
#endif
    static RSSurfaceNodeAbilityState GetRandomRSSurfaceNodeAbilityState();
    static std::optional<Drawing::Matrix> GetRandomOptionalDrawingMatrix();

    // RSAnimationCommand params
    static AnimationCallbackEvent GetRandomAnimationCallbackEvent();
    static std::vector<std::pair<uint64_t, uint64_t>> GetRandomUint64AndUint64PairVector();
    static RSInteractiveAnimationPosition GetRandomRSInteractiveAnimationPosition();
    static std::shared_ptr<RSRenderCurveAnimation> GetRandomRSRenderCurveAnimationSharedPtr();
    static std::shared_ptr<RSRenderParticleAnimation> GetRandomRSRenderParticleAnimationSharedPtr();
    static std::shared_ptr<RSRenderKeyframeAnimation> GetRandomRSRenderKeyframeAnimationSharedPtr();
    static std::shared_ptr<RSRenderPathAnimation> GetRandomRSRenderPathAnimationSharedPtr();
    static std::shared_ptr<RSRenderTransition> GetRandomRSRenderTransitionSharedPtr();
    static std::shared_ptr<RSRenderSpringAnimation> GetRandomRSRenderSpringAnimationSharedPtr();
    static std::shared_ptr<RSRenderInterpolatingSpringAnimation>
        GetRandomRSRenderInterpolatingSpringAnimationSharedPtr();

    // RSNodeShowingCommand params
    static std::shared_ptr<RSRenderPropertyBase> GetRandomRSRenderPropertyBaseSharedPtr();
    static RSNodeGetShowingPropertiesAndCancelAnimation::PropertiesMap GetRandomPropertiesMap();

    // RSFrameRateLinkerCommand params
    static ComponentScene GetRandomComponentScene();
    static FrameRateRange GetRandomFrameRateRange();

    static Drawing::Point GetRandomDrawingPoint();
    static Drawing::Color GetRandomDrawingColor();
    static Drawing::RectI GetRandomDrawingRectI();
    static SkMatrix GetRandomSkMatrix();
    static Range<float> GetRandomFloatRange();
    static Range<int64_t> GetRandomInt64Range();
    static Range<Color> GetRandomColorRange();
    static std::vector<std::shared_ptr<EmitterUpdater>> GetRandomSmallEmitterUpdaterSharedPtrVector();
    static std::shared_ptr<ParticleNoiseFields> GetRandomSmallParticleNoiseFieldsSharedPtr();
    static ForegroundColorStrategyType GetRandomForegroundColorStrategyType();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_DATA_CUSTOMIZED_TYPE_H
