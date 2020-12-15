#include <string>
#include <rttr/type>

namespace Nx
{
	/*!
	 * Serialize the given instance to a json encoded string.
	 */
	std::string to_json(rttr::instance obj);

	/*!
	 * Deserialize the given json string \p json to the given instance \p obj.
	 */
	bool from_json(const std::string& json, rttr::instance obj);


}
