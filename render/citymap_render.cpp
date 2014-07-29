#include "citymap_render.h"
#include <cassert>

namespace Render {

std::vector<std::unique_ptr<CitymapRendererFactory> > CitymapRenderer::registeredRenderers;

CitymapRenderer*
CitymapRenderer::getRenderer()
{
	float preference = 0.0f;
	CitymapRendererFactory *bestRenderer = nullptr;

	for (auto &renderer: CitymapRenderer::registeredRenderers)
	{
		if (!bestRenderer ||
		    renderer->getPreference() > preference)
		{
			bestRenderer = renderer.get();
			preference = renderer->getPreference();
		}
	}
	assert(bestRenderer);
	return bestRenderer->create();
}

void
CitymapRenderer::registerRenderer(CitymapRendererFactory *fact)
{
	CitymapRenderer::registeredRenderers.emplace_back(fact);
}

}; //namespace Render
