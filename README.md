# ToyBox

**DirectX 12 기반 Rendering & UI Engine**

ToyBox는 DirectX 12를 기반으로 렌더링 파이프라인과 리소스 관리, 비동기 로딩, UI 및 에디터 기능을 직접 설계하고 구현한 개인 엔진 프로젝트입니다.

단순한 렌더링 기능 구현에 그치지 않고, 프로젝트가 커질수록 발생하는 **렌더링 의존성, GPU 동기화, 리소스 수명, 멀티 뷰, 비동기 로딩 등의 문제를 구조적으로 해결하는 것**을 주요 목표로 개발하고 있습니다.

현재는 단일 뷰 기반 렌더러에서 발전하여 **RenderGraph와 GPU TaskScheduler를 중심으로 한 렌더링 아키텍처**를 구축하고 있으며, 여러 View를 독립적으로 렌더링하고 Composite하는 구조까지 확장했습니다.

---

## Rendering

ToyBox의 핵심 영역입니다.

DirectX 12의 Command Queue, Resource Barrier, Descriptor, Fence 등의 저수준 개념을 직접 관리하면서 렌더링 시스템을 구성하고 있습니다.

### RenderGraph

렌더링 Pass 사이의 리소스 의존성을 기반으로 실행 순서와 Resource Barrier를 구성하는 RenderGraph를 구현했습니다.

* Pass 기반 렌더링
* Resource Import / Export
* Resource Access 기반 Dependency 추적
* Resource State 추적
* 자동 Barrier 생성
* Pass 간 Dependency 처리
* Graph Fork 지원
* Transient Resource 관리
* RenderGraph 실행 시 Descriptor Heap 구성
* RenderGraph Resource를 빠르게 조회하기 위한 ID 기반 접근

렌더링 코드의 실행 순서에 의존하지 않고, **각 Pass가 어떤 Resource를 어떻게 사용하는지를 기반으로 Graph를 구성**하는 방향으로 발전시켰습니다.

---

### GPU Task Scheduler

초기에는 CPU/GPU 작업을 함께 처리하는 Task 구조에서 시작했지만, 현재는 렌더링에 필요한 GPU 작업의 실행과 동기화를 RenderGraph와 결합하는 방향으로 발전시켰습니다.

* GPU Queue 기반 Task 실행
* Direct / Compute / Copy Queue 지원
* Task 간 Dependency
* Fence 기반 GPU Synchronization
* Task 및 GPU 작업이 남아있는 상태에서의 안전한 Shutdown
* RenderGraph와 TaskScheduler를 이용한 GPU 작업 관리

최근 구조에서는 CPU 작업과 GPU 작업의 책임을 분리하고 **GPU 작업의 lifetime을 Context와 Graph execution을 중심으로 관리**하도록 정리했습니다.

---

### Multi-View Rendering

기존 Single View 구조를 Multi-View 구조로 확장했습니다.

각 View가 독립적인 Camera와 Viewport, Render Data를 가지도록 구성하고, 하나의 Frame에서 여러 View를 렌더링할 수 있도록 변경했습니다.

* View 단위 Camera
* View 단위 UI Camera
* Viewport
* View별 Render Override
* View별 Draw Data
* 공통 Scene Data
* Shadow Resource 공유
* Multi-View Text Rendering
* View별 Constant Buffer 관리
* View Target Resource

현재 View는 최대 10개까지 관리할 수 있도록 구성되어 있으며, View ID를 명시적으로 관리합니다.

예를 들어 하나의 Frame에서 다음과 같은 View 구성이 가능합니다.

```text
Main View
 ├─ Camera
 ├─ Scene
 └─ UI

Additional View
 ├─ Camera
 ├─ Scene
 ├─ Character UI
 └─ ...
```

UI Camera 역시 단순한 Screen Space Orthographic Camera뿐만 아니라 외부에서 지정할 수 있도록 구성하여 캐릭터 머리 위의 HP Bar와 같은 View-dependent UI도 표현할 수 있도록 했습니다.

