# Copy / Upload / Clear 実装方針

## 概要
Buffer / Texture のコピー、CPU データのアップロード、リソースのクリア、MSAA resolve を `CommandList` に追加します。
現在 `CommandList.h` にはコピー系 API の草案がコメントアウトされていますが、公開 API とバックエンド実装が不足しています。

## 実装状況
- 実装済み
  - `BufferCopyRegion` / `TextureCopyRegion` / `BufferTextureCopyRegion`
  - `CommandList::copyBuffer`
  - `CommandList::copyTexture`
  - `CommandList::copyBufferToTexture`
  - `CommandList::copyTextureToBuffer`
  - `CommandList::writeTexture`
  - `Buffer::updateDirect`
  - `CommandList::clearColorTexture`
  - `CommandList::clearDepthStencilTexture`
  - `CommandList::clearBufferUInt`
  - `CommandList::resolveTexture`
  - DirectX12 / Vulkan バックエンド実装
- 制限事項 / 今後の拡張
  - 圧縮フォーマットは現時点の `writeTexture` / buffer-texture copy の検証対象外です。block size / rowPitch 検証は圧縮フォーマット対応時に追加します。
  - DirectX12 の clear は、RTV / DSV / UAV view の生成単位に依存します。複数 mip / array / face をまたぐ厳密な subresource range clear は、必要になった時点で view range 生成を拡張します。

## 目的
- Asset / Graphics 層が GPU リソースを明示的に更新できるようにする。
- 一時 RenderTexture / UAV / Buffer を任意タイミングで初期化できるようにする。
- MSAA RenderTexture を通常 Texture / SwapChain 用 Texture へ resolve できるようにする。
- DirectX12 / Vulkan のコピーコマンド差異を RHI 側で吸収する。

## 追加 API 案
```cpp
struct BufferCopyRegion {
    Ref<Buffer> source;
    Ref<Buffer> target;
    u64 sourceOffset = 0;
    u64 targetOffset = 0;
    u64 size = 0;
};

struct TextureCopyRegion {
    Ref<Texture> source;
    Ref<Texture> target;
    TextureSubresource sourceSubresource;
    TextureSubresource targetSubresource;
    Point sourceOrigin;
    Point targetOrigin;
    Size size;
};

struct BufferTextureCopyRegion {
    Ref<Buffer> buffer;
    Ref<Texture> texture;
    u64 bufferOffset = 0;
    u32 rowPitch = 0;
    u32 depthPitch = 0;
    TextureSubresource textureSubresource;
    Point textureOrigin;
    Size size;
};

class CommandList {
public:
    virtual void copyBuffer(const BufferCopyRegion& region) = 0;
    virtual void copyTexture(const TextureCopyRegion& region) = 0;
    virtual void copyBufferToTexture(const BufferTextureCopyRegion& region) = 0;
    virtual void copyTextureToBuffer(const BufferTextureCopyRegion& region) = 0;

    virtual void writeTexture(const Ref<Texture>& texture, const TextureSubresource& subresource, BlobView data, u32 rowPitch, u32 depthPitch = 0) = 0;

    virtual void clearColorTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, const Color& color) = 0;
    virtual void clearDepthStencilTexture(const Ref<Texture>& texture, const TextureSubresourceRange& range, bool clearDepth, f32 depth, bool clearStencil, u8 stencil) = 0;
    virtual void clearBufferUInt(const Ref<Buffer>& buffer, u32 value) = 0;

    virtual void resolveTexture(const Ref<Texture>& source, const Ref<Texture>& target, const TextureSubresourceRange& sourceRange, const TextureSubresourceRange& targetRange) = 0;
};
```

## データ設計
- `TextureSubresource` は既存の `CommandParam.h` にありますが、コピーでは範囲指定が必要です。
- `TextureSubresourceRange` を追加し、mip / array / face の複数範囲を表現します。
- `Texture` と `RenderTexture` の関係は整理が必要です。コピー対象は `Texture` を基本にし、`RenderTexture` は内部的に `Texture` と同じ native resource を持つ扱いに寄せます。

```cpp
struct TextureSubresourceRange {
    u32 firstArray = 0;
    u32 arrayCount = 1;
    u32 firstFace = 0;
    u32 faceCount = 1;
    u32 firstMip = 0;
    u32 mipCount = 1;
};
```

## DirectX12 実装方針
- `copyBuffer` は `ID3D12GraphicsCommandList::CopyBufferRegion` を使用する。
- `copyTexture` は `CopyTextureRegion` を使用する。
- Buffer の直接更新は `Buffer::updateDirect` を使用する。
- `writeTexture` は upload heap の一時バッファを作成し、コピーコマンドへ変換する。
- `clearColorTexture` は RTV または UAV の clear を使い分ける。
- `clearDepthStencilTexture` は DSV を使用する。
- `resolveTexture` は `ResolveSubresource` を使用する。
- コピー前後の状態遷移は `StateAndBarrier.md` の仕様に従う。

## Vulkan 実装方針
- `copyBuffer` は `vkCmdCopyBuffer` を使用する。
- `copyTexture` は `vkCmdCopyImage` を使用する。
- `copyBufferToTexture` / `copyTextureToBuffer` は `vkCmdCopyBufferToImage` / `vkCmdCopyImageToBuffer` を使用する。
- Buffer の直接更新は `Buffer::updateDirect` を使用する。
- `writeTexture` は staging buffer を経由する。
- clear は `vkCmdClearColorImage` / `vkCmdClearDepthStencilImage` を使用する。
- `resolveTexture` は `vkCmdResolveImage` を使用する。

## 注意点
- `CopySource` / `CopyDest` の状態遷移がないと API は危険なので、手動バリア API と同時または直後に実装する。
- 圧縮フォーマットでは rowPitch / block size の検証が必要です。
- `writeTexture` は rowPitch が 0 の場合に RHI 側で計算できるようにしてもよいが、初期実装では明示指定を推奨します。
- `RenderTexture` を `Texture` から派生させるか、native resource 共通化するかは実装前に確認が必要です。

## テスト方針
- `RHI-test` に Buffer copy テストを追加する。
- Texture copy / clear は小さい 2D texture を作成して readback で検証する。
- MSAA resolve はサポート環境差があるため、Feature query 実装後に条件付きテストにする。
