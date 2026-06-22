# Advanced Rendering Features 実装方針

## 概要
Query、Mesh Shader、Ray Tracing などの大型機能を扱います。
これらは nvrhi には含まれていますが、OctbitEngine では基盤機能の整備後に個別仕様化する方針です。

## 実装状況
- 実装済み
  - `EventQuery`
  - `Device::createEventQuery`
  - `Device::pollEventQuery`
  - `Device::waitEventQuery`
  - `CommandList::setEventQuery`
  - `TimerQuery`
  - `CommandList::beginTimerQuery`
  - `CommandList::endTimerQuery`
  - DirectX12 / Vulkan の EventQuery / TimerQuery 実装
- 制限事項 / 今後の拡張
  - Mesh Shader / Task Shader
  - Variable Rate Shading の利用 API
  - Conservative Rasterization の利用 API
  - Ray Tracing Pipeline
  - Acceleration Structure
  - Shader Table

## 対象機能
- EventQuery / TimerQuery
- Mesh Shader / Task Shader
- Variable Rate Shading
- Conservative Rasterization
- Ray Tracing Pipeline
- Acceleration Structure
- Shader Table

## 対象外の機能
- ShaderLibrary は追加しません。
  - 複数の Shader を束ねる用途は `Vector<Ref<Shader>>` や上位の Asset / Graphics 層の管理オブジェクトで表現します。
  - RHI では個別の `Shader` を生成、保持、バインドできれば十分です。
- Shader specialization は追加しません。
  - パイプラインバリアントは Shader コンパイル時または上位層のシェーダ管理で扱います。
  - RHI の責務は最終的な Shader binary と PipelineState を受け取ることに限定します。
- Native interop API は RHI の共通 API として追加しません。
  - backend 固有型へアクセスしたい場合は、`texture.cast<DirectX12Texture>()->getNativeResource()` のように具象型へ安全に cast して取得します。
  - 共通 RHI ヘッダに `void*` ベースの native handle を追加しません。

## Query API 案
```cpp
class EventQuery : public GraphicObject {};
class TimerQuery : public GraphicObject {};

class Device {
public:
    virtual Ref<EventQuery> createEventQuery() = 0;
    virtual bool pollEventQuery(const Ref<EventQuery>& query) = 0;
    virtual void waitEventQuery(const Ref<EventQuery>& query) = 0;

    virtual Ref<TimerQuery> createTimerQuery() = 0;
    virtual bool pollTimerQuery(const Ref<TimerQuery>& query) = 0;
    virtual f32 getTimerQueryTime(const Ref<TimerQuery>& query) = 0;
};

class CommandList {
public:
    virtual void beginTimerQuery(const Ref<TimerQuery>& query) = 0;
    virtual void endTimerQuery(const Ref<TimerQuery>& query) = 0;
};
```

## Mesh Shader 方針
- 既存の `ShaderStage::Task` / `ShaderStage::Mesh` を利用する。
- `MeshPipelineState` を追加するか、既存 `PipelineStateDesc` に mesh shader 用フィールドを追加するかを検討する。
- `CommandList::dispatchMesh(u32 groupX, u32 groupY, u32 groupZ)` を追加する。
- `Device::getFeatures().meshShader` を確認してから利用する。

## Ray Tracing 方針
Ray Tracing は追加範囲が大きいため、以下に分割します。
1. Acceleration Structure resource
2. BLAS / TLAS build command
3. RayTracingPipeline
4. ShaderTable
5. `dispatchRays`
6. Descriptor binding で acceleration structure を扱う仕組み

```cpp
class AccelerationStructure : public GraphicObject {};
class RayTracingPipelineState : public GraphicObject {};
class ShaderTable : public GraphicObject {};

class CommandList {
public:
    virtual void buildBottomLevelAS(const Ref<AccelerationStructure>& as, Span<const GeometryDesc> geometries) = 0;
    virtual void buildTopLevelAS(const Ref<AccelerationStructure>& as, Span<const InstanceDesc> instances) = 0;
    virtual void dispatchRays(const DispatchRaysParam& param) = 0;
};
```

## 実装順の推奨
1. TimerQuery
2. Mesh Shader
3. Ray Tracing

## 注意点
- Query は Debug / Profiler と連携する可能性が高いため、Debug モジュール側の要件も確認する。
- Ray Tracing は Descriptor / Barrier / Queue / Staging が揃ってから着手する。
- Mesh Shader と Ray Tracing は Feature query 必須にする。
- Native resource 取得は DirectX12RHI / VulkanRHI などの具象型 API として実装し、RHI 共通 API には含めない。

## テスト方針
- TimerQuery は GPU 時間が 0 以上で取得できることを確認する。
- Mesh Shader / Ray Tracing は対応 GPU のみで実行する条件付きテストにする。
