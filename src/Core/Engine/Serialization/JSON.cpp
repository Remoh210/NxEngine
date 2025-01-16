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
	/////////////////////////////////////////////////////////////////////////////////////////

	void to_json_recursively(const instance& obj, PrettyWriter<StringBuffer>& writer);

	/////////////////////////////////////////////////////////////////////////////////////////

	bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer);

	bool write_atomic_types_to_json(const type& t, const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		// if (t.is_arithmetic())
		// {
		// 	if (t == type::get<bool>())
		// 		writer.Bool(var.to_bool());
		// 	else if (t == type::get<char>())
		// 		writer.Bool(var.to_bool());
		// 	else if (t == type::get<int8_t>())
		// 		writer.Int(var.to_int8());
		// 	else if (t == type::get<int16_t>())
		// 		writer.Int(var.to_int16());
		// 	else if (t == type::get<int32_t>())
		// 		writer.Int(var.to_int32());
		// 	else if (t == type::get<int64_t>())
		// 		writer.Int64(var.to_int64());
		// 	else if (t == type::get<uint8_t>())
		// 		writer.Uint(var.to_uint8());
		// 	else if (t == type::get<uint16_t>())
		// 		writer.Uint(var.to_uint16());
		// 	else if (t == type::get<uint32_t>())
		// 		writer.Uint(var.to_uint32());
		// 	else if (t == type::get<uint64_t>())
		// 		writer.Uint64(var.to_uint64());
		// 	else if (t == type::get<float>())
		// 		writer.Double(var.to_float());
		// 	else if (t == type::get<double>())
		// 		writer.Double(var.to_double());
		//
		// 	return true;
		// }
		// else if (t.is_enumeration())
		// {
		// 	bool ok = false;
		// 	auto result = var.to_string(&ok);
		// 	if (ok)
		// 	{
		// 		writer.String(var.to_string());
		// 	}
		// 	else
		// 	{
		// 		ok = false;
		// 		auto value = var.to_uint64(&ok);
		// 		if (ok)
		// 			writer.Uint64(value);
		// 		else
		// 			writer.Null();
		// 	}
		//
		// 	return true;
		// }
		// else if (t == type::get<std::string>())
		// {
		// 	writer.String(var.to_string());
		// 	return true;
		// }
		//
		// return false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	static void write_array(const variant_sequential_view& view, PrettyWriter<StringBuffer>& writer)
	{
		// writer.StartArray();
		// for (const auto& item : view)
		// {
		// 	if (item.is_sequential_container())
		// 	{
		// 		write_array(item.create_sequential_view(), writer);
		// 	}
		// 	else
		// 	{
		// 		variant wrapped_var = item.extract_wrapped_value();
		// 		type value_type = wrapped_var.get_type();
		// 		if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
		// 		{
		// 			write_atomic_types_to_json(value_type, wrapped_var, writer);
		// 		}
		// 		else // object
		// 		{
		// 			to_json_recursively(wrapped_var, writer);
		// 		}
		// 	}
		// }
		// writer.EndArray();
	}


	/////////////////////////////////////////////////////////////////////////////////////////

	static void write_associative_container(const variant_associative_view& view, PrettyWriter<StringBuffer>& writer)
	{
		// static const string_view key_name("key");
		// static const string_view value_name("value");
		//
		// writer.StartArray();
		//
		// if (view.is_key_only_type())
		// {
		// 	for (auto& item : view)
		// 	{
		// 		write_variant(item.first, writer);
		// 	}
		// }
		// else
		// {
		// 	for (auto& item : view)
		// 	{
		// 		writer.StartObject();
		// 		writer.String(key_name.data(), static_cast<rapidjson::SizeType>(key_name.length()), false);
		//
		// 		write_variant(item.first, writer);
		//
		// 		writer.String(value_name.data(), static_cast<rapidjson::SizeType>(value_name.length()), false);
		//
		// 		write_variant(item.second, writer);
		//
		// 		writer.EndObject();
		// 	}
		// }
		//
		// writer.EndArray();
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	bool write_variant(const variant& var, PrettyWriter<StringBuffer>& writer)
	{
		// auto value_type = var.get_type();
		// auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
		// bool is_wrapper = wrapped_type != value_type;
		//
		// if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
		// 	is_wrapper ? var.extract_wrapped_value() : var, writer))
		// {
		// }
		// else if (var.is_sequential_container())
		// {
		// 	write_array(var.create_sequential_view(), writer);
		// }
		// else if (var.is_associative_container())
		// {
		// 	write_associative_container(var.create_associative_view(), writer);
		// }
		// else //Object
		// {
		// 	auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
		// 	if (!child_props.empty())
		// 	{
		// 		to_json_recursively(var, writer);
		// 	}
		// 	else
		// 	{
		// 		bool ok = false;
		// 		auto text = var.to_string(&ok);
		// 		if (!ok)
		// 		{
		// 			writer.String(text);
		// 			return false;
		// 		}
		//
		// 		writer.String(text);
		// 	}
		// }
		//
		// return true;
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	void to_json_recursively(const instance& obj2, PrettyWriter<StringBuffer>& writer)
	{
		// writer.StartObject();
		// instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		//
		// std::string ClassName = obj.get_derived_type().get_name().to_string();
		// //Remove *
		// //ClassName.pop_back();
		//
		// writer.String("Class");
		// writer.String(ClassName);
		//
		// auto prop_list = obj.get_derived_type().get_properties();
		// for (auto prop : prop_list)
		// {
		// 	if (prop.get_metadata("NO_SERIALIZE"))
		// 		continue;
		//
		// 	variant prop_value = prop.get_value(obj);
		// 	if (!prop_value)
		// 		continue; // cannot serialize, because we cannot retrieve the value
		//
		// 	const auto name = prop.get_name();
		// 	writer.String(name.data(), static_cast<rapidjson::SizeType>(name.length()), false);
		// 	if (!write_variant(prop_value, writer))
		// 	{
		// 		std::cerr << "cannot serialize property: " << name << std::endl;
		// 	}
		// }
		//
		// writer.EndObject();
	}
	
	void extract_basic_types(variant& var, const Value& json_value)
	{
		//Get the class
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
					var = json_value.GetUint();
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
	
	static void extract_array_recursively(variant_sequential_view& view, const Value& json_array_value);

	void from_json_recursively(rttr::variant& variant, const rttr::type& variant_type, const Value& json_object);

	void extrect_components(rttr::variant& variant, const rttr::type& variant_type, const Value& json_object)
	{
		
	}

	bool extract_variant(variant& var, const Value& json_object)
	{
		// auto value_type = var.get_type();
		// auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
		// bool is_wrapper = wrapped_type != value_type;
		//
		// // if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
		// // 	is_wrapper ? var.extract_wrapped_value() : var, writer))
		// // {
		// // 	extract_basic_types(is_wrapper ? wrapped_type : value_type,
		// // 	is_wrapper ? var.extract_wrapped_value() : var);
		// // }
		//
		// if (var.is_sequential_container())
		// {
		// 	extract_array_recursively(var.create_sequential_view(), json_object);
		// }
		// else if (var.is_associative_container())
		// {
		// 	//write_associative_container(var.create_associative_view(), writer);
		// }
		// else
		// {
		// 	extract_basic_types(var, json_object);
		// }
		//
		return true;
	}
	
	static bool extract_array(const variant& var, Value& json_object)
	{
		return true;
	}

	static void extract_array_recursively(variant_sequential_view& view, const Value& json_array_value)
	{

		if (!view.is_valid())
		{
			return;
			std::cerr << "The sequential view is invalid.\n";
		}
		
		int32 Size = json_array_value.Size();
		int32 ViewSize = view.get_size();
		if (false == view.set_size(Size))
		{
			return;
		}
		int32 ViewSizeAfter = view.get_size();
		const type array_value_type = view.get_rank_type(1);
		
		for (SizeType i = 0; i < json_array_value.Size(); ++i)
		{
			// Get the variant of the element at index i
			rttr::variant element_var = view.get_value(i);
		
			// Get the type of the element
			rttr::type element_type = element_var.get_type();
			
			
			auto& json_index_value = json_array_value[i];
			if (json_index_value.IsArray())
			{
				auto sub_array_view = view.get_value(i).create_sequential_view();
				extract_array_recursively(sub_array_view, json_index_value);
		
				view.set_value(i, sub_array_view);
			}
			else if (json_index_value.IsObject())
			{
				if(json_index_value.HasMember("Class"))
				{
					type objectType = type::get_by_name(json_index_value["Class"].GetString());
					variant var_tmp = objectType.create();
					from_json_recursively(var_tmp, objectType, json_index_value);

					
					//TODO Make it dynamic for all classes
					const rttr::type baseComponentType = type::get_by_name("BaseComponent*");
					if (objectType.is_derived_from(baseComponentType))
					{
						if (var_tmp.can_convert(baseComponentType))
						{
							var_tmp.convert(baseComponentType);
						}
					}
					
					if(!view.set_value(i, var_tmp))
					{
						DEBUG_LOG_TEMP("Unabe to set value to view");
						return;
					}
					
					//rttr::instance objectInstance = objectType.create();
					//type objectTypeFromInstance = objectInstance.get_type();

					// if (!objectType.get_base_classes().empty())
					// {
					// 	const rttr::type baseComponentType = *objectInstance.get_type().get_base_classes().begin();
					// 	if (objectType.is_derived_from(baseComponentType))
					// 	{
					// 		if (var_tmp.can_convert(baseComponentType))
					// 		{
					// 			var_tmp.convert(baseComponentType);
					// 		}
					// 	}
					// }
					
				}
			}
			else
			{
				variant extracted_value;
				extract_atomic_types(extracted_value, json_index_value);
				view.set_value(i, extracted_value);
			}
		}
	}

	variant extract_value(Value::ConstMemberIterator& itr, const type& t)
	{
		auto& json_value = itr->value;
		variant extracted_value;
		///??
		// extract_basic_types(extracted_value, json_value);
		// const bool could_convert = extracted_value.convert(t);
		// if (!could_convert)
		// {
		// 	if (json_value.IsObject())
		// 	{
		// 		constructor ctor = t.get_constructor();
		// 		for (auto& item : t.get_constructors())
		// 		{
		// 			if (item.get_instantiated_type() == t)
		// 				ctor = item;
		// 		}
		// 		extracted_value = ctor.invoke();
		// 		from_json_recursively(extracted_value, t, json_value);
		// 	}
		// }
	
		return extracted_value;
	}

	static void extract_associative_view_recursively(variant_associative_view& view, const Value& json_array_value)
	{
		// for (SizeType i = 0; i < json_array_value.Size(); ++i)
		// {
		// 	auto& json_index_value = json_array_value[i];
		// 	if (json_index_value.IsObject()) // a key-value associative view
		// 	{
		// 		Value::ConstMemberIterator key_itr = json_index_value.FindMember("key");
		// 		Value::ConstMemberIterator value_itr = json_index_value.FindMember("value");
		//
		// 		if (key_itr != json_index_value.MemberEnd() &&
		// 			value_itr != json_index_value.MemberEnd())
		// 		{
		// 			auto key_var = extract_value(key_itr, view.get_key_type());
		// 			auto value_var = extract_value(value_itr, view.get_value_type());
		// 			if (key_var && value_var)
		// 			{
		// 				view.insert(key_var, value_var);
		// 			}
		// 		}
		// 	}
		// 	else // a key-only associative view
		// 	{
		// 		variant extracted_value;
		// 		extract_basic_types(extracted_value, json_index_value);
		// 		if (extracted_value && extracted_value.convert(view.get_key_type()))
		// 		{
		// 			view.insert(extracted_value);
		// 		}
		// 	}
		// }
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	void from_json_recursively(rttr::variant& variant, const rttr::type& variant_type, const Value& json_object)
	{
		if(json_object.IsObject())
		{
			const bool is_wrapper = variant_type.is_wrapper();
			const rttr::type& value_type = variant_type.is_wrapper() ? variant_type.get_wrapped_type() : variant_type;
			auto prop_list = value_type.get_properties();
			for (auto& prop : prop_list)
			{

				if (prop.is_readonly())
				{
					DEBUG_LOG_TEMP("Property is not writable!");
					continue;
				}
				
				const Value::ConstMemberIterator& ret = json_object.FindMember(prop.get_name().to_string().c_str());
				if (ret == json_object.MemberEnd())
					continue;
				
				const type value_t = prop.get_type();
				const auto& json_value = ret->value;
				
				switch (json_value.GetType())
				{
				case kArrayType:
					{
						if (value_t.is_sequential_container())
						{
							if (!variant.is_valid())
							{
								DEBUG_LOG_TEMP("!Error Creating sequential container, varian is invalid");
								return;
							}
							
							rttr::variant ArrayVar = prop.get_value(variant);
							variant_sequential_view view = ArrayVar.create_sequential_view();
							extract_array_recursively(view, json_value);
							size_t ViewSize = view.get_size();

							if(!prop.set_value(variant, ArrayVar))
							{
								DEBUG_LOG_TEMP("!Error setting sequential container property");
								return;
							}
							
							size_t TestSize = view.get_size();
						}
						else if (value_t.is_associative_container())
						{
							variant_associative_view associative_view;
							extract_associative_view_recursively(associative_view, json_value);
							prop.set_value(variant, associative_view);
						}
						
						break;
					}
				case kObjectType:
					{
						if(json_value.HasMember("Class"))
						{
							rttr::variant PropertyVar;
							
							type objectType = type::get_by_name(json_value["Class"].GetString());
							PropertyVar = objectType.create();
							
							if (!variant.is_valid())
							{
								DEBUG_LOG_TEMP("!Error Creating object of class %s from property, varian is invalid", json_value["Class"].GetString());
								return;
							}
							if (!PropertyVar.is_valid())
							{
								DEBUG_LOG_TEMP("!Error Creating object of class %s from property", json_value["Class"].GetString());
								return;
							}

							rttr:type TCType = type::get_by_name("vec3f");
							
							if(objectType == TCType)
							{
								vec3f TestVec = PropertyVar.get_value<vec3f>();
								DEBUG_LOG_TEMP("vec3f In");
							}
							
							from_json_recursively(PropertyVar, objectType, json_value);
							
							if (!prop.set_value(variant, PropertyVar))
							{
								DEBUG_LOG_TEMP("Unable to set property");
								return;
							}

							if(objectType == TCType)
							{
								vec3f TestVec = PropertyVar.get_value<vec3f>();
								DEBUG_LOG_TEMP("vec3f Out");
							}
						}
						break;
					}
				default:
					{
						rttr::variant PropertyVar = prop.get_value(variant);

						rttr::type PropType = PropertyVar.get_type();
						if(!PropertyVar.is_valid())
						{
							DEBUG_LOG_TEMP("Unable to set property, variant invalid");
							return;
						}
						
						extract_atomic_types(PropertyVar, json_value);
						
						if (!prop.set_value(variant, PropertyVar))
						{
							DEBUG_LOG_TEMP("Unable to set property");
							return;
						}
						
						// from_json_recursively(PropertyVar, json_value);
						// extract_basic_types(PropertyVar, json_value);
						// prop.set_value(variant, PropertyVar);
						// break;
						// if (extracted_value.convert(value_t)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
						// 	prop.set_value(obj, extracted_value);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace Nx
{
	std::string scene_to_json(Scene scene)
	{
		StringBuffer sb;
		// PrettyWriter<StringBuffer> writer(sb);
		//
		// writer.StartObject();
		//
		// writer.String("Scene");
		// writer.String(scene.sceneName);
		//
		// writer.String("GameObjects");
		// writer.StartArray();
		//
		// for(GameObject* game_object : scene.sceneObjects)
		// {
		// 	to_json_recursively(game_object, writer);
		// }
		//
		// writer.EndArray();
		// writer.EndObject();

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
		variant var = GameObjectType.create();
		
		from_json_recursively(var, GameObjectType, jsonObjectValue);
		
		GameObject* obj = var.get_value<GameObject*>();
		obj->name = "TESTING";
		
		for(auto& it : obj->get_type().get_properties())
		{
			DEBUG_LOG_TEMP("var type: %s", it.get_name().to_string().c_str());
		}

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

	// Scene CreateSceneFromFile(ECS::World* world, const NString& name)
	// {
	// 	Scene outScene;
	// 	outScene.sceneName = name;
	//
	//
	// 	return outScene;
	// }
	
	std::string to_json(rttr::instance obj)
	{
		if (!obj.is_valid())
		{
			return std::string("Invalid");
		}

		StringBuffer sb;
		PrettyWriter<StringBuffer> writer(sb);

		to_json_recursively(obj, writer);

		return sb.GetString();
	}

	bool from_json(Document& doc)
	{
		// const rapidjson::Value& GameObjects = doc["GameObjects"].GetArray();
		// for (int i = 0; i < GameObjects.Size(); i++)
		// {
		// 	NString ClassName = GameObjects[i]["Class"].GetString();
		// 	
		// 	type t = type::get_by_name(ClassName);
		//
		// 	variant var = t.create({std::string(ClassName)});
		// 	std::cout << var.get_type().get_name() << "\n";
		//
		// }

		return true;
	}
}
