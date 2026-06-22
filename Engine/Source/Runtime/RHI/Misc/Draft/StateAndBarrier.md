# Texture / Buffer State / Barrier 実装方針

## 概要
Texture / Buffer の状態遷移を RHI の公開 API として扱えるようにします。
現状はバックエンド内部に状態キャッシュがありますが、コピー、compute、UAV、indirect を安全に扱うには明示的な状態制御が必要です。

## 目的
- コピー、アップロード、compute、UAV 書き込みを正しい resource state で実行できるようにする。
- FrameGraph や上位 Graphics 層が barrier を制御できるようにする。
- 自動バリアと手動バリアの両方を選択できるようにする。

## 公開 API
```cpp
enum class TextureState {
    Unknown,
    Common,
    ShaderResource,
    UnorderedAccess,
    RenderTarget,
    DepthRead,
    DepthWrite,
    CopySource,
    CopyDest,
    ResolveSource,
    ResolveDest,
    Present,
};

enum class BufferState {
    Unknown,
    Common,
    Vertex,
    Index,
    Constant,
    ShaderResource,
    UnorderedAccess,
    IndirectArgument,
    CopySource,
    CopyDest,
};

struct TextureBarrier {
    Ref<Texture> texture;
    TextureSubresourceRange range;
    TextureState before = TextureState::Unknown;
    TextureState after = TextureState::Unknown;
};

struct BufferBarrier {
    Ref<Buffer> buffer;
    BufferState before = BufferState::Unknown;
    BufferState after = BufferState::Unknown;
};

class CommandList {
public:
    virtual void setEnableAutomaticBarriers(bool enable) = 0;
    virtual void addTextureBarrier(const TextureBarrier& barrier) = 0;
    virtual void addBufferBarrier(const BufferBarrier& barrier) = 0;
    virtual void addUAVBarrier(const Ref<Texture>& texture) = 0;
    virtual void addUAVBarrier(const Ref<Buffer>& buffer) = 0;
    virtual void commitBarriers() = 0;
};
```

## 状態設計
- RHI 公開 API では Texture と Buffer の状態を分離します。
- `TextureBarrier` は `TextureState` のみを、`BufferBarrier` は `BufferState` のみを受け取ります。
- 共通 `ResourceState` は公開しません。バックエンド内部で必要な場合は `TextureState` / `BufferState` からネイティブ状態へ直接変換します。
- `Barrier` は `GraphicObject` を持たない純粋なパラメータ型なので `Amuse/RHI/Types/Barrier.h` に配置します。

## 自動バリア
初期実装では `CommandList` が以下のタイミングで自動遷移します。
- RenderPass 開始時の RenderTarget / DepthWrite。
- `copy*` 実行時の CopySource / CopyDest。
- `dispatch` 実行前の UAV / SRV / ConstantBuffer。
- `applySwapChain` 前後の CopySource / Present。

手動バリアを使う場合は `setEnableAutomaticBarriers(false)` を呼び、利用者が `transition*` と `commitBarriers` を明示します。

## DirectX12 実装方針
- `TextureState` / `BufferState` を `D3D12_RESOURCE_STATES` へ変換する。
- pending barrier list を `DirectX12CommandList` に持つ。
- `commitBarriers` で `ResourceBarrier` をまとめて発行する。
- UAV barrier は `D3D12_RESOURCE_BARRIER_TYPE_UAV` を使用する。
- subresource は mip / array / plane の線形 index へ変換する。

## Vulkan 実装方針
- `TextureState` / `BufferState` を `VkImageLayout`, `VkAccessFlags`, `VkPipelineStageFlags` へ変換する。
- Texture は `vkCmdPipelineBarrier` の image memory barrier を使用する。
- Buffer は buffer memory barrier を使用する。
- UAV 相当は shader write/read の memory barrier として扱う。
- Vulkan は layout と access の両方が必要なため、変換テーブルを明示的に持つ。

## 注意点
- `Unknown` は「現在状態を RHI が知らない」ことを表し、バックエンドは保守的な barrier を発行します。
- 自動バリアと手動バリアを混ぜると状態追跡が複雑になるため、CommandList 単位で制御します。
- RenderPass 中に許可されない状態遷移は validation error にします。
- `RenderTexture` と `Texture` の native resource 共通化が必要です。

## テスト方針
- Buffer copy の前後に手動 barrier を入れて成功することを確認する。
- 自動 barrier 有効時は barrier を明示しなくても copy / dispatch が成功することを確認する。
- 自動 barrier 無効時に不正状態で実行した場合、Debug build で検出する。
