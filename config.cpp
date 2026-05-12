/*
	(c) 2025 | ZenSplitItemUI | Zenarchist
*/

class CfgPatches
{
	class ZenSplitItemUI
	{
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"ZenModCore"
		};
	};
};

class CfgMods
{
	class ZenSplitItemUI
	{
		author = "Zenarchist";
		type = "mod";
		version = "1.0";
		defines[]=
		{
			//"ZenSplitItemUIDebug"
		};
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		inputs = "ZenSplitItemUI/data/inputs.xml";
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = 
				{ 
					"ZenSplitItemUI/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value = "";
				files[] = 
				{ 
					"ZenSplitItemUI/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value = "";
				files[] = 
				{ 
					"ZenSplitItemUI/scripts/5_Mission"
				};
			};
		};
	};
};