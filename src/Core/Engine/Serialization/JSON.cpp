#include <cstdio>
#include <string>
#include <vector>
#include <array>

#include <iostream>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include <rapidjson/istreamwrapper.h>

#include <fstream>

#include <rttr/type>

#include "Common/dataStructs/String.h"
#include "Common/Transform/Transform.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/SceneManager/SceneManager.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "rendering/ArrayBitmap.h"

using namespace rapidjson;
using namespace rttr;





namespace
{
	// Write
	
	void to_json_recursively(const instance& obj, PrettyWriter<StringBuffer>& writer);

	void write_array(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer);

	void write_associative_container(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer);

	bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer);

	bool write_atomic_types_to_json(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer);

	// Read

	void from_json_recursively(rttr::variant& variant, const Value& json_object);

	void from_json_recursively(rttr::variant& variant, const rttr::type& variant_type, const Value& json_object);

	void extract_array_recursively(variant_sequential_view& view, const Value& json_array_value);

	void extract_associative_view_recursively(variant_associative_view& view, const Value& json_array_value);

	void extract_atomic_types(variant& var, const Value& json_value);

	void extract_atomic_types_from_type(variant& var, const rttr::type t, const Value& json_value);

	void extract_atomic_types_from_type(variant& var, const rttr::type t, const Value& json_value)
	{
		if (t.is_arithmetic())
		{
			if (t == type::get<bool>())
				var = json_value.GetBool();
			else if (t == type::get<char>())
				var = static_cast<char>(json_value.GetBool());
			else if (t == type::get<int8_t>())
				var = static_cast<int8_t>(json_value.GetInt());
			else if (t == type::get<int16_t>())
				var = static_cast<int16_t>(json_value.GetInt());
			else if (t == type::get<int32_t>())
				var = static_cast<int32_t>(json_value.GetInt());
			else if (t == type::get<int64_t>())
				var = static_cast<int64_t>(json_value.GetInt64());
			else if (t == type::get<uint8_t>())
				var = static_cast<uint8_t>(json_value.GetUint());
			else if (t == type::get<uint16_t>())
				var = static_cast<uint16_t>(json_value.GetUint());
			else if (t == type::get<uint32_t>())
				var = static_cast<uint32_t>(json_value.GetUint());
			else if (t == type::get<uint64_t>())
				var = static_cast<uint64_t>(json_value.GetUint64());
			else if (t == type::get<float>())
				var = static_cast<float>(json_value.GetFloat());
			else if (t == type::get<double>())
				var = static_cast<float>(json_value.GetDouble());
		}
		else if (t.is_enumeration())
		{
			var = static_cast<int8_t>(json_value.GetInt());
		}
		else if (t == type::get<std::string>())
		{
			var = std::string((json_value.GetString()));
		}
	}

	//Implementations

	bool write_atomic_types_to_json(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		if (t.is_arithmetic())
		{
			if (t == type::get<bool>())
				writer.Bool(var.to_bool());
			else if (t == type::get<char>())
				writer.Bool(var.to_bool());
			else if (t == type::get<int8_t>())
				writer.Int(var.to_int8());
			else if (t == type::get<int16_t>())
				writer.Int(var.to_int16());
			else if (t == type::get<int32_t>())
				writer.Int(var.to_int32());
			else if (t == type::get<int64_t>())
				writer.Int64(var.to_int64());
			else if (t == type::get<uint8_t>())
				writer.Uint(var.to_uint8());
			else if (t == type::get<uint16_t>())
				writer.Uint(var.to_uint16());
			else if (t == type::get<uint32_t>())
				writer.Uint(var.to_uint32());
			else if (t == type::get<uint64_t>())
				writer.Uint64(var.to_uint64());
			else if (t == type::get<float>())
				writer.Double(var.to_float());
			else if (t == type::get<double>())
				writer.Double(var.to_double());
		
			return true;
		}
		else if (t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if (ok)
			{
				writer.String(var.to_string().c_str());
			}
			else
			{
				ok = false;
				auto value = var.to_uint64(&ok);
				if (ok)
					writer.Uint64(value);
				else
					writer.Null();
			}
		
			return true;
		}
		else if (t == type::get<std::string>())
		{
			writer.String(var.to_string().c_str());
			return true;
		}
		
		return false;
	}

	void write_array(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer)
	{
		writer.StartArray();
		for (const auto& item : view)
		{
			if (item.is_sequential_container())
			{
				write_array(item.create_sequential_view(), writer);
			}
			else
			{
				variant wrapped_var = item.extract_wrapped_value();
				type value_type = wrapped_var.get_type();
				if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
				{
					write_atomic_types_to_json(value_type, wrapped_var, writer);
				}
				else // object
				{
					to_json_recursively(wrapped_var, writer);
				}
			}
		}
		writer.EndArray();
	}

	void write_associative_container(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer)
	{
		static const string_view key_name("key");
		static const string_view value_name("value");
		
		writer.StartArray();
		
		if (view.is_key_only_type())
		{
			for (auto& item : view)
			{
				write_variant(item.first, writer);
			}
		}
		else
		{
			for (auto& item : view)
			{
				writer.StartObject();
				writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
		
				write_variant(item.first, writer);
		
				writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
		
				write_variant(item.second, writer);
		
				writer.EndObject();
			}
		}
		
		writer.EndArray();
	}

	bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		rttr::type value_type = var.get_type();
		bool is_wrapper = value_type.is_wrapper();
		value_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	
		
		if (write_atomic_types_to_json(value_type,
			is_wrapper ? var.extract_wrapped_value() : var, writer))
		{
		}
		else if (var.is_sequential_container())
		{
			write_array(var.create_sequential_view(), writer);
		}
		else if (var.is_associative_container())
		{
			write_associative_container(var.create_associative_view(), writer);
		}
		else //Object
		{
			auto child_props =value_type.get_properties();
			if (!child_props.empty())
			{
				to_json_recursively(var, writer);
			}
			else
			{
				bool ok = false;
				auto text = var.to_string(&ok);
				if (!ok)
				{
					writer.String(text.c_str());
					return false;
				}
		
				writer.String(text.c_str());
			}
		}
		
		return true;
	}

	void to_json_recursively(const instance& obj2, PrettyWriter<StringBuffer>& writer)
	{
		writer.StartObject();
		instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		
		std::string ClassName = obj.get_derived_type().get_name().to_string();
		
		writer.String("Class");
		writer.String(ClassName.c_str());
		
		auto prop_list = obj.get_derived_type().get_properties();
		for (auto prop : prop_list)
		{
			if (prop.get_metadata("NO_SERIALIZE") || prop.get_metadata("Transient"))
				continue;
		
			variant prop_value = prop.get_value(obj);
			if (!prop_value)
			{
				DEBUG_LOG_TEMP("Property value is not valid");
				continue; 
			}
			
			const auto name = prop.get_name();
			writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
			if (!write_variant(prop_value, writer))
			{
				DEBUG_LOG_TEMP("Cannot serialize property: %s", name.to_string().c_str());
			}
		}
		
		writer.EndObject();
	}

	void extract_atomic_types(variant& var, const Value& json_value)
	{
		switch (json_value.GetType())
		{
		case kStringType:
			{
				var = std::string(json_value.GetString());
				break;
			}
		case kNullType:
			{
				break;
			}
		case kFalseType:
			{
				var = json_value.GetBool();
				break;
			}
		case kTrueType:
			{
				var = json_value.GetBool();
				break;
			}
		case kNumberType:
			{
				if (json_value.IsInt())
					var = json_value.GetInt();
				else if (json_value.IsDouble())
					var = static_cast<float>(json_value.GetDouble());
				else if (json_value.IsUint())
					var = static_cast<uint32>(json_value.GetUint());
				else if (json_value.IsInt64())
					var = json_value.GetInt64();
				else if (json_value.IsUint64())
					var = json_value.GetUint64();
				break;
			}
			// we handle only the basic types here
		case kObjectType:
		case kArrayType: return;
		}
	}

	void extract_array_recursively(variant_sequential_view& view, const Value& json_array_value)
	{
		if (!view.is_valid())
		{
			DEBUG_LOG_TEMP("Variant sequential view is invalid");
			return;
		}
		
		const size_t size = json_array_value.Size();
		if (false == view.set_size(size))
		{
			DEBUG_LOG_TEMP("Couldn't set sequential view size");
			return;
		}
		
		for (SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			auto& json_index_value = json_array_value[i];
			
			rttr::variant element_var = view.get_value(i);
			rttr::type element_type = element_var.get_type();

			from_json_recursively(element_var, element_type, json_index_value);
			
			if(!view.set_value(i, element_var))
			{
				DEBUG_LOG_TEMP("Unabe to set value to view");
				return;
			}
		}
	}

	void extract_associative_view_recursively(variant_associative_view& view, const Value& json_array_value)
	{
		if(!view.is_valid())
		{
			DEBUG_LOG_TEMP("associative view is invalid");
			return;
		}
		
		rttr::type key_type = view.get_key_type();
		rttr::type value_type = view.get_value_type();
		
		for (SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			// Extract key value
			rttr::variant key_variant;
			Value::ConstMemberIterator json_key_value = json_array_value[i].FindMember("key");
			from_json_recursively(key_variant, key_type, json_key_value->value);

			// Extract value
			rttr::variant value_variant;
			Value::ConstMemberIterator json_value = json_array_value[i].FindMember("value");
			
			from_json_recursively(value_variant, value_type, json_value->value);

			if(!key_variant.is_valid())
			{
				DEBUG_LOG_TEMP("key variant is invalid");
				return;
			}

			if(!value_variant.is_valid())
			{
				DEBUG_LOG_TEMP("value variant is invalid");
				return;
			}

			view.insert(key_variant, value_variant);
		}
	}

	
	void from_json_recursively(rttr::variant& variant, const Value& json_object)
	{
		if (!json_object.HasMember("Class"))
		{
			DEBUG_LOG_TEMP("Couldn't find class member!");
			return;
		}

		rttr::type type_from_json = type::get_by_name(json_object["Class"].GetString());
		if (!type_from_json.is_valid())
		{
			DEBUG_LOG_TEMP("Invalid type");
			return;
		}
	
		from_json_recursively(variant, type_from_json, json_object);
	}
	
	void from_json_recursively(rttr::variant& variant, const rttr::type& variant_type, const Value& json_object)
	{
		if(!variant_type.is_valid())
		{
			DEBUG_LOG_TEMP("Variant type is invalid");
			return;
		}
		
		const bool is_wrapper = variant_type.is_wrapper();
		const rttr::type& value_type = is_wrapper ? variant_type.get_wrapped_type() : variant_type;

		switch (json_object.GetType())
		{
		case kObjectType:
			{
				if (!json_object.HasMember("Class"))
				{
					DEBUG_LOG_TEMP("Couldn't find class member!");
					break;
				}

				rttr::type updated_variant_type = type::get_by_name(json_object["Class"].GetString());
				variant = updated_variant_type.create();
				
				auto prop_list = updated_variant_type.get_properties();
				for (const rttr::property& prop : prop_list)
				{
					if (prop.is_readonly())
					{
						DEBUG_LOG_TEMP("Property is not writable!");
						continue;
					}

					const Value::ConstMemberIterator& ret = json_object.FindMember(prop.get_name().to_string().c_str());
					if (ret == json_object.MemberEnd())
						continue;

					const auto& property_json_value = ret->value;
					rttr::variant new_property_var = prop.get_value(variant);
					rttr::type property_type = prop.get_type();
					from_json_recursively(new_property_var, property_type, property_json_value);
					
					if (!prop.set_value(variant, new_property_var))
					{
						DEBUG_LOG_TEMP("Unable to set property");
						return;
					}
				}

				const rttr::type baseComponentType = type::get_by_name("BaseComponent*");
				if (updated_variant_type.is_derived_from(baseComponentType))
				{
					if (variant.can_convert(baseComponentType))
					{
						variant.convert(baseComponentType);
					}
				}
				break;
			}
		case kArrayType:
			{
				if (value_type.is_sequential_container())
				{
					variant_sequential_view sequential_view = variant.create_sequential_view();
					extract_array_recursively(sequential_view, json_object);
				}
				else if (value_type.is_associative_container())
				{
					rttr::variant_associative_view associative_view = variant.create_associative_view();
					extract_associative_view_recursively(associative_view, json_object);
				}
				break;
			}
		default:
			{
				extract_atomic_types_from_type(variant, value_type, json_object);
			}
		}
	}
}


namespace Nx
{
	std::string scene_to_json(Scene scene)
	{
		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);
		
		writer.StartObject();
		
		writer.String("Scene");
		writer.String(scene.sceneName.c_str());
		
		writer.String("GameObjects");
		writer.StartArray();
		
		for(GameObject* game_object : scene.sceneObjects)
		{
			to_json_recursively(game_object, writer);
		}
		
		writer.EndArray();
		writer.EndObject();

		return sb.GetString();
	}

	Scene parse_scene(ECS::World* World, NString sceneFile)
	{
		Scene outScene;
		outScene.sceneName = sceneFile;
		
		Document doc;
		std::ifstream inputFile;
		inputFile.open(sceneFile.c_str());
		if (!inputFile.is_open())
		{
			DEBUG_LOG_TEMP(" SceneManager::LoadScene: Could not open file for reading");
			return outScene;
		}
		
		rapidjson::IStreamWrapper ISW(inputFile);
		doc.ParseStream(ISW);
		
		DEBUG_LOG_TEMP("SceneManager::LoadScene");
		const rapidjson::Value& GameObjects = doc["GameObjects"].GetArray();
		
		const Value& jsonObjectValue = GameObjects[0];
		NString ClassName = jsonObjectValue["Class"].GetString();
		type GameObjectType = type::get_by_name(ClassName);
		//variant var = GameObjectType.create();
		variant var;
		
		from_json_recursively(var, jsonObjectValue);


		//Testing
		GameObject* obj = var.get_value<GameObject*>();
		obj->name = "TESTING";
		
		rttr::variant var_obj = obj;
		variant ComponentsProp = var_obj.get_type().get_property_value("Components", var_obj);
		variant_sequential_view view = ComponentsProp.create_sequential_view();
		
		for(auto const& it : view)
		{
			BaseComponent* BS = it.get_value<BaseComponent*>();
			DEBUG_LOG_TEMP("var type: %s", it.get_type().get_name().to_string().c_str());

			TransformComponent* TC = it.get_value<TransformComponent*>();
			DEBUG_LOG_TEMP("var type: %s", it.get_type().get_name().to_string().c_str());
		}
		
		
		// for (size_t i = 0; i < GameObjects.Size(); i++)
		// {
		// 	NString ClassName = GameObjects[i]["Class"].GetString();
		// 	
		// 	type NewGameObjectType = type::get_by_name(ClassName);
		// 	variant var = NewGameObjectType.create({std::string("test")});
		// 	const Value& NewjsonObjectValue = GameObjects[i];
		// 	
		// 	from_json_recursively(var, var.get_type(), jsonObjectValue);
		//
		// 	variant ComponentsProp = var.get_type().get_property_value("Components", var);
		// 	variant_sequential_view view = ComponentsProp.create_sequential_view();
		//
		// 	for(auto const& it : view)
		// 	{
		// 		//DEBUG_LOG_TEMP("var type: %s", it.get_type().get_name().to_string().c_str());
		// 	}
		// 	//NString ObjName = NameVar.create_sequential_view();
		// 	
		// 	GameObject* NewGameObject = &var.get_value<GameObject>();
		//
		// 	//variant VarTest = var.get_value<GameObject>();
		// 	rttr::instance objInstanceTest = var.get_value<GameObject>();
		// 	auto prop_list = objInstanceTest.get_wrapped_instance().get_type().get_properties();
		//
		// 	for(auto& it : objInstanceTest.get_type().get_properties())
		// 	{
		// 		//DEBUG_LOG_TEMP("var type: %s", it.get_name().to_string().c_str());
		// 	}
		//
		// 	outScene.AddObject(NewGameObject);
		// 	
		// 	//DEBUG_LOG_TEMP("var type: %s", var.get_type().get_name().to_string().c_str());
		// 	//DEBUG_LOG_TEMP("var name: %s", ObjName.c_str());
		// 	//std::cout << var.get_type().get_name() << "\n";
		// 	//string_view tst = var.get_type().get_name();
		// }

		return outScene;
	}
}
