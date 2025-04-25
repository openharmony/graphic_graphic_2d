/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H

#include <optional>
#include <unordered_map>

#include "animation/rs_animation_timing_curve.h"
#include "animation/rs_animation_timing_protocol.h"
#include "animation/rs_motion_path_option.h"
#include "animation/rs_particle_params.h"
#include "animation/rs_symbol_node_config.h"
#include "animation/rs_transition_effect.h"
#include "command/rs_animation_command.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_modifier_extractor.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_showing_properties_freezer.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "ui_effect/effect/include/background_color_effect_para.h"
#include "ui_effect/effect/include/visual_effect.h"
#include "ui_effect/filter/include/filter.h"
#include "ui_effect/filter/include/filter_pixel_stretch_para.h"
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_water_ripple_para.h"
#include "ui_effect/filter/include/filter_fly_out_para.h"
#include "ui_effect/filter/include/filter_distort_para.h"
#include "ui_effect/filter/include/filter_radius_gradient_blur_para.h"

#include "transaction/rs_transaction_handler.h"
#include "transaction/rs_sync_transaction_handler.h"
#include "recording/recording_canvas.h"

namespace OHOS {
namespace Rosen {
using DrawFunc = std::function<void(std::shared_ptr<Drawing::Canvas>)>;
using PropertyCallback = std::function<void()>;
using BoundsChangedCallback = std::function<void (const Rosen::Vector4f&)>;
using ExportTypeChangedCallback = std::function<void(bool)>;
using DrawNodeChangeCallback = std::function<void(std::shared_ptr<RSNode> rsNode, bool isPositionZ)>;
class RSAnimation;
class RSCommand;
class RSImplicitAnimParam;
class RSImplicitAnimator;
class RSModifier;
class RSObjAbsGeometry;
class RSUIContext;

class RSC_EXPORT RSNode : public std::enable_shared_from_this<RSNode> {
public:
    using WeakPtr = std::weak_ptr<RSNode>;
    using SharedPtr = std::shared_ptr<RSNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::RS_NODE;
    virtual RSUINodeType GetType() const
    {
        return Type;
    }

    RSNode(const RSNode&) = delete;
    RSNode(const RSNode&&) = delete;
    RSNode& operator=(const RSNode&) = delete;
    RSNode& operator=(const RSNode&&) = delete;
    virtual ~RSNode();

    // this id is ONLY used in hierarchy operation commands, this may differ from id_ when the node is a proxy node.
    virtual NodeId GetHierarchyCommandNodeId() const
    {
        return id_;
    }

    virtual void AddChild(SharedPtr child, int index = -1);
    void MoveChild(SharedPtr child, int index);
    virtual void RemoveChild(SharedPtr child);
    void RemoveChildByNodeSelf(WeakPtr child);
    void RemoveFromTree();
    virtual void ClearChildren();
    const std::vector<WeakPtr>& GetChildren() const
    {
        return children_;
    }
    // ONLY support index in [0, childrenTotal) or index = -1, otherwise return std::nullopt
    RSNode::SharedPtr GetChildByIndex(int index) const;

    // Add/RemoveCrossParentChild only used as: the child is under multiple parents(e.g. a window cross multi-screens)
    void AddCrossParentChild(SharedPtr child, int index);
    void RemoveCrossParentChild(SharedPtr child, SharedPtr newParent);
    void SetIsCrossNode(bool isCrossNode);

    // PC extend screen use this
    void AddCrossScreenChild(SharedPtr child, int index, bool autoClearCloneNode = false);
    void RemoveCrossScreenChild(SharedPtr child);

    NodeId GetId() const
    {
        return id_;
    }

    virtual FollowType GetFollowType() const
    {
        return FollowType::NONE;
    }

    bool IsInstanceOf(RSUINodeType type) const;
    template<typename T>
    RSC_EXPORT bool IsInstanceOf() const;

    // type-safe reinterpret_cast
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(const std::shared_ptr<RSNode>& node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }
    template<typename T>
    std::shared_ptr<const T> ReinterpretCastTo() const
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<const T>(shared_from_this()) : nullptr;
    }

    void DumpTree(int depth, std::string& out) const;
    virtual void Dump(std::string& out) const;

