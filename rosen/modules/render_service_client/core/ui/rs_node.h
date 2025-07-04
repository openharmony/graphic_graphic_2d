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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_node.h
 *
 * @brief Defines the properties and methods for RSNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H

#include <optional>
#include <unordered_map>

#include "recording/recording_canvas.h"
#include "ui_effect/effect/include/background_color_effect_para.h"
#include "ui_effect/effect/include/border_light_effect_para.h"
#include "ui_effect/effect/include/visual_effect.h"
#include "ui_effect/filter/include/filter.h"
#include "ui_effect/filter/include/filter_blur_para.h"
#include "ui_effect/filter/include/filter_distort_para.h"
#include "ui_effect/filter/include/filter_fly_out_para.h"
#include "ui_effect/filter/include/filter_hdr_para.h"
#include "ui_effect/filter/include/filter_pixel_stretch_para.h"
#include "ui_effect/filter/include/filter_radius_gradient_blur_para.h"
#include "ui_effect/filter/include/filter_water_ripple_para.h"

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
#include "modifier_ng/rs_modifier_ng_type.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "transaction/rs_sync_transaction_handler.h"
#include "transaction/rs_transaction_handler.h"

namespace OHOS {
namespace Rosen {
using DrawFunc = std::function<void(std::shared_ptr<Drawing::Canvas>)>;
using PropertyCallback = std::function<void()>;
using BoundsChangedCallback = std::function<void (const Rosen::Vector4f&)>;
using ExportTypeChangedCallback = std::function<void(bool)>;
using DrawNodeChangeCallback = std::function<void(std::shared_ptr<RSNode> rsNode, bool isPositionZ)>;
using PropertyNodeChangeCallback = std::function<void()>;
class RSAnimation;
class RSCommand;
class RSImplicitAnimParam;
class RSImplicitAnimator;
class RSModifier;
class RSObjAbsGeometry;
class RSUIContext;
class RSUIFilter;
class RSNGFilterBase;
class RSNGShaderBase;
enum class CancelAnimationStatus;

namespace ModifierNG {
class RSModifier;
class RSCustomModifier;
class RSForegroundFilterModifier;
class RSBackgroundFilterModifier;
enum class RSModifierType : uint16_t;
}
/**
 * @class RSNode
 *
 * @brief The class for RSNode(render service node).
 */
class RSC_EXPORT RSNode : public std::enable_shared_from_this<RSNode> {
public:
    using WeakPtr = std::weak_ptr<RSNode>;
    using SharedPtr = std::shared_ptr<RSNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::RS_NODE;
    /**
     * @brief Get the type of the RSNode.
     *
     * @return The type of the RSNode.
     */
    virtual RSUINodeType GetType() const
    {
        return Type;
    }

    RSNode(const RSNode&) = delete;
    RSNode(const RSNode&&) = delete;
    RSNode& operator=(const RSNode&) = delete;
    RSNode& operator=(const RSNode&&) = delete;

    /**
     * @brief Destructor for RSNode.
     */
    virtual ~RSNode();

    /*
    * <important>
    * If you want to add a draw interface to RSNode, decide whether to set the draw node type, otherwise,
    * RSNode will be removed because there is no draw properties.
    */
    void SetDrawNodeType(DrawNodeType nodeType);

    // this id is ONLY used in hierarchy operation commands, this may differ from id_ when the node is a proxy node.
    virtual NodeId GetHierarchyCommandNodeId() const
    {
        return id_;
    }

    /**
     * @brief Adds a child node to the current node at the specified index.
     *
     * @param child The shared pointer to the child node to be added.
     * @param index The position at which the child node should be inserted.
     *              If the index is -1 (default), the child is added to the end.
     */
    virtual void AddChild(SharedPtr child, int index = -1);

    bool AddCompositeNodeChild(SharedPtr node, int index);

    /**
     * @brief Moves a child node to a new index within the current node.
     *
     * @param child The shared pointer to the child node to be moved.
     * @param index The new position for the child node.
     */
    void MoveChild(SharedPtr child, int index);

    /**
     * @brief Removes a child node from the current node.
     *
     * @param child The shared pointer to the child node to be removed.
     */
    virtual void RemoveChild(SharedPtr child);
    void RemoveChildByNodeSelf(WeakPtr child);

    /**
     * @brief Removes the current node from the node tree
     */
    void RemoveFromTree();

    /**
     * @brief Removes all child nodes associated with this node.
     */
    virtual void ClearChildren();
    const std::vector<WeakPtr>& GetChildren() const
    {
        return children_;
    }
    // ONLY support index in [0, childrenTotal) or index = -1, otherwise return std::nullopt
    RSNode::SharedPtr GetChildByIndex(int index) const;

    /**
     * @brief Adds a child node to the current node.
     *
     * Only used as: the child is under multiple parents(e.g. a window cross multi-screens)
     *
     * @param child The shared pointer to the child node to be added.
     * @param index The position at which the child node should be inserted.
     */
    void AddCrossParentChild(SharedPtr child, int index);
    void RemoveCrossParentChild(SharedPtr child, SharedPtr newParent);

    /**
     * @brief Sets whether the node is a cross-screen node.
     *
     * @param isCrossNode Indicates whether the node is a cross-screen node.
     */
    void SetIsCrossNode(bool isCrossNode);

    // PC extend screen use this
    void AddCrossScreenChild(SharedPtr child, int index, bool autoClearCloneNode = false);
    void RemoveCrossScreenChild(SharedPtr child);

    /**
     * @brief Gets the unique identifier of this node.
     *
     * @return The NodeId associated with this node.
     */
    NodeId GetId() const
    {
        return id_;
    }

    virtual FollowType GetFollowType() const
    {
        return FollowType::NONE;
    }

    /**
     * @brief Checks if the current node is an instance of the specified RSUINodeType.
     *
     * This method determines whether the node's type matches the provided type.
     *
     * @param type The RSUINodeType to check against.
     * @return true if the node is an instance of the specified type; false otherwise.
     */
    bool IsInstanceOf(RSUINodeType type) const;

    /**
     * @brief Checks if the current node is an instance of the specified template type.
     *
     * @return true if the node is an instance of the specified template type; false otherwise.
     */
    template<typename T>
    RSC_EXPORT bool IsInstanceOf() const;

    /**
     * @brief Safely casts a shared pointer of RSNode to a shared pointer of the specified type T.
     *
     * type-safe reinterpret_cast
     *
     * @param node The shared pointer to RSNode to be cast.
     * @return std::shared_ptr<T> The casted shared pointer if successful, or nullptr if the input is null.
     */
    template<typename T>
    static std::shared_ptr<T> ReinterpretCast(const std::shared_ptr<RSNode>& node)
    {
        return node ? node->ReinterpretCastTo<T>() : nullptr;
    }

    /**
     * @brief Casts this object to a shared pointer of type T.
     *
     * @return std::shared_ptr<T> A shared pointer of type T if the cast is valid; nullptr otherwise.
     */
    template<typename T>
    std::shared_ptr<T> ReinterpretCastTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }

