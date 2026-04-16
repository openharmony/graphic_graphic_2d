/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <iostream>
#include <unistd.h>

#include "window.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_ui_director.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "common/rs_vector4.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 2560;
constexpr int NODE_WIDTH = 400;
constexpr int NODE_HEIGHT = 400;
constexpr float DEFAULT_LIGHT_INTENSITY = 1.0f;
constexpr float DEFAULT_LIGHT_ATTENUATION = 0.5f;
constexpr float LIGHT_DEPTH = 50.0f;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void ApplySpatialPointLightShader(std::shared_ptr<RSCanvasNode> node,
    const Vector3f& lightPos, const Vector4f& lightColor, float intensity, float attenuation)
{
    auto shader = std::make_shared<RSNGSpatialPointLight>();
    shader->Setter<SpatialPointLightLightIntensityTag>(intensity);
    shader->Setter<SpatialPointLightLightPositionTag>(lightPos);
    shader->Setter<SpatialPointLightLightColorTag>(lightColor);
    shader->Setter<SpatialPointLightAttenuationTag>(attenuation);
    node->SetBackgroundNGShader(shader);
}

void ApplyCascadeShader(std::shared_ptr<RSCanvasNode> node)
{
    auto shader1 = std::make_shared<RSNGSpatialPointLight>();
    shader1->Setter<SpatialPointLightLightIntensityTag>(1.0f);
    shader1->Setter<SpatialPointLightLightPositionTag>(Vector3f(100.0f, 100.0f, LIGHT_DEPTH));
    shader1->Setter<SpatialPointLightLightColorTag>(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    shader1->Setter<SpatialPointLightAttenuationTag>(DEFAULT_LIGHT_ATTENUATION);

    auto shader2 = std::make_shared<RSNGBorderLight>();
    shader2->Setter<BorderLightPositionTag>(Vector3f(0.5f, 0.5f, 0.0f));
    shader2->Setter<BorderLightColorTag>(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    shader2->Setter<BorderLightIntensityTag>(0.8f);
    shader2->Setter<BorderLightWidthTag>(5.0f);
    shader2->Setter<BorderLightCornerRadiusTag>(30.0f);

    shader1->Append(shader2);
    node->SetBackgroundNGShader(shader1);
}

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    cout << "SpatialPointLight demo Init Rosen Backend!" << endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorBLACK);

    // Node 1: Basic spatial point light shader
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(200, 200, NODE_WIDTH, NODE_HEIGHT);
    nodes[0]->SetFrame(200, 200, NODE_WIDTH, NODE_HEIGHT);
    nodes[0]->SetBackgroundColor(SK_ColorGRAY);
    ApplySpatialPointLightShader(nodes[0],
        Vector3f(400.0f, 400.0f, LIGHT_DEPTH),
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
        DEFAULT_LIGHT_INTENSITY,
        DEFAULT_LIGHT_ATTENUATION);

    // Node 2: Red point light
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[1]->SetBounds(700, 200, NODE_WIDTH, NODE_HEIGHT);
    nodes[1]->SetFrame(700, 200, NODE_WIDTH, NODE_HEIGHT);
    nodes[1]->SetBackgroundColor(SK_ColorGRAY);
    ApplySpatialPointLightShader(nodes[1],
        Vector3f(900.0f, 400.0f, LIGHT_DEPTH),
        Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
        1.5f,
        0.3f);

    // Node 3: Green point light
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[2]->SetBounds(200, 700, NODE_WIDTH, NODE_HEIGHT);
    nodes[2]->SetFrame(200, 700, NODE_WIDTH, NODE_HEIGHT);
    nodes[2]->SetBackgroundColor(SK_ColorGRAY);
    ApplySpatialPointLightShader(nodes[2],
        Vector3f(400.0f, 900.0f, LIGHT_DEPTH),
        Vector4f(0.0f, 1.0f, 0.0f, 1.0f),
        2.0f,
        0.8f);

    // Node 4: Cascade shader (SpatialPointLight + BorderLight)
    nodes.emplace_back(RSCanvasNode::Create());
    nodes[3]->SetBounds(700, 700, NODE_WIDTH, NODE_HEIGHT);
    nodes[3]->SetFrame(700, 700, NODE_WIDTH, NODE_HEIGHT);
    nodes[3]->SetBackgroundColor(SK_ColorGRAY);
    ApplyCascadeShader(nodes[3]);

    for (auto& node : nodes) {
        rootNode->AddChild(node, -1);
    }
    rsUiDirector->SetRSRootNode(rootNode->ReinterpretCastTo<RSRootNode>());
}

void UpdateIntensityAnimation()
{
    static float phase = 0.0f;
    phase += 0.05f;

    float intensity = 0.5f + 0.5f * sin(phase);
    auto shader = std::make_shared<RSNGSpatialPointLight>();
    shader->Setter<SpatialPointLightLightIntensityTag>(intensity);
    shader->Setter<SpatialPointLightLightPositionTag>(
        Vector3f(400.0f, 400.0f, LIGHT_DEPTH));
    shader->Setter<SpatialPointLightLightColorTag>(
        Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    shader->Setter<SpatialPointLightAttenuationTag>(DEFAULT_LIGHT_ATTENUATION);
    nodes[0]->SetBackgroundNGShader(shader);
}

int main()
{
    cout << "SpatialPointLight demo start!" << endl;

    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

    auto window = Window::Create("spatial_point_light_demo", option);
    window->Show();
    auto rect = window->GetRect();

    while (rect.width_ == 0 && rect.height_ == 0) {
        cout << "Create window failed: " << rect.width_ << " " << rect.height_ << endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("spatial_point_light_demo", option);
        window->Show();
        rect = window->GetRect();
    }

    cout << "Create window " << rect.width_ << " " << rect.height_ << endl;

    auto surfaceNode = window->GetSurfaceNode();
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    cout << "SpatialPointLight demo stage 1 - Init" << endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    rsUiDirector->SendMessages();
    sleep(2);

    cout << "SpatialPointLight demo stage 2 - Animation" << endl;
    for (int frame = 0; frame < 60; frame++) {
        UpdateIntensityAnimation();
        rsUiDirector->SendMessages();
        usleep(50000);
    }
    sleep(1);

    cout << "SpatialPointLight demo end!" << endl;
    window->Hide();
    window->Destroy();

    return 0;
}