modded class ItemBase
{
	//! Vanilla
	override void OnRightClick()
	{
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenSplitUI"))
		{
			super.OnRightClick();
			return;
		}
		#endif
		
		// Right click + ctrl is down - open UI
		if (g_Game.IsLeftCtrlDown() && CanBeSplit() && !g_Game.GetPlayer().GetInventory().HasInventoryReservation(this, NULL))
		{
			if (g_Game.IsClient() && g_Game.GetUIManager() && !g_Game.GetUIManager().FindMenu(MENU_ZEN_SPLITGUI))
			{
				ZenGUIBase_Game gui = ZenGUIBase_Game.Cast(g_Game.GetUIManager().EnterScriptedMenu(MENU_ZEN_SPLITGUI, g_Game.GetUIManager().GetMenu()));
				if (gui)
				{
					gui.Zen_SetItem(this);
				}
			}
		}
		else 
		{
			// These are not the droids we're looking for - just pass to super.
			super.OnRightClick();
		}
	}

	//! Client: Send split quantity request from GUI to server.
	void Zen_SplitItemQuantityRPC(int quantity)
	{
		if (!ScriptInputUserData.CanStoreInputUserData())
		{
			Error("!ScriptInputUserData.CanStoreInputUserData()");
			return;
		}
		
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());

		EntityAI root = GetHierarchyRoot();
		Man playerOwner = GetHierarchyRootPlayer();

		InventoryLocation dst = new InventoryLocation;

		if (!playerOwner && root && root == this)
		{
			Zen_SetInventoryLocationToVicinityOrCurrent(root, dst);
		}
		else
		{
			GetInventory().GetCurrentInventoryLocation(dst);

			if (!dst.GetParent() || dst.GetParent() && !dst.GetParent().GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.CARGO, dst))
			{
				if (!player.GetInventory().FindFreeLocationFor(this, FindInventoryLocationType.CARGO, dst) || !playerOwner)
				{
					Zen_SetInventoryLocationToVicinityOrCurrent(root, dst);
				}
				else
				{
					dst.SetCargo(dst.GetParent(), this, dst.GetIdx(), dst.GetRow(), dst.GetCol(), dst.GetFlip());

					if (player.GetInventory().HasInventoryReservation(this, dst))
					{
						Zen_SetInventoryLocationToVicinityOrCurrent(root, dst);
					}
					else
					{
						player.GetInventory().AddInventoryReservationEx(null, dst, GameInventory.c_InventoryReservationTimeoutShortMS);
					}
				}
			}
		}

		ScriptInputUserData ctx = new ScriptInputUserData;
		ctx.Write(ZEN_SPLIT_ITEM);
		ctx.Write(quantity);

		ItemBase thiz = this;
		ctx.Write(thiz);

		dst.WriteToContext(ctx);
		ctx.Send();
	}
	
	protected void Zen_SetInventoryLocationToVicinityOrCurrent(EntityAI root, inout InventoryLocation dst)
	{
		if (root)
		{
			vector transform[4];
			root.GetTransform(transform);
			dst.SetGround(this, transform);
		}
		else
		{
			GetInventory().GetCurrentInventoryLocation(dst);
		}
	}

	//! Server - verify item can actually be split, then splizzle that shizzle
	void SplitItemToInventoryLocation_ZenSplit(notnull InventoryLocation dst, int quantity)
	{
		if (!CanBeSplit())
		{
			Error("ZenSplit: CanBeSplit failed");
			return;
		}
	
		Magazine source_magazine = Magazine.Cast(this);
	
		if (source_magazine)
		{
			int source_ammo_count = source_magazine.GetAmmoCount();
	
			if (source_ammo_count <= 1)
			{
				Error("ZenSplit: Ammo count too low");
				return;
			}
	
			int split_ammo_count = Math.Clamp(quantity, 1, source_ammo_count - 1);
	
			Magazine new_magazine = Magazine.Cast(GameInventory.LocationCreateEntity(dst, GetType(), ECE_IN_INVENTORY, RF_DEFAULT));
	
			if (!new_magazine)
			{
				Error("ZenSplit: Failed to create ammo pile");
				return;
			}
	
			if (new_magazine.GetAmmoMax() < split_ammo_count)
			{
				split_ammo_count = new_magazine.GetAmmoMax();
			}
	
			new_magazine.SetResultOfSplit(true);
			MiscGameplayFunctions.TransferItemProperties(this, new_magazine);
	
			source_magazine.ServerSetAmmoCount(source_ammo_count - split_ammo_count);
			new_magazine.ServerSetAmmoCount(split_ammo_count);
	
			Error("ZenSplit: Ammo split source=" + source_magazine.GetAmmoCount() + " new=" + new_magazine.GetAmmoCount());
			return;
		}
	
		float current_quantity = GetQuantity();
		float split_quantity_new = Math.Clamp(quantity, 1, current_quantity - 1);
	
		if (!ShouldSplitQuantity(split_quantity_new))
		{
			Error("ZenSplit: ShouldSplitQuantity failed quantity=" + split_quantity_new + " current=" + current_quantity);
			return;
		}
	
		ItemBase new_item = ItemBase.Cast(GameInventory.LocationCreateEntity(dst, GetType(), ECE_IN_INVENTORY, RF_DEFAULT));
	
		if (!new_item)
		{
			Error("ZenSplit: Failed to create regular item");
			return;
		}
	
		if (new_item.GetQuantityMax() < split_quantity_new)
		{
			split_quantity_new = new_item.GetQuantityMax();
		}
	
		new_item.SetResultOfSplit(true);
		MiscGameplayFunctions.TransferItemProperties(this, new_item);
	
		if (dst.IsValid() && dst.GetType() == InventoryLocationType.ATTACHMENT && split_quantity_new > 1)
		{
			AddQuantity(-1, false, true);
			new_item.SetQuantity(1, false, true);
		}
		else
		{
			AddQuantity(-split_quantity_new, false, true);
			new_item.SetQuantity(split_quantity_new, false, true);
		}
	}
}
