//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/DirectX12RHI/pch.h>
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Query.h>

namespace Amuse {
	class DirectX12Device;

	//! @brief GPUイベントクエリ実装(DirectX12)
	//! @ingroup AmuseDirectX12RHI
	class DirectX12EventQuery : public EventQuery {
	public:
		//! @brief コンストラクタ
		explicit DirectX12EventQuery(DirectX12Device& device);

		//! @brief 妥当な状態か
		bool isValid() const;

		//! @brief 名前を取得
		const String& getName() const override;

		//! @brief 完了判定に使うキューのfence valueを設定
		void signal(u64 fenceValue);

		//! @brief 完了判定に使うキューのfence valueを取得
		u64 getFenceValue() const { return m_fenceValue; }

	private:
		DirectX12Device& m_device;
		String m_name = "DirectX12EventQuery";
		u64 m_fenceValue = 0;
	};

	//! @brief GPUタイマークエリ実装(DirectX12)
	//! @ingroup AmuseDirectX12RHI
	class DirectX12TimerQuery : public TimerQuery {
	public:
		//! @brief コンストラクタ
		explicit DirectX12TimerQuery(DirectX12Device& device);

		//! @brief 妥当な状態か
		bool isValid() const;

		//! @brief 名前を取得
		const String& getName() const override;

		//! @brief 開始タイムスタンプを記録
		void recordBegin(ID3D12GraphicsCommandList& commandList);

		//! @brief 終了タイムスタンプを記録
		void recordEnd(ID3D12GraphicsCommandList& commandList);

		//! @brief 計測時間を秒単位で取得
		f32 getTime() const;

	private:
		DirectX12Device& m_device;
		String m_name = "DirectX12TimerQuery";
		ComPtr<ID3D12QueryHeap> m_queryHeap;
		ComPtr<ID3D12Resource> m_readbackBuffer;
	};

}
