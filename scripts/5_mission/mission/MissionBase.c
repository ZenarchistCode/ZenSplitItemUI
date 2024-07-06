modded class MissionBase
{
	override UIScriptedMenu CreateScriptedMenu(int id)
    {
        UIScriptedMenu menu = super.CreateScriptedMenu(id);

        if (!menu)
        {
            switch (id)
            {
				//! SPLIT GUI
                case MENU_ZEN_SPLITGUI:
                {
                    menu = new ZenSplitItemUI;
                    break;
                }
            }
        }

        return menu;
    }
}