#pragma once

namespace nPhysics
{
	class iDebugRenderer
	{

	protected:
		iDebugRenderer() {}
		iDebugRenderer(const iDebugRenderer& other) {}

		//better way to explicitly remove copy op?
		//iDebugRenderer& operator=(const iDebugRenderer& other) = delete;
		iDebugRenderer& operator=(const iDebugRenderer& other) { return *this; }
	};

}