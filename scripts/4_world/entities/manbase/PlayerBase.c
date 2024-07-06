modded class PlayerBase
{
	override bool HandleRemoteItemManipulation(int userDataType, ParamsReadContext ctx)
	{
		// Custom item manipulation type: split a stackable item based on client-requested quantity instead of 50% stack.
		if (userDataType == ZEN_SPLIT_ITEM)
		{
			ItemBase item1	= null;
			int quantity	= 1;

			if (!ctx.Read(quantity))
				return false;

			if (!ctx.Read(item1))
				return false;

			InventoryLocation dst = new InventoryLocation;
			if (dst.ReadFromContext(ctx))
			{
				item1.SplitItemToInventoryLocation_ZenSplit(dst, quantity);
				return true;
			}

			return false;
		}

		return super.HandleRemoteItemManipulation(userDataType, ctx);
	}
}
