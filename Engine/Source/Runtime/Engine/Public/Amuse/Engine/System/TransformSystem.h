//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/Core/Core.h>
#include <Amuse/Engine/System/SystemAccess.h>
#include <entt/entt.hpp>

namespace Amuse {


	using Entity = entt::entity;

	struct TransformComponent {
		struct Flags {
			struct TRS {
				bool position : 1;
				bool rotation : 1;
				bool scale : 1;
			};

			TRS local;
			TRS world;
		};
		Transform local;
		Transform world;
		Flags flags;
	};
	struct HierarchyComponent {
		Entity parent;	//!< 親
		Entity child;	//!< 最初の子
		Entity next;	//!< 次の兄弟
	};
	struct TransformDirtyFlag {};


	struct TransformSystem {
		static SystemAccess Access;

		static void Update(entt::registry& world);
	};

}