    /**
     * @brief Casts this object to a shared pointer of the specified type T.
     *
     * @return std::shared_ptr<const T> A shared pointer to the object as type T if the cast is valid, nullptr otherwise.
     */
    template<typename T>
    std::shared_ptr<const T> ReinterpretCastTo() const
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<const T>(shared_from_this()) : nullptr;
    }

    /**
     * @brief Dumps the tree information into the provided output string.
     *
     * @param depth The current depth in the tree structure.
     * @param out The string to which the dump information will be appended.
     */
    void DumpTree(int depth, std::string& out) const;

    /**
     * @brief Dumps the information of current node into the provided output string.
     *
     * @param out The string to which the dump information will be appended.
     */
    virtual void Dump(std::string& out) const;

    /**
     * @brief Dumps the information of current node into a string.
     *
     * @param depth The current depth in the tree structure.
     * @return std::string A string containing the dump information of the node.
     */
    virtual std::string DumpNode(int depth) const;

    /**
     * @brief Retrieves the parent node of the current node.
     *
     * @return SharedPtr A smart pointer to the parent node, or nullptr if this node has no parent.
     */
    SharedPtr GetParent();

    /**
     * @brief Set the id of the current node.
     *
     * @param id The NodeId to be set.
     */
    void SetId(const NodeId& id)
    {
        id_ = id;
    }

    /**
     * @brief Checks whether the UniRender feature is enabled for this node.
     *
     * @return true if UniRender is enabled; false otherwise.
     */
    bool IsUniRenderEnabled() const;
    /**
     * @brief Checks if the current node is a Render Service node.
     *
     * @return true if the node is a Render Service node; false otherwise.
     */
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
    static CancelAnimationStatus CloseImplicitCancelAnimationReturnStatus(
        const std::shared_ptr<RSUIContext> rsUIContext = nullptr);
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

    /**
     * @brief Retrieves the motion path option associated with this node.
     *
     * @return A std::shared_ptr to the RSMotionPathOption instance if set; otherwise, nullptr.
     */
    const std::shared_ptr<RSMotionPathOption> GetMotionPathOption() const;

    void SetMotionPathOptionToProperty(
        const ModifierNG::RSModifierType& modifierType, const ModifierNG::RSPropertyType& propertyType);

    /**
     * @brief Draws on the node using the specified modifier type and drawing function.
     *
     *  [PLANNING]: support SurfaceNode
     *
     * @param type The type of modifier to be applied during the drawing process.
     * @param func The drawing function to be executed on the node.
     */
    virtual void DrawOnNode(RSModifierType type, DrawFunc func) {}

    /**
     * @brief Retrieves the staging properties associated with this node.
     *
     * @return A constant reference to the RSModifierExtractor holding the staging properties.
     */
    const RSModifierExtractor& GetStagingProperties() const;

    const RSShowingPropertiesFreezer& GetShowingProperties() const;

    /**
     * @brief Sets the bounds of the node.
     *
     * The bounds typically define the position and size of the node within its parent.
     *
     * @param bounds A Vector4f representing the new bounds (X, Y, width, height).
     */
    virtual void SetBounds(const Vector4f& bounds);

    /**
     * @brief Sets the bounds of the node.
     *
     * @param positionX The X coordinate of the new bounds.
     * @param positionY The Y coordinate of the new bounds.
     * @param width The width of the new bounds.
     * @param height The height of the new bounds.
     */
    virtual void SetBounds(float positionX, float positionY, float width, float height);

    /**
     * @brief Sets the width of bounds.
     *
     * @param width The new width to set for the bounds.
     */
    virtual void SetBoundsWidth(float width);

    /**
     * @brief Sets the height of bounds.
     *
     * @param height The new height to set for the bounds.
     */
    virtual void SetBoundsHeight(float height);

    /**
     * @brief Sets the frame of the node.
     *
     * @param frame A Vector4f representing the new frame, containing values for x, y, width, and height.
     */
    virtual void SetFrame(const Vector4f& frame);

    /**
     * @brief Sets the frame of the node.
     *
     * @param positionX The X coordinate of the frame.
     * @param positionY The Y coordinate of the frame.
     * @param width The width of the frame.
     * @param height The height of the frame.
     */
    virtual void SetFrame(float positionX, float positionY, float width, float height);

    /**
     * @brief Sets the x-coordinate of the node's frame.
     *
     * @param positionX The x-coordinate of the frame.
     */
    virtual void SetFramePositionX(float positionX);

    /**
     * @brief Sets the y-coordinate of the node's frame.
     *
     * @param positionY The y-coordinate of the frame.
     */
    virtual void SetFramePositionY(float positionY);

    /**
     * @brief Freezes current frame data when enabled, preventing content refresh until unfrozen.
     *
     * This property is valid only for CanvasNode and SurfaceNode in uniRender.
     *
     * @param isFreeze Freeze state flag
     *                - true: Freeze current frame content
     *                - false: Resume dynamic updates
     * @see RSCanvasNode::SetFreeze(bool isFreeze)
     * @see RSSurfaceNode::SetFreeze(bool isFreeze)
     */
    virtual void SetFreeze(bool isFreeze);

    /**
     * @brief Sets the name of the node.
     *
     * @param nodeName The new name to assign to the node.
     */
    void SetNodeName(const std::string& nodeName);

    /**
     * @brief Sets the sandbox of the node.
     *
     * @param parentPosition The position of the parent node in the sandbox.
     */
    void SetSandBox(std::optional<Vector2f> parentPosition);

    /**
     * @brief Sets the Z position of the node.
     *
     * @param positionZ The new Z position value for the node.
     */
    void SetPositionZ(float positionZ);

    /**
     * @brief Sets whether the Z position is applicable for 3D cameras.
     *
     * @param isApplicable If true, the Z position will be considered for 3D cameras; otherwise, it will not.
     */
    void SetPositionZApplicableCamera3D(bool isApplicable);

    /**
     * @brief Sets the pivot point of the node.
     *
     * The pivot point is used as the reference point for transformations such as rotation and scaling.
     *
     * @param pivot A Vector2f representing the pivot point (X, Y).
     */
    void SetPivot(const Vector2f& pivot);

    /**
     * @brief Sets the pivot point of the node.
     *
     * @param pivotX The X coordinate of the pivot point.
     * @param pivotY The Y coordinate of the pivot point.
     */
    void SetPivot(float pivotX, float pivotY);

    /**
     * @brief Sets the pivot point of the node.
     *
     * @param pivotX The X coordinate of the pivot point.
     */
    void SetPivotX(float pivotX);

    /**
     * @brief Sets the pivot point of the node.
     *
     * @param pivotY The Y coordinate of the pivot point.
     */
    void SetPivotY(float pivotY);

    /**
     * @brief Sets the pivot point of the node.
     *
     * @param pivotZ The Z coordinate of the pivot point.
     */
    void SetPivotZ(float pivotZ);

    /**
     * @brief Sets the corner radius of the node.
     *
     * @param cornerRadius The new corner radius value.
     */
    void SetCornerRadius(float cornerRadius);

    /**
     * @brief Sets the corner radius of the node.
     *
     * @param cornerRadius A Vector4f representing the corner radius for each corner (top-left, top-right, bottom-right, bottom-left).
     */
    void SetCornerRadius(const Vector4f& cornerRadius);

