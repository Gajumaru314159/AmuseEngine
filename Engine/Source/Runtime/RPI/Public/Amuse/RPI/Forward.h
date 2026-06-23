//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#pragma once
#include <Amuse/RHI/Forward.h>

namespace Amuse::RPI {

	struct MaterialBlockDesc;

	class FG;

	struct RenderSceneDesc;

	class RenderFeature;
	class RenderFeatureSet;
	class RenderPipeline;
	class RenderScene;
	class RenderView;
	class RenderPass;

	class Mesh;
	class Material;

	AMUSE_EVENT_NOTIFIER(RenderFeatureEvent, RenderFeature&);
	AMUSE_EVENT_NOTIFIER(RenderPipelineEvent, RenderPipeline&);
	AMUSE_EVENT_NOTIFIER(RenderSceneEvent, RenderScene&);
	AMUSE_EVENT_NOTIFIER(RenderViewEvent, RenderView&);

}