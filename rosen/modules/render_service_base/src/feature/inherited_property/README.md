# Inherited Property Framework

## Overview

The Inherited Property framework provides a unified way to manage per-node inheritable
attributes (e.g. ColorPicker-like properties that descendant nodes inherit). It offers:

- `IInheritedProperty`: a polymorphic base class so different property types are managed
  through identical parameter and return types.
- `RSInheritedPropertyManager`: storage and lookup keyed by `NodeId`, with cleanup by
  `NodeId` or by `pid`.

This framework only provides storage and lifecycle management. Business behavior
(e.g. how a property is computed or rendered) belongs to the concrete property classes.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                     RSMainThread::CleanResources                     │
│              (calls ClearInheritedProperties on process exit)        │
└────────────────────────────┬────────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│                            RSContext                                 │
│         (owns RSInheritedPropertyManager as a member,                │
│          accessed via GetMutableInheritedPropertyManager())          │
└────────────────────────────┬────────────────────────────────────────┘
                             │ manages
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│                    RSInheritedPropertyManager                        │
│   map<NodeId, map<InheritedPropertyType, shared_ptr<Property>>>      │
│   - Store(nodeId, property)   - Get(nodeId, type) / GetAs<T>()       │
│   - Clear(nodeId[, type])     - ClearByPid(pid)                      │
└────────────────────────────┬────────────────────────────────────────┘
                             │ holds
                             ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        IInheritedProperty                            │
│        (base interface: GetType() discriminator only)                │
└────────────────────────────┬────────────────────────────────────────┘
                             │ implements
                             ▼
                  ┌─────────────────────┐
                  │ concrete properties │
                  │  (e.g. ColorPicker) │
                  └─────────────────────┘
```

## Key Semantics

- **Storage key**: a property is stored under `(nodeId, property->GetType())`. One node
  may hold multiple property types; storing the same type again overwrites the old one.
- **`InheritedPropertyType::NONE` and null properties are never stored.**
- **Type-safe read**: `GetAs<T>(nodeId, type)` verifies the stored property's runtime
  `GetType()` before `static_pointer_cast`, and returns `nullptr` on mismatch.
- **pid cleanup**: `ClearByPid(pid)` removes all nodes whose `ExtractPid(nodeId) == pid`.

## Threading

`RSInheritedPropertyManager` has no internal locking. Thread affinity follows its owner
`RSContext` (RS main thread). If another thread needs a property value, copy it on the
main thread and pass the copy; do not share the manager across threads.

## Lifecycle and Cleanup

- **Process exit**: `RSMainThread::CleanResources(pid)` calls
  `ClearInheritedProperties(pid)`, which forwards to `ClearByPid(pid)`.
- **Node-level cleanup**: `Clear(nodeId)` removes all property types of a node, while
  `Clear(nodeId, type)` removes a single type (the node entry is dropped once its last
  type is cleared). Both are invoked by the integrating feature when the node removes
  the property; the framework does not hook node destruction itself.

## Integration Steps (for a new property type)

1. Append a new entry to `InheritedPropertyType` in
   `render_service_base/include/feature/inherited_property/i_inherited_property.h`.
2. Implement a concrete class deriving from `IInheritedProperty`; add business behavior
   interfaces as needed.
3. Store/read through `RSContext::GetMutableInheritedPropertyManager()` on the main thread.
4. Call `Clear(nodeId)` where the node removes the property.

## Tests

- `RSInheritedPropertyManagerTest` (`render_service_base/unittest/feature/inherited_property/`):
  covers Store/Get/GetAs/Clear/ClearByPid branches and the `RSContext` accessors.
- `RSMainThreadTest.ClearInheritedPropertiesTest`: covers null-context tolerance and
  pid-scoped cleanup through `RSMainThread`.