    /**
     * @brief Sets the rotation of the node.
     *
     * @param quaternion A Quaternion representing the rotation to be applied to the node.
     */
    void SetRotation(const Quaternion& quaternion);

    /**
     * @brief Sets the rotation of the node.
     *
     * @param degreeX The rotation angle around the X-axis in degrees.
     * @param degreeY The rotation angle around the Y-axis in degrees.
     * @param degreeZ The rotation angle around the Z-axis in degrees.
     */
    void SetRotation(float degreeX, float degreeY, float degreeZ);

    /**
     * @brief Sets the rotation of the node.
     *
     * @param degree The rotation angle in degrees.
     */
    void SetRotation(float degree);

    /**
     * @brief Sets the rotation of the node around the X-axis.
     *
     * @param degree The rotation angle around the X-axis in degrees.
     */
    void SetRotationX(float degree);

    /**
     * @brief Sets the rotation of the node around the Y-axis.
     *
     * @param degree The rotation angle around the Y-axis in degrees.
     */
    void SetRotationY(float degree);
    void SetCameraDistance(float cameraDistance);

    /**
     * @brief Sets the translation vector for this node.
     *
     * @param translate A two-dimensional vector representing the translation distances in the x and y axes.
     */
    void SetTranslate(const Vector2f& translate);

    /**
     * @brief Sets the translation values for the node along the X, Y, and Z axes.
     *
     * @param translateX The translation distance along the X-axis.
     * @param translateY The translation distance along the Y-axis.
     * @param translateZ The translation distance along the Z-axis.
     */
    void SetTranslate(float translateX, float translateY, float translateZ);

    /**
     * @brief Sets the translation value for the node along the X-axis.
     *
     * @param translate The translation distance along the X-axis.
     */
    void SetTranslateX(float translate);

    /**
     * @brief Sets the translation value for the node along the Y-axis.
     *
     * @param translate The translation distance along the Y-axis.
     */
    void SetTranslateY(float translate);

    /**
     * @brief Sets the translation value for the node along the Z-axis.
     *
     * @param translate The translation distance along the Z-axis.
     */
    void SetTranslateZ(float translate);

    /**
     * @brief Sets the scale factor for the node.
     *
     * Greater than 1.0f will scale up, less than 1.0f will scale down.
     *
     * @param scale The scale factor to apply to the node.
     */
    void SetScale(float scale);

    /**
     * @brief Sets the scale factors for the node along the X and Y axes.
     *
     * @param scaleX The scale factor along the X-axis.
     * @param scaleY The scale factor along the Y-axis.
     */
    void SetScale(float scaleX, float scaleY);

    /**
     * @brief Sets the scale factor for this node.
     *
     * @param scale A Vector2f representing the scale factors for the X and Y axes.
     */
    void SetScale(const Vector2f& scale);

    /**
     * @brief Sets the horizontal scaling factor for the node.
     *
     * No scaling will be applied on the Y-axis.
     *
     * @param scaleX The scaling factor to apply on the X-axis.
     */
    void SetScaleX(float scaleX);

    /**
     * @brief Sets the vertical scaling factor for the node.
     *
     * No scaling will be applied on the X-axis.
     *
     * @param scaleY The scaling factor to apply on the Y-axis.
     */
    void SetScaleY(float scaleY);

    /**
     * @brief Sets the scaling factor for the node along the Z-axis.
     *
     * @param scaleZ The scaling factor to apply on the Z-axis.
     */
    void SetScaleZ(float scaleZ);

    /**
     * @brief Sets the skew factor for the node.
     *
     * @param skew The skew factor to apply to the node.
     */
    void SetSkew(float skew);

    /**
     * @brief Sets the skew factors for the node along the X and Y axes.
     *
     * @param skewX The skew factor along the X-axis.
     * @param skewY The skew factor along the Y-axis.
     */
    void SetSkew(float skewX, float skewY);

    /**
     * @brief Sets the skew factors for the node along the X, Y, and Z axes.
     *
     * @param skewX The skew factor along the X-axis.
     * @param skewY The skew factor along the Y-axis.
     * @param skewZ The skew factor along the Z-axis.
     */
    void SetSkew(float skewX, float skewY, float skewZ);

    /**
     * @brief Sets the skew factors for the node.
     *
     * @param skew A Vector3f representing the skew factors for the X, Y, and Z axes.
     */
    void SetSkew(const Vector3f& skew);

    /**
     * @brief Sets the skew factor for the node along the X-axis.
     *
     * @param skewX The skew factor to apply on the X-axis.
     */
    void SetSkewX(float skewX);

    /**
     * @brief Sets the skew factor for the node along the Y-axis.
     *
     * @param skewY The skew factor to apply on the Y-axis.
     */
    void SetSkewY(float skewY);

    /**
     * @brief Sets the skew factor for the node along the Z-axis.
     *
     * @param skewZ The skew factor to apply on the Z-axis.
     */
    void SetSkewZ(float skewZ);

    /**
     * @brief Sets the perspective value for the node.
     *
     * @param persp The perspective value to apply.
     */
    void SetPersp(float persp);

    /**
     * @brief Sets the perspective values for the node.
     *
     * @param perspX The perspective value along the X-axis.
     * @param perspY The perspective value along the Y-axis.
     */
    void SetPersp(float perspX, float perspY);

    /**
     * @brief Sets the perspective values for the node.
     *
     * @param perspX The perspective value along the X-axis.
     * @param perspY The perspective value along the Y-axis.
     * @param perspZ The perspective value along the Z-axis.
     * @param perspW The perspective value along the W-axis.
     */
    void SetPersp(float perspX, float perspY, float perspZ, float perspW);

    /**
     * @brief Sets the perspective values for the node.
     *
     * @param persp A Vector4f representing the perspective values for the X, Y, Z, and W axes.
     */
    void SetPersp(const Vector4f& persp);

    /**
     * @brief Sets the perspective value for the node along the X-axis.
     *
     * If the property is not initialized,perspY,perspZ will be set to 0.0f,and perspW will be set to 1.0f.
     *
     * @param perspX The perspective value to apply on the X-axis.
     */
    void SetPerspX(float perspX);

    /**
     * @brief Sets the perspective value for the node along the Y-axis.
     *
     * If the property is not initialized,perspX,perspZ will be set to 0.0f,and perspW will be set to 1.0f.
     *
     * @param perspY The perspective value to apply on the Y-axis.
     */
    void SetPerspY(float perspY);

    /**
     * @brief Sets the perspective value for the node along the Z-axis.
     *
     * If the property is not initialized,perspX,perspY will be set to 0.0f,and perspW will be set to 1.0f.
     *
     * @param perspZ The perspective value to apply on the Z-axis.
     */
    void SetPerspZ(float perspZ);

    /**
     * @brief Sets the perspective value for the node along the W-axis.
     *
     * If the property is not initialized,perspX,perspY,perspZ will be set to 0.0f
     *
     * @param perspW The perspective value to apply on the W-axis.
     */
    void SetPerspW(float perspW);

    /**
     * @brief Sets the alpha value for the node.
     *
     * @param alpha The alpha value to apply (0.0f to 1.0f).
     */
    void SetAlpha(float alpha);

