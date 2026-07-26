//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Geometry/Bounding.h>
#include <Amuse/RHI/VertexLayout.h>
#include <Amuse/RHI/CommandList.h>
#include <Amuse/RPI/Material/Material.h>

namespace Amuse {

	//! @brief メッシュ LOD の GPU バッファ
	//! @ingroup AmuseRPI
	struct MeshLodBuffer
	{
		Ref<VertexLayout> layout; //!< 頂点レイアウト
		Ref<Buffer> vertices; //!< 頂点バッファ
		Ref<Buffer> indices; //!< インデックスバッファ
	};
	//! @brief メッシュ LOD の CPU 側頂点ソース
	//! @ingroup AmuseRPI
	struct MeshLodSource
	{
		Vector<u32>		indices; //!< インデックス配列
		Vector<Vec3>	positions; //!< 位置配列
		Vector<Vec3>	normals; //!< 法線配列
		Vector<Vec3>	binormals; //!< 従法線配列
		Vector<Vec3>	tangents; //!< 接線配列
		Vector<Vec2>	uvs; //!< UV 配列
		Vector<Color>	colors; //!< 頂点色配列
	};

	//! @brief メッシュ内のマテリアル割り当て範囲
	//! @ingroup AmuseRPI
	struct MeshSection
	{
		s32 material = 0; //!< マテリアルインデックス
		s32 baseVertex = 0; //!< ベース頂点
		s32 indexStart = 0; //!< インデックス開始位置
		s32 indexCount = 0; //!< インデックス数
	};

	//! @brief メッシュの LOD データ
	//! @ingroup AmuseRPI
	struct MeshLod
	{
		MeshLodBuffer buffer; //!< GPU バッファ
		Optional<MeshLodSource> source; //!< CPU 側頂点ソース
		Vector<MeshSection> sections; //!< 描画セクション
		Bounds bounds; //!< 境界ボックス
		bool bKeepSource = false; //!< ソース保持フラグ
	};

	//! @brief 描画用メッシュリソース
	//! @ingroup AmuseRPI
	class Mesh : public RefObject
	{
	public:
		Bounds bounds; //!< 境界ボックス
		Vector<MeshLod> lods; //!< LOD 一覧
		Vector<Ref<Material>> materials; //!< マテリアル一覧
	public:
		//! @brief 空のメッシュを生成する
		static Ref<Mesh> Create();
		//! @brief ファイルからメッシュを読み込む
		static Ref<Mesh> Load(StringView path);
	public:

		//! @brief CPU 側ソースから GPU バッファを生成する
		void apply();
		//! @brief CPU 側ソースを破棄する
		void clearSources();

	protected:
		Mesh() = default;
	};

}