    virtual std::string DumpNode(int depth) const;
    SharedPtr GetParent();

    void SetId(const NodeId& id)
    {
        id_ = id;
    }

    bool IsUniRenderEnabled() const;
    bool IsRenderServiceNode() const;
    void SetTakeSurfaceForUIFlag();

    static std::vector<std::shared_ptr<RSAnimation>> Animate(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback,
        const std::function<void()>& finishCallback = nullptr, const std::function<void()>& repeatCallback = nullptr);

    static std::vector<std::shared_ptr<RSAnimation>> AnimateWithCurrentOptions(
        const PropertyCallback& callback, const std::function<void()>& finishCallback, bool timingSensitive = true);
    static std::vector<std::shared_ptr<RSAnimation>> AnimateWithCurrentCallback(
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
        const PropertyCallback& callback);

    static void RegisterTransitionPair(NodeId inNodeId, NodeId outNodeId, const bool isInSameWindow);
    static void UnregisterTransitionPair(NodeId inNodeId, NodeId outNodeId);

    static void OpenImplicitAnimation(const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const std::function<void()>& finishCallback = nullptr);
    static std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation();
    static bool CloseImplicitCancelAnimation();
    static bool IsImplicitAnimationOpen();

    static void ExecuteWithoutAnimation(const PropertyCallback& callback,
        const std::shared_ptr<RSUIContext> rsUIContext = nullptr,
        std::shared_ptr<RSImplicitAnimator> implicitAnimator = nullptr);

    static void AddKeyFrame(
        float fraction, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);
    static void AddKeyFrame(float fraction, const PropertyCallback& callback);
    static void AddDurationKeyFrame(
        int duration, const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);

    // multi-instance
    static std::vector<std::shared_ptr<RSAnimation>> Animate(const std::shared_ptr<RSUIContext> rsUIContext,
        const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback,
        const std::function<void()>& finishCallback = nullptr, const std::function<void()>& repeatCallback = nullptr);
    static std::vector<std::shared_ptr<RSAnimation>> AnimateWithCurrentOptions(
        const std::shared_ptr<RSUIContext> rsUIContext, const PropertyCallback& callback,
        const std::function<void()>& finishCallback, bool timingSensitive = true);
    static std::vector<std::shared_ptr<RSAnimation>> AnimateWithCurrentCallback(
        const std::shared_ptr<RSUIContext> rsUIContext, const RSAnimationTimingProtocol& timingProtocol,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);

    static void RegisterTransitionPair(const std::shared_ptr<RSUIContext> rsUIContext,
        NodeId inNodeId, NodeId outNodeId, const bool isInSameWindow);
    static void UnregisterTransitionPair(const std::shared_ptr<RSUIContext> rsUIContext,
        NodeId inNodeId, NodeId outNodeId);

    static void OpenImplicitAnimation(const std::shared_ptr<RSUIContext> rsUIContext,
        const RSAnimationTimingProtocol& timingProtocol, const RSAnimationTimingCurve& timingCurve,
        const std::function<void()>& finishCallback = nullptr);
    static std::vector<std::shared_ptr<RSAnimation>> CloseImplicitAnimation(
        const std::shared_ptr<RSUIContext> rsUIContext);
    static bool CloseImplicitCancelAnimation(const std::shared_ptr<RSUIContext> rsUIContext);
    static bool IsImplicitAnimationOpen(const std::shared_ptr<RSUIContext> rsUIContext);
    static void AddKeyFrame(const std::shared_ptr<RSUIContext> rsUIContext, float fraction,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);
    static void AddKeyFrame(
        const std::shared_ptr<RSUIContext> rsUIContext, float fraction, const PropertyCallback& callback);
    static void AddDurationKeyFrame(const std::shared_ptr<RSUIContext> rsUIContext, int duration,
        const RSAnimationTimingCurve& timingCurve, const PropertyCallback& callback);
    void NotifyTransition(const std::shared_ptr<const RSTransitionEffect>& effect, bool isTransitionIn);

    void AddAnimation(const std::shared_ptr<RSAnimation>& animation, bool isStartAnimation = true);
    void RemoveAllAnimations();
    void RemoveAnimation(const std::shared_ptr<RSAnimation>& animation);
    void SetMotionPathOption(const std::shared_ptr<RSMotionPathOption>& motionPathOption);
    const std::shared_ptr<RSMotionPathOption> GetMotionPathOption() const;