    /**
     * @brief Sets the alpha value for offscreen rendering.
     *
     * @param alphaOffscreen true if offscreen alpha is enabled; false otherwise.
     */
    void SetAlphaOffscreen(bool alphaOffscreen);

    /**
     * @brief Sets the foreground color of environment.
     *
     * @param colorValue The color value to set.
     */
    void SetEnvForegroundColor(uint32_t colorValue);

    /**
     * @brief Sets the foreground color strategy of environment.
     *
     * @param colorType The strategy type to set.
     */
    void SetEnvForegroundColorStrategy(ForegroundColorStrategyType colorType);

    /**
     * @brief Sets the parameter for the particle effect.
     *
     * @param particleParams The parameters for the particle effect.
     * @param finishCallback The callback function to be executed after setting the parameters. Default is nullptr.
     */
    void SetParticleParams(
        std::vector<ParticleParams>& particleParams, const std::function<void()>& finishCallback = nullptr);
    void SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para);
    void SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para);

    /**
     * @brief Sets the foreground color of the node.
     *
     * @param colorValue The color value to set.
     */
    void SetForegroundColor(uint32_t colorValue);

    /**
     * @brief Sets the background color of the node.
     *
     * @param colorValue The color value to set.
     */
    void SetBackgroundColor(uint32_t colorValue);

    /**
     * @brief Sets the background color of the node, support color with different color space.
     *
     * @param color The color to set.
     */
    void SetBackgroundColor(RSColor& color);

    /**
     * @brief Sets the background shader for this node.
     *
     * @param shader A shared pointer to an RSShader object representing the shader to be applied as the background.
     */
    void SetBackgroundShader(const std::shared_ptr<RSShader>& shader);

    /**
     * @brief Sets the background shader progress for this node.
     *
     * @param process The progress value to set for the background shader.
     */
    void SetBackgroundShaderProgress(const float& process);

    /**
     * @brief Sets the background image for this node.
     *
     * @param image A shared pointer to an RSImage object representing the background image.
     */
    void SetBgImage(const std::shared_ptr<RSImage>& image);

    /**
     * @brief Sets the inner rectangle area for the background image.
     *
     * @param innerRect A Vector4f representing the inner rectangle for the background image.
     */
    void SetBgImageInnerRect(const Vector4f& innerRect);

    /**
     * @brief Sets the background image size.
     *
     * @param width The width of the background image.
     * @param height The height of the background image.
     */
    void SetBgImageSize(float width, float height);

    /**
     * @brief Sets the background image width.
     *
     * @param width The width of the background image.
     */
    void SetBgImageWidth(float width);

    /**
     * @brief Sets the background image height.
     *
     * @param height The height of the background image.
     */
    void SetBgImageHeight(float height);

    /**
     * @brief Sets the background image position.
     *
     * @param positionX The X coordinate of the background image position.
     * @param positionY The Y coordinate of the background image position.
     */
    void SetBgImagePosition(float positionX, float positionY);

    /**
     * @brief Sets the background image position along the X-axis.
     *
     * @param positionX The X coordinate of the background image position.
     */
    void SetBgImagePositionX(float positionX);

    /**
     * @brief Sets the background image position along the Y-axis.
     *
     * @param positionY The Y coordinate of the background image position.
     */
    void SetBgImagePositionY(float positionY);

    /**
     * @brief Sets the border color of the node.
     *
     * @param colorValue The color value to set for the border.
     */
    void SetBorderColor(uint32_t colorValue);

    /**
     * @brief Sets the border color of the node.
     *
     * @param left The color value for the left border.
     * @param top The color value for the top border.
     * @param right The color value for the right border.
     * @param bottom The color value for the bottom border.
     */
    void SetBorderColor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

    /**
     * @brief Sets the border color of the node.
     *
     * @param color A Vector4f representing the border color for each side.
     */
    void SetBorderColor(const Vector4<Color>& color);

    /**
     * @brief Sets the border width of the node.
     *
     * @param width The width value to set for the border.
     */
    void SetBorderWidth(float width);

    /**
     * @brief Sets the border width of the node.
     *
     * @param left The width value for the left border.
     * @param top The width value for the top border.
     * @param right The width value for the right border.
     * @param bottom The width value for the bottom border.
     */
    void SetBorderWidth(float left, float top, float right, float bottom);

    /**
     * @brief Sets the border width of the node.
     *
     * @param width A Vector4f representing the border width for each side (left, top, right, bottom).
     */
    void SetBorderWidth(const Vector4f& width);

    /**
     * @brief Sets the border style of the node.
     *
     * @param styleValue The style value to set for the border.
     */
    void SetBorderStyle(uint32_t styleValue);

    /**
     * @brief Sets the border style of the node.
     *
     * @param left The style value for the left border.
     * @param top The style value for the top border.
     * @param right The style value for the right border.
     * @param bottom The style value for the bottom border.
     */
    void SetBorderStyle(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

    /**
     * @brief Sets the border style of the node.
     *
     * @param style Indicates border style for each side (left, top, right, bottom).
     */
    void SetBorderStyle(const Vector4<BorderStyle>& style);

    /**
     * @brief Sets the dash width of the border.
     *
     * @param dashWidth Indicates widths of the dash segments for the border.
     */
    void SetBorderDashWidth(const Vector4f& dashWidth);

    /**
     * @brief Sets the dash gap of the border.
     *
     * @param dashGap Indicates gaps between the dash segments for the border.
     */
    void SetBorderDashGap(const Vector4f& dashGap);

    /**
     * @brief Sets the color of the outer border.
     *
     * @param color Indicates outer border color for each side.
     */
    void SetOuterBorderColor(const Vector4<Color>& color);

    /**
     * @brief Sets the width of the outer border.
     *
     * @param width Indicates outer border width for each side.
     */
    void SetOuterBorderWidth(const Vector4f& width);

    /**
     * @brief Sets the style of the outer border.
     *
     * @param style Indicates outer border style for each side.
     */
    void SetOuterBorderStyle(const Vector4<BorderStyle>& style);

    /**
     * @brief Sets the radius of the outer border.
     *
     * @param radius Indicates radius for each side of outer border.
     */
    void SetOuterBorderRadius(const Vector4f& radius);

    /**
     * @brief Sets the outline color of the node.
     *
     * @param color Indicates outline color,each color contains rgb and alpha.
     */
    void SetOutlineColor(const Vector4<Color>& color);

    /**
     * @brief Sets the outline width of the node.
     *
     * @param width Indicates outline width for each side.
     */
    void SetOutlineWidth(const Vector4f& width);

    /**
     * @brief Sets the outline style of the node.
     *
     * @param style Indicates values to be used as the outline style.
     */
    void SetOutlineStyle(const Vector4<BorderStyle>& style);

    /**
     * @brief Sets the dash width for the outline of the node.
     *
     * @param dashWidth Indicates widths of the dash segments for the outline.
     */
    void SetOutlineDashWidth(const Vector4f& dashWidth);

    /**
     * @brief Sets the dash gap for the outline of the node.
     *
     * @param dashGap Indicates gaps between the dash segments for the outline.
     */
    void SetOutlineDashGap(const Vector4f& dashGap);

    /**
     * @brief Sets the outline radius of the node.
     *
     * @param radius Indicates outline radius for each side.
     */
    void SetOutlineRadius(const Vector4f& radius);

    // UIEffect
    /**
     * @brief Sets the background filter for the UI.
     *
     * @param backgroundFilter Pointer to a Filter that defines the background filter effect.
     */
    void SetUIBackgroundFilter(const OHOS::Rosen::Filter* backgroundFilter);

    /**
     * @brief Sets the compositing filter for the UI.
     *
     * @param compositingFilter Pointer to a Filter that defines the compositing filter effect.
     */
    void SetUICompositingFilter(const OHOS::Rosen::Filter* compositingFilter);

    /**
     * @brief Sets the foreground filter for the UI.
     *
     * @param foregroundFilter Pointer to a Filter that defines the foreground filter effect.
     */
    void SetUIForegroundFilter(const OHOS::Rosen::Filter* foregroundFilter);

    /**
     * @brief Sets the visual effect for the UI.
     *
     * @param visualEffect Pointer to a VisualEffect that defines the visual effect.
     */
    void SetVisualEffect(const VisualEffect* visualEffect);

    void SetBackgroundUIFilter(const std::shared_ptr<RSUIFilter> backgroundFilter);
    void SetForegroundUIFilter(const std::shared_ptr<RSUIFilter> foregroundFilter);

    /**
     * @brief Sets the foreground effect radius.
     *
     * @param blurRadius Indicates radius of the foreground effect.
     */
    void SetForegroundEffectRadius(const float blurRadius);

    /**
     * @brief Sets the background effect radius.
     *
     * @param backgroundFilter Indicates the background filter to be applied.
     */
    void SetBackgroundFilter(const std::shared_ptr<RSFilter>& backgroundFilter);

    /**
     * @brief Sets the background filter.
     *
     * @param backgroundFilter Indicates the background filter to be applied.
     */
    void SetBackgroundNGFilter(const std::shared_ptr<RSNGFilterBase>& backgroundFilter);

    /**
     * @brief Sets the foreground filter.
     *
     * @param foregroundFilter Indicates the foreground filter to be applied.
     */
    void SetForegroundNGFilter(const std::shared_ptr<RSNGFilterBase>& foregroundFilter);

    /**
     * @brief Sets the background shader.
     *
     * @param backgroundShader Indicates the background shader to be applied.
     */
    void SetBackgroundNGShader(const std::shared_ptr<RSNGShaderBase>& backgroundShader);

    /**
     * @brief Sets the foreground shader.
     *
     * @param foregroundShader Indicates the foreground shader to be applied.
     */
    void SetForegroundShader(const std::shared_ptr<RSNGShaderBase>& foregroundShader);

    /**
     * @brief Sets the filter.
     *
     * @param filter Indicates the filter to be applied.
     */
    void SetFilter(const std::shared_ptr<RSFilter>& filter);

    /**
     * @brief Sets the parameters for linear gradient blur.
     *
     * @param para Indicates the parameters for linear gradient blur.
     */
    void SetLinearGradientBlurPara(const std::shared_ptr<RSLinearGradientBlurPara>& para);

    /**
     * @brief Sets the parameters for motion blur.
     *
     * @param radius Indicates the radius for motion blur.
     * @param anchor Indicates the anchor for motion blur.
     */
    void SetMotionBlurPara(const float radius, const Vector2f& anchor);

    /**
     * @brief Sets the parameters for magnifier.
     *
     * @param para Indicates the parameters for magnifier.
     */
    void SetMagnifierParams(const std::shared_ptr<RSMagnifierParams>& para);

    /**
     * @brief Sets the rate for dynamic light.
     *
     * @param rate Indicates the rate of dynamic light.
     */
    void SetDynamicLightUpRate(const float rate);

    /**
     * @brief Sets the degree of dynamic light up.
     *
     * @param lightUpDegree Indicates the degree of dynamic light up.
     */
    void SetDynamicLightUpDegree(const float lightUpDegree);
    void SetDynamicDimDegree(const float dimDegree);

    /**
     * @brief Sets the Blender.
     *
     * @param blender Pointer to a Blender object.
     */
    void SetBlender(const Blender* blender);

    /**
     * @brief Sets the parameters for foreground brightness.
     *
     * @param params Indicates the parameters for foreground brightness.
     */
    void SetFgBrightnessParams(const RSDynamicBrightnessPara& params);

    /**
     * @brief Sets the rates for foreround brightness.
     *
     * @param rates Indicates the rates for foreground brightness.
     */
    void SetFgBrightnessRates(const Vector4f& rates);

    /**
     * @brief Sets the saturation for foreground brightness.
     *
     * @param saturation Indicates the saturation for foreground brightness.
     */
    void SetFgBrightnessSaturation(const float& saturation);

    /**
     * @brief Sets the positive coefficient for foreground brightness.
     *
     * @param coeff Indicates the positive coefficient for foreground brightness.
     */
    void SetFgBrightnessPosCoeff(const Vector4f& coeff);

    /**
     * @brief Sets the negative coefficient for foreground brightness.
     *
     * @param coeff Indicates the negative coefficient for foreground brightness.
     */
    void SetFgBrightnessNegCoeff(const Vector4f& coeff);
    void SetFgBrightnessFract(const float& fract);

    /**
     * @brief Sets the hdr using for foreground brightness.
     *
     * @param hdr Indicates the hdr using for foreground brightness.
     */
    void SetFgBrightnessHdr(const bool hdr);

    /**
     * @brief Sets the parameters for background brightness.
     *
     * @param params Indicates the parameters for background brightness.
     */
    void SetBgBrightnessParams(const RSDynamicBrightnessPara& params);

    /**
     * @brief Sets the rates for background brightness.
     *
     * @param rates Indicates the rates for background brightness.
     */
    void SetBgBrightnessRates(const Vector4f& rates);

    /**
     * @brief Sets the saturation for background brightness.
     *
     * @param saturation Indicates the saturation for background brightness.
     */
    void SetBgBrightnessSaturation(const float& saturation);

    /**
     * @brief Sets the positive coefficient for background brightness.
     *
     * @param coeff Indicates the positive coefficient for background brightness.
     */
    void SetBgBrightnessPosCoeff(const Vector4f& coeff);

    /**
     * @brief Sets the negative coefficient for background brightness.
     *
     * @param coeff Indicates the negative coefficient for background brightness.
     */
    void SetBgBrightnessNegCoeff(const Vector4f& coeff);
    void SetBgBrightnessFract(const float& fract);
    void SetBorderLightShader(std::shared_ptr<VisualEffectPara> visualEffectPara);


    /**
     * @brief Sets the grey coefficient.
     *
     * @param greyCoef Indicates the grey coefficient.
     */
    void SetGreyCoef(const Vector2f greyCoef);

    /**
     * @brief Sets the compositing filter.
     *
     * @param compositingFilter Pointer to a Filter that defines the compositing filter effect.
     */
    void SetCompositingFilter(const std::shared_ptr<RSFilter>& compositingFilter);

    /**
     * @brief Sets the shadow color.
     *
     * @param colorValue Indicates the color value to be set.
     */
    void SetShadowColor(uint32_t colorValue);

    /**
     * @brief Sets the offset of the shadow.
     *
     * @param offsetX Indicates the offset value along the X-axis.
     * @param offsetY Indicates the offset value along the Y-axis.
     */
    void SetShadowOffset(float offsetX, float offsetY);

    /**
     * @brief Sets the shadow offset along the X-axis.
     *
     * @param offsetX Indicates the offset value along the X-axis.
     */
    void SetShadowOffsetX(float offsetX);

    /**
     * @brief Sets the shadow offset along the Y-axis.
     *
     * @param offsetY Indicates the offset value along the Y-axis.
     */
    void SetShadowOffsetY(float offsetY);

    /**
     * @brief Sets the alpha of the shadow
     *
     * @param alpha Indicates the alpha value to be set.
     */
    void SetShadowAlpha(float alpha);

    /**
     * @brief Sets the elevation of the shadow.
     *
     * @param elevation Indicates the elevation value to be set.
     */
    void SetShadowElevation(float elevation);

    /**
     * @brief Sets the radius of the shadow.
     *
     * @param radius Indicates the radius value to be set.
     */
    void SetShadowRadius(float radius);

    /**
     * @brief Sets the path of the shadow.
     *
     * @param shadowPath Indicates the path of the shadow.
     */
    void SetShadowPath(const std::shared_ptr<RSPath>& shadowPath);

    /**
     * @brief Sets whether a shadow mask should be applied to this node.
     *
     * @param shadowMask Indicates whether to enable (true) or disable (false) the shadow mask.
     */
    void SetShadowMask(bool shadowMask);

    /**
     * @brief Sets the strategy of the shadow mask.
     *
     * @param strategy Indicates the strategy of the shadow mask.
     */
    void SetShadowMaskStrategy(SHADOW_MASK_STRATEGY strategy);

    /**
     * @brief Sets whether the shadow should be filled.
     *
     * @param shadowIsFilled Indicates whether the shadow is filled (true) or not (false).
     */
    void SetShadowIsFilled(bool shadowIsFilled);

    /**
     * @brief Sets the color strategy of the shadow.
     *
     * @param shadowColorStrategy Indicates the color strategy of the shadow.
     */
    void SetShadowColorStrategy(int shadowColorStrategy);

    /**
     * @brief Sets the gravity for the frame of this node.
     *
     * @param gravity The gravity value to apply.
     */
    void SetFrameGravity(Gravity gravity);

    /**
     * @brief Sets a rounded rectangle clipping region for the node.
     *
     * @param clipRect The bounds of the clipping rectangle,represented as (x, y, width, height).
     * @param clipRadius The radii for the rectangle's corners,represented as (topLeft, topRight, bottomRight, bottomLeft).
     */
    void SetClipRRect(const Vector4f& clipRect, const Vector4f& clipRadius);

    /**
     * @brief Sets the clipping region of the node to the specified rounded rectangle.
     *
     * @param rrect A shared pointer to an RRect object that defines the clipping region.
     */
    void SetClipRRect(const std::shared_ptr<RRect>& rrect);

    /**
     * @brief Sets the clipping bounds for this node using the specified path.
     *
     * @param clipToBounds A shared pointer to an RSPath object that defines the clipping region.
     */
    void SetClipBounds(const std::shared_ptr<RSPath>& clipToBounds);

    /**
     * @brief Sets whether the content of this node should be clipped to its bounds.
     *
     * @param clipToBounds True to enable clipping to bounds; false to disable.
     */
    void SetClipToBounds(bool clipToBounds);

    /**
     * @brief Sets whether the content of this node should be clipped to its frame.
     *
     * @param clipToFrame True to enable clipping to frame; false to disable.
     */
    void SetClipToFrame(bool clipToFrame);

    /**
     * @brief Sets custom clipping rectangle.
     *
     * @param clipRect A Vector4f representing the clipping rectangle (x, y, width, height).
     */
    void SetCustomClipToFrame(const Vector4f& clipRect);

    /**
     * @brief Sets the brightness of HDR (High Dynamic Range).
     *
     * @param hdrBrightness The HDR brightness value to set.
     */
    void SetHDRBrightness(const float& hdrBrightness);

    /**
     * @brief Sets the HDR brightness factor to display node.
     *
     * @param factor The HDR brightness factor to set.
     */
    void SetHDRBrightnessFactor(float factor);

    /**
     * @brief Sets the visibility of the node.
     *
     * @param visible True to make the node visible; false to hide it.
     */
    void SetVisible(bool visible);
    void SetMask(const std::shared_ptr<RSMask>& mask);

    /**
     * @brief Sets the spherize degree.
     *
     * @param spherizeDegree The degree of spherization to apply.
     */
    void SetSpherizeDegree(float spherizeDegree);

    /**
     * @brief Sets the brightness ratio of HDR UI component.
     *
     * @param hdrUIBrightness The HDR UI component brightness ratio.
     */
    void SetHDRUIBrightness(float hdrUIBrightness);

    /**
     * @brief Sets the degree of light up effect.
     *
     * @param lightUpEffectDegree The degree of the light up effect to apply.
     */
    void SetLightUpEffectDegree(float lightUpEffectDegree);

    void SetAttractionEffect(float fraction, const Vector2f& destinationPoint);
    void SetAttractionEffectFraction(float fraction);
    void SetAttractionEffectDstPoint(const Vector2f& destinationPoint);

    void SetPixelStretch(const Vector4f& stretchSize, Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP);
    void SetPixelStretchPercent(const Vector4f& stretchPercent,
        Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP);

    void SetWaterRippleParams(const RSWaterRipplePara& params, float progress);
    void SetFlyOutParams(const RSFlyOutPara& params, float degree);

    void SetDistortionK(const float distortionK);

    /**
     * @brief Sets the paint order for the node.
     *
     * @param drawContentLast Indicates whether to draw the content of the node last.
     */
    void SetPaintOrder(bool drawContentLast);

    void SetTransitionEffect(const std::shared_ptr<const RSTransitionEffect>& effect)
    {
        transitionEffect_ = effect;
    }

    void SetUseEffect(bool useEffect);
    void SetUseEffectType(UseEffectType useEffectType);
    void SetAlwaysSnapshot(bool enable);

    void SetEnableHDREffect(uint32_t type, bool enableHdrEffect);

    void SetUseShadowBatching(bool useShadowBatching);

    void SetColorBlendMode(RSColorBlendMode colorBlendMode);

    void SetColorBlendApplyType(RSColorBlendApplyType colorBlendApplyType);

    // driven render was shelved, functions will be deleted soon [start]
    void MarkDrivenRender(bool flag) {}
    void MarkDrivenRenderItemIndex(int index) {}
    void MarkDrivenRenderFramePaintState(bool flag) {}
    void MarkContentChanged(bool isChanged) {}
    // driven render was shelved, functions will be deleted soon [end]

#if defined(MODIFIER_NG)
    void AddModifier(const std::shared_ptr<ModifierNG::RSModifier> modifier);

    void RemoveModifier(const std::shared_ptr<ModifierNG::RSModifier> modifier);
#else
    /**
     * @brief Adds a modifier to the current node.
     *
     * If the specific modifier is already exist, it will not be added again.
     *
     * @param modifier A shared pointer to the RSModifier to be added.
     */
    void AddModifier(const std::shared_ptr<RSModifier> modifier);

    /**
     * @brief Removes a modifier from the current node.
     *
     * @param modifier A shared pointer to the RSModifier to be removed.
     */
    void RemoveModifier(const std::shared_ptr<RSModifier> modifier);
#endif

    const std::shared_ptr<ModifierNG::RSModifier> GetModifierByType(const ModifierNG::RSModifierType& type);

    /**
     * @brief Sets whether the node is a custom text type.
     *
     * @param isCustomTextType true if the text type is custom; false otherwise.
     */
    void SetIsCustomTextType(bool isCustomTextType);

    /**
     * @brief Gets whether the node is a custom text type.
     *
     * @return true if the text type is custom; false otherwise.
     */
    bool GetIsCustomTextType();

    void SetIsCustomTypeface(bool isCustomTypeface);

    bool GetIsCustomTypeface();

    /**
     * @brief Sets the drawing region for the node.
     *
     * @param rect Indicates a rectF object representing the drawing region.
     */
    void SetDrawRegion(std::shared_ptr<RectF> rect);

    /**
     * @brief Sets if need use the cmdlist drawing region for the node.
     *
     * @param needUseCmdlistDrawRegion Whether to need use the cmdlist drawing region for this node.
     */
    void SetNeedUseCmdlistDrawRegion(bool needUseCmdlistDrawRegion);

    /**
     * @brief Mark the node as a group node for rendering pipeline optimization
     *
     * NodeGroup generates off-screen cache containing this node and its entire subtree
     * Off-screen cache may exhibit conflicts between background effects
     * When using node group optimization, be aware of rendering effect limitations:
     *     - Avoid co-occurrence of surface capture, blur, brightness adjustment and blend operations
     *     - These effects may produce visual artifacts or performance degradation
     *
     * @param isNodeGroup       Whether to enable group rendering optimization for this node
     * @param isForced          When true, forces group marking ignoring system property checks
     * @param includeProperty   When true, packages node properties with the group
     */
    void MarkNodeGroup(bool isNodeGroup, bool isForced = true, bool includeProperty = false);

    // Mark opinc node
    void MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate = false);
    // will be abandoned
    void MarkUifirstNode(bool isUifirstNode);
    // Mark uifirst leash node
    void MarkUifirstNode(bool isForceFlag, bool isUifirstEnable);

    void SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch);

    void MarkNodeSingleFrameComposer(bool isNodeSingleFrameComposer);

    void MarkRepaintBoundary(const std::string& tag);
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

    /**
     * @brief Sets the id of the instance.
     *
     * @param instanceId The id of the instance to set.
     */
    void SetInstanceId(int32_t instanceId);

    /**
     * @brief Gets the id of the instance.
     *
     * @return The id of the instance.
     */
    int32_t GetInstanceId() const
    {
        return instanceId_;
    }

    /**
     * @brief Get the name of the node.
     *
     * @return The name of the node as a std::string.
     */
    const std::string GetNodeName() const
    {
        return nodeName_;
    }

    static DrawNodeChangeCallback drawNodeChangeCallback_;
    static void SetDrawNodeChangeCallback(DrawNodeChangeCallback callback);
    static PropertyNodeChangeCallback propertyNodeChangeCallback_;
    /**
     * @brief Sets the callback function for property node change events
     *
     * @param callback Function pointer to the callback handler
     */
    static void SetPropertyNodeChangeCallback(PropertyNodeChangeCallback callback);

    /**
     * @brief Enables or disables property node change callback notifications
     *
     * @param needCallback Boolean flag to control callback triggering
     */
    static void SetNeedCallbackNodeChange(bool needCallback);
    bool GetIsDrawn();
    void SetDrawNode();
    DrawNodeType GetDrawNodeType() const;
    void SyncDrawNodeType(DrawNodeType nodeType);

    const std::shared_ptr<RSPropertyBase> GetPropertyById(const PropertyId& propertyId);
    const std::shared_ptr<RSPropertyBase> GetPropertyByType(
        const ModifierNG::RSModifierType& modifierType, const ModifierNG::RSPropertyType& propertyType);

    /**
     * @brief Gets the context for the RSUI.
     *
     * @return A shared pointer to the RSUIContext object.
     */
    std::shared_ptr<RSUIContext> GetRSUIContext()
    {
        return rsUIContext_.lock();
    }
    void SetUIContextToken();

    /**
     * @brief Sets the context for the RSUI.
     *
     * @param rsUIContext A shared pointer to the RSUIContext object.
     */
    void SetRSUIContext(std::shared_ptr<RSUIContext> rsUIContext);

    /**
     * @brief Sets whether to skip check in multi-instance.
     *
     * @param isSkipCheckInMultiInstance true to skip check; false otherwise.
     */
    void SetSkipCheckInMultiInstance(bool isSkipCheckInMultiInstance);

