# Animation System Module Knowledge Base

## OVERVIEW

Animation system for OpenHarmony graphics subsystem. Provides comprehensive animation framework supporting property animations, spring animations, keyframe animations, path animations, transitions, and particle effects. Uses dual-process architecture with Client-side control and RenderService-side execution. Compiles as animation components in `librender_service_client` and `librender_service_base`.

## STRUCTURE

```
rosen/modules/
├── render_service_client/core/animation/    # Client-side animation control
│   ├── rs_animation.h/cpp                  # Base RSAnimation class
│   ├── rs_property_animation.h/cpp          # Base property animation
│   ├── rs_curve_animation.h/cpp             # Non-physics curve animations
│   ├── rs_spring_animation.h/cpp            # Spring physics animations
│   ├── rs_interpolating_spring_animation.h/cpp  # Another kind of spring animations
│   ├── rs_keyframe_animation.h/cpp          # Keyframe animations
│   ├── rs_path_animation.h/cpp              # Path-based animations
│   ├── rs_transition.h/cpp                  # Node transition animations
│   ├── rs_symbol_animation.h/cpp             # Symbol/icon animations
│   ├── rs_implicit_animator.h/cpp          # Implicit animation manager
│   ├── rs_interactive_implict_animator.h/cpp # Interactive animation controller
│   ├── rs_animation_callback.h/cpp          # Animation callback
│   ├── rs_implicit_animation_param.h/cpp    # Implicit animation parameters
│   ├── rs_implicit_animator_map.h/cpp      # Per-thread animator manager todo1
│   ├── rs_animation_common.h                # Common definitions
│   ├── rs_animation_timing_curve.h/cpp      # Timing curves and interpolation
│   ├── rs_transition_effect.h/cpp           # Transition effects
│   └── test/                              # Unit tests
└── render_service_base/include/animation/   # RenderService-side execution
    ├── rs_render_animation.h                 # Base RSRenderAnimation
    ├── rs_render_property_animation.h         # Render property animation
    ├── rs_render_curve_animation.h            # Render curve animations
    ├── rs_render_spring_animation.h           # Render spring animations
    ├── rs_render_interpolating_spring_animation.h  # Render interpolating spring
    ├── rs_render_keyframe_animation.h         # Render keyframe animations
    ├── rs_render_path_animation.h             # Render path animations
    ├── rs_render_transition.h                # Render transition animations
    ├── rs_render_particle_animation.h          # Render particle animations
    ├── rs_animation_manager.h                 # Animation manager
    ├── rs_animation_fraction.h                # Animation fraction calculator
    ├── rs_animation_rate_decider.h            # Frame rate decider
    ├── rs_render_interactive_implict_animator.h # Render interactive animator
    ├── rs_value_estimator.h                   # Value estimators (curve/keyframe/spring)
    ├── rs_spring_model.h                     # Spring physics model
    ├── rs_interpolator.h                      # Interpolator system
    └── test/                                 # Render side unit tests
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Add new animation type (Client) | `render_service_client/core/animation/rs_*_animation.h/cpp` | Inherit RSPropertyAnimation/RSAnimation, implement lifecycle methods |
| Add new animation type (Render) | `render_service_base/include/animation/rs_render_*_animation.h` | Inherit RSRenderPropertyAnimation |
| Modify animation lifecycle | `render_service_client/core/animation/rs_animation.cpp` | Start(), Pause(), Resume(), Finish(), Reverse() |
| Change property value calculation | `render_service_base/include/animation/rs_value_estimator.h` | UpdateAnimationValue() in RSCurve/Keyframe/SpringValueEstimator |
| Modify spring physics | `render_service_base/include/animation/rs_spring_model.h` | CalculateSpringParameters(), CalculateDisplacement() |
| Change animation fraction calculation | `render_service_base/include/animation/rs_animation_fraction.h` | GetAnimationFraction(), UpdateRemainTimeFraction() |
| Add new interpolator type | `render_service_base/include/animation/rs_interpolator.h` | Inherit RSInterpolator, implement InterpolateImpl() |
| Modify implicit animation | `render_service_client/core/animation/rs_implicit_animator.cpp` | Open/CloseImplicitAnimation, CreateImplicitAnimation() |
| Add new implicit animation param | `render_service_client/core/animation/rs_implicit_animation_param.h` | Inherit RSImplicitAnimationParam, implement CreateAnimation() |
| Modify interactive animation | `render_service_client/core/animation/rs_interactive_implict_animator.cpp` | StartAnimation(), PauseAnimation(), SetFraction() |
| Change animation serialization | `render_service_base/include/animation/rs_render_animation.h` | Marshalling(), ParseParam(), Unmarshalling() |
| Modify animation manager | `render_service_base/include/animation/rs_animation_manager.h` | AddAnimation(), RemoveAnimation(), Animate() |
| Change frame rate decision | `render_service_base/include/animation/rs_animation_rate_decider.h` | AddDecisionElement(), MakeDecision() |
| Modify particle animation | `render_service_base/include/animation/rs_render_particle_animation.h` | Animate(), UpdateEmitter(), UpdateNoiseField() |
| Add symbol animation effect | `render_service_client/core/animation/rs_symbol_animation.h` | SetSymbolAnimation(), SpliceAnimation() |
| Change transition effect | `render_service_client/core/animation/rs_transition_effect.h` | Define new RSTransitionEffect subclass |
| Test animation lifecycle | `rosen/test/render_service_client/unittest/animation/` | Inherit animation test base, verify state transitions |
| Test render side animation | `rosen/test/render_service_base/unittest/animation/` | Test RSRenderAnimation, value estimation |
| Check animation support status | Client-side `IsSupportInteractiveAnimator()` | Returns true for CurveAnimation, InterpolatingSpringAnimation |

## CODE MAP

### Client-side Classes

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| RSAnimation | Base class | render_service_client/core/animation/rs_animation.h | Top-level animation base. Manages lifecycle, state, callbacks |
| RSPropertyAnimation | Class | render_service_client/core/animation/rs_property_animation.h | Base for property-based animations. Manages start/end/by values |
| RSCurveAnimation | Class | render_service_client/core/animation/rs_curve_animation.h | Timing curve animations (ease-in, ease-out, custom) |
| RSSpringAnimation | Class | render_service_client/core/animation/rs_spring_animation.h | Physics-based spring animations |
| RSInterpolatingSpringAnimation | Class | render_service_client/core/animation/rs_interpolating_spring_animation.h | Spring animations with  normalized property value |
| RSKeyframeAnimation | Class | render_service_client/core/animation/rs_keyframe_animation.h | An animation sequence with key points. |
| RSPathAnimation | Class | render_service_client/core/animation/rs_path_animation.h | Path-based motion animations |
| RSTransition | Class | render_service_client/core/animation/rs_transition.h | Aniamtion during node appear/disappear |
| RSSymbolAnimation | Class | render_service_client/core/animation/rs_symbol_animation.h | Symbol/icon effect animations |
| RSImplicitAnimator | Class | render_service_client/core/animation/rs_implicit_animator.h | Animation manager in client |
| RSInteractiveImplictAnimator | Class | render_service_client/core/animation/rs_interactive_implict_animator.h | Controls multiple animations for gesture-driven scenarios |
| AnimationFinishCallback | Class | render_service_client/core/animation/rs_animation_callback.h | One-time completion callback |
| AnimationRepeatCallback | Class | render_service_client/core/animation/rs_animation_callback.h | Per-repeat callback |
| InteractiveAnimatorFinishCallback | Class | render_service_client/core/animation/rs_animation_callback.h | Interactive animator completion callback |
| RSImplicitAnimationParam | Base class | render_service_client/core/animation/rs_implicit_animation_param.h | Base for implicit animation parameters |
| RSImplicitCurveAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Curve animation parameters |
| RSImplicitSpringAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Spring animation parameters |
| RSImplicitInterpolatingSpringAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Interpolating spring animation parameters |
| RSImplicitKeyframeAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Keyframe animation parameters |
| RSImplicitPathAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Path animation parameters |
| RSImplicitTransitionParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Transition animation parameters |
| RSImplicitCancelAnimationParam | Class | render_service_client/core/animation/rs_implicit_animation_param.h | Cancel and sync animation parameters |
| RSAnimationTimingProtocol | Struct | render_service_client/core/animation/rs_animation_timing_protocol.h | Duration, delay, repeat, direction, autoReverse, fillMode |
| RSAnimationTimingCurve | Class | render_service_client/core/animation/rs_animation_timing_curve.h | Interpolation behavior (LINEAR, EASE, CUSTOM, SPRING) |
| RSTransitionEffect | Base class | render_service_client/core/animation/rs_transition_effect.h | Transition configuration (fade, scale, slide) |

### Render-side Classes

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| RSRenderAnimation | Class (Parcelable) | render_service_base/include/animation/rs_render_animation.h | Render-side animation base. Implements serialization |
| RSRenderPropertyAnimation | Class | render_service_base/include/animation/rs_render_property_animation.h | Base for render-side property animations |
| RSRenderCurveAnimation | Class | render_service_base/include/animation/rs_render_curve_animation.h | Render-side curve animations |
| RSRenderSpringAnimation | Class | render_service_base/include/animation/rs_render_spring_animation.h | Render-side spring animations |
| RSRenderInterpolatingSpringAnimation | Class | render_service_base/include/animation/rs_render_interpolating_spring_animation.h | Render-side interpolating spring animations |
| RSRenderKeyframeAnimation | Class | render_service_base/include/animation/rs_render_keyframe_animation.h | Render-side keyframe animations |
| RSRenderPathAnimation | Class | render_service_base/include/animation/rs_render_path_animation.h | Render-side path animations |
| RSRenderTransition | Class | render_service_base/include/animation/rs_render_transition.h | Render-side transition animations |
| RSRenderParticleAnimation | Class | render_service_base/include/animation/rs_render_particle_animation.h | Render-side particle animations |
| RSAnimationManager | Class | render_service_base/include/animation/rs_animation_manager.h | Manages all animations for a node |
| RSAnimationFraction | Class | render_service_base/include/animation/rs_animation_fraction.h | Calculates animation fraction from time |
| RSAnimationRateDecider | Class | render_service_base/include/animation/rs_animation_rate_decider.h | Decides optimal frame rate based on animation |
| RSRenderInteractiveImplictAnimator | Class | render_service_base/include/animation/rs_render_interactive_implict_animator.h | Render-side interactive animator controller |
| RSValueEstimator | Base class | render_service_base/include/animation/rs_value_estimator.h | Base for value calculation |
| RSCurveValueEstimator | Template class | render_service_base/include/animation/rs_value_estimator.h | Curve animation value estimator |
| RSKeyframeValueEstimator | Template class | render_service_base/include/animation/rs_value_estimator.h | Keyframe animation value estimator |
| RSSpringValueEstimator | Template class | render_service_base/include/animation/rs_value_estimator.h | Spring animation value estimator |
| RSSpringModel | Template class | render_service_base/include/animation/rs_spring_model.h | Spring-damper physics model |
| RSInterpolator | Base class (Parcelable) | render_service_base/include/animation/rs_interpolator.h | Base for interpolation |
| LinearInterpolator | Class | render_service_base/include/animation/rs_interpolator.h | Linear interpolation |
| RSCustomInterpolator | Class | render_service_base/include/animation/rs_interpolator.h | Custom function interpolation |
| RSRenderPropertyBase | Class | render_service_base/include/modifier/rs_render_property.h | Base for render-side properties |
| RSRenderAnimatableProperty | Template class | render_service_base/include/modifier/rs_render_property.h | Animatable render property |

### Key Enums and Types

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| AnimationState | Enum | rs_animation.h, rs_render_animation.h | INITIALIZED, RUNNING, PAUSED, FINISHED |
| AnimationId | Typedef | rs_animation_common.h | 64-bit animation ID (PID | counter) |
| NodeId | Typedef | rs_animation_common.h | Node identifier |
| PropertyId | Typedef | rs_animation_common.h | Property identifier |
| RotationMode | Enum | rs_animation_common.h | ROTATE_NONE, ROTATE_AUTO, ROTATE_COUNTER_CLOCKWISE |
| RSInteractiveAnimationPosition | Enum | rs_animation_common.h | START, CURRENT, END (interactive finish position) |
| FinishCallbackType | Enum | rs_animation_callback.h | TIME_SENSITIVE, TIME_INSENSITIVE |
| ForwardDirection | Enum | rs_animation_fraction.h | NORMAL, REVERSE |
| FillMode | Enum | rs_animation_timing_protocol.h | FORWARDS, BACKWARDS, BOTH, NONE |
| InterpolatorType | Enum | rs_interpolator.h | LINEAR, CUSTOM, CUBIC_BEZIER, SPRING, STEPS |
| RSPropertyUnit | Enum | rs_animation_rate_decider.h | Property units for frame rate decision |
| ImplicitAnimationParamType | Enum | rs_animation_common.h | CURVE, SPRING, INTERPOLATING_SPRING, KEYFRAME, PATH, TRANSITION, CANCEL |
| RSInteractiveAnimationState | Enum | rs_interactive_implict_animator.h | INACTIVE, ACTIVE, RUNNING, PAUSED |
| FrameRateRange | Struct | rs_frame_rate_range.h | Frame rate range (min, max, preferred) |

## UNIQUE STYLES

- **Dual-process architecture**: Client-side lightweight control (RSAnimation) vs RenderService-side actual execution (RSRenderAnimation)
- **Parcelable-based IPC**: All RSRenderAnimation subclasses implement Parcelable for cross-process serialization
- **Stack-based implicit animation**: RSImplicitAnimator uses stacks (globalImplicitParams_, implicitAnimationParams_) for nested animation blocks
- **Interactive animation grouping**: RSInteractiveImplictAnimator groups multiple animations for unified gesture control
- **Spring physics modeling**: RSSpringModel implements under-damped/critically-damped/over-damped spring equations
- **Value estimator pattern**: RSValueEstimator base with template subclasses for different animation types
- **Animation fraction calculation**: RSAnimationFraction normalizes time to fraction (0.0-1.0) handling delay/repeat/direction
- **Frame rate decision**: RSAnimationRateDecider analyzes velocity and node size to choose optimal frame rate
- **Symbol animation composition**: RSSymbolAnimation uses Drawing::DrawingPiecewiseParameter for complex effect composition
- **Property-based animation**: All property animations inherit RSPropertyAnimation and animate RSAnimatableProperty/RSRenderAnimatableProperty values
- **Lifecycle callbacks**: AnimationFinishCallback, AnimationRepeatCallback, InteractiveAnimatorFinishCallback for different completion scenarios
- **Timing curve system**: RSAnimationTimingCurve and RSInterpolator provide flexible interpolation (linear, ease, custom, spring, steps)