    virtual void DrawOnNode(RSModifierType type, DrawFunc func) {} // [PLANNING]: support SurfaceNode

    const RSModifierExtractor& GetStagingProperties() const;
    const RSShowingPropertiesFreezer& GetShowingProperties() const;

    template<typename ModifierName, typename PropertyName, typename T>
    void SetProperty(RSModifierType modifierType, T value);

    virtual void SetBounds(const Vector4f& bounds);
    virtual void SetBounds(float positionX, float positionY, float width, float height);
    virtual void SetBoundsWidth(float width);
    virtual void SetBoundsHeight(float height);

    virtual void SetFrame(const Vector4f& frame);
    virtual void SetFrame(float positionX, float positionY, float width, float height);
    virtual void SetFramePositionX(float positionX);
    virtual void SetFramePositionY(float positionY);

    // The property is valid only for CanvasNode and SurfaceNode in uniRender.
    virtual void SetFreeze(bool isFreeze);
    void SetNodeName(const std::string& nodeName);

    void SetSandBox(std::optional<Vector2f> parentPosition);

    void SetPositionZ(float positionZ);
    void SetPositionZApplicableCamera3D(bool isApplicable);

    void SetPivot(const Vector2f& pivot);
    void SetPivot(float pivotX, float pivotY);
    void SetPivotX(float pivotX);
    void SetPivotY(float pivotY);
    void SetPivotZ(float pivotZ);

    void SetCornerRadius(float cornerRadius);
    void SetCornerRadius(const Vector4f& cornerRadius);

    void SetRotation(const Quaternion& quaternion);
    void SetRotation(float degreeX, float degreeY, float degreeZ);
    void SetRotation(float degree);
    void SetRotationX(float degree);
    void SetRotationY(float degree);
    void SetCameraDistance(float cameraDistance);

    void SetTranslate(const Vector2f& translate);
    void SetTranslate(float translateX, float translateY, float translateZ);
    void SetTranslateX(float translate);
    void SetTranslateY(float translate);
    void SetTranslateZ(float translate);

    void SetScale(float scale);
    void SetScale(float scaleX, float scaleY);
    void SetScale(const Vector2f& scale);
    void SetScaleX(float scaleX);
    void SetScaleY(float scaleY);
    void SetScaleZ(const float& scaleZ);

    void SetSkew(float skew);
    void SetSkew(float skewX, float skewY);
    void SetSkew(float skewX, float skewY, float skewZ);
    void SetSkew(const Vector3f& skew);
    void SetSkewX(float skewX);
    void SetSkewY(float skewY);
    void SetSkewZ(float skewZ);

    void SetPersp(float persp);
    void SetPersp(float perspX, float perspY);
    void SetPersp(float perspX, float perspY, float perspZ, float perspW);
    void SetPersp(const Vector4f& persp);
    void SetPerspX(float perspX);
    void SetPerspY(float perspY);
    void SetPerspZ(float perspZ);
    void SetPerspW(float perspW);

    void SetAlpha(float alpha);
    void SetAlphaOffscreen(bool alphaOffscreen);

    void SetEnvForegroundColor(uint32_t colorValue);
    void SetEnvForegroundColorStrategy(ForegroundColorStrategyType colorType);
    void SetParticleParams(
        std::vector<ParticleParams>& particleParams, const std::function<void()>& finishCallback = nullptr);
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);
    void SetForegroundColor(uint32_t colorValue);
    void SetBackgroundColor(uint32_t colorValue);
    void SetBackgroundShader(const std::shared_ptr<RSShader>& shader);
    void SetBackgroundShaderProgress(const float& process);

    void SetBgImage(const std::shared_ptr<RSImage>& image);
    void SetBgImageInnerRect(const Vector4f& innerRect);
    void SetBgImageSize(float width, float height);
    void SetBgImageWidth(float width);
    void SetBgImageHeight(float height);
    void SetBgImagePosition(float positionX, float positionY);
    void SetBgImagePositionX(float positionX);
    void SetBgImagePositionY(float positionY);