#ifdef RS_ENABLE_VK
    /**
     * @brief Gets whether the canvas enables hybrid rendering.
     *
     * @return true if hybrid rendering is enabled; false otherwise.
     */
    bool IsHybridRenderCanvas() const
    {
        return hybridRenderCanvas_;
    }

    /**
     * @brief Sets whether the canvas enables hybrid rendering.
     *
     * @param hybridRenderCanvas true to enable hybrid rendering; false otherwise.
     */
    virtual void SetHybridRenderCanvas(bool hybridRenderCanvas) {};
#endif

    /**
     * @brief Gets whether the node is on the tree.
     *
     * @return true if the node is on the tree; false otherwise.
     */
    bool GetIsOnTheTree()
    {
        return isOnTheTree_;
    }

    /**
     * @brief Enables/disables control-level occlusion culling for the node's subtree
     *
     * When enabled, this node and its entire subtree will participate in
     * control-level occlusion culling. The specified key occlusion node acts as the primary
     * occluder within the subtree.
     *
     * @param enable
     *     - true: Enables occlusion culling for this node and its subtree
     *     - false: Disables occlusion culling for this node and its subtree
     * @param keyOcclusionNodeId
     *     The ID of the key occluding node within the subtree. This node will be treated as
     *     the primary occlusion source when determining visibility of other nodes in the subtree.
     *     Pass INVALID_NODE_ID if no specific occluder is designated.
     */
    void UpdateOcclusionCullingStatus(bool enable, NodeId keyOcclusionNodeId);

