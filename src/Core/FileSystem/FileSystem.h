#pragma once

#include <string>
#include <cstdlib>

namespace Nx
{
	typedef std::string(*Builder) (const std::string& path);

	class FileSystem
	{
	public:

		static std::string GetPath(const std::string& path);

		static std::string const& GetRoot();

	private:
		static Builder GetPathBuilder();

		static std::string GetPathRelativeRoot(const std::string& path);

		static std::string GetPathRelativeBinary(const std::string& path);
	};
}

