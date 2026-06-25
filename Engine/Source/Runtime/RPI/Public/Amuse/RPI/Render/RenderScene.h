//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RPI/Forward.h>
#include <Amuse/RPI/Render/RenderFeature.h>
#include <Amuse/RPI/Render/RenderPipeline.h>

namespace Amuse {

    //! @brief RenderScene が保持する描画機能の集合
    //! @ingroup AmuseRPI
    class RenderFeatureSet {
    public:
        //! @brief 型情報を指定して描画機能を追加する
        RenderFeature* add(Type type, UPtr<RenderFeature> feature) {
            m_features[type] = std::move(feature);
            return m_features[type].get();
        }
        //! @brief 指定型の描画機能を生成して追加する
        template<class T,class... TArgs> auto add(TArgs&&... args) -> std::enable_if_t<std::is_constructible_v<T, TArgs...>, T*> {
            auto result = new T(std::forward<TArgs>(args)...);
            m_features[Type::Get<T>()].reset(result);
            return result;
        }
        //! @brief 指定型の描画機能を検索する
        template<class T> T* find()const {
			return reinterpret_cast<T*>(find(Type::Get<T>()));
        }
        //! @brief 型情報から描画機能を検索する
        RenderFeature* find(Type type)const {
            auto found = m_features.find(type);
            if (found == m_features.end())return nullptr;
            return found->second.get();
        }
    private:
        HashMap<Type, UPtr<RenderFeature>> m_features;
    };

	//! @brief      描画シーン
	//! @ref 		RenderScene
	//! @ingroup AmuseRPI
	class RenderScene{
	public:

        //! @brief 描画シーンを生成する
        RenderScene();
        //! @brief 描画シーンを破棄する
        ~RenderScene();

        //! @brief      RenderFeatureを見つける
        template<class T> T* findFeature()const;
        //! @brief 型情報から RenderFeature を見つける
        RenderFeature* findFeature(Type type)const;

        //! @brief 指定インデックスの描画パイプラインを設定する
        void setPipeline(s32 index, Ref<RenderPipeline> pipeline);

        //! @brief      描画
        void render(FG& fg);

        //! @brief      解放時イベント
        void addReleasedEvent(RenderSceneEventHandle& handle, RenderSceneEventDelegate func);

    public:

        //! @brief 所属する RenderView を順に訪問する
        void visitView(Func<void(RenderView&)> visitor);

    private:

		friend class RenderView;

        //! @brief      RenderViewを追加する
        void addView(RenderView* view);

        //! @brief      RenderViewを削除する
        void removeView(RenderView* view);

    private:

        RenderFeatureSet                        m_features;
        Map<s32,Ref<RenderPipeline>>            m_pipelines;
		Vector<RenderView*>                     m_views;

        RenderSceneEventNotifier                m_releasedNotifier;
	};


    //! @brief      RenderFeatureを見つける
    template<class T>
    T* RenderScene::findFeature()const {
        return reinterpret_cast<T*>(findFeature(Type::Get<T>()));
    }

}