protected:
    explicit RSNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr,
        bool isOnTheTree = false);
    explicit RSNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr, bool isOnTheTree = false);

    void DumpModifiers(std::string& out) const;

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

    /**
     * @brief Called when child nodes are added to this node.
     */
    virtual void OnAddChildren();

    /**
     * @brief Called when child nodes are removed from this node.
     */
    virtual void OnRemoveChildren();

    virtual bool NeedForcedSendToRemote() const
    {
        return false;
    }

    void DoFlushModifier();

    std::vector<PropertyId> GetModifierIds() const;
    bool isCustomTextType_ = false;
    bool isCustomTypeface_ = false;

    /**
     * @brief Gets the mutex used for property access.
     *
     * @return Reference to the internal std::recursive_mutex used for property synchronization.
     */
    std::recursive_mutex& GetPropertyMutex() const
    {
        return propertyMutex_;
    }

    /**
     * @brief Checks if the function is being accessed from multiple threads.
     *
     * @param func The name of the function to check.
     * @return true if accessed from multiple threads; false otherwise.
     */
    bool CheckMultiThreadAccess(const std::string& func) const;

    /**
     * @brief Registers the node map.
     */
    virtual void RegisterNodeMap() {}

    /**
     * @brief Gets the transaction handler for the RS.
     *
     * @return A shared pointer to the RSTransactionHandler object.
     */
    std::shared_ptr<RSTransactionHandler> GetRSTransaction() const;

    /**
     * @brief Adds a command to the command list.
     *
     * @param command A unique pointer to the RSCommand to be added.
     * @param isRenderServiceCommand Indicates whether the command is a render service command. Default is false.
     * @param followType Specifies the follow type for the command. Default is FollowType::NONE.
     * @param nodeId The ID of the node associated with the command. Default is 0.
     * @return true if the command was successfully added; false otherwise.
     */
    bool AddCommand(std::unique_ptr<RSCommand>& command, bool isRenderServiceCommand = false,
        FollowType followType = FollowType::NONE, NodeId nodeId = 0) const;

    /**
     * @brief Sets whether the node is on the tree.
     *
     * @param flag true if the node is on the tree; false otherwise.
     */
    void SetIsOnTheTree(bool flag);

    std::array<std::shared_ptr<ModifierNG::RSModifier>, ModifierNG::MODIFIER_TYPE_COUNT> modifiersNGCreatedBySetter_;
    static inline bool isMultiInstanceOpen_ = false;

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

