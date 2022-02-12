#include "amtpch.h"
#include "LayerStack.h"

namespace Amethyst
{
	LayerStack::LayerStack() : layerIndex(0)
	{
	}
	
	LayerStack::~LayerStack()
	{
		for (int i = 0; i < layers.size(); ++i)
			delete layers[i];
	}
	
	void LayerStack::PushLayer(Layer* layer)
	{
		layers.emplace(layers.begin() + layerIndex, layer);
		layerIndex++;
	}
	
	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layers.erase(it);
			layerIndex--;
		}
	}
	
	void LayerStack::PushOverlay(Layer* overlay)
	{
		layers.emplace_back(overlay);
	}
	
	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator it = std::find(layers.begin(), layers.end(), overlay);
		if (it != layers.end())
			layers.erase(it);
	}
}