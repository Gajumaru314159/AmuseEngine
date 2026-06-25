//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {

	//! @brief		Wavefront OBJ Writer
	//! @ingroup AmuseCore
	class OBJ {
	public:
		OBJ() = default;

		//! @brief データを保存する。
		bool save(StringView) const;

		//! @brief OBJ ファイルへコメント行を追加する。
		void comment(StringView comment);
		//! @brief 参照する MTL ファイル名を追加する。
		void mtllib(StringView filename);
		//! @brief 以降の面に適用するマテリアル名を指定する。
		void material(StringView name);
		//! @brief 以降の要素に適用するグループ名を指定する。
		void group(StringView);
		//! @brief 以降の要素に適用するオブジェクト名を指定する。
		void name(StringView);

		//! @brief 頂点位置を追加し、OBJ の頂点インデックスを返す。
		s32 position(Vec3);
		//! @brief テクスチャ座標を追加し、OBJ の座標インデックスを返す。
		s32 texcoord(Vec2);
		//! @brief 法線を追加し、OBJ の法線インデックスを返す。
		s32 normal(Vec3);

		//! @brief 以降の面に適用するスムージンググループを指定する。
		void smoothLevel(s32 level);

		//! @brief 登録済み頂点位置の数を取得する。
		s32 pcount()const;
		//! @brief 登録済みテクスチャ座標の数を取得する。
		s32 tcount()const;
		//! @brief 登録済み法線の数を取得する。
		s32 ncount()const;

		//! @brief 頂点位置インデックスだけを持つ面を追加する。
		template<class ...Args> void face_p(Args... args) { face_p_internal(0, args...); }
		//! @brief 頂点位置とテクスチャ座標インデックスを持つ面を追加する。
		template<class ...Args> void face_pt(Args... args) { face_pt_internal(0, args...); }
		//! @brief 頂点位置と法線インデックスを持つ面を追加する。
		template<class ...Args> void face_pn(Args... args) { face_pn_internal(0, args...); }
		//! @brief 頂点位置、テクスチャ座標、法線インデックスを持つ面を追加する。
		template<class ...Args> void face_ptn(Args... args) { face_ptn_internal(0, args...); }
		//! @brief 頂点位置インデックスで線分列を追加する。
		template<class ...Args> void line(Args... args) { line_internal(0, args...); }

		//! @brief 頂点位置を登録しながら面を追加する。
		template<class ...Args> void poly_p(Args... args) { poly_p_internal(0, args...); }
		//! @brief 頂点位置とテクスチャ座標を登録しながら面を追加する。
		template<class ...Args> void poly_pt(Args... args) { poly_pt_internal(0, args...); }
		//! @brief 頂点位置と法線を登録しながら面を追加する。
		template<class ...Args> void poly_pn(Args... args) { poly_pn_internal(0, args...); }
		//! @brief 頂点位置を登録しながら線分列を追加する。
		template<class ...Args> void poly_line(Args... args) { poly_line_internal(0, args...); }

	private:

		void face_p_internal(s32 count);
		void face_pt_internal(s32 count);
		void face_pn_internal(s32 count);
		void face_ptn_internal(s32 count);
		void line_internal(s32 count);

		template<class ...Args>
		void face_p_internal(s32 count, s32 p, Args... args) {
			if (count == 0) m_ss << "f";
			m_ss << " " << p;
			face_p_internal(count + 1, args...);
		}

		template<class ...Args>
		void face_pt_internal(s32 count, s32 p, s32 t, Args... args) {
			if (count == 0) m_ss << "f";
			m_ss << " " << p << "/" << t;
			face_pt_internal(count + 1, args...);
		}

		template<class ...Args>
		void face_pn_internal(s32 count, s32 p, s32 n, Args... args) {
			if (count == 0) m_ss << "f";
			m_ss << " " << p << "//" << n;
			face_pn_internal(count + 1, args...);
		}

		template<class ...Args>
		void face_ptn_internal(s32 count, s32 p, s32 t, s32 n, Args... args) {
			if (count == 0) m_ss << "f";
			m_ss << " " << p << "/" << t << "/" << n;
			face_pn_internal(count + 1, args...);
		}

		template<class ...Args>
		void line_internal(s32 count, s32 p, Args... args) {
			if (count == 0) m_ss << "l";
			m_ss << " " << p;
			line_internal(count + 1, args...);
		}

		void poly_p_internal(s32 count);
		void poly_pt_internal(s32 count);
		void poly_pn_internal(s32 count);
		void poly_ptn_internal(s32 count);
		void poly_line_internal(s32 count);

		template<class ...Args>
		void poly_p_internal(s32 count, Vec3 p, Args... args) {
			position(p);
			poly_p_internal(count + 1, args...);
		}

		template<class ...Args>
		void poly_pt_internal(s32 count, Vec3 p, Vec2 t, Args... args) {
			position(p);
			texcoord(t);
			poly_pt_internal(count + 1, args...);
		}

		template<class ...Args>
		void poly_pn_internal(s32 count, Vec3 p, Vec3 n, Args... args) {
			position(p);
			normal(n);
			poly_pn_internal(count + 1, args...);
		}

		template<class ...Args>
		void poly_ptn_internal(s32 count, Vec3 p, Vec2 t, Vec3 n, Args... args) {
			position(p);
			texcoord(t);
			normal(n);
			poly_ptn_internal(count + 1, args...);
		}

		template<class ...Args>
		void poly_line_internal(s32 count, Vec3 p, Args... args) {
			position(p);
			poly_line_internal(count + 1, args...);
		}

	private:
		std::stringstream m_ss;
		s32 m_pcount = 0;
		s32 m_tcount = 0;
		s32 m_ncount = 0;
	};


	//! @brief		Material Template Library Writer
	//! @ingroup AmuseCore
	class MTL {
	public:
		//! @brief データを保存する。
		bool save(StringView) const;

		//! @brief 新しいマテリアル定義を開始する。
		void newmtl(StringView name);
		//! @brief 環境光色と必要に応じてテクスチャを設定する。
		void ambient(Color color = {0.2f,0.2f,0.2f}, StringView tex = "");
		//! @brief 拡散反射色と必要に応じてテクスチャを設定する。
		void diffuse(Color color = {0.8f,0.8f,0.8f}, StringView tex = "");
		//! @brief 鏡面反射色と必要に応じてテクスチャを設定する。
		void specular(Color color = {1.0f,1.0f,1.0f}, StringView tex = "");
		//! @brief 鏡面反射の強さと必要に応じてテクスチャを設定する。
		void shininess(f32 ns = 0.0f, StringView tex = "");
		//! @brief 透明度と必要に応じてテクスチャを設定する。
		void alpha(f32 alpha, StringView tex = "");
		//! @brief MTL の照明モデル番号を設定する。
		void illumination(s32);
		//! @brief 屈折率を設定する。
		void ior(f32);
		//! @brief 法線マップのテクスチャを設定する。
		void normal(StringView tex);
		//! @brief バンプマップのテクスチャを設定する。
		void bump(StringView tex);
		//! @brief ディスプレイスメントマップのテクスチャを設定する。
		void displacement(StringView tex);
		//! @brief デカールマップのテクスチャを設定する。
		void decal(StringView tex);
	private:
		std::stringstream m_ss;
	};

}
