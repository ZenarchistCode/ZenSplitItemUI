modded class ItemBase
{
	//! Vanilla
	override void OnRightClick()
	{
		// Right click + ctrl is down - open UI
		if (GetDayZGame().IsLeftCtrlDown() && CanBeSplit() && !GetGame().GetPlayer().GetInventory().HasInventoryReservation(this, NULL))
		{
			if (GetGame().IsClient() && GetGame().GetUIManager() && !GetGame().GetUIManager().FindMenu(MENU_ZEN_SPLITGUI))
			{
				ZenSplitItemUIBase gui = ZenSplitItemUIBase.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_ZEN_SPLITGUI, GetGame().GetUIManager().GetMenu()));
				if (gui)
					gui.SetItem(this);
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
		// Copied from vanilla, only change is adding split quantity int from GUI to the RPC send and assigning a custom RPC id (ZEN_SPLIT_ITEM)
		if (ScriptInputUserData.CanStoreInputUserData())
		{
			vector m4[4];
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
					
			EntityAI root = GetHierarchyRoot();
					
			InventoryLocation dst = new InventoryLocation;
			if (!player.GetInventory().FindFirstFreeLocationForNewEntity(GetType(), FindInventoryLocationType.CARGO, dst))
			{						
				if (root)
				{
					root.GetTransform(m4);
					dst.SetGround(this, m4);
				}
				else
					GetInventory().GetCurrentInventoryLocation(dst);
			}
			else
			{
				dst.SetCargo(dst.GetParent(), this, dst.GetIdx(), dst.GetRow(), dst.GetCol(), dst.GetFlip());
				if (GetGame().GetPlayer().GetInventory().HasInventoryReservation(null, dst))
				{
					if (root)
					{
						root.GetTransform(m4);
						dst.SetGround(this, m4);
					}
					else
						GetInventory().GetCurrentInventoryLocation(dst);
				}
				else
				{
					GetGame().GetPlayer().GetInventory().AddInventoryReservationEx(null, dst, GameInventory.c_InventoryReservationTimeoutShortMS);
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
	}

	//! Server - verify item can actually be split, then splizzle that shizzle
	void SplitItemToInventoryLocation_ZenSplit(notnull InventoryLocation dst, int quantity)
	{
		if (!CanBeSplit())
			return;

		ItemBase new_item = ItemBase.Cast(GameInventory.LocationCreateEntity(dst, GetType(), ECE_IN_INVENTORY, RF_DEFAULT));
		float split_quantity_new = Math.Clamp(quantity, 1, quantity);

		if (new_item)
		{
			// Ammo quantity is handled differently to ItemBase
			if (new_item.IsAmmoPile())
			{
				Ammunition_Base ammo = Ammunition_Base.Cast(new_item);
				if (ammo.GetAmmoMax() < split_quantity_new)
				{
					split_quantity_new = ammo.GetAmmoMax();
				}
			
				ammo.SetResultOfSplit(true);
				MiscGameplayFunctions.TransferItemProperties(this, ammo);
				ammo.ServerSetAmmoCount(split_quantity_new);

				Ammunition_Base ammo2 = Ammunition_Base.Cast(this);
				if (ammo2)
					ammo2.ServerAddAmmoCount(-split_quantity_new);
			}
			else 
			// Regular ItemBase
			{
				if (new_item.GetQuantityMax() < split_quantity_new)
				{
					split_quantity_new = new_item.GetQuantityMax();
				}
			
				new_item.SetResultOfSplit(true);
				MiscGameplayFunctions.TransferItemProperties(this, new_item);
			
				if (dst.IsValid() && dst.GetType() == InventoryLocationType.ATTACHMENT && split_quantity_new > 1)
				{
					AddQuantity(-1);
					new_item.SetQuantity(1);
				}
				else
				{
					AddQuantity(-split_quantity_new);
					new_item.SetQuantity(split_quantity_new);				
				}
			}
		}	
	}
}
