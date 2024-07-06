class CfgPatches
{
	class ZenSplitItemUI
	{
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};

class CfgMods
{
	class ZenSplitItemUI
	{
		dir = "ZenSplitItemUI";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "ZenSplitItemUI";
		credits = "";
		author = "Zenarchist";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game","World","Mission" };
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = { "ZenSplitItemUI/scripts/3_game" };
			};
			class worldScriptModule
			{
				value = "";
				files[] = { "ZenSplitItemUI/scripts/4_world" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "ZenSplitItemUI/scripts/5_mission" };
			};
		};
	};
};