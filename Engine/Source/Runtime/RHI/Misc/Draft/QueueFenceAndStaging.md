# Queue / Fence / Staging 実装方針

## 概要

CommandList の実行、GPU 同期、CPU readback / staging resource を整理します。
現状は `CommandList::flush()` が `Device::entryCommandList` に依存しており、Queue / Fence / wait の公開 API が不足しています。

## 実装状況

- 実装済み
  - `Fence`
  - `Device::executeCommandLists`
  - `Device::waitForIdle`
  - `Device::queueWait`
  - `CpuAccess`
  - `Device::mapBuffer` / `Device::unmapBuffer`
  - 純粋な `CopySource` バッファを upload 用 CPU 可視バッファとして生成
  - 純粋な `CopyDest` バッファを readback 用 CPU 可視バッファとして生成
  - Buffer copy -> readback の roundtrip テスト
  - Texture write -> texture to readback buffer の roundtrip テスト
  - `CommandList::flush()` の削除と `Device::executeCommandLists()` への移行
- 制限事項 / 今後の拡張
  - 専用 `StagingTexture` API は追加せず、現時点では `CopyDest` buffer と `Device::mapBuffer()` を texture readback の標準経路にします。
  - Graphics / Compute / Copy は API 上は `CommandQueueType` で分けていますが、現時点のバックエンド実装は同一ネイティブキューへ集約します。独立ネイティブキューは queue family / ownership transfer / cross-queue barrier を含む別仕様として扱います。

## 目的

- Graphics / Compute / Copy queue を RHI で扱えるようにする。
- GPU 完了待ちと queue 間同期を明示できるようにする。
- Buffer / Texture の readback を安全に行えるようにする。
- `flush()` の Singleton 依存は互換維持を前提にせず、上位コードを `Device::executeCommandLists()` へ移行する。

## 追加 API 案

```cpp
enum class CommandQueueType {
    Graphics,
    Compute,
    Copy,
};

struct FenceDesc {
    String name;
    u64 initialValue = 0;
};

class Fence : public GraphicObject {
public:
    virtual u64 getCompletedValue() const = 0;
    virtual void wait(u64 value) = 0;
};

class Device {
public:
    virtual Ref<Fence> createFence(const FenceDesc& desc) = 0;
    virtual u64 executeCommandLists(Span<Ref<CommandList>> commandLists, CommandQueueType queue = CommandQueueType::Graphics) = 0;
    virtual void waitForIdle(CommandQueueType queue = CommandQueueType::Graphics) = 0;
    virtual void queueWait(CommandQueueType waitQueue, CommandQueueType executeQueue, u64 fenceValue) = 0;
};
```

## Staging API 案

```cpp
enum class CpuAccess {
    Read,
    Write,
    ReadWrite,
};

struct StagingTextureDesc {
    String name;
    TextureDesc texture;
    CpuAccess access = CpuAccess::Read;
};

class StagingTexture : public GraphicObject {
public:
    virtual const TextureDesc& desc() const = 0;
};

class Device {
public:
    virtual Ref<StagingTexture> createStagingTexture(const StagingTextureDesc& desc) = 0;
    virtual void* mapBuffer(const Ref<Buffer>& buffer, CpuAccess access) = 0;
    virtual void unmapBuffer(const Ref<Buffer>& buffer) = 0;
    virtual void* mapStagingTexture(const Ref<StagingTexture>& texture, const TextureSubresource& subresource, u32* rowPitch) = 0;
    virtual void unmapStagingTexture(const Ref<StagingTexture>& texture) = 0;
};
```

## DirectX12 実装方針

- `CommandQueueType` ごとに `ID3D12CommandQueue` を持つ。
- `Fence` は `ID3D12Fence` と Win32 event を持つ。
- `executeCommandLists` は実行後に fence value を signal して返す。
- `queueWait` は `ID3D12CommandQueue::Wait` を使用する。
- staging / readback は readback heap / upload heap を使用する。

## Vulkan 実装方針

- Queue family の選択を `VulkanDevice` 初期化時に行う。
- `Fence` は `vk::Fence` または timeline semaphore を使用する。
- 初期実装は queue family が共通でもよいが、API は分けておく。
- staging は host visible memory を持つ buffer / image を使用する。
- Texture readback は基本的に staging buffer 経由にし、staging image は必要になるまで後回しでもよい。

## 移行方針

1. `Device::executeCommandLists` を追加する。
2. `Device::waitForIdle` と `Fence` を追加し、GPU完了待ちを明示化する。
3. 上位コードを `flush()` から `Device` 経由の実行に移行する。
4. `flush()` は不要になった時点で削除する。互換維持のためのラッパー実装は追加しない。
5. Texture readback は `copyTextureToBuffer` と readback buffer map を標準経路にする。専用 staging texture は必要性が出た時点で別途追加する。

## 注意点

- 初期実装では Graphics / Compute / Copy は同一ネイティブキューに集約する。
- 将来的に独立 queue を使う場合は `CommandQueueType` ごとの queue family / command queue と Feature query を追加する。
- Vulkan の queue family ownership transfer は初期実装では避け、同一 family を優先する。
- `mapBuffer` は CPU visible な Buffer だけ許可し、GPU local Buffer の readback は staging 経由にする。

## テスト方針

- CommandList を `executeCommandLists` で実行し、fence 完了を待てることを確認する。
- Buffer write -> copy -> readback の roundtrip テストを追加する。
- `waitForIdle` が GPU 作業完了後に戻ることを確認する。
