# Graphics<a name="EN-US_TOPIC_0000001105482134"></a>

-   [Introduction](#section1333751883213)
-   [Directory Structure](#section1882912343)
-   [Constraints](#section68982123611)
-   [Compilation and Building](#section671864110372)
-   [Available APIs](#section197399520386)
    -   [WindowManager](#section5851104093816)
    -   [Window](#section3483122273912)
    -   [SubWindow](#section96481249183913)
    -   [Surface](#section12366161544010)
    -   [SurfaceBuffer](#section12001640184711)
    -   [VsyncHelper](#section1392116294211)

-   [Usage](#section18359134910422)
    -   [Transferring a Producer Surface](#section193464304411)
    -   [Creating a Producer Surface](#section1276823075112)
    -   [Producing a SurfaceBuffer](#section614545716513)
    -   [Consuming a SurfaceBuffer](#section315285412535)
    -   [Adding Custom Data to a SurfaceBuffer](#section612412125616)
    -   [Registering a Vsync Callback Listener](#section1148115214576)

-   [\#EN-US\_TOPIC\_0000001105482134/section1939493174420](#section1939493174420)
-   [Repositories Involved](#section6488145313)

## Introduction<a name="section1333751883213"></a>

The Graphics subsystem provides graphics and window management capabilities, which can be invoked by using Java or JS APIs. It can be used for UI development for all standard-system devices.

The following figure shows the architecture of the Graphics subsystem.

![](figures/graphic.png)

-   Surface

    Provides APIs for managing the graphics buffer and the efficient and convenient rotation buffer.

-   Vsync

    Provides APIs for managing registration and response of all vertical sync signals.

-   WindowManager

    Provides APIs for creating and managing windows.

-   WaylandProtocols

    Provides the communication protocols between the window manager and synthesizer.

-   Compositor

    Implements synthesis of layers.

-   Renderer

    Functions as the back-end rendering module of the synthesizer.

-   Wayland protocols

    Provides Wayland inter-process communication protocols.

-   Shell

    Provides multi-window capabilities.

-   Input Manger

    Functions as the multimodal input module that receives input events.


## Directory Structure<a name="section1882912343"></a>

```
foundation/graphic/standard/
├── frameworks              # Framework code
│   ├── bootanimation       # Boot Animation code
│   ├── surface             # Surface code
│   ├── vsync               # Vsync code
│   └── wm                  # WindowManager code
├── interfaces              # External APIs
│   ├── innerkits           # Native APIs
│   └── kits                # JS APIs and NAPIs
└── utils                   # Utilities
```

## Constraints<a name="section68982123611"></a>

-   Language version: C++ 11 or later

## Compilation and Building<a name="section671864110372"></a>

The dependent APIs include the following:

-   graphic\_standard:libwms\_client
-   graphic\_standard:libsurface
-   graphic\_standard:libvsync\_client

## Available APIs<a name="section197399520386"></a>

### WindowManager<a name="section5851104093816"></a>

<a name="table119mcpsimp"></a>
<table><thead align="left"><tr id="row124mcpsimp"><th class="cellrowborder" valign="top" width="37%" id="mcps1.1.3.1.1"><p id="p126mcpsimp"><a name="p126mcpsimp"></a><a name="p126mcpsimp"></a><strong id="b193946358456"><a name="b193946358456"></a><a name="b193946358456"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="63%" id="mcps1.1.3.1.2"><p id="p129mcpsimp"><a name="p129mcpsimp"></a><a name="p129mcpsimp"></a><strong id="b128701367459"><a name="b128701367459"></a><a name="b128701367459"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row132mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p134mcpsimp"><a name="p134mcpsimp"></a><a name="p134mcpsimp"></a>GetInstance</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p136mcpsimp"><a name="p136mcpsimp"></a><a name="p136mcpsimp"></a>Obtains the pointer to a singleton <strong id="b18411195111456"><a name="b18411195111456"></a><a name="b18411195111456"></a>WindowManager</strong> instance.</p>
</td>
</tr>
<tr id="row137mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p139mcpsimp"><a name="p139mcpsimp"></a><a name="p139mcpsimp"></a>GetMaxWidth</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p141mcpsimp"><a name="p141mcpsimp"></a><a name="p141mcpsimp"></a>Obtains the width of the screen.</p>
</td>
</tr>
<tr id="row142mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p144mcpsimp"><a name="p144mcpsimp"></a><a name="p144mcpsimp"></a>GetMaxHeight</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p146mcpsimp"><a name="p146mcpsimp"></a><a name="p146mcpsimp"></a>Obtains the height of the screen.</p>
</td>
</tr>
<tr id="row147mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p149mcpsimp"><a name="p149mcpsimp"></a><a name="p149mcpsimp"></a>CreateWindow</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p151mcpsimp"><a name="p151mcpsimp"></a><a name="p151mcpsimp"></a>Creates a standard window.</p>
</td>
</tr>
<tr id="row152mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p154mcpsimp"><a name="p154mcpsimp"></a><a name="p154mcpsimp"></a>CreateSubWindow</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p156mcpsimp"><a name="p156mcpsimp"></a><a name="p156mcpsimp"></a>Creates a child window.</p>
</td>
</tr>
<tr id="row157mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p159mcpsimp"><a name="p159mcpsimp"></a><a name="p159mcpsimp"></a>StartShotScreen</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p161mcpsimp"><a name="p161mcpsimp"></a><a name="p161mcpsimp"></a>Takes a screenshot.</p>
</td>
</tr>
<tr id="row162mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p164mcpsimp"><a name="p164mcpsimp"></a><a name="p164mcpsimp"></a>StartShotWindow</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p166mcpsimp"><a name="p166mcpsimp"></a><a name="p166mcpsimp"></a>Captures a window.</p>
</td>
</tr>
<tr id="row167mcpsimp"><td class="cellrowborder" valign="top" width="37%" headers="mcps1.1.3.1.1 "><p id="p169mcpsimp"><a name="p169mcpsimp"></a><a name="p169mcpsimp"></a>SwitchTop</p>
</td>
<td class="cellrowborder" valign="top" width="63%" headers="mcps1.1.3.1.2 "><p id="p171mcpsimp"><a name="p171mcpsimp"></a><a name="p171mcpsimp"></a>Moves the specified window to the top.</p>
</td>
</tr>
</tbody>
</table>

### Window<a name="section3483122273912"></a>

<a name="table173mcpsimp"></a>
<table><thead align="left"><tr id="row178mcpsimp"><th class="cellrowborder" valign="top" width="48%" id="mcps1.1.3.1.1"><p id="p180mcpsimp"><a name="p180mcpsimp"></a><a name="p180mcpsimp"></a><strong id="b1734339115416"><a name="b1734339115416"></a><a name="b1734339115416"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="52%" id="mcps1.1.3.1.2"><p id="p183mcpsimp"><a name="p183mcpsimp"></a><a name="p183mcpsimp"></a><strong id="b660764018548"><a name="b660764018548"></a><a name="b660764018548"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row186mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p188mcpsimp"><a name="p188mcpsimp"></a><a name="p188mcpsimp"></a>Show</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p190mcpsimp"><a name="p190mcpsimp"></a><a name="p190mcpsimp"></a>Displays the current window.</p>
</td>
</tr>
<tr id="row191mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p193mcpsimp"><a name="p193mcpsimp"></a><a name="p193mcpsimp"></a>Hide</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p195mcpsimp"><a name="p195mcpsimp"></a><a name="p195mcpsimp"></a>Hides the current window.</p>
</td>
</tr>
<tr id="row196mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p198mcpsimp"><a name="p198mcpsimp"></a><a name="p198mcpsimp"></a>Move</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p200mcpsimp"><a name="p200mcpsimp"></a><a name="p200mcpsimp"></a>Moves the current window to a specified position.</p>
</td>
</tr>
<tr id="row201mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p203mcpsimp"><a name="p203mcpsimp"></a><a name="p203mcpsimp"></a>SwitchTop</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p205mcpsimp"><a name="p205mcpsimp"></a><a name="p205mcpsimp"></a>Moves the current window to the top.</p>
</td>
</tr>
<tr id="row206mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p208mcpsimp"><a name="p208mcpsimp"></a><a name="p208mcpsimp"></a>ChangeWindowType</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p210mcpsimp"><a name="p210mcpsimp"></a><a name="p210mcpsimp"></a>Changes the type of the current window.</p>
</td>
</tr>
<tr id="row211mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p213mcpsimp"><a name="p213mcpsimp"></a><a name="p213mcpsimp"></a>ReSize</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p215mcpsimp"><a name="p215mcpsimp"></a><a name="p215mcpsimp"></a>Resizes the current window.</p>
</td>
</tr>
<tr id="row216mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p218mcpsimp"><a name="p218mcpsimp"></a><a name="p218mcpsimp"></a>Rotate</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p220mcpsimp"><a name="p220mcpsimp"></a><a name="p220mcpsimp"></a>Rotates the current window.</p>
</td>
</tr>
<tr id="row221mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p223mcpsimp"><a name="p223mcpsimp"></a><a name="p223mcpsimp"></a>RegistPointerButtonCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p225mcpsimp"><a name="p225mcpsimp"></a><a name="p225mcpsimp"></a>Registers the callback for Button events of the mouse.</p>
</td>
</tr>
<tr id="row226mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p228mcpsimp"><a name="p228mcpsimp"></a><a name="p228mcpsimp"></a>RegistPointerEnterCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p230mcpsimp"><a name="p230mcpsimp"></a><a name="p230mcpsimp"></a>Registers the callback for Enter events of the mouse.</p>
</td>
</tr>
<tr id="row231mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p233mcpsimp"><a name="p233mcpsimp"></a><a name="p233mcpsimp"></a>RegistPointerLeaveCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p235mcpsimp"><a name="p235mcpsimp"></a><a name="p235mcpsimp"></a>Registers the callback for Leave events of the mouse.</p>
</td>
</tr>
<tr id="row236mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p238mcpsimp"><a name="p238mcpsimp"></a><a name="p238mcpsimp"></a>RegistPointerMotionCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p240mcpsimp"><a name="p240mcpsimp"></a><a name="p240mcpsimp"></a>Registers the callback for Motion events of the mouse.</p>
</td>
</tr>
<tr id="row241mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p243mcpsimp"><a name="p243mcpsimp"></a><a name="p243mcpsimp"></a>RegistPointerAxisDiscreteCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p245mcpsimp"><a name="p245mcpsimp"></a><a name="p245mcpsimp"></a>Registers the callback for AxisDiscrete events of the mouse.</p>
</td>
</tr>
<tr id="row246mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p248mcpsimp"><a name="p248mcpsimp"></a><a name="p248mcpsimp"></a>RegistPointerAxisSourceCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p250mcpsimp"><a name="p250mcpsimp"></a><a name="p250mcpsimp"></a>Registers the callback for AxisSource events of the mouse.</p>
</td>
</tr>
<tr id="row251mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p253mcpsimp"><a name="p253mcpsimp"></a><a name="p253mcpsimp"></a>RegistPointerAxisStopCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p255mcpsimp"><a name="p255mcpsimp"></a><a name="p255mcpsimp"></a>Registers the callback for AxisStop events of the mouse.</p>
</td>
</tr>
<tr id="row256mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p258mcpsimp"><a name="p258mcpsimp"></a><a name="p258mcpsimp"></a>RegistPointerAxisCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p260mcpsimp"><a name="p260mcpsimp"></a><a name="p260mcpsimp"></a>Registers the callback for Axis events of the mouse.</p>
</td>
</tr>
<tr id="row261mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p263mcpsimp"><a name="p263mcpsimp"></a><a name="p263mcpsimp"></a>RegistTouchUpCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p265mcpsimp"><a name="p265mcpsimp"></a><a name="p265mcpsimp"></a>Registers the callback for TouchUp events.</p>
</td>
</tr>
<tr id="row266mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p268mcpsimp"><a name="p268mcpsimp"></a><a name="p268mcpsimp"></a>RegistTouchDownCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p270mcpsimp"><a name="p270mcpsimp"></a><a name="p270mcpsimp"></a>Registers the callback for TouchDown events.</p>
</td>
</tr>
<tr id="row271mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p273mcpsimp"><a name="p273mcpsimp"></a><a name="p273mcpsimp"></a>RegistTouchEmotionCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p275mcpsimp"><a name="p275mcpsimp"></a><a name="p275mcpsimp"></a>Registers the callback for TouchEmotion events.</p>
</td>
</tr>
<tr id="row276mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p278mcpsimp"><a name="p278mcpsimp"></a><a name="p278mcpsimp"></a>RegistTouchFrameCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p280mcpsimp"><a name="p280mcpsimp"></a><a name="p280mcpsimp"></a>Registers the callback for TouchFrame events.</p>
</td>
</tr>
<tr id="row281mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p283mcpsimp"><a name="p283mcpsimp"></a><a name="p283mcpsimp"></a>RegistTouchCancelCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p285mcpsimp"><a name="p285mcpsimp"></a><a name="p285mcpsimp"></a>Registers the callback for TouchCancel events.</p>
</td>
</tr>
<tr id="row286mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p288mcpsimp"><a name="p288mcpsimp"></a><a name="p288mcpsimp"></a>RegistTouchShapeCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p290mcpsimp"><a name="p290mcpsimp"></a><a name="p290mcpsimp"></a>Registers the callback for TouchShape events.</p>
</td>
</tr>
<tr id="row291mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p293mcpsimp"><a name="p293mcpsimp"></a><a name="p293mcpsimp"></a>RegistTouchOrientationCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p295mcpsimp"><a name="p295mcpsimp"></a><a name="p295mcpsimp"></a>Registers the callback for TouchOrientation events.</p>
</td>
</tr>
<tr id="row296mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p298mcpsimp"><a name="p298mcpsimp"></a><a name="p298mcpsimp"></a>RegistKeyboardKeyCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p300mcpsimp"><a name="p300mcpsimp"></a><a name="p300mcpsimp"></a>Registers the callback for Key events of the keyboard.</p>
</td>
</tr>
<tr id="row301mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p303mcpsimp"><a name="p303mcpsimp"></a><a name="p303mcpsimp"></a>RegistKeyboardKeyMapCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p305mcpsimp"><a name="p305mcpsimp"></a><a name="p305mcpsimp"></a>Registers the callback for KeyMap events of the keyboard.</p>
</td>
</tr>
<tr id="row306mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p308mcpsimp"><a name="p308mcpsimp"></a><a name="p308mcpsimp"></a>RegistKeyboardLeaveCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p310mcpsimp"><a name="p310mcpsimp"></a><a name="p310mcpsimp"></a>Registers the callback for Leave events of the keyboard.</p>
</td>
</tr>
<tr id="row311mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p313mcpsimp"><a name="p313mcpsimp"></a><a name="p313mcpsimp"></a>RegistKeyboardEnterCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p315mcpsimp"><a name="p315mcpsimp"></a><a name="p315mcpsimp"></a>Registers the callback for Enter events of the keyboard.</p>
</td>
</tr>
<tr id="row316mcpsimp"><td class="cellrowborder" valign="top" width="48%" headers="mcps1.1.3.1.1 "><p id="p318mcpsimp"><a name="p318mcpsimp"></a><a name="p318mcpsimp"></a>RegistKeyboardRepeatInfoCb</p>
</td>
<td class="cellrowborder" valign="top" width="52%" headers="mcps1.1.3.1.2 "><p id="p320mcpsimp"><a name="p320mcpsimp"></a><a name="p320mcpsimp"></a>Registers the callback for RepeatInfo events of the keyboard.</p>
</td>
</tr>
</tbody>
</table>

### SubWindow<a name="section96481249183913"></a>

<a name="table322mcpsimp"></a>
<table><thead align="left"><tr id="row327mcpsimp"><th class="cellrowborder" valign="top" width="49%" id="mcps1.1.3.1.1"><p id="p329mcpsimp"><a name="p329mcpsimp"></a><a name="p329mcpsimp"></a><strong id="b131086571302"><a name="b131086571302"></a><a name="b131086571302"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="51%" id="mcps1.1.3.1.2"><p id="p332mcpsimp"><a name="p332mcpsimp"></a><a name="p332mcpsimp"></a><strong id="b041313581403"><a name="b041313581403"></a><a name="b041313581403"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row335mcpsimp"><td class="cellrowborder" valign="top" width="49%" headers="mcps1.1.3.1.1 "><p id="p337mcpsimp"><a name="p337mcpsimp"></a><a name="p337mcpsimp"></a>Move</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.3.1.2 "><p id="p339mcpsimp"><a name="p339mcpsimp"></a><a name="p339mcpsimp"></a>Moves the current child window.</p>
</td>
</tr>
<tr id="row340mcpsimp"><td class="cellrowborder" valign="top" width="49%" headers="mcps1.1.3.1.1 "><p id="p342mcpsimp"><a name="p342mcpsimp"></a><a name="p342mcpsimp"></a>SetSubWindowSize</p>
</td>
<td class="cellrowborder" valign="top" width="51%" headers="mcps1.1.3.1.2 "><p id="p344mcpsimp"><a name="p344mcpsimp"></a><a name="p344mcpsimp"></a>Sets the size of the current child window.</p>
</td>
</tr>
</tbody>
</table>

### Surface<a name="section12366161544010"></a>

<a name="table346mcpsimp"></a>
<table><thead align="left"><tr id="row351mcpsimp"><th class="cellrowborder" valign="top" width="41%" id="mcps1.1.3.1.1"><p id="p353mcpsimp"><a name="p353mcpsimp"></a><a name="p353mcpsimp"></a><strong id="b8875439216"><a name="b8875439216"></a><a name="b8875439216"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="59%" id="mcps1.1.3.1.2"><p id="p356mcpsimp"><a name="p356mcpsimp"></a><a name="p356mcpsimp"></a><strong id="b14608174222"><a name="b14608174222"></a><a name="b14608174222"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row359mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p361mcpsimp"><a name="p361mcpsimp"></a><a name="p361mcpsimp"></a>CreateSurfaceAsConsumer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p363mcpsimp"><a name="p363mcpsimp"></a><a name="p363mcpsimp"></a>Creates a surface for the buffer consumer.</p>
</td>
</tr>
<tr id="row364mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p366mcpsimp"><a name="p366mcpsimp"></a><a name="p366mcpsimp"></a>CreateSurfaceAsProducer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p368mcpsimp"><a name="p368mcpsimp"></a><a name="p368mcpsimp"></a>Creates a surface for the buffer producer. Only production-related APIs can be used.</p>
</td>
</tr>
<tr id="row369mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p371mcpsimp"><a name="p371mcpsimp"></a><a name="p371mcpsimp"></a>GetProducer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p373mcpsimp"><a name="p373mcpsimp"></a><a name="p373mcpsimp"></a>Obtains an internal <strong id="b1982865713319"><a name="b1982865713319"></a><a name="b1982865713319"></a>IBufferProducer</strong> object of <strong id="b14235152710419"><a name="b14235152710419"></a><a name="b14235152710419"></a>Surface</strong>.</p>
</td>
</tr>
<tr id="row374mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p376mcpsimp"><a name="p376mcpsimp"></a><a name="p376mcpsimp"></a>RequestBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p378mcpsimp"><a name="p378mcpsimp"></a><a name="p378mcpsimp"></a>Requests a <strong id="b17591192851510"><a name="b17591192851510"></a><a name="b17591192851510"></a>SurfaceBuffer</strong> object to be produced.</p>
</td>
</tr>
<tr id="row379mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p381mcpsimp"><a name="p381mcpsimp"></a><a name="p381mcpsimp"></a>CancelBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p383mcpsimp"><a name="p383mcpsimp"></a><a name="p383mcpsimp"></a>Cancels a <strong id="b11160135061513"><a name="b11160135061513"></a><a name="b11160135061513"></a>SurfaceBuffer</strong> object to be produced.</p>
</td>
</tr>
<tr id="row384mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p386mcpsimp"><a name="p386mcpsimp"></a><a name="p386mcpsimp"></a>FlushBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p388mcpsimp"><a name="p388mcpsimp"></a><a name="p388mcpsimp"></a>Flushes a produced <strong id="b19478235151718"><a name="b19478235151718"></a><a name="b19478235151718"></a>SurfaceBuffer</strong> object with certain information.</p>
</td>
</tr>
<tr id="row389mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p391mcpsimp"><a name="p391mcpsimp"></a><a name="p391mcpsimp"></a>AcquireBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p393mcpsimp"><a name="p393mcpsimp"></a><a name="p393mcpsimp"></a>Requests a <strong id="b4187111419210"><a name="b4187111419210"></a><a name="b4187111419210"></a>SurfaceBuffer</strong> object to be consumed.</p>
</td>
</tr>
<tr id="row394mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p396mcpsimp"><a name="p396mcpsimp"></a><a name="p396mcpsimp"></a>ReleaseBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p398mcpsimp"><a name="p398mcpsimp"></a><a name="p398mcpsimp"></a>Returns a consumed <strong id="b626752116217"><a name="b626752116217"></a><a name="b626752116217"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row399mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p401mcpsimp"><a name="p401mcpsimp"></a><a name="p401mcpsimp"></a>GetQueueSize</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p403mcpsimp"><a name="p403mcpsimp"></a><a name="p403mcpsimp"></a>Obtains the number of concurrent buffers.</p>
</td>
</tr>
<tr id="row404mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p406mcpsimp"><a name="p406mcpsimp"></a><a name="p406mcpsimp"></a>SetQueueSize</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p408mcpsimp"><a name="p408mcpsimp"></a><a name="p408mcpsimp"></a>Sets the number of concurrent buffers.</p>
</td>
</tr>
<tr id="row409mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p411mcpsimp"><a name="p411mcpsimp"></a><a name="p411mcpsimp"></a>SetDefaultWidthAndHeight</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p413mcpsimp"><a name="p413mcpsimp"></a><a name="p413mcpsimp"></a>Sets the default width and height.</p>
</td>
</tr>
<tr id="row414mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p416mcpsimp"><a name="p416mcpsimp"></a><a name="p416mcpsimp"></a>GetDefaultWidth</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p418mcpsimp"><a name="p418mcpsimp"></a><a name="p418mcpsimp"></a>Obtains the default width.</p>
</td>
</tr>
<tr id="row419mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p421mcpsimp"><a name="p421mcpsimp"></a><a name="p421mcpsimp"></a>GetDefaultHeight</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p423mcpsimp"><a name="p423mcpsimp"></a><a name="p423mcpsimp"></a>Obtains the default height.</p>
</td>
</tr>
<tr id="row424mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p426mcpsimp"><a name="p426mcpsimp"></a><a name="p426mcpsimp"></a>SetUserData</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p428mcpsimp"><a name="p428mcpsimp"></a><a name="p428mcpsimp"></a>Stores string data, which will not be transferred through IPC.</p>
</td>
</tr>
<tr id="row429mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p431mcpsimp"><a name="p431mcpsimp"></a><a name="p431mcpsimp"></a>GetUserData</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p433mcpsimp"><a name="p433mcpsimp"></a><a name="p433mcpsimp"></a>Obtains string data.</p>
</td>
</tr>
<tr id="row434mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p436mcpsimp"><a name="p436mcpsimp"></a><a name="p436mcpsimp"></a>RegisterConsumerListener</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p438mcpsimp"><a name="p438mcpsimp"></a><a name="p438mcpsimp"></a>Registers a consumer listener to listen for buffer flush events.</p>
</td>
</tr>
<tr id="row439mcpsimp"><td class="cellrowborder" valign="top" width="41%" headers="mcps1.1.3.1.1 "><p id="p441mcpsimp"><a name="p441mcpsimp"></a><a name="p441mcpsimp"></a>UnregisterConsumerListener</p>
</td>
<td class="cellrowborder" valign="top" width="59%" headers="mcps1.1.3.1.2 "><p id="p443mcpsimp"><a name="p443mcpsimp"></a><a name="p443mcpsimp"></a>Unregiseters a consumer listener.</p>
</td>
</tr>
</tbody>
</table>

### SurfaceBuffer<a name="section12001640184711"></a>

<a name="table445mcpsimp"></a>
<table><thead align="left"><tr id="row450mcpsimp"><th class="cellrowborder" valign="top" width="32%" id="mcps1.1.3.1.1"><p id="p452mcpsimp"><a name="p452mcpsimp"></a><a name="p452mcpsimp"></a><strong id="b1560753920274"><a name="b1560753920274"></a><a name="b1560753920274"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="68%" id="mcps1.1.3.1.2"><p id="p455mcpsimp"><a name="p455mcpsimp"></a><a name="p455mcpsimp"></a><strong id="b010044116274"><a name="b010044116274"></a><a name="b010044116274"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row458mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p460mcpsimp"><a name="p460mcpsimp"></a><a name="p460mcpsimp"></a>GetBufferHandle</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p462mcpsimp"><a name="p462mcpsimp"></a><a name="p462mcpsimp"></a>Obtains the <strong id="b15990748192713"><a name="b15990748192713"></a><a name="b15990748192713"></a>BufferHandle</strong> pointer to the <strong id="b10706250132715"><a name="b10706250132715"></a><a name="b10706250132715"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row463mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p465mcpsimp"><a name="p465mcpsimp"></a><a name="p465mcpsimp"></a>GetWidth</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p467mcpsimp"><a name="p467mcpsimp"></a><a name="p467mcpsimp"></a>Obtains the width of the <strong id="b109605268315"><a name="b109605268315"></a><a name="b109605268315"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row468mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p470mcpsimp"><a name="p470mcpsimp"></a><a name="p470mcpsimp"></a>GetHeight</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p472mcpsimp"><a name="p472mcpsimp"></a><a name="p472mcpsimp"></a>Obtains the height of the <strong id="b142632918311"><a name="b142632918311"></a><a name="b142632918311"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row473mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p475mcpsimp"><a name="p475mcpsimp"></a><a name="p475mcpsimp"></a>GetFormat</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p477mcpsimp"><a name="p477mcpsimp"></a><a name="p477mcpsimp"></a>Obtains the color format of the <strong id="b11157143123112"><a name="b11157143123112"></a><a name="b11157143123112"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row478mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p480mcpsimp"><a name="p480mcpsimp"></a><a name="p480mcpsimp"></a>GetUsage</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p482mcpsimp"><a name="p482mcpsimp"></a><a name="p482mcpsimp"></a>Obtains the usage of the <strong id="b15685153383114"><a name="b15685153383114"></a><a name="b15685153383114"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row483mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p485mcpsimp"><a name="p485mcpsimp"></a><a name="p485mcpsimp"></a>GetPhyAddr</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p487mcpsimp"><a name="p487mcpsimp"></a><a name="p487mcpsimp"></a>Obtains the physical address of the <strong id="b1569003614310"><a name="b1569003614310"></a><a name="b1569003614310"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row488mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p490mcpsimp"><a name="p490mcpsimp"></a><a name="p490mcpsimp"></a>GetKey</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p492mcpsimp"><a name="p492mcpsimp"></a><a name="p492mcpsimp"></a>Obtains the key of the <strong id="b1679393910314"><a name="b1679393910314"></a><a name="b1679393910314"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row493mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p495mcpsimp"><a name="p495mcpsimp"></a><a name="p495mcpsimp"></a>GetVirAddr</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p497mcpsimp"><a name="p497mcpsimp"></a><a name="p497mcpsimp"></a>Obtains the virtual address of the <strong id="b56714253110"><a name="b56714253110"></a><a name="b56714253110"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row498mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p500mcpsimp"><a name="p500mcpsimp"></a><a name="p500mcpsimp"></a>GetSize</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p502mcpsimp"><a name="p502mcpsimp"></a><a name="p502mcpsimp"></a>Obtains the size of the <strong id="b12547184533112"><a name="b12547184533112"></a><a name="b12547184533112"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row503mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p505mcpsimp"><a name="p505mcpsimp"></a><a name="p505mcpsimp"></a>SetInt32</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p507mcpsimp"><a name="p507mcpsimp"></a><a name="p507mcpsimp"></a>Sets the 32-bit integer for the <strong id="b284244873112"><a name="b284244873112"></a><a name="b284244873112"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row508mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p510mcpsimp"><a name="p510mcpsimp"></a><a name="p510mcpsimp"></a>GetInt32</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p512mcpsimp"><a name="p512mcpsimp"></a><a name="p512mcpsimp"></a>Obtains the 32-bit integer for the <strong id="b11398105163116"><a name="b11398105163116"></a><a name="b11398105163116"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row513mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p515mcpsimp"><a name="p515mcpsimp"></a><a name="p515mcpsimp"></a>SetInt64</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p517mcpsimp"><a name="p517mcpsimp"></a><a name="p517mcpsimp"></a>Sets the 64-bit integer for the <strong id="b19859155463116"><a name="b19859155463116"></a><a name="b19859155463116"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
<tr id="row518mcpsimp"><td class="cellrowborder" valign="top" width="32%" headers="mcps1.1.3.1.1 "><p id="p520mcpsimp"><a name="p520mcpsimp"></a><a name="p520mcpsimp"></a>GetInt64</p>
</td>
<td class="cellrowborder" valign="top" width="68%" headers="mcps1.1.3.1.2 "><p id="p522mcpsimp"><a name="p522mcpsimp"></a><a name="p522mcpsimp"></a>Obtains the 64-bit integer for the <strong id="b753775763118"><a name="b753775763118"></a><a name="b753775763118"></a>SurfaceBuffer</strong> object.</p>
</td>
</tr>
</tbody>
</table>

### VsyncHelper<a name="section1392116294211"></a>

<a name="table524mcpsimp"></a>
<table><thead align="left"><tr id="row529mcpsimp"><th class="cellrowborder" valign="top" width="38%" id="mcps1.1.3.1.1"><p id="p531mcpsimp"><a name="p531mcpsimp"></a><a name="p531mcpsimp"></a><strong id="b133428253019"><a name="b133428253019"></a><a name="b133428253019"></a>API</strong></p>
</th>
<th class="cellrowborder" valign="top" width="62%" id="mcps1.1.3.1.2"><p id="p534mcpsimp"><a name="p534mcpsimp"></a><a name="p534mcpsimp"></a><strong id="b16934026309"><a name="b16934026309"></a><a name="b16934026309"></a>Description</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row537mcpsimp"><td class="cellrowborder" valign="top" width="38%" headers="mcps1.1.3.1.1 "><p id="p539mcpsimp"><a name="p539mcpsimp"></a><a name="p539mcpsimp"></a>Current</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.1.3.1.2 "><p id="p541mcpsimp"><a name="p541mcpsimp"></a><a name="p541mcpsimp"></a>Obtains the <strong id="b25453472308"><a name="b25453472308"></a><a name="b25453472308"></a>VsyncHelper</strong> object of the current runner.</p>
</td>
</tr>
<tr id="row542mcpsimp"><td class="cellrowborder" valign="top" width="38%" headers="mcps1.1.3.1.1 "><p id="p544mcpsimp"><a name="p544mcpsimp"></a><a name="p544mcpsimp"></a>VsyncHelper</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.1.3.1.2 "><p id="p546mcpsimp"><a name="p546mcpsimp"></a><a name="p546mcpsimp"></a>Constructs a <strong id="b157281123143212"><a name="b157281123143212"></a><a name="b157281123143212"></a>VsyncHelper</strong> object using an <strong id="b1650443412326"><a name="b1650443412326"></a><a name="b1650443412326"></a>EventHandler</strong> object.</p>
</td>
</tr>
<tr id="row547mcpsimp"><td class="cellrowborder" valign="top" width="38%" headers="mcps1.1.3.1.1 "><p id="p549mcpsimp"><a name="p549mcpsimp"></a><a name="p549mcpsimp"></a>RequestFrameCallback</p>
</td>
<td class="cellrowborder" valign="top" width="62%" headers="mcps1.1.3.1.2 "><p id="p551mcpsimp"><a name="p551mcpsimp"></a><a name="p551mcpsimp"></a>Registers a frame callback.</p>
</td>
</tr>
</tbody>
</table>

## Usage<a name="section18359134910422"></a>

### Transferring a Producer Surface<a name="section193464304411"></a>

1.  Named service
    -   Service registration:

        ```
        // Obtain a consumer surface.
        sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
        // Extract the producer object.
        sptr<IBufferProducer> producer = surface->GetProducer();
        // Register the service.
        auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sm->AddSystemAbility(IPC_SA_ID, producer->AsObject());
        ```

    -   Construction of a producer surface:

        ```
        // Obtain a producer object.
        auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> robj = sm->GetSystemAbility(IPC_SA_ID);
        // Construct a surface.
        sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(robj);
        sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
        ```


2.  Anonymous service
    -   Sending of a surface:

        ```
        // Obtain a consumer surface.
        sptr<Surface> surface = CreateSurfaceAsConsumer();
        // Extract the producer object.
        sptr<IRemoteObject> producer = surface->GetProducer();
        // Return the producer object to the client.
        parcel.WriteRemoteObject(producer);
        ```



### Creating a Producer Surface<a name="section1276823075112"></a>

```
// Obtain a producer object.
sptr<IRemoteObject> remoteObject = parcel.ReadRemoteObject();
// Construct a surface.
sptr<IBufferProducer> bp = iface_cast<IBufferProducer>(robj);
sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bp);
```

### Producing a SurfaceBuffer<a name="section614545716513"></a>

```
// Prerequisite: a producer surface
BufferRequestConfig requestConfig = {
    .width = 1920, // Screen width
    .height = 1080, // Screen height
    .strideAlignment = 8, // Stride alignment byte
    .format = PIXEL_FMT_RGBA_8888, // Color format
    .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA, // Usage
    .timeout = 0, // Delay
};

sptr<SurfaceBuffer> buffer;
int32_t releaseFence;

SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, requestConfig);
if (ret != SURFACE_ERROR_OK) {
    // failed
}

BufferFlushConfig flushConfig = {
    .damage = {                   // Redrawing buffer zone
        .x = 0,                   // Horizontal coordinate of the start point
        .y = 0,                   // Vertical coordinate of the start point
        .w = buffer->GetWidth(), // Width of the buffer zone
        .h = buffer->GetHeight(), // Height of the buffer zone
    },
    .timestamp = 0 // Time displayed to consumers. Value 0 indicates the current time.
};

ret = surface->FlushBuffer(buffer, -1, flushConfig);
if (ret != SURFACE_ERROR_OK) {
    // failed
}
```

### Consuming a SurfaceBuffer<a name="section315285412535"></a>

```
// Prerequisite: a consumer surface
class TestConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override {
        sptr<SurfaceBuffer> buffer;
        int32_t flushFence;
        SurfaceError ret = surface->AcquireBuffer(buffer, flushFence, timestamp, damage);
        if (ret != SURFACE_ERROR_OK) {
            // failed
        }
        // ...
        ret = surface->ReleaseBuffer(buffer, -1);
        if (ret != SURFACE_ERROR_OK) {
            // failed
        }
    }
};

sptr<IBufferConsumerListener> listener = new TestConsumerListener();
SurfaceError ret = surface->RegisterConsumerListener(listener);
if (ret != SURFACE_ERROR_OK) {
    // failed
}
```

### Adding Custom Data to a SurfaceBuffer<a name="section612412125616"></a>

```
sptr<SurfaceBuffer> buffer;
SurfaceError ret = buffer->SetInt32(1, 3);
if (ret != SURFACE_ERROR_OK) {
// failed
}

int32_t val;
ret = buffer->GetInt32(1, val);
if (ret != SURFACE_ERROR_OK) {
// failed
}
```

### Registering a Vsync Callback Listener<a name="section1148115214576"></a>

1.  Construct a  **VsyncHelper**  object using  **handler**.

    ```
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto helper = new VsyncHelper(handler);
    runner->Run();
    
    struct FrameCallback cb = {
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = [](int64_t timestamp, void* userdata) {
        },
    };
    
    VsyncError ret = helper->RequestFrameCallback(cb);
    if (ret != VSYNC_ERROR_OK) {
        // failed
    }
    ```

2.  Use  **Current**  in  **handler**.

    ```
    auto runner = AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask([]() {
        auto helper = VsyncHelper::Current();
        struct FrameCallback cb = {
            .timestamp_ = 0,
            .userdata_ = nullptr,
            .callback_ = [](int64_t timestamp, void* userdata) {
            },
        };
        VsyncError ret = helper->RequestFrameCallback(cb);
        if (ret != VSYNC_ERROR_OK) {
            // failed
        }
    });
    
    runner->Run();
    ```


## Repositories Involved<a name="section6488145313"></a>

Graphics subsystem

**graphic\_standard**

ace\_ace\_engine

aafwk\_L2

multimedia\_media\_standard

multimedia\_camera\_standard

