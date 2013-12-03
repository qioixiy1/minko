#ifndef LUAGLUE_METHOD_H_GUARD
#define LUAGLUE_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGlueDebug.h"

#include "LuaGlue/LuaGlueBase.h"

template<typename _Class>
class LuaGlueClass;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;

	public:
		typedef _Class ClassType;
		typedef typename std::remove_reference<typename std::remove_const<_Ret>::type>::type ReturnType;
		typedef _Ret (_Class::*MethodType)( _Args... );
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ /*printf("new class %s method %s\n", typeid(_Class).name(), typeid(MethodType).name());*/ }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		int invoke(lua_State *state)
		{
			//printf("invoker: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)lua_touserdata(state, 1);
#endif
			ReturnType ret = applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			
			stack<ReturnType>::put(glueClass->luaGlue(), state, ret);
			return 1;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...> : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef void (_Class::*MethodType)(_Args...);
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ /*printf("new class %s method %s\n", typeid(_Class).name(), typeid(MethodType).name());*/ }

		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		int invoke(lua_State *state)
		{
			//printf("invokev: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)lua_touserdata(state, 1);
#endif
			//printf("obj: %p\n", obj);
			applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueConstMethod : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef typename std::remove_reference<typename std::remove_const<_Ret>::type>::type ReturnType;
		typedef _Ret (_Class::*MethodType)( _Args... ) const;
		
		LuaGlueConstMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ /*printf("new class %s method %s\n", typeid(_Class).name(), typeid(MethodType).name());*/ }
		
		~LuaGlueConstMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		int invoke(lua_State *state)
		{
			//printf("invoker: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)lua_touserdata(state, 1);
#endif
			ReturnType ret = applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			
			stack<ReturnType>::put(glueClass->luaGlue(), state, ret);
			return 1;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueConstMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueConstMethod<void, _Class, _Args...> : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef void (_Class::*MethodType)(_Args...) const;
		
		LuaGlueConstMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ /*printf("new class %s method %s\n", typeid(_Class).name(), typeid(MethodType).name());*/ }

		~LuaGlueConstMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		int invoke(lua_State *state)
		{
			//printf("invokev: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)lua_touserdata(state, 1);
#endif
			//printf("obj: %p\n", obj);
			applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueConstMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

// shared_ptr specializations

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod<_Ret, std::shared_ptr<_Class>, _Args...> : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> SharedType;
		typedef typename std::remove_reference<typename std::remove_const<_Ret>::type>::type ReturnType;
		typedef _Ret (_Class::*MethodType)( _Args... );
		
		LuaGlueMethod(LuaGlueClass<ClassType> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ LG_Debug("new shared class %s method %s", typeid(_Class).name(), typeid(MethodType).name()); }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		int invoke(lua_State *state)
		{
			//printf("invoker: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<SharedType> obj = *(LuaGlueObject<SharedType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<SharedType> obj = *(LuaGlueObject<SharedType> *)lua_touserdata(state, 1);
#endif
			ReturnType ret = applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			
			stack<ReturnType>::put(glueClass->luaGlue(), state, ret);
			return 1;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, std::shared_ptr<_Class>, _Args...> : public LuaGlueMethodBase
{
	private:
		template <typename... T>
 		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> SharedType;
		typedef void (_Class::*MethodType)(_Args...);
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ LG_Debug("new void shared class %s method %s", typeid(_Class).name(), typeid(MethodType).name()); }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		int invoke(lua_State *state)
		{
			//printf("invokev: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<SharedType> obj = *(LuaGlueObject<SharedType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<SharedType> obj = *(LuaGlueObject<SharedType> *)lua_touserdata(state, 1);
#endif
			//printf("obj: %p\n", obj);
			applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};


#endif /* LUAGLUE_METHOD_H_GUARD */
