# Compute / Indirect 実装方針

## 概要
Compute pipeline の実行 API と、GPU バッファを引数にする indirect draw / dispatch を追加します。
現状は `ComputePipelineState` が存在しますが、`CommandList` から compute pipeline を設定して dispatch する公開 API が不足しています。

## 目的
- GPU culling、particle simulation、post process、texture generation を RHI で扱えるようにする。
- indirect draw により CPU を介さない描画数制御を可能にする。
- 既存の `IndirectDraw.md` を実装可能な正式仕様へ発展させる。

## 追加 API 案
```cpp
struct DispatchParam {
    u32 groupX = 1;
    u32 groupY = 1;
    u32 groupZ = 1;
};

struct IndirectCommandParam {
    Ref<Buffer> argumentBuffer;
    u64 argumentOffset = 0;
    u32 commandCount = 1;
};

class CommandList {
public:
    virtual void setComputePipelineState(const Ref<ComputePipelineState>& pipeline) = 0;
    virtual void dispatch(const DispatchParam& param) = 0;
    virtual void dispatch(u32 groupX, u32 groupY = 1, u32 groupZ = 1) = 0;

    virtual void drawIndirect(const IndirectCommandParam& param) = 0;
    virtual void drawIndexedIndirect(const IndirectCommandParam& param) = 0;
    virtual void dispatchIndirect(const IndirectCommandParam& param) = 0;
};
```

## Indirect 引数構造
DirectX12 と Vulkan で基本的な引数構造は近いですが、将来の拡張や backend 固有差異を考慮し、利用者が Buffer に直接 ABI 構造を書き込む前提にはしません。
RHI には `DrawParam` / `DrawIndexedParam` / `DispatchParam` から indirect argument buffer を書き込むユーティリティを用意します。

```cpp
class Buffer {
public:
    virtual void writeDrawParam(const DrawParam& param, u64 offset = 0);
    virtual void writeDrawParams(Span<const DrawParam> params, u64 offset = 0);
    virtual void writeDrawIndexedParam(const DrawIndexedParam& param, u64 offset = 0);
    virtual void writeDrawIndexedParams(Span<const DrawIndexedParam> params, u64 offset = 0);
    virtual void writeDispatchParam(const DispatchParam& param, u64 offset = 0);
    virtual void writeDispatchParams(Span<const DispatchParam> params, u64 offset = 0);
};

class Device {
public:
    virtual u32 getDrawIndirectArgumentSize() const;
    virtual u32 getDrawIndexedIndirectArgumentSize() const;
    virtual u32 getDispatchIndirectArgumentSize() const;
};
```

初期実装では DirectX12 / Vulkan の共通 ABI に合わせて書き込みます。
ただしサイズ取得は `Device` 経由、実データの書き込みは `Buffer` 経由にし、backend 差異が出た場合は派生 `Device` / `Buffer` で差し替えます。
`IndirectArgument` は `Buffer` / `Device` の薄いラッパーにしかならないため公開 API としては持ちません。

## DirectX12 実装方針
- `setComputePipelineState` は `SetComputeRootSignature` と `SetPipelineState` を呼ぶ。
- `dispatch` は `ID3D12GraphicsCommandList::Dispatch` を呼ぶ。
- indirect は `ID3D12CommandSignature` が必要です。
- command signature は `Device` またはバックエンド内部キャッシュで管理する。
- `argumentBuffer` は `D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT` へ遷移する。

## Vulkan 実装方針
- `setComputePipelineState` は `vkCmdBindPipeline(..., vk::PipelineBindPoint::eCompute, ...)` を呼ぶ。
- `dispatch` は `vkCmdDispatch` を呼ぶ。
- indirect は `vkCmdDrawIndirect` / `vkCmdDrawIndexedIndirect` / `vkCmdDispatchIndirect` を呼ぶ。
- `argumentBuffer` は `VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT` が必要です。

## BufferDesc への追加確認
既存の `BufferFlag::IndirectArgument` は存在するため、以下を確認します。
- DirectX12: resource state として indirect argument に遷移できること。
- Vulkan: buffer usage に `eIndirectBuffer` が付与されること。
- validation: indirect API に渡す Buffer は `IndirectArgument` を持つ必要があること。

## 注意点
- Compute と Graphics の RootSignature / DescriptorTable 設定は共通化できますが、DirectX12 では Graphics / Compute で root signature 設定 API が別です。
- `setRootConstant` は compute pipeline にも適用できる必要があります。
- `drawIndirect` / `drawIndexedIndirect` の stride は `Device::get*IndirectArgumentSize()` から取得します。将来的に count buffer 対応を追加します。

## テスト方針
- `dispatch` で RWBuffer に値を書き、readback で確認する。
- `drawIndirect` は既存 Triangle テストを indirect argument buffer 経由に置き換えたテストを追加する。
- `dispatchIndirect` は CPU で作成した indirect buffer から 1 group dispatch できることを確認する。
