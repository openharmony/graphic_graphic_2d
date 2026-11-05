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
#include <string>
#include <unistd.h>

#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace {
constexpr int SLEEP_FOR_FLUSH_MS = 500;

void PrintHeader(const std::string& title)
{
    std::cout << "\n====== " << title << " ======" << std::endl;
}

void PrintClientNode(const std::string& tag, const std::shared_ptr<RSNode>& node,
    const std::shared_ptr<RSUIContext>& context)
{
    std::string dump;
    node->Dump(dump);
    std::cout << "[CLIENT][" << tag << "] nodeId:" << node->GetId()
        << " token:" << (context ? std::to_string(context->GetToken()) : "null")
        << " contextIsSceneBoard:" << (context && context->IsSceneBoard() ? "true" : "false") << std::endl;
    std::cout << "[CLIENT][" << tag << "] Dump: " << dump << std::endl;
}

// Extracts the value inside "field[value]" from a dump string for consistency checks.
std::string ExtractDumpField(const std::string& dump, const std::string& field)
{
    auto pos = dump.find(field);
    if (pos == std::string::npos) {
        return "NOT_FOUND";
    }
    auto start = pos + field.size();
    auto end = dump.find(']', start);
    return dump.substr(start, end == std::string::npos ? std::string::npos : end - start);
}
} // namespace

int main()
{
    std::cout << "====== SceneBoard consistency demo ======" << std::endl;
    std::cout << "client pid:" << getpid() << std::endl;

    // ------------------------------------------------------------------
    // Step 1: create two contexts — contextA marked as SceneBoard, contextB unmarked.
    // ------------------------------------------------------------------
    PrintHeader("Step 1: create directors/contexts (A=SceneBoard, B=normal)");
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    sptr<IRemoteObject> connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    if (connectToRender == nullptr) {
        std::cout << "FAILED to connect render service, exit" << std::endl;
        return 0;
    }
    auto directorA = RSUIDirector::Create(connectToRender);
    if (directorA == nullptr || directorA->GetRSUIContext() == nullptr) {
        std::cout << "FAILED to create directorA, exit" << std::endl;
        return 0;
    }
    auto contextA = directorA->GetRSUIContext();
    directorA->MarkAsSceneBoard(); // one-way identity declaration for the SCB instance
    std::cout << "[CLIENT] contextA token:" << contextA->GetToken()
        << " isSceneBoard:" << (contextA->IsSceneBoard() ? "true" : "false") << std::endl;

    auto directorB = RSUIDirector::Create(connectToRender);
    if (directorB == nullptr || directorB->GetRSUIContext() == nullptr) {
        std::cout << "FAILED to create directorB, exit" << std::endl;
        return 0;
    }
    auto contextB = directorB->GetRSUIContext();
    std::cout << "[CLIENT] contextB token:" << contextB->GetToken()
        << " isSceneBoard:" << (contextB->IsSceneBoard() ? "true" : "false") << std::endl;

    // ------------------------------------------------------------------
    // Step 2: create nodes under both instances; client dump carries the identity.
    // ------------------------------------------------------------------
    PrintHeader("Step 2: create nodes under A(marked) and B(unmarked)");
    auto canvasA = RSCanvasNode::Create(false, false, contextA);
    auto canvasB = RSCanvasNode::Create(false, false, contextB);
    PrintClientNode("canvasA@A", canvasA, contextA);
    PrintClientNode("canvasB@B", canvasB, contextB);

    RSSurfaceNodeConfig surfaceConfigA = { .SurfaceNodeName = "demo_scb_surface_A" };
    auto surfaceA = RSSurfaceNode::Create(surfaceConfigA, false, contextA);
    RSSurfaceNodeConfig surfaceConfigB = { .SurfaceNodeName = "demo_scb_surface_B" };
    auto surfaceB = RSSurfaceNode::Create(surfaceConfigB, false, contextB);
    PrintClientNode("surfaceA@A", surfaceA, contextA);
    PrintClientNode("surfaceB@B", surfaceB, contextB);

    usleep(SLEEP_FOR_FLUSH_MS * 1000); // wait for implicit transaction flush

    // ------------------------------------------------------------------
    // Step 3: migrate canvasA from contextA to contextB.
    // Expectation: current-owner token switches to B (client dump UIContext changes),
    // while the server-side birth identity (isSceneBoard) must stay TRUE.
    // ------------------------------------------------------------------
    PrintHeader("Step 3: migrate canvasA from A to B (owner changes, birth identity stays)");
    canvasA->SetRSUIContext(contextB);
    PrintClientNode("canvasA@B(after migrate)", canvasA, contextB);
    usleep(SLEEP_FOR_FLUSH_MS * 1000);

    // ------------------------------------------------------------------
    // Step 4: client-side consistency check between node identity and its context.
    // ------------------------------------------------------------------
    PrintHeader("Step 4: client-side consistency check");
    auto check = [](const std::string& tag, const std::string& dumpToken,
        const std::shared_ptr<RSUIContext>& context, const std::string& expectScb) {
        bool tokenMatch = (context != nullptr) && (dumpToken == std::to_string(context->GetToken()));
        std::string scb = (context && context->IsSceneBoard()) ? "true" : "false";
        bool scbMatch = (scb == expectScb);
        std::cout << "[CHECK][" << tag << "] dump.UIContext=" << dumpToken
            << " context.token=" << (context ? std::to_string(context->GetToken()) : "null")
            << " -> token " << (tokenMatch ? "MATCH" : "MISMATCH")
            << "; contextIsSceneBoard=" << scb << " (expect " << expectScb << ") -> "
            << (scbMatch ? "MATCH" : "MISMATCH") << std::endl;
    };
    std::string dumpTmp;
    canvasA->Dump(dumpTmp);
    check("canvasA@B", ExtractDumpField(dumpTmp, "UIContext["), contextB, "false");
    canvasB->Dump(dumpTmp);
    check("canvasB@B", ExtractDumpField(dumpTmp, "UIContext["), contextB, "false");
    surfaceA->Dump(dumpTmp);
    check("surfaceA@A", ExtractDumpField(dumpTmp, "UIContext["), contextA, "true");
    surfaceB->Dump(dumpTmp);
    check("surfaceB@B", ExtractDumpField(dumpTmp, "UIContext["), contextB, "false");

    // ------------------------------------------------------------------
    // Step 5: how to compare with the server side.
    // ------------------------------------------------------------------
    PrintHeader("Step 5: server-side dump comparison guide");
    std::cout << "Run on device to capture the server node dump:" << std::endl
        << "  hdc shell \"hidumper -s 10 -a \\\"render_service dumpNodeTree\\\"\"" << std::endl;
    std::cout << "Fields to compare for each node id printed above:" << std::endl
        << "  server 'RSUIContextToken: [ <token> ]'  <->  client 'UIContext[<token>]'" << std::endl
        << "  server 'isSceneBoard: 1/0'              <->  birth identity of the creating context"
        << std::endl;
    std::cout << "Expected after migration:" << std::endl
        << "  canvasA: server isSceneBoard stays 1 (birth identity, not retroactive)," << std::endl
        << "           server RSUIContextToken becomes contextB's token (owner follows migration)"
        << std::endl
        << "           while client contextIsSceneBoard prints false (current-context view)."
        << std::endl;
    return 0;
}