    void SetBorderColor(uint32_t colorValue);
    void SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetBorderColor(const Vector4<Color>& color);
    void SetBorderWidth(float width);
    void SetBorderWidth(float left, float top, float right, float bottom);
    void SetBorderWidth(const Vector4f& width);
    void SetBorderStyle(uint32_t styleValue);
    void SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetBorderStyle(const Vector4<BorderStyle>& style);
    void SetBorderDashWidth(const Vector4f& dashWidth);
    void SetBorderDashGap(const Vector4f& dashGap);
    void SetOuterBorderColor(const Vector4<Color>& color);
    void SetOuterBorderWidth(const Vector4f& width);
    void SetOuterBorderStyle(const Vector4<BorderStyle>& style);
    void SetOuterBorderRadius(const Vector4f& radius);
    void SetOutlineColor(const Vector4<Color>& color);
    void SetOutlineWidth(const Vector4f& width);
    void SetOutlineStyle(const Vector4<BorderStyle>& style);
    void SetOutlineDashWidth(const Vector4f& dashWidth);
    void SetOutlineDashGap(const Vector4f& dashGap);
    void SetOutlineRadius(const Vector4f& radius);

    // UIEffect
    void SetUIBackgroundFilter(const OHOS::Rosen::Filter* backgroundFilter);
    void SetUICompositingFilter(const OHOS::Rosen::Filter* compositingFilter);
    void SetUIForegroundFilter(const OHOS::Rosen::Filter* foregroundFilter);
    void SetVisualEffect(const VisualEffect* visualEffect);

