//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Utility/DI.h>
#include <Amuse/RHI/RHI.h>
#include <Amuse/RHI/Config.h>
#include <Amuse/Core/Utility/Pimpl.h>
#include <Amuse/DirectX12RHI/DirectX12RHIConfig.h>
#include <Amuse/DirectX12RHI/Buffer/DirectX12BufferUploader.h>
#include <Amuse/DirectX12RHI/Texture/DirectX12TextureUploader.h>
#include <Amuse/DirectX12RHI/Descriptor/DirectX12DescriptorUploader.h>
#include <Amuse/DirectX12RHI/Descriptor/DescriptorHeap.h>
#include "Amuse/DirectX12RHI/Sampler/DirectX12Sampler.h"

// 前方宣言
namespace Amuse::RHI {
    using namespace Amuse::Core;
	class SmallBufferAllocator;
}

namespace Amuse::Platform {
    using namespace Amuse::Core;
	class WindowManager;
}

namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief DirectX 12 RHI デバイス
	class DirectX12Device : public Device{
	public:
		//! @brief  DirectX12RHIの起動に必要なサービスを登録
		static void Inject(ServiceInjector&);
	public:
		//! @brief DirectX 12 デバイスを初期化する
		DirectX12Device(Amuse::Platform::WindowManager&, GraphicObjectManager&, const RHIConfig*, const DirectX12RHIConfig*);
		~DirectX12Device();

		//! @brief  ゲーム更新イベント
		void update() override;


		//! @brief  妥当な状態か
		bool isValid()const override;


		//! @brief	API名を取得
		String getAPIName()const { return "DirectX12"; };


		//===============================================================
		// 更新
		//===============================================================

		//! @brief コマンドリストを登録する
		void entryCommandList(const Ref<CommandList>&) override;
		//! @brief フェンスを生成する
		Ref<Fence> createFence(const FenceDesc& desc) override;
		//! @brief コマンドリストを実行する
		u64 executeCommandLists(Span<Ref<CommandList>> commandLists, CommandQueueType queue = CommandQueueType::Graphics) override;
		//! @brief キューのアイドル状態を待機する
		void waitForIdle(CommandQueueType queue = CommandQueueType::Graphics) override;
		//! @brief キュー間のフェンス待機を設定する
		void queueWait(CommandQueueType waitQueue, CommandQueueType executeQueue, u64 fenceValue) override;


		//! @brief 利用可能なビデオカード一覧を取得する
		Vector<VideoCard> getVideoCards()const override;


		//===============================================================
		// 生成
		//===============================================================

		//! @brief  スワップ・チェーンを生成
		Ref<SwapChain> createSwapChain(const SwapChainDesc& desc)override;


		//! @brief  コマンドリスト生成
		Ref<CommandList> createCommandList(const CommandListDesc& desc)override;


		//! @brief  ルートシグネチャを生成
		Ref<RootSignature> createRootSignature(const RootSignatureDesc& desc)override;


		//! @brief  パイプラインステートを生成
		Ref<PipelineState> createPipelineState(const PipelineStateDesc& desc)override;


		//! @brief  コンピュートパイプラインステートを生成
		Ref<ComputePipelineState> createComputePipelineState(const ComputePipelineStateDesc& desc)override;


		//! @brief  テクスチャを生成
		Ref<Texture> createTexture(const TextureDesc& desc)override;
		//! @brief テクスチャを生成する
		Ref<Texture> createTexture(StringView name, TextureType type, Size size, Span<const IntColor> colors) override;
		//! @brief テクスチャを生成する
		Ref<Texture> createTexture(StringView name,BlobView blob)override;
		//! @brief テクスチャを生成する
		Ref<Texture> createTexture(const TextureViewDesc& desc) override;


		//! @brief  レンダーテクスチャを生成
		Ref<RenderTexture> createRenderTexture(const RenderTextureDesc& desc)override;


		//! @brief  サンプラーを生成
		Ref<Sampler> createSampler(const SamplerDesc& desc)override;


		//! @brief  バッファーを生成
		Ref<Buffer> createBuffer(const BufferDesc& desc) override;
		//! @brief バッファを生成する
		Ref<Buffer> createBuffer(const BufferViewDesc& desc) override;


		//! @brief  シェーダをコンパイル
		Ref<Shader> compileShader(const ShaderCompileDesc& desc) override;


		//! @brief  シェーダをロード
		Ref<Shader> loadShader(BlobView, ShaderStage) override;


		//! @brief  デスクリプタ・テーブルを生成
		Ref<DescriptorLayout>	createDescriptorLayout(const DescriptorLayoutDesc& desc) override;
		//! @brief ディスクリプタテーブルを生成する
		Ref<DescriptorTable>	createDescriptorTable(const DescriptorTableDesc& desc) override;
		//! @brief イベントクエリを生成する
		Ref<EventQuery>			createEventQuery() override;
		//! @brief イベントクエリの完了状態を確認する
		bool					pollEventQuery(const Ref<EventQuery>& query) override;
		//! @brief イベントクエリの完了を待機する
		void					waitEventQuery(const Ref<EventQuery>& query) override;
		//! @brief タイマークエリを生成する
		Ref<TimerQuery>			createTimerQuery() override;
		//! @brief タイマークエリの完了状態を確認する
		bool					pollTimerQuery(const Ref<TimerQuery>& query) override;
		//! @brief タイマークエリの計測時間を取得する
		f32						getTimerQueryTime(const Ref<TimerQuery>& query) override;



		//! @brief  GraphicFileHandleを生成
		Ref<GraphicFileHandle>  createGraphicFileHandle(StringView path) override;

		//! @brief  GraphicFileEventを生成
		Ref<GraphicFileEvent>   createGraphicFileEvent() override;

		//! @brief  GraphicFileQueueを生成
		Ref<GraphicFileQueue>   createGraphicFileQueue(const GraphicFileQueueDesc&) override;

		//! @brief  GraphicFileQueueを生成
		bool                    generateGraphicFile(StringView input, StringView output, s32 compressionLevel) override;

		//! @brief  プラットフォームごとのGraphicFileから事前情報を取得
		Vector<GraphicFileMipInfo> prepareGraphicFile(StringView path) override;

	public:

		//! @brief サポートしているテクスチャフォーマットか 
		bool supports(TextureFormat format, TextureType type)const override;

		//! @brief サポートしているテクスチャフォーマットか 
		bool supportsForRenderTexture(TextureFormat format)const override;

		//! @brief サポートしているシェーダーステージか 
		bool supports(ShaderStage format)const override;

		//! @brief サポートしているRHI機能を取得
		RHIFeatures getFeatures()const override;

		//! @brief 指定用途でサポートしているテクスチャフォーマットか
		bool supports(TextureFormat format, FormatSupport usage)const override;

	public:

		//! @brief 保留中のコマンドをクリアする
		void clearCommands();

		//===============================================================
		// ゲッター
		//===============================================================

		//! @brief  ネイティブ・デバイスを取得
		ComPtr<ID3D12Device8>& getNative();


		//! @brief  ファクトリを取得
		ComPtr<IDXGIFactory7>& getFactory();


		//! @brief  シェーダーコンパイラ―を取得
		ComPtr<IDxcCompiler3>& getShaderCompiler();


		//! @brief  シェーダーインクルードハンドラーを取得
		ComPtr<IDxcIncludeHandler>& getIncludeHandler();


		//! @brief  システム・コマンド・キューを取得
		ComPtr<ID3D12CommandQueue>& getCommandQueue();


		//! @brief  システム・コマンド・リストを取得
		//ComPtr<ID3D12GraphicsCommandList>& getSystemCommandList();

		DirectX12BufferUploader& getBufferUploader() { return *m_bufferUploader; }

		//! @brief テクスチャアップローダを取得する
		DirectX12TextureUploader& getTextureUploader() { return *m_textureUploader; }

		//! @brief ディスクリプタアップローダを取得する
		DirectX12DescriptorUploader& getDescriptorUploader() { return *m_descriptorUploader; }


		//! @brief          ハンドルをアロケート
		//! 
		//! @param type     ヒープタイプ
		//! @param handle   アロケート先ハンドル
		//! @param size     割り当て個数
		void allocateHandle(DescriptorHeapType type, class DescriptorHandle& handle, s32 size);
		//! @brief ステージング用ディスクリプタハンドルを確保する
		auto allocateStagingHandle(DescriptorHeapType type, s32 size = 1) -> D3D12_CPU_DESCRIPTOR_HANDLE;


		//! @brief          デスクリプタヒープを設定
		void setDescriptorHeaps(class DirectX12CommandList& cmdList);


		//! @brief  IDStorageFactoryを取得
		ComPtr<IDStorageFactory>& getDirectStorageFactory();

		//! @brief  SmallBufferAllocatorを取得
		SmallBufferAllocator& getSmallBufferAllocator(BufferFlag flag);


	private:

		bool initializeDXGIDevice();
		bool initializeDescriptorHeaps();
		bool initializeShaderCompiler();
		bool initializeUploaders();
		bool initializeDirectStorage();

#ifdef AMUSE_DEBUG
		bool initializeDebugMessageCallback();
		void finalizeDebugMessageCallback();
		static void CALLBACK debugMessageCallback(
			D3D12_MESSAGE_CATEGORY category,
			D3D12_MESSAGE_SEVERITY severity,
			D3D12_MESSAGE_ID id,
			LPCSTR description,
			void* context
		);
#endif

	private:
		template<class TKey,class TValue,class THasher, class TEqual>
		class ResourceCache
		{
		public:
			Ref<TValue> getOrCreate(DirectX12Device& device,const TKey& key)
			{
				ScopeLock lock(m_lock);
				auto itr = m_cache.find(key);
				if (itr != m_cache.end())
				{
					return itr->second;
				}
				return m_cache[key] = Ref<TValue>(new TValue(device,key));
			}
			void clear()
			{
				ScopeLock lock(m_lock);
				m_cache.clear();
			}
		private:
			SpinLock m_lock;
			HashMap<TKey,Ref<TValue>,THasher,TEqual> m_cache;
		};
	private:
		RHIConfig							m_config;
		DirectX12RHIConfig					m_dx12config;

		ComPtr<ID3D12Device8>               m_device;                   // D3D12のデバイス本体
		ComPtr<IDXGIFactory7>               m_dxgiFactory;              // DXGIインターフェイス
		ComPtr<IDxcCompiler3>				m_shaderCompiler;
		ComPtr<IDxcUtils>					m_shaderUtils;
		ComPtr<IDxcIncludeHandler>			m_shaderIncludeHandler;

		UPtr<class CommandQueue>			m_commandQueue;
		Ref<CommandList>					m_copyCommandList;


		ComPtr<ID3D12Fence>                 m_fence;
		UINT64                              m_fenceVal;

		// デバッグレイヤーメッセージコールバック
#ifdef AMUSE_DEBUG
		ComPtr<ID3D12InfoQueue1>            m_infoQueue;
		DWORD                               m_callbackCookie = 0;
#endif

		using SamplerCache = ResourceCache<SamplerDesc, DirectX12Sampler, SamplerDescHasher, SamplerDescEqual>;
		SamplerCache	m_samplerCache;

		MemoryStorage<DirectX12BufferUploader>		m_bufferUploader;
		MemoryStorage<DirectX12TextureUploader>		m_textureUploader;
		MemoryStorage<DirectX12DescriptorUploader>	m_descriptorUploader;

		HashMap<DescriptorHeapType, UPtr<class DescriptorHeap>>        m_descriptorHeaps;          // デスクリプタ・ヒープ・リスト
		HashMap<DescriptorHeapType, UPtr<class DescriptorStagingHeap>> m_descriptorStagingHeaps;   // デスクリプタ・ヒープ・リスト

		ComPtr<IDStorageFactory>			g_dsfactory;

		// SmallBufferAllocator管理
		HashMap<BufferFlag, Amuse::Core::UPtr<SmallBufferAllocator>>	m_smallBufferAllocators;

#ifdef AMUSE_DEBUG
		UPtr<class PIXModule> m_pixModule;
#endif

	};

}
//===============================================================
// インライン
//===============================================================
namespace Amuse::RHI {
    using namespace Amuse::Core;

	//! @brief  ネイティブ・デバイスを取得
	inline ComPtr<ID3D12Device8>& DirectX12Device::getNative() {
		return m_device;
	}


	//! @brief  ファクトリを取得
	inline ComPtr<IDXGIFactory7>& DirectX12Device::getFactory() {
		return m_dxgiFactory;
	}

	//! @brief  シェーダーコンパイラ―を取得
	inline ComPtr<IDxcCompiler3>& DirectX12Device::getShaderCompiler() {
		return m_shaderCompiler;
	}

	//! @brief  シェーダーインクルードハンドラーを取得
	inline ComPtr<IDxcIncludeHandler>& DirectX12Device::getIncludeHandler() {
		return m_shaderIncludeHandler;
	}

}