#if defined(MODIFIER_NG)
    /**
     * @brief Sets a property value for a specific modifier.
     *
     * If property already exists, it will be updated.
     * If property does not exist, it will be created.
     *
     * @param value The value to assign to the property.
     */
    template<typename ModifierType, auto Setter, typename T>
    void SetPropertyNG(T value);

    /**
     * @brief Sets a property value for a specific modifier.
     *
     * If property already exists, it will be updated.
     * If property does not exist, it will be created.
     *
     * @param value The value to assign to the property.
     * @param animatable The property is animatable or not.
     */
    template<typename ModifierType, auto Setter, typename T>
    void SetPropertyNG(T value, bool animatable);

    /**
     * @brief Sets a UIFilter property value for a specific modifier.
     *
     * If property already exists, it will be updated.
     * If property does not exist, it will be created.
     *
     * @param value The value to assign to the property.
     */
    template<typename ModifierType, auto Setter, typename T>
    void SetUIFilterPropertyNG(T value);
#else
    /**
     * @brief Sets a property value for a specific modifier.
     *
     * If property already exists, it will be updated.
     * If property does not exist, it will be created.
     *
     * @param modifierType The type of the modifier to which the property belongs.
     * @param value The value to assign to the property.
     */
    template<typename ModifierName, typename PropertyName, typename T>
    void SetProperty(RSModifierType modifierType, T value);
