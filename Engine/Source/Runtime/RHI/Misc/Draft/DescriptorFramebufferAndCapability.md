# Descriptor / Attachment / Capability 実装方針

## 概要
DescriptorTable、Dynamic Rendering の attachment 指定、Feature / Format support query の方針を整理します。
nvrhi では BindingSet / DescriptorTable / Framebuffer / queryFeatureSupport が明確に分かれていますが、OctbitEngine では既存設計と Vulkan Dynamic Rendering 方針に合わせます。

## 目的
- 既存 DescriptorTable 実装を前提に、追加実装が必要な範囲を明確にする。
- Framebuffer 抽象を追加せず、RenderPass 開始時に attachment を動的指定する方針を明確にする。
- バックエンドや GPU ごとの機能差を RHI 利用者が問い合わせられるようにする。

## Capability 追加 API 案
```cpp
struct RHIFeatures {
    bool computeQueue = false;
    bool copyQueue = false;
    bool indirectDraw = false;
    bool indirectDispatch = false;
    bool bindless = false;
    bool rayTracing = false;
    bool meshShader = false;
    bool variableRateShading = false;
    bool conservativeRasterization = false;
    bool stagingTexture = false;
};

enum class FormatSupport {
    Texture,
    RenderTarget,
    DepthStencil,
    ShaderResource,
    UnorderedAccess,
    Copy,
    Resolve,
};

class Device {
public:
    virtual RHIFeatures getFeatures() const = 0;
    virtual bool supports(TextureFormat format, FormatSupport usage) const = 0;
};
```

## Descriptor 方針
- `DescriptorLayout` / `DescriptorTable` は実装済みとして扱います。
- 追加の `DescriptorWrite` / `resize` / `capacity` API はこの仕様では追加しません。
- bindless は既存の `RootSignatureFlag::EnableBindless`、`BindlessHandle`、各 backend の descriptor heap / descriptor set 実装を前提にします。
- 今後 DescriptorTable に不足が見つかった場合は、Descriptor 専用の別仕様として切り出します。

## Attachment / Dynamic Rendering 方針
- Framebuffer 抽象は追加しません。
- RenderPass 開始時に `BeginPassParam` で attachment を動的指定します。
- Vulkan は `VulkanDevice` / `VulkanCommandList` 側の Dynamic Rendering 方針に合わせ、固定 `VkFramebuffer` を前提にしません。
- DirectX12 も同様に `BeginRenderPass` 時に RTV / DSV を動的に指定する方針を維持します。
- PipelineState は現行通り `PipelineStateDesc::colors` と `PipelineStateDesc::depth` で attachment format 互換性を定義します。

## Capability 実装方針
- 既存の `supports(TextureFormat, TextureType)` と `supports(ShaderStage)` は残す。
- RHI 機能は `Device::getFeatures()` で `RHIFeatures` 構造体としてまとめて取得する。
- `supports(TextureFormat, FormatSupport)` を追加し、より細かいフォーマット問い合わせを可能にする。
- DirectX12 / Vulkan の feature query を初期化時にキャッシュする。

## 注意点
- Framebuffer を追加すると RenderPass / FrameGraph / Dynamic Rendering の責務が重複しやすいため、追加しない。
- Attachment のサイズ不一致や format 不一致の検証は `beginRenderPass` または PipelineState 作成時の validation として扱う。
- Format support は API ごとに結果が異なるため、厳密な問い合わせ結果を返す必要があります。

## テスト方針
- Dynamic Rendering 方針に沿って、`beginRenderPass` に複数 attachment を直接指定して描画できることを確認する。
- Attachment サイズ不一致や format 不一致を validation で検出できることを確認する。
- `getFeatures()` と `supports(TextureFormat, FormatSupport)` が代表的な format / feature で妥当な値を返すことを確認する。