    void SetForegroundEffectRadius(const float blurRadius);
    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);
    void SetFilter(const std::shared_ptr<RSFilter>& filter);
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);
    void SetMotionBlurPara(const float radius, const Vector2f& anchor);
    void SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& para);
    void SetDynamicLightUpRate(const float rate);
    void SetDynamicLightUpDegree(const float lightUpDegree);
    void SetDynamicDimDegree(const float dimDegree);
    void SetBlender(const Blender* blender);
    void SetFgBrightnessParams(const RSDynamicBrightnessPara& params);
    void SetFgBrightnessRates(const Vector4f& rates);
    void SetFgBrightnessSaturation(const float& saturation);
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    void SetFgBrightnessFract(const float& fract);
    void SetBgBrightnessParams(const RSDynamicBrightnessPara& params);
    void SetBgBrightnessRates(const Vector4f& rates);
    void SetBgBrightnessSaturation(const float& saturation);
    void SetBgBrightnessPosCoeff(const Vector4f& coeff);
    void SetBgBrightnessNegCoeff(const Vector4f& coeff);
    void SetBgBrightnessFract(const float& fract);
    void SetGreyCoef(const Vector2f greyCoef);
    void SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter);

    void SetShadowColor(uint32_t colorValue);
    void SetShadowOffset(float offsetX, float offsetY);
    void SetShadowOffsetX(float offsetX);
    void SetShadowOffsetY(float offsetY);
    void SetShadowAlpha(float alpha);
    void SetShadowElevation(float elevation);
    void SetShadowRadius(float radius);
    void SetShadowPath(const std::shared_ptr<RSPath>& shadowPath);
    void SetShadowMask(bool shadowMask);
    void SetShadowIsFilled(bool shadowIsFilled);
    void SetShadowColorStrategy(int shadowColorStrategy);

    void SetFrameGravity(Gravity gravity);

    void SetClipRRect(const Vector4f& clipRect, const Vector4f& clipRadius);
    void SetClipRRect(const std::shared_ptr<RRect>& rrect);
    void SetClipBounds(const std::shared_ptr<RSPath>& clipToBounds);
    void SetClipToBounds(bool clipToBounds);
    void SetClipToFrame(bool clipToFrame);
    void SetCustomClipToFrame(const Vector4f& clipRect);
    void SetHDRBrightness(const float& hdrBrightness);

    void SetVisible(bool visible);
    void SetMask(const std::shared_ptr<RSMask>& mask);
    void SetSpherizeDegree(float spherizeDegree);
    void SetLightUpEffectDegree(float LightUpEffectDegree);

    void SetAttractionEffect(float fraction, const Vector2f& destinationPoint);
    void SetAttractionEffectFraction(float fraction);
    void SetAttractionEffectDstPoint(Vector2f destinationPoint);

    void SetPixelStretch(const Vector4f& stretchSize, Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP);
    void SetPixelStretchPercent(const Vector4f& stretchPercent,
        Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP);

    void SetWaterRippleParams(const RSWaterRipplePara& params, float progress);
    void SetFlyOutParams(const RSFlyOutPara& params, float degree);

    void SetDistortionK(const float distortionK);

    void SetPaintOrder(bool drawContentLast);

    void SetTransitionEffect(const std::shared_ptr<const RSTransitionEffect>& effect)
    {
        transitionEffect_ = effect;
    }

    void SetUseEffect(bool useEffect);
    void SetUseEffectType(UseEffectType useEffectType);

    void SetUseShadowBatching(bool useShadowBatching);

    void SetColorBlendMode(RSColorBlendMode colorBlendMode);

    void SetColorBlendApplyType(RSColorBlendApplyType colorBlendApplyType);

    // driven render was shelved, functions will be deleted soon [start]
    void MarkDrivenRender(bool flag) {}
    void MarkDrivenRenderItemIndex(int index) {}
    void MarkDrivenRenderFramePaintState(bool flag) {}
    void MarkContentChanged(bool isChanged) {}
    // driven render was shelved, functions will be deleted soon [end]

    void AddModifier(const std::shared_ptr<RSModifier> modifier);
    void RemoveModifier(const std::shared_ptr<RSModifier> modifier);

    void SetIsCustomTextType(bool isCustomTextType);

    bool GetIsCustomTextType();

    void SetIsCustomTypeface(bool isCustomTypeface);

    bool GetIsCustomTypeface();

    void SetDrawRegion(std::shared_ptr<RectF> rect);

    // Mark preferentially draw node and childrens
    void MarkNodeGroup(bool isNodeGroup, bool isForced = true, bool includeProperty = false);

    // Mark opinc node
    void MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate = false);
    // will be abandoned
    void MarkUifirstNode(bool isUifirstNode);
    // Mark uifirst leash node
    void MarkUifirstNode(bool isForceFlag, bool isUifirstEnable);

    void SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch);

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer);

    void SetGrayScale(float grayScale);

    void SetLightIntensity(float lightIntensity);

    void SetLightColor(uint32_t lightColorValue);

    void SetLightPosition(const Vector4f& lightPosition);

    void SetLightPosition(float positionX, float positionY, float positionZ);

    void SetIlluminatedBorderWidth(float illuminatedBorderWidth);

    void SetIlluminatedType(uint32_t illuminatedType);

    void SetBloom(float bloomIntensity);

    void SetBrightness(float brightness);

    void SetContrast(float contrast);

    void SetSaturate(float saturate);

    void SetSepia(float sepia);

    void SetInvert(float invert);

    void SetAiInvert(const Vector4f& aiInvert);

    void SetSystemBarEffect();

    void SetHueRotate(float hueRotate);

    void SetColorBlend(uint32_t colorValue);

    int32_t CalcExpectedFrameRate(const std::string& scene, float speed);

    void SetOutOfParent(OutOfParentType outOfParent);

    void SetFrameNodeInfo(int32_t id, std::string tag);

    virtual void SetTextureExport(bool isTextureExportNode);

    void SyncTextureExport(bool isTextureExportNode);

    int32_t GetFrameNodeId();

    std::string GetFrameNodeTag();

    virtual void SetBoundsChangedCallback(BoundsChangedCallback callback){};
    bool IsTextureExportNode() const
    {
        return isTextureExportNode_;
    }

    size_t GetAnimationsCount() const
    {
        return animations_.size();
    }
    void SetExportTypeChangedCallback(ExportTypeChangedCallback callback);

    bool IsGeometryDirty() const;
    bool IsAppearanceDirty() const;
    void MarkDirty(NodeDirtyType type, bool isDirty);

    float GetGlobalPositionX() const;
    float GetGlobalPositionY() const;

    std::shared_ptr<RSObjAbsGeometry> GetLocalGeometry() const;
    std::shared_ptr<RSObjAbsGeometry> GetGlobalGeometry() const;
    void UpdateLocalGeometry();
    void UpdateGlobalGeometry(const std::shared_ptr<RSObjAbsGeometry>& parentGlobalGeometry);

    std::mutex childrenNodeLock_; // lock for map operation
    // key: symbolSpanID, value:nodeid and symbol animation node list
    std::unordered_map<uint64_t, std::unordered_map<NodeId, SharedPtr>> canvasNodesListMap_;

    // key: status : 0 invalid, 1 appear, value:symbol node animation config
    std::array<std::vector<OHOS::Rosen::AnimationNodeConfig>, 2> replaceNodesSwapArr_;

    void SetInstanceId(int32_t instanceId);
    int32_t GetInstanceId() const
    {
        return instanceId_;
    }

    const std::string GetNodeName() const
    {
        return nodeName_;
    }

    static DrawNodeChangeCallback drawNodeChangeCallback_;
    static void SetDrawNodeChangeCallback(DrawNodeChangeCallback callback);
    bool GetIsDrawn();
    void SetDrawNode();

    std::shared_ptr<RSUIContext> GetRSUIContext()
    {
        return rsUIContext_.lock();
    }
    void SetUIContextToken();
    void SetRSUIContext(std::shared_ptr<RSUIContext> rsUIContext);

    void SetSkipCheckInMultiInstance(bool isSkipCheckInMultiInstance);

