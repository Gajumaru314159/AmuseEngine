//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>

namespace Amuse {
	
	struct TransformComponent {
		Transform local;
		Transform world;
	};
	struct SocketAttachCompoennt {

	};

	struct HierarchyComponent {};

	struct LayerComponent {

	};


	struct AnimatorComponent {

	};

	struct SplineComponent {

	};

	// オーディオ
	struct AudioSourceComponent {

	};
	struct AudioListenerComponent {

	};
	
	// レンダリング
	struct CameraComponent {
		f32 fov{ 60.0f };
	};

	struct MeshCompoennt {

	};
	struct SkeletalMeshCompoennt {

	};

	struct InstancingComponent {
		Vector<Transform> transforms;
	};

	struct DecalComponent {

	};

	struct BillboardComponent {

	};

	struct ExponentialHeightFogComponent {

	};

	struct EffectComponent {

	};

	struct PostProcessComponent {

	};

	struct TerrainComponent {

	};

	// ライト
	struct PointLightCompoennt {

	};
	struct SpotLightCompoennt {

	};
	struct DirectionalLightCompoennt {

	};
	struct AreaLightCompoennt {

	};
	struct SkyLightComponewnt {

	};

	// 物理
	struct BouyancyComponent {

	};
	struct RigidBoadyCompoennt {

	};
	struct BoxColliderComponent {

	};
	struct SphereColliderComponent {

	};
	struct CapsuleColliderComponent {

	};
	struct SylinderColliderCompoennt {

	};
	struct RagdollComponent {

	};
	struct SpringCompoennt {

	};

	// AI
	struct NavMeshComponent {

	};
	struct NavMeshLinkComponent {

	};
}