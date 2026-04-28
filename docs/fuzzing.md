# Fuzzing Guide

This guide explains when to add fuzz tests for graphics IPC and RSCommand
changes, and where to wire them into the source tree.

## When To Add Fuzz Tests

Add or update fuzz coverage when a change introduces or modifies an externally
reachable input path. In this repository, that usually means:

- a new RS IPC interface;
- new parameters or behavior for an existing RS IPC interface;
- a new `RSCommand`;
- new parameters or behavior for an existing `RSCommand`;
- new parsing of app-controlled data, such as image, font, shader, JSON, or
  parcel data;
- new NDK or public API entry points that accept caller-provided data.

Internal-only algorithms do not automatically require fuzz tests. If a file is
counted in fuzz coverage but is not reachable from external input, align with
the security/domain owner before excluding it from coverage expectations.

## IPC Interfaces

New RS IPC interfaces need coverage in three places.

### 1. Client API Fuzzer

Add a case under one of the `rsinterfaces*_fuzzer` targets. This covers the
client call chain, for example:

```text
RSInterfaces -> RSRenderServiceClient -> RSClientToServiceConnectionProxy
```

Use random data for every API parameter. Keep each fuzzer target reasonably
small; existing guidance is to avoid putting too many interfaces into one
target, because long-running fuzzers mutate less effectively.

### 2. SAFuzz IPC Simulator

Add an entry to:

```text
rosen/modules/safuzz/rs_ipc_dos_simulator/configs/test_case_config.json
```

For an IPC interface, configure the parcel exactly as the proxy writes it:

- `testCaseDesc`: `<INTERFACE_NAME>_TEST_001`
- `interfaceName`: the IPC enum/interface name
- `inputParams`: semicolon-separated writer helpers, such as
  `WriteUint64;WriteInt32`
- `writeInterfaceToken`: `true` if the proxy writes the descriptor token
- `messageOption`: `TF_SYNC` or `TF_ASYNC`, matching the proxy
- `connectionType`: match nearby cases for the same connection path

If the interface needs a parcel writer that does not exist yet, add it to the
SAFuzz message parcel utility layer instead of hand-encoding data in the config.

### 3. Service Stub Fuzzer

Add a case under one of the `rsrenderserviceconnection*_fuzzer` targets. This
covers the service-side request handling path, for example:

```text
RSClientToServiceConnectionStub -> RSClientToServiceConnection
```

Build the `MessageParcel` with the same field order used by the proxy, then call
`OnRemoteRequest` with the matching interface code.

## RSCommand Changes

`RSCommand` objects are sent through `COMMIT_TRANSACTION`, so command changes
must be added to the SAFuzz transaction simulator.

### 1. Add A Config Case

Add a `COMMIT_TRANSACTION` case to:

```text
rosen/modules/safuzz/rs_ipc_dos_simulator/configs/test_case_config.json
```

Use this shape:

```json
{
  "testCaseDesc": "COMMIT_TRANSACTION_TEST_XXX",
  "interfaceName": "COMMIT_TRANSACTION",
  "inputParams": "WriteRSTransactionData",
  "writeInterfaceToken": false,
  "messageOption": "TF_ASYNC",
  "commandList": "YourCommandName*100",
  "commandListRepeat": 1,
  "connectionType": 0
}
```

Choose the next available `COMMIT_TRANSACTION_TEST_XXX` value. Match
`connectionType` with nearby `COMMIT_TRANSACTION` command cases unless the new
command clearly belongs to a different connection path.

### 2. Add A Random Command Helper

Declare a random builder in the command utility header for the command family.
For example, a surface-node command with parameters `NodeId` and `bool` uses:

```cpp
ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetDarkColorMode, Uint64, Bool);
```

Common locations include:

```text
rosen/modules/safuzz/rs_ipc_dos_simulator/command/
```

Use existing random data types when possible. Add a custom random type only when
the command parameter is a complex type that is not already supported.

### 3. Register The Command Name

Register the command in:

```text
rosen/modules/safuzz/rs_ipc_dos_simulator/transaction/rs_transaction_data_utils.cpp
```

Use the command category and command name:

```cpp
DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetDarkColorMode),
```

The string in `commandList` must exactly match the command name registered here.

### 4. Update Existing Normal Fuzzers When There Is A Matching Path

If a similar command is already covered in a normal fuzzer, add the new command
beside it. For example, if `MarkUIHidden` is covered in both the surface-node
command fuzzer and the client-side `RSSurfaceNode` fuzzer, add a related
surface-node command in the same locations.

This is not a substitute for SAFuzz command registration, but it improves
coverage of direct helper and client API paths.

## Random Data Rules

Use the framework random helpers rather than hard-coded constants:

- `Uint64` for `NodeId`, `ScreenId`, and other `uint64_t` aliases;
- `Uint32`, `Int32`, `Uint8`, and similar helpers for scalar values;
- `Bool` for boolean flags;
- existing customized random helpers for structs and smart pointers.

Only add new customized random helpers when the parameter type cannot be built
from the existing helpers.

## Validation Checklist

Before submitting fuzz-related changes:

- confirm the new config entry is valid JSON;
- confirm `commandList` matches the registered command name exactly;
- run `git diff --check`;
- run the most focused fuzz target build available;
- if the focused build is blocked by unrelated baseline failures, record the
  first unrelated compile error in the change notes;
- for AGT validation, run the SAFuzz incremental test and attach the report to
  the submit checklist.

Useful local checks:

```sh
python3 -m json.tool rosen/modules/safuzz/rs_ipc_dos_simulator/configs/test_case_config.json >/dev/null
git diff --check
hb build graphic_2d -t --skip-download --build-target <target>
```

## Example: Surface Node Bool Command

For a command such as `RSSurfaceNodeSetDarkColorMode(NodeId, bool)`, the minimum
SAFuzz command setup is:

```cpp
// rs_surface_node_command_utils.h
ADD_RANDOM_COMMAND_WITH_PARAM_2(RSSurfaceNodeSetDarkColorMode, Uint64, Bool);
```

```cpp
// rs_transaction_data_utils.cpp
DECLARE_ADD_RANDOM(RSSurfaceNodeCommand, RSSurfaceNodeSetDarkColorMode),
```

```json
{
  "testCaseDesc": "COMMIT_TRANSACTION_TEST_XXX",
  "interfaceName": "COMMIT_TRANSACTION",
  "inputParams": "WriteRSTransactionData",
  "writeInterfaceToken": false,
  "messageOption": "TF_ASYNC",
  "commandList": "RSSurfaceNodeSetDarkColorMode*100",
  "commandListRepeat": 1,
  "connectionType": 0
}
```

If the same feature has a client API wrapper, also add it beside similar calls
in the corresponding normal fuzzer.
