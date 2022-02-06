#include <Amethyst.h>

class AmethystEditor : public Amethyst::Application
{
public:
	AmethystEditor()
	{

	}

	~AmethystEditor()
	{

	}

};

Amethyst::Application* Amethyst::CreateApp()
{
	return new AmethystEditor();
}