#ifdef RS_ENABLE_VK
    bool IsHybridRenderCanvas() const
    {
        return hybridRenderCanvas_;
    }
    virtual void SetHybridRenderCanvas(bool hybridRenderCanvas) {};
#endif

    bool GetIsOnTheTree()
    {
        return isOnTheTree_;
    }
protected:
    explicit RSNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr,
        bool isOnTheTree = false);
    explicit RSNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr, bool isOnTheTree = false);

    bool isRenderServiceNode_;
    bool isTextureExportNode_ = false;
    bool skipDestroyCommandInDestructor_ = false;
    ExportTypeChangedCallback exportTypeChangedCallback_ = nullptr;

    // Used for same layer rendering, to determine whether RT or RS generates renderNode when the type of node switches
    bool hasCreateRenderNodeInRT_ = false;
    bool hasCreateRenderNodeInRS_ = false;

    bool drawContentLast_ = false;

#ifdef RS_ENABLE_VK
    bool hybridRenderCanvas_ = false;
#endif

    virtual void OnAddChildren();
    virtual void OnRemoveChildren();

    virtual bool NeedForcedSendToRemote() const
    {
        return false;
    }

    void DoFlushModifier();

    std::vector<PropertyId> GetModifierIds() const;
    bool isCustomTextType_ = false;
    bool isCustomTypeface_ = false;

    std::recursive_mutex& GetPropertyMutex() const
    {
        return propertyMutex_;
    }
    bool CheckMultiThreadAccess(const std::string& func) const;
    virtual void RegisterNodeMap() {}
    std::shared_ptr<RSTransactionHandler> GetRSTransaction() const;
    bool AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false,
        FollowType followType = FollowType::NONE, NodeId nodeId = 0) const;
    void SetIsOnTheTree(bool flag);

private:
    static NodeId GenerateId();
    static void InitUniRenderEnabled();
    NodeId id_;
    WeakPtr parent_;
    int32_t instanceId_ = INSTANCE_ID_UNDEFINED;
    int32_t frameNodeId_ = -1;
    std::string frameNodeTag_;
    std::string nodeName_ = "";
    std::vector<WeakPtr> children_;
    void SetParent(WeakPtr parent);
    void RemoveChildByNode(SharedPtr child);
    virtual void CreateRenderNodeForTextureExportSwitch() {};

    void SetBackgroundBlurRadius(float radius);
    void SetBackgroundBlurSaturation(float saturation);
    void SetBackgroundBlurBrightness(float brightness);
    void SetBackgroundBlurMaskColor(Color maskColor);
    void SetBackgroundBlurColorMode(int colorMode);
    void SetBackgroundBlurRadiusX(float blurRadiusX);
    void SetBackgroundBlurRadiusY(float blurRadiusY);
    void SetBgBlurDisableSystemAdaptation(bool disableSystemAdaptation);

    void SetForegroundBlurRadius(float radius);
    void SetForegroundBlurSaturation(float saturation);
    void SetForegroundBlurBrightness(float brightness);
    void SetForegroundBlurMaskColor(Color maskColor);
    void SetForegroundBlurColorMode(int colorMode);
    void SetForegroundBlurRadiusX(float blurRadiusX);
    void SetForegroundBlurRadiusY(float blurRadiusY);
    void SetFgBlurDisableSystemAdaptation(bool disableSystemAdaptation);

    bool AnimationCallback(AnimationId animationId, AnimationCallbackEvent event);
    bool HasPropertyAnimation(const PropertyId& id);
    std::vector<AnimationId> GetAnimationByPropertyId(const PropertyId& id);
    bool FallbackAnimationsToContext();
    void FallbackAnimationsToRoot();
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void FinishAnimationByProperty(const PropertyId& id);
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void CancelAnimationByProperty(const PropertyId& id, const bool needForceSync = false);
    const std::shared_ptr<RSModifier> GetModifier(const PropertyId& propertyId);
    virtual void OnBoundsSizeChanged() const {};
    void UpdateModifierMotionPathOption();
    void MarkAllExtendModifierDirty();
    void ResetExtendModifierDirty();
    void SetParticleDrawRegion(std::vector<ParticleParams>& particleParams);

    // Planning: refactor RSUIAnimationManager and remove this method
    void ClearAllModifiers();

    uint32_t dirtyType_ = static_cast<uint32_t>(NodeDirtyType::NOT_DIRTY);

    std::shared_ptr<RSObjAbsGeometry> localGeometry_;
    std::shared_ptr<RSObjAbsGeometry> globalGeometry_;

    float globalPositionX_ = 0.f;
    float globalPositionY_ = 0.f;

    bool extendModifierIsDirty_ { false };

    bool isNodeGroup_ = false;

    bool isNodeSingleFrameComposer_ = false;

    bool isSuggestOpincNode_ = false;
    bool isDrawNode_ = false;

    bool isUifirstNode_ = true;
    bool isForceFlag_ = false;
    bool isUifirstEnable_ = false;
    bool isSkipCheckInMultiInstance_ = false;
    RSUIFirstSwitch uiFirstSwitch_ = RSUIFirstSwitch::NONE;
    std::weak_ptr<RSUIContext> rsUIContext_;

    RSModifierExtractor stagingPropertiesExtractor_;
    RSShowingPropertiesFreezer showingPropertiesFreezer_;
    std::map<PropertyId, std::shared_ptr<RSModifier>> modifiers_;
    std::map<uint16_t, std::shared_ptr<RSModifier>> modifiersTypeMap_;
    std::map<RSModifierType, std::shared_ptr<RSModifier>> propertyModifiers_;
    std::shared_ptr<RectF> drawRegion_;
    OutOfParentType outOfParent_ = OutOfParentType::UNKNOWN;

    std::unordered_map<AnimationId, std::shared_ptr<RSAnimation>> animations_;
    std::unordered_map<PropertyId, uint32_t> animatingPropertyNum_;
    std::shared_ptr<RSMotionPathOption> motionPathOption_;
    std::shared_ptr<const RSTransitionEffect> transitionEffect_;

    std::recursive_mutex animationMutex_;
    mutable std::recursive_mutex propertyMutex_;

    bool isOnTheTree_ = false;
    bool isOnTheTreeInit_ = false;

    friend class RSUIDirector;
    friend class RSTransition;
    friend class RSSpringAnimation;
    friend class RSShowingPropertiesFreezer;
    friend class RSPropertyBase;
    friend class RSPropertyAnimation;
    friend class RSPathAnimation;
    friend class RSModifierExtractor;
    friend class RSModifier;
    friend class RSKeyframeAnimation;
    friend class RSInterpolatingSpringAnimation;
    friend class RSImplicitCancelAnimationParam;
    friend class RSImplicitAnimator;
    friend class RSGeometryTransModifier;
    friend class RSExtendedModifier;
    friend class RSCurveAnimation;
    friend class RSAnimation;
    template<typename T>
    friend class RSProperty;
    template<typename T>
    friend class RSAnimatableProperty;
    friend class RSInteractiveImplictAnimator;
};
// backward compatibility
using RSBaseNode = RSNode;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
