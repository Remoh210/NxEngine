#include <Core/Application/Application.h>

int main()
{
	Application* App = Application::Create(1024, 900);

	App->Run();

    return 0;
}