---

### Composite Rendering

각 View를 독립적인 View Target에 렌더링한 뒤 최종적으로 Backbuffer에 Composite하는 구조를 구현했습니다.

```text
                ┌─ Main View ────────┐
                │                    │
Scene ──────────┼─ Shadow / Surface ─┼──> View Target
                │                    │
                └─ UI / Debug ───────┘
                         │
                         ▼
                  CompositeRenderer
                         │
                         ▼
                    Backbuffer
```

이를 통해 View와 최종 화면 출력을 분리하고, View별 Rendering Target을 독립적으로 관리할 수 있도록 구성했습니다.

---

## Rendering Pipeline

렌더링 시스템은 Service / Pipeline / Renderer / Resource 영역을 분리하는 방향으로 계속 리팩토링하고 있습니다.

```text
RenderService
     │
     ▼
RenderPipeline
     │
     ├── RenderGraph
     ├── SceneRenderer
     ├── CompositeRenderer
     └── Renderers
             ├── Surface
             ├── Shadow
             ├── UI
             ├── Debug
             └── MipGenerator
```

Renderer 내부에서 사용하는 동적 Frame Data와 Backend Data를 분리하고, `DrawPacket`을 통해 Scene과 실제 Rendering Pipeline 사이의 의존성을 줄였습니다.

---

## Resource System

렌더링 리소스와 Asset을 분리하고 Provider / Repository 구조를 사용합니다.

```text
Asset
  │
  ▼
Asset Loader
  │
  ▼
Resource
  │
  ├── Texture
  ├── Mesh
  ├── Material
  ├── Environment
  └── Font
       │
       ▼
    Provider
       │
       ▼
   Repository
```

주요 기능:

* Resource ID 기반 관리
* Strongly Typed Handle
* Resource Provider
* Repository
* Resource Lifetime 관리
* GPU Fence 기반 Release
* Pending Release Queue
* Resource Cache

최근에는 `unordered_map` 중심의 구조에서 고정된 범위의 데이터에 대해 Array/ID 기반 접근을 사용하도록 변경하고 있으며, RenderGraph에서도 Resource 접근 비용을 줄이기 위한 전용 ID Allocator를 사용하고 있습니다.

---

## Asset Pipeline

Asset loading은 CPU Loading과 GPU Upload를 분리하여 비동기적으로 처리합니다.

```text
Asset
  │
  ▼
AssetAsyncLoader
  │
  ├── File Loading
  ├── Decode
  └── Resource Creation
           │
           ▼
      GPU Upload
           │
           ▼
       Resource
```

* 비동기 Asset Loading
* Texture / Mesh / Sound Loading
* GPU Upload Budget
* GPU 작업량에 따른 Loading 조절
* Runtime Resource ID
* Shader Cache
* Built-in Asset
* Resource별 Loading / Update

Texture와 Mesh는 GPU가 수행해야 하는 작업량을 고려하여 로딩량을 조절하는 방식으로 개선했습니다.

---

## Bindless Rendering

Shader Model 6.6 기반 Bindless Resource 접근을 사용합니다.

* Bindless Descriptor
* Descriptor Allocator
* Runtime Resource Binding
* Explicit Texture Slot
* Descriptor 재사용

Texture를 암묵적인 배열 위치로 접근하는 방식에서 벗어나 명시적인 Texture Slot과 Resource ID를 사용하는 방향으로 구조를 정리했습니다.

---

## Materials & Shaders

현재 렌더링은 여러 Material과 Shader를 조합할 수 있는 구조로 구성하고 있습니다.

### Material

* PBR
* Phong
* Debug Material
* Default Material
* Runtime Material

### Shader

* Runtime Shader Registration
* Shader Key
* Shader Cache
* Pipeline State Cache
* Root Signature Builder

PBR 및 Phong Material을 공통 Surface Rendering 구조에서 처리할 수 있도록 구성하고 있습니다.

---

## Shadow & Environment

### Shadow

* Shadow Map
* Shadow Renderer
* Shadow Graph Builder
* Light View
* DSV / SRV
* Multi-View Shadow Resource 공유

Shadow Map은 여러 View에서 공통으로 사용할 수 있도록 Scene-level Resource로 관리되고 있습니다.

### Environment

HDR/EXR 환경 데이터를 기반으로 Cubemap을 구성하고 이를 Rendering에 사용합니다.

```text
EXR
 │
 ▼
Cubemap
 │
 ├── Skybox
 ├── Reflection
 └── Irradiance
```

KTX2 및 Filament `cmgen`을 이용한 Environment Asset Pipeline도 구성했습니다.

---

## Texture & Mipmap

Texture는 GPU 작업을 고려하여 Upload와 Mipmap Generation을 비동기로 처리합니다.

* Texture Upload
* Mipmap Generation
* sRGB / Linear Texture
* Data Texture
* Compute Queue 기반 Mipmap Generation
* Texture Resource Lifetime

Mipmap Generator 역시 특정 Texture 처리에 종속되지 않고 확장 가능한 구조로 정리했습니다.

---

## Text Rendering

UI와 별도로 텍스트 렌더링 시스템도 구축했습니다.

### Font

* FreeType
* HarfBuzz
* Dynamic Font Atlas
* Glyph Cache
* Font Size Bucket
* Skyline Packing

### Rendering Mode

* Bitmap
* MTSDF

### Text Layout

* Outline
* Shadow
* Gradient
* Glow
* Underline
* Horizontal / Vertical Alignment
* Word Wrap
* Overflow
* Line Spacing
* CJK Line Break
* Newline / Whitespace 처리

동적 Glyph Atlas를 구성하고 필요한 Glyph만 Runtime에 생성하여 Atlas에 추가하는 구조를 구현했습니다.

---

# Audio

Rendering과 함께 Engine Runtime에서 사용하는 Audio 시스템도 직접 구성했습니다.

```text
Sound Asset
     │
     ▼
Asset Pipeline
     │
     ├── WAV
     └── OGG
          │
          ▼
        PCM
          │
          ▼
        Mixer
          │
          ▼
      Audio Output
```

주요 기능:

* Static Sound
* Streaming Sound
* 비동기 Sound Loading
* WAV / OGG 지원
* PCM 기반 내부 포맷
* Audio Handle
* Sound Resource 관리
* Pending Sound
* Streaming Playback

WAV는 로딩시 `dr_wav`, OGG는 `libVorbis`를 사용하고, Mixer에서 처리하기 쉬운 PCM 형태로 통일했습니다. 또한 Sound 역시 Asset Pipeline을 통해 비동기적으로 로딩하도록 구성했습니다.

# Input

게임 클라이언트에서 사용하는 Input 시스템도 Engine 영역에서 분리하여 구성하고 있습니다.

* Keyboard
* Mouse
* Input State
* Event 기반 입력 처리
* UI Input 처리
* Runtime Input 전달

렌더링과 UI 시스템이 직접 플랫폼 입력에 의존하지 않도록 계층을 분리하는 것을 목표로 하고 있습니다.

---

# UI System

UI는 Composite Pattern 기반의 계층 구조를 사용합니다.

```text
UIRoot
 ├── Panel
 │    ├── Image
 │    ├── Text
 │    └── Button
 │
 └── Window
      ├── Header
      └── Content
```

주요 기능:

* UI Component
* Composite UI Tree
* Image
* Text
* Button
* Panel
* Layout
* Texture Atlas
* Source Rect
* 1 / 3 / 9 Grid

UI Component의 데이터와 Rendering을 분리하여 UI가 렌더링 시스템의 구체적인 구현에 직접 의존하지 않도록 구성했습니다.

---

# UI Event System

UI 입력은 Component Tree를 기반으로 처리합니다.

```text
Input
  │
  ▼
UI Locator
  │
  ▼
Target Component
  │
  ▼
Parent
  │
  ▼
Parent ...
```

* Event Bubbling
* Parent / Child Event Propagation
* UI Component Event Handling

이를 통해 Button과 같은 개별 UI Component가 입력 시스템 전체를 알 필요 없이 자신의 이벤트를 처리할 수 있도록 구성했습니다.

---

# UI Editor

Dear ImGui를 이용하여 Runtime UI를 편집할 수 있는 Editor 기능을 구현했습니다.

주요 기능:

* UI Tree Inspector
* Property Inspector
* UI Component 생성 / 삭제
* Component 연결 관계 시각화
* Undo / Redo
* Runtime UI Debugging

게임 실행 중 UI 구조와 속성을 확인하고 수정할 수 있도록 구성하여 **UI Runtime과 Editor를 함께 개발할 수 있는 환경**을 만들고 있습니다.

---

# Debugging & Profiling

렌더링 엔진 개발 과정에서 GPU/CPU 문제를 추적하기 위한 도구를 적극적으로 사용합니다.

* Tracy Profiler
* DirectX 12 Debug Layer
* RenderDoc
* Runtime Inspector

특히 Resource Lifetime, Command Queue Synchronization, Resource State, sRGB/Gamma와 같은 DirectX 12 특유의 문제를 추적하고 수정하는 것을 중요하게 다루고 있습니다.

---

# Architecture

현재 프로젝트는 다음과 같은 계층을 목표로 구성하고 있습니다.

```text
┌─────────────────────────────┐
│         Game Client         │
├─────────────────────────────┤
│       Engine Services       │
│    Render / Audio / Input   │
├─────────────────────────────┤
│    Backend System(Device)   │
│    Render / Audio / Input   │
├─────────────────────────────┤
│          Platform           │
│      DirectX 12 / SDL3      │
├─────────────────────────────┤
│            Core             │
│ Handle / Allocator / Memory │
│ Container / Utility         │
└─────────────────────────────┘
```

프로젝트가 커질수록 하나의 클래스에 기능을 추가하는 방식보다는 **역할과 lifetime, 데이터 흐름을 명확하게 분리하는 방향**으로 지속적으로 리팩토링하고 있습니다.

---

# Design Patterns

프로젝트 전반에서 다음과 같은 패턴과 설계 기법을 사용하고 있습니다.

* Handle-based Resource Management
* Template Method
* Composite
* Command
* Strategy
* Adapter
* Observer
* Repository
* Traits
* RAII
* CRTP
* 기타 등등

단순히 패턴을 적용하는 것보다 **실제 엔진에서 발생하는 의존성 및 lifetime 문제를 해결하기 위한 수단**으로 사용하는 것을 목표로 합니다.

---

# Technologies

| Category       | Technology                |
| -------------- | ------------------------- |
| Language       | C++20                     |
| Graphics       | DirectX 12                |
| Window / Input | SDL3                      |
| UI Tool        | Dear ImGui                |
| Profiling      | Tracy                     |
| Text           | FreeType / HarfBuzz       |
| SDF            | msdfgen                   |
| Image          | stb                       |
| Audio          | libVorbis / dr_wav        |
| Environment    | KTX-Software              |
| Serialization  | nlohmann/json             |

---

# Development Direction

단순한 기능 구현에 머무르지 않고 **실제 엔진 개발 과정에서 발생하는 성능, 동기화, 리소스 수명, 확장성 등의 문제를 해결하면서 지속적으로 사용할 수 있는 엔진 구조와 개발 환경을 만드는 것**을 목표로 합니다.

특히 현재는 다음 영역을 중심으로 개발하고 있습니다.

* RenderGraph 고도화
* GPU Task Scheduling
* Multi-View Rendering
* Resource Lifetime 관리
* Async Asset Pipeline
* Bindless Rendering
* PBR / Shadow / Environment
* Text Rendering
* UI / UI Editor
* Audio
* Input

새로운 기능을 추가하는 것뿐 아니라 기존 구조에서 발생하는 문제를 발견하고, 이를 해결하기 위해 **데이터 구조와 시스템 경계를 다시 설계하는 과정 자체**를 중요하게 생각합니다.
