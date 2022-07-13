#pragma once
#include <string>
#include "string_utils.h"
class CVarParameter;

enum class CVarFlags : uint32_t
{
	None = 0,
	Noedit = 1 << 1,
	EditReadOnly = 1 << 2,
	Advanced = 1 << 3,
};



class CVarSystem
{
public:
	static CVarSystem* Get();

	virtual CVarParameter* GetCVar(StringUtils::StringHash hash) = 0;

	virtual double* GetFloatCVar(StringUtils::StringHash hash) = 0;

	virtual int32_t* GetIntCVar(StringUtils::StringHash hash) = 0;

	virtual const char* GetStringCVar(StringUtils::StringHash string) = 0;

	//virtual ImVec4 GetVec4CVar(StringUtils::StringHash hash) = 0;


	virtual void SetFloatCVar(StringUtils::StringHash hash, double value) = 0;

	virtual void SetIntCVar(StringUtils::StringHash hash, int32_t value) = 0;

	virtual void SetStringCVar(StringUtils::StringHash hash, const char* value) = 0;

	//virtual void SetVec4CVar(StringUtils::StringHash hash, ImVec4 value) = 0;


	virtual CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue) = 0;

	virtual CVarParameter* CreateIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue) = 0;

	virtual CVarParameter* CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) = 0;
	
	//virtual CVarParameter* CreateVec4CVar(const char* name, const char* description, ImVec4 defaultValue, ImVec4 currentValue) = 0;

};


template <typename T>
struct AutoCVar
{
protected:
	int index;
	using CVarType = T;
};

struct AutoCVar_Float : AutoCVar<double>
{
	AutoCVar_Float(const char* name, const char* description, double defaultValue, CVarFlags flags = CVarFlags::None);

	double Get();
	double* GetPtr();
	float GetFloat();
	float* GetFloatPtr();
	void Set(double val);
};

struct AutoCVar_Int : AutoCVar<int32_t>
{
	AutoCVar_Int(const char* name, const char* description, int32_t defaultValue, CVarFlags flags = CVarFlags::None);

	int32_t Get();
	int32_t* GetPtr();

	void Set(int32_t val);

	void Toggle();
};

struct AutoCVar_String : AutoCVar<std::string>
{
	AutoCVar_String(const char* name, const char* description, const char* defaultValue, CVarFlags flags = CVarFlags::None);

	const char* Get();
	void Set(std::string&& val);
};

//struct AutoCVar_Vec4 : AutoCVar<ImVec4>
//{
//	AutoCVar_Vec4(const char* name, const char* description, ImVec4 defaultValue, CVarFlags flags = CVarFlags::None);
//
//	const char* Get();
//	void Set(ImVec4& val);
//};