#endif

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

    void NotifyPageNodeChanged();
    void CheckModifierType(RSModifierType modifierType);
    bool AnimationCallback(AnimationId animationId, AnimationCallbackEvent event);
    bool HasPropertyAnimation(const PropertyId& id);
    std::vector<AnimationId> GetAnimationByPropertyId(const PropertyId& id);
    bool FallbackAnimationsToContext();
    void FallbackAnimationsToRoot();
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void FinishAnimationByProperty(const PropertyId& id);
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void CancelAnimationByProperty(const PropertyId& id, const bool needForceSync = false);


    /**
     * @brief Retrieves the modifier associated with the specified property ID.
     *
     * @param propertyId The identifier of the property whose modifier is to be retrieved.
     * @return The shared pointer to the corresponding RSModifier,or nullptr if not found.
     */
    const std::shared_ptr<RSModifier> GetModifier(const PropertyId& propertyId);
    const std::shared_ptr<RSPropertyBase> GetProperty(const PropertyId& propertyId);
    void RegisterProperty(std::shared_ptr<RSPropertyBase> property);
    void UnregisterProperty(const PropertyId& propertyId);


    /**
     * @brief Called when the bounds size of the node has changed.
     */
    virtual void OnBoundsSizeChanged() const {};
    void UpdateModifierMotionPathOption();
    void MarkAllExtendModifierDirty();
    void ResetExtendModifierDirty();
    void SetParticleDrawRegion(std::vector<ParticleParams>& particleParams);

    void DetachUIFilterProperties(const std::shared_ptr<ModifierNG::RSModifier>& modifier);

    /**
     * @brief Clears all modifiers associated with this node.
     *
     * Planning: refactor RSUIAnimationManager and remove this method
     */
    void ClearAllModifiers();

    uint32_t dirtyType_ = static_cast<uint32_t>(NodeDirtyType::NOT_DIRTY);

    std::shared_ptr<RSObjAbsGeometry> localGeometry_;
    std::shared_ptr<RSObjAbsGeometry> globalGeometry_;

    float globalPositionX_ = 0.f;
    float globalPositionY_ = 0.f;

    bool extendModifierIsDirty_ { false };

    bool isNodeGroup_ = false;
    bool isRepaintBoundary_ = false;

    bool isNodeSingleFrameComposer_ = false;

    bool isSuggestOpincNode_ = false;
    bool isDrawNode_ = false;
    // Used to identify whether the node has real drawing property
    DrawNodeType drawNodeType_ = DrawNodeType::PureContainerType;
    static bool isNeedCallbackNodeChange_;

    bool isUifirstNode_ = true;
    bool isForceFlag_ = false;
    bool isUifirstEnable_ = false;
    bool isSkipCheckInMultiInstance_ = false;
    RSUIFirstSwitch uiFirstSwitch_ = RSUIFirstSwitch::NONE;
    std::weak_ptr<RSUIContext> rsUIContext_;

    uint32_t hdrEffectType_ = 0;

    RSModifierExtractor stagingPropertiesExtractor_;
    RSShowingPropertiesFreezer showingPropertiesFreezer_;
    std::map<PropertyId, std::shared_ptr<RSModifier>> modifiers_;
    std::map<PropertyId, std::shared_ptr<RSPropertyBase>> properties_;
    std::map<uint16_t, std::shared_ptr<RSModifier>> modifiersTypeMap_;
    std::map<RSModifierType, std::shared_ptr<RSModifier>> propertyModifiers_;
    std::map<ModifierId, std::shared_ptr<ModifierNG::RSModifier>> modifiersNG_;

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
    friend class ModifierNG::RSModifier;
    friend class ModifierNG::RSCustomModifier;
    friend class RSBackgroundUIFilterModifier;
    friend class RSForegroundUIFilterModifier;
    friend class RSKeyframeAnimation;
    friend class RSInterpolatingSpringAnimation;
    friend class RSImplicitCancelAnimationParam;
    friend class RSImplicitAnimator;
    friend class RSGeometryTransModifier;
    friend class RSExtendedModifier;
    friend class RSCurveAnimation;
    friend class RSAnimation;
    friend class ModifierNG::RSForegroundFilterModifier;
    friend class ModifierNG::RSBackgroundFilterModifier;
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

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_NODE_H
