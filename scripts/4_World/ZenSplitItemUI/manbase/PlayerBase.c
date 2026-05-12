modded class PlayerBase
{
	override bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		if (userDataType == ZEN_SPLIT_ITEM)
		{
			return Zen_HandleSplitItemQuantity(ctx);
		}

		return super.OnInputUserDataProcess(userDataType, ctx);
	}

	protected bool Zen_HandleSplitItemQuantity(ParamsReadContext ctx)
	{
		ItemBase item1 = null;
		int quantity = 1;

		if (!ctx.Read(quantity))
		{
			Error("ZenSplit: Failed to read quantity!");
			return false;
		}

		if (!ctx.Read(item1))
		{
			Error("ZenSplit: Failed to read item1!");
			return false;
		}

		if (!item1)
		{
			Error("ZenSplit: No item1!");
			return false;
		}

		InventoryLocation dst = new InventoryLocation;
		if (!dst.ReadFromContext(ctx))
		{
			Error("ZenSplit: Failed to read dst!");
			return false;
		}

		InventoryLocation src = new InventoryLocation;
		item1.GetInventory().GetCurrentInventoryLocation(src);

		DayZPlayerInventory inventory = GetDayZPlayerInventory();

		if (!inventory.PlayerCheckRequestSrc(src, GameInventory.c_MaxItemDistanceRadius))
		{
			Error("ZenSplit: Invalid src!");
			return false;
		}

		if (!inventory.PlayerCheckRequestDst(src, dst, GameInventory.c_MaxItemDistanceRadius))
		{
			Error("ZenSplit: Invalid dst!");
			return false;
		}

		item1.SplitItemToInventoryLocation_ZenSplit(dst, quantity);
		return true;
	}
}