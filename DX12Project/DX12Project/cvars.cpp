#include "cvars.h"
#include <unordered_map>


enum class CVarType : char
{
	INT,
	FLOAT,
	STRING,
};

class CVarParameter
{
public:
	friend class CVarSystemImpl;

	int32_t arrayIndex;

	CVarType type;
	CVarFlags flags;
	std::string name;
	std::string description;
};

template<typename T>
struct CVarStorage
{
	T initial;
	T current;
	CVarParameter* parameter;
};

template<typename T>
struct CVarArray
{
	CVarStorage<T>* cvars;
	int32_t lastCVar{ 0 };

	CVarArray(size_t size)
	{
		cvars = new CVarStorage <T>[size]();
	}

	T GetCurrent(int32_t index)
	{
		return cvars[index].current;
	}

	T* GetCurrentPtr(int32_t index)
	{
		return &cvars[index].current;
	};

	void SetCurrent(const T& val, int32_t index)
	{
		cvars[index].current = val;
	}

	int Add(const T& value, CVarParameter* param)
	{
		int index = lastCVar;

		cvars[index].current = value;
		cvars[index].initial = value;
		cvars[index].parameter = param;
		
		param->arrayIndex = index;
		lastCVar++;
		return index;
	}

	int Add(const T& initialValue, const T& currentValue, CVarParameter* param)
	{
		int index = lastCVar;

		cvars[index].current = currentValue;
		cvars[index].initial = initialValue;
		cvars[index].parameter = param;

		param->arrayIndex = index;
		lastCVar++;

		return index;
	}
};

class CVarSystemImpl : public CVarSystem
{

public:


	//Overrides

	CVarParameter* GetCVar(StringUtils::StringHash hash) override final;

	CVarParameter* CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue) override final;

	CVarParameter* CreateIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue) override final;

	CVarParameter* CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue) override final;

	double* GetFloatCVar(StringUtils::StringHash hash) override final;
	int32_t* GetIntCVar(StringUtils::StringHash hash) override final;
	const char* GetStringCVar(StringUtils::StringHash hash) override final;


	void SetFloatCVar(StringUtils::StringHash hash, double value) override final;

	void SetIntCVar(StringUtils::StringHash hash, int32_t value) override final;

	void SetStringCVar(StringUtils::StringHash hash, const char* value) override final;

	//override ends


	constexpr static int MAX_INT_CVARS = 1000;
	CVarArray<int32_t> intCVars2{ MAX_INT_CVARS };

	constexpr static int MAX_FLOAT_CVARS = 1000;
	CVarArray<double> floatCVars{ MAX_FLOAT_CVARS };

	constexpr static int MAX_STRING_CVARS = 200;
	CVarArray<std::string> stringCVars{ MAX_STRING_CVARS };


	//With these template get functions you can do things such as auto array = GetCVarArray<int32_t>(); which gets an array for all int32 cvars

	template<typename T>
	CVarArray<T>* GetCVarArray();

	template<>
	CVarArray<int32_t>* GetCVarArray()
	{
		return &intCVars2;
	}

	template<>
	CVarArray<double>* GetCVarArray()
	{
		return &floatCVars;
	}

	template<>
	CVarArray<std::string>* GetCVarArray()
	{
		return &stringCVars;
	}


	//templated get and set functions for the current value of the cvar
	template<typename T>
	T* GetCVarCurrent(uint32_t namehash)
	{
		CVarParameter* par = GetCVar(namehash);
		if (!par)
		{
			return nullptr;
		}
		else
		{
			return GetCVarArray<T>()->GetCurrentPtr(par->arrayIndex);
		}
	}

	template<typename T>
	void SetCVarCurrent(uint32_t namehash, const T& value)
	{
		CVarParameter* CVar = GetCVar(namehash);
		if (CVar)
		{
			GetCVarArray<T>()->SetCurrent(value, CVar->arrayIndex);
		}
	}

	static CVarSystemImpl* Get()
	{
		return static_cast<CVarSystemImpl*>(CVarSystem::Get());
	}

private:
	CVarParameter* InitCVar(const char* name, const char* description);

	std::unordered_map<uint32_t, CVarParameter> savedCVars;
};


CVarSystem* CVarSystem::Get()
{
	static CVarSystemImpl cvarSys{};
	return &cvarSys;
}


CVarParameter* CVarSystemImpl::InitCVar(const char* name, const char* description)
{
	if (GetCVar(name)) return nullptr; // If CVar already exists, return

	int32_t namehash = StringUtils::StringHash{ name };
	savedCVars[namehash] = CVarParameter{};

	CVarParameter& newParam = savedCVars[namehash];

	newParam.name = name;
	newParam.description = description;

	return &newParam;
}

CVarParameter* CVarSystemImpl::CreateFloatCVar(const char* name, const char* description, double defaultValue, double currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr; // CVar already exists

	param->type = CVarType::FLOAT;

	GetCVarArray<double>()->Add(defaultValue, currentValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(const char* name, const char* description, int32_t defaultValue, int32_t currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr; // CVar already exists

	param->type = CVarType::INT;

	GetCVarArray<int32_t>()->Add(defaultValue, currentValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(const char* name, const char* description, const char* defaultValue, const char* currentValue)
{
	CVarParameter* param = InitCVar(name, description);
	if (!param) return nullptr; // CVar already exists

	param->type = CVarType::STRING;

	GetCVarArray<std::string>()->Add(defaultValue, currentValue, param);

	return param;
}

CVarParameter* CVarSystemImpl::GetCVar(StringUtils::StringHash hash)
{
	auto it = savedCVars.find(hash);

	if (it != savedCVars.end())
	{
		return &(*it).second;
	}

	return nullptr;
}

double* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<double>(hash);
}

int32_t* CVarSystemImpl::GetIntCVar(StringUtils::StringHash hash)
{
	return GetCVarCurrent<int32_t>(hash);
}

const char* CVarSystemImpl::GetStringCVar(StringUtils::StringHash hash)
{
	return nullptr;
}

void CVarSystemImpl::SetFloatCVar(StringUtils::StringHash hash, double value)
{
	SetCVarCurrent<double>(hash, value);
}

void CVarSystemImpl::SetIntCVar(StringUtils::StringHash hash, int32_t value)
{
	SetCVarCurrent<int32_t>(hash, value);
}

void CVarSystemImpl::SetStringCVar(StringUtils::StringHash hash, const char* value)
{
	SetCVarCurrent<std::string>(hash, value);
}

template<typename T>
T GetCVarCurrentByIndex(int32_t index)
{
	return CVarSystemImpl::Get()->GetCVarArray<T>()->GetCurrent(index);
}

template <typename T>
T* PtrGetCVarCurrentByIndex(int32_t index)
{
	return CVarSystemImpl::Get()->GetCVarArray<T>()->GetCurrentPtr(index);
}

template <typename T>
void SetCVarCurrentByIndex(int32_t index, const T& data)
{
	CVarSystemImpl::Get()->GetCVarArray<T>()->SetCurrent(data, index);
}


//Auto CVar implementation
//================================================

AutoCVar_Float::AutoCVar_Float(const char* name, const char* description, double defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateFloatCVar(name, description, defaultValue, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}


double AutoCVar_Float::Get()
{
	return GetCVarCurrentByIndex<CVarType>(index);
}

double* AutoCVar_Float::GetPtr()
{
	return PtrGetCVarCurrentByIndex<CVarType>(index);
}

float AutoCVar_Float::GetFloat()
{
	return static_cast<float>(Get());
}

float* AutoCVar_Float::GetFloatPtr()
{
	float* result = reinterpret_cast<float*>(GetPtr());
	return result;
}

void AutoCVar_Float::Set(double f)
{
	SetCVarCurrentByIndex<CVarType>(index, f);
}

AutoCVar_Int::AutoCVar_Int(const char* name, const char* description, int32_t defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateIntCVar(name, description, defaultValue, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

int32_t AutoCVar_Int::Get()
{
	return GetCVarCurrentByIndex<CVarType>(index);
}

int32_t* AutoCVar_Int::GetPtr()
{
	return PtrGetCVarCurrentByIndex<CVarType>(index);
}

void AutoCVar_Int::Set(int32_t val)
{
	SetCVarCurrentByIndex<CVarType>(index, val);
}

void AutoCVar_Int::Toggle()
{
	bool enabled = Get() != 0;

	Set(enabled ? 0 : 1);
}

AutoCVar_String::AutoCVar_String(const char* name, const char* description, const char* defaultValue, CVarFlags flags)
{
	CVarParameter* cvar = CVarSystem::Get()->CreateStringCVar(name, description, defaultValue, defaultValue);
	cvar->flags = flags;
	index = cvar->arrayIndex;
}

const char* AutoCVar_String::Get()
{
	return GetCVarCurrentByIndex<CVarType>(index).c_str();
};

void AutoCVar_String::Set(std::string&& val)
{
	SetCVarCurrentByIndex<CVarType>(index, val);
}

