//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <Amuse/RHI/Types/TextureFormat.h>

namespace Amuse {
	//! @brief CPUアクセス種別
	//! @ingroup AmuseRHI
	enum class CpuAccess {
		Read,		//!< 読み取り
		Write,		//!< 書き込み
		ReadWrite,	//!< 読み書き
	};

	//! @brief      バッファ状態
	//! 
	//! @details    D3D12_RESOURCE_STATE VkImageLayout VkAccessFlagBits参照
	//!             | BufferState                       | D3D12_RESOURCE_STATE              | VkAccessFlagBits              |
	//!             |-----------------------------------|-----------------------------------|-------------------------------|
    //!             | Common                            | COMMON                            | -                             |
	//!             | VertexBuffer                      | VERTEX_AND_CONSTANT_BUFFER        | VERTEX_ATTRIBUTE_READ         |
	//!             | IndexBuffer                       | INDEX_BUFFER                      | INDEX_READ                    |
	//!             | ConstantBuffer                    | VERTEX_AND_CONSTANT_BUFFER        | UNIFORM_READ                  |
	//!             | ShadeResource                     | ALL_SHADER_RESOURCE               | SHADER_READ                   |
	//!             | UnorderedAccess                   | UNORDERED_ACCESS                  | SHADER_WRITE                  |
	//!             | IndirectArgument                  | INDIRECT_ARGUMENT                 | INDIRECT_COMMAND_READ         |
	//!             | CopyDest                          | COPY_DEST                         | TRANSFER_SRC_OPTIMAL          |
	//!             | CopySource                        | COPY_SOURCE                       | TRANSFER_DST_OPTIMAL          |
	//! @ingroup AmuseRHI
	enum class BufferState {
		Unknown,				//!< 現在状態をRHIが知らない
		Common,					//!< 
		Vertex,					//!< 頂点
		Index,					//!< インデックス
		Constant,				//!< 定数
		ShaderResource,			//!< シェーダリソース (読み取りリソース)
		UnorderedAccess,        //!< アンオーダード・アクセス (読み書きリソース)
		IndirectArgument,       //!< 間接引数
		CopySource,             //!< コピー元
		CopyDest,               //!< コピー先
	};

	//! @brief  バインド・フラグ
	//! @ingroup AmuseRHI
	enum class BufferFlag {
		Vertex				= get_bit(0),	//!< 頂点バッファ許可
		Index				= get_bit(1),	//!< インデックスバッファ許可
		Constant			= get_bit(2),	//!< 定数バッファ許可
		ShaderResource		= get_bit(3),	//!< シェーダでバインド許可
		UnorderedAccess		= get_bit(4),   //!< UnorderedAccessのバインド許可
		CopySource			= get_bit(5),   //!< コピー元許可
		CopyDest			= get_bit(6),   //!< コピー先許可
		IndirectArgument	= get_bit(7)	//!< 間接引数許可
	};
	//! @brief  バインド・フラグ・セット
	using BufferFlags = BitFlags<BufferFlag>;

	//! @brief	バッファリソースをどのリソースタイプとして参照するか
	//! @see	BindingType
	//! @ingroup AmuseRHI
	enum class BufferViewType {
		None,
		Buffer,
		RWBuffer,
		StructuredBuffer,
		RWStructuredBuffer,
		ByteAddressBuffer,
		RWByteAddressBuffer,
		ConstantBuffer,
	};

	//! @brief 既存バッファのビュー生成情報
	//! @ingroup AmuseRHI
	struct BufferViewDesc {
		Ref<Buffer>		base; //!< ベースバッファ
		BufferViewType	type; //!< ビュータイプ
		TextureFormat	format = TextureFormat::Unknown; //!< Buffer/RWBufferで使用する要素フォーマット
	};



	//! @brief  バッファ定義
	//! @ingroup AmuseRHI
	struct BufferDesc {

		String			name;			//!< 名前
		BufferState		state;			//!< バッファタイプ
		u64             size;			//!< バッファサイズ
		u32             stride;			//!< ストライド幅 StructuredBufferで使用する場合structureのサイズを設定する。
		BufferFlags     flags;			//!< バインドフラグ

	public:

		//! @brief 有効な設定か判定する
		bool isValid() const;

	public:

		//! @brief  コンストラクタ
		BufferDesc() = default;


		//! @brief  コンストラクタ
		BufferDesc(
			BufferState     state,
			u64             size,
			u32				stride,
			BufferFlags     flags)
			: state(state)
			, size(size)
			, stride(stride)
			, flags(flags)
		{}

		//! @brief  頂点バッファ用初期化
		template<typename TVertex>
		static BufferDesc Vertex(
			u64             count,
			BufferFlags     flags = BufferFlag::Vertex
		)
		{
			return BufferDesc(
				BufferState::Vertex,
				sizeof(TVertex)* count,
				sizeof(TVertex),
				flags
			);
		}

		//! @brief  インデックスバッファ用初期化
		template<typename TIndex>
		static BufferDesc Index(
			u64             count,
			BufferFlags     flags = BufferFlag::Index
		)
		{
			return BufferDesc(
				BufferState::Index,
				sizeof(TIndex) * count,
				sizeof(TIndex),
				flags
			);
		}

		//! @brief  定数バッファ用初期化
		//! @details サイズが256の倍数になるように調整されます。
		static BufferDesc Constant(
			u64				size,
			BufferState		initialState = BufferState::Constant,
			BufferFlags     flags = BufferFlag::Constant
		)
		{
			return BufferDesc(
				initialState,
				align_up(size, 256),
				0,
				flags
			);
		}

		//! @brief  ByteAddressバッファ用初期化
		//! @details サイズが256の倍数になるように調整されます。
		static BufferDesc ByteAddress(
			u64				size,
			BufferState		initialState = BufferState::ShaderResource,
			BufferFlags     flags = BufferFlag::ShaderResource
		)
		{
			return BufferDesc(
				initialState,
				align_up(size,16),
				0,
				flags
			);
		}

		//! @brief  ByteAddressバッファ用初期化
		//! @details サイズが256の倍数になるように調整されます。
		template<typename T>
		static BufferDesc Structured(
			u64				count,
			BufferState		initialState = BufferState::ShaderResource,
			BufferFlags     flags = BufferFlag::ShaderResource | BufferFlag::UnorderedAccess
		)
		{
			return BufferDesc(
				initialState,
				count * sizeof(T),
				sizeof(T),
				flags
			);
		}

	};

}
