#include <iostream>
#include <assert.h> 
#include "Window_Manager.h"
#include <string>
#include "solver.h"
#include <lua.hpp>
void run_io_test() 
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	std::string sep = "\n----------------------------------------\n";
	Matrix3d m1;
	VectorXf v1(4);
	v1 << 1, 2, 3, 4;
	m1 << 1.111111, 2, 3.33333, 4, 5, 6, 7, 8.888888, 9;
	IOFormat CommaInitFmt(StreamPrecision, DontAlignCols, ", ", ", ", "", "", " << ", ";");
	IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
	IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");
	IOFormat HeavyFmt(FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
	std::cout << m1 << sep;
	std::cout << v1.format(CleanFmt) << sep;
	std::cout << m1.format(CommaInitFmt) << sep;
	std::cout << m1.format(CleanFmt) << sep;
	std::cout << m1.format(OctaveFmt) << sep;
	std::cout << m1.format(HeavyFmt) << sep;
	system("pause");
}

void run_solver_test() 
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
	std::string sep = "\n----------------------------------------\n";
	mesh msh;
	VectorXf forces(6), meshfab(3), meshmat(3), nodxyz(6);
	VectorXi ftag(6), elenod(6);
	forces << 0, 0, 0, 0, 2, 1;
	meshfab << 1, 0.5, 2 * sqrt(2);
	meshmat << 100, 100, 100;
	nodxyz << 0, 0, 10, 0, 10, 10;
	elenod << 0, 1, 2, 1, 2, 0;
	ftag << to_underlying( force_type::UNKNOWN ),
		to_underlying( force_type::UNKNOWN ),
		to_underlying(force_type::KNOWN),
		to_underlying(force_type::UNKNOWN),
		to_underlying(force_type::KNOWN),
		to_underlying(force_type::KNOWN);

	msh.node_forces = forces;
	msh.props.mesh_fabrication = meshfab;
	msh.props.mesh_material = meshmat;
	msh.elements = elenod;
	msh.nodesxy = nodxyz;
	msh.forces_tag = ftag;
	msh.isComplete = true;
	if (msh.isComplete) 
	{
		solver vsolver(msh);
		std::cout << msh.node_forces.format(CleanFmt) << sep;
		std::cout << msh.props.mesh_fabrication.format(CleanFmt) << sep;
		std::cout << msh.props.mesh_material.format(CleanFmt) << sep;
		std::cout << msh.nodesxy.format(CleanFmt) << sep;
		std::cout << msh.elements.format(CleanFmt) << sep;
		system("pause");
	}
}

void runWindow() 
{
	Window_Manager wmanager;
	wmanager.wInit();
	wmanager.doLoop();
	wmanager.closeRoutine();
}
/*
Lua Print overload
*/
int PrintOut(lua_State *L, std::ostream& out)
{
	int n = lua_gettop(L);  /* number of arguments */
	if (!n)
		return 0;
	int i;
	lua_getglobal(L, "tostring");
	for (i = 1; i <= n; i++)
	{
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to "
				LUA_QL("print"));
		if (i > 1) out << "\t";
		out << s;
		lua_pop(L, 1);  /* pop result */
	}
	out << std::endl;
	return 0;
}

int lua_Print(lua_State *L)
{
	return PrintOut(L, std::cout);
}

void luaTest()
{
	
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	
	printf("Hello from VSC++\n");
	// Open lua
	lua_State *L = lua_open();

	// Load the libraries
	luaL_openlibs(L);
	lua_getglobal(L, "_G");
	lua_register(L, "print", &lua_Print);
	lua_pop(L, 1);
	// Execution of a lua string
	luaL_dostring(L, "print(\"Hello from LuaJIT\")");
	
	// Close lua
	lua_gc(L, LUA_GCCOLLECT, 0);
	lua_close(L);
	L = 0;
	system("pause");
}

int main(int argc, char ** argv) 
{

	//run_io_test();
	//runWindow();
	//run_solver_test();
	luaTest();
	return 0;
}