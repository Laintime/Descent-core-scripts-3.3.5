/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

void WorldSession::HandleInitiateTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 guid;
	recv_data >> guid;
	Player * pTarget = _player->GetMapMgr()->GetPlayer( guid );
	uint32 TradeStatus = TRADE_STATUS_PROPOSED;
	WorldPacket data(SMSG_TRADE_STATUS, 12);

	if(pTarget == 0)
	{
#ifdef USING_BIG_ENDIAN
		TradeStatus = swap32(uint32(TRADE_STATUS_PLAYER_NOT_FOUND));
#else
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
#endif
		OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
		return;
	}

	// Handle possible error outcomes
	if(pTarget->CalcDistance(_player) > 10.0f)		// This needs to be checked
		TradeStatus = TRADE_STATUS_TOO_FAR_AWAY;
	else if(pTarget->IsDead())
		TradeStatus = TRADE_STATUS_DEAD;
	else if(pTarget->mTradeTarget != 0)
		TradeStatus = TRADE_STATUS_ALREADY_TRADING;
	else if(pTarget->GetTeam() != _player->GetTeam() && GetPermissionCount() == 0 && !sWorld.interfaction_trade)
		TradeStatus = TRADE_STATUS_WRONG_FACTION;

	data << TradeStatus;

	if(TradeStatus == TRADE_STATUS_PROPOSED)
	{
		_player->ResetTradeVariables();
		pTarget->ResetTradeVariables();

		pTarget->mTradeTarget = _player->GetLowGUID();
		_player->mTradeTarget = pTarget->GetLowGUID();

		pTarget->mTradeStatus = TradeStatus;
		_player->mTradeStatus = TradeStatus;

		data << _player->GetGUID();
	}

	pTarget->m_session->SendPacket(&data);
}

void WorldSession::HandleBeginTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 TradeStatus = TRADE_STATUS_INITIATED;

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
#ifdef USING_BIG_ENDIAN
		TradeStatus = swap32(uint32(TRADE_STATUS_PLAYER_NOT_FOUND));
#else
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
#endif
		OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
		return;
	}
	// We're too far from target now?
	Player * plr2 = objmgr.GetPlayer(_player->mTradeTarget);
	if( plr2 == NULL )
	{
		return; //wtf ? player is inside the map and not in objectmanager ?
	}
	if( _player->CalcDistance( plr2 ) > 10.0f )
		TradeStatus = TRADE_STATUS_TOO_FAR_AWAY;

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
/*	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);*/
	WorldPacket data(SMSG_TRADE_STATUS, 8);
	data << TradeStatus << uint32(0x19);
	plr->m_session->SendPacket(&data);
	SendPacket(&data);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;
}

void WorldSession::HandleBusyTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 TradeStatus = TRADE_STATUS_PLAYER_BUSY;

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
#ifdef USING_BIG_ENDIAN
		TradeStatus = swap32(uint32(TRADE_STATUS_PLAYER_NOT_FOUND));
#else
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
#endif
		OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
		return;
	}

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;

	plr->ResetTradeVariables();
	_player->ResetTradeVariables();
}

void WorldSession::HandleIgnoreTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	uint32 TradeStatus = TRADE_STATUS_PLAYER_IGNORED;

	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || plr == 0)
	{
#ifdef USING_BIG_ENDIAN
		TradeStatus = swap32(uint32(TRADE_STATUS_PLAYER_NOT_FOUND));
#else
		TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
#endif
		OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
		return;
	}

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;

	plr->ResetTradeVariables();
	_player->ResetTradeVariables();
}

void WorldSession::HandleCancelTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	if(_player->mTradeTarget == 0 || _player->mTradeStatus == TRADE_STATUS_COMPLETE)
	{ 
		return;
	}

#ifdef USING_BIG_ENDIAN
	uint32 TradeStatus = swap32(uint32(TRADE_STATUS_CANCELLED));
#else
    uint32 TradeStatus = TRADE_STATUS_CANCELLED;
#endif

    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

	Player * plr = _player->GetTradeTarget();
    if(plr)
    {
        if(plr->m_session && plr->m_session->GetSocket())
		plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	
		plr->ResetTradeVariables();
    }
	
	_player->ResetTradeVariables();
}

void WorldSession::HandleUnacceptTrade(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) 
	{ 
		return;
	}
	Player * plr = _player->GetTradeTarget();
	//_player->ResetTradeVariables();

	if(_player->mTradeTarget == 0 || plr == 0)
	{ 
		return;
	}

#ifdef USING_BIG_ENDIAN
	uint32 TradeStatus = swap32(uint32(TRADE_STATUS_UNACCEPTED));
#else
	uint32 TradeStatus = TRADE_STATUS_UNACCEPTED;
#endif

	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);



	TradeStatus = TRADE_STATUS_STATE_CHANGED;

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;

  //cebernic,why target & self erased? we are not cancelled !
  //this merged from p2wow's svn.
	//plr->mTradeTarget = 0;
	//_player->mTradeTarget = 0;

/*	plr->mTradeTarget = 0;
	_player->mTradeTarget = 0;
	plr->ResetTradeVariables(); */
}

void WorldSession::HandleSetTradeItem(WorldPacket & recv_data)
{
	if(_player->mTradeTarget == 0)
	{ 
		sLog.outDebug("HandleSetTradeItem: missing trade target\n");
		return;
	}

	if(!_player->IsInWorld()) 
	{ 
		sLog.outDebug("HandleSetTradeItem: not in world\n");
		return;
	}
	  
	CHECK_PACKET_SIZE(recv_data, 3);

	uint8 TradeSlot = recv_data.contents()[0];
	uint8 SourceBag = recv_data.contents()[1];
	uint8 SourceSlot = recv_data.contents()[2];
	Player * pTarget = _player->GetTradeTarget();

	Item * pItem = _player->GetItemInterface()->GetInventoryItem(SourceBag, SourceSlot);

	if( pTarget == NULL || pItem == 0 || TradeSlot > 6 || ( TradeSlot < 6 && pItem->IsSoulbound() ) )
	{ 
		sLog.outDebug("HandleSetTradeItem: target missing or incorect item/slot. target(%u),item(%u),slot(%u))\n",pTarget != NULL,pItem != 0,TradeSlot );
 		return;
	}

	if( pItem->IsAccountbound() )
	{
		PlayerInfo* pinfo = ObjectMgr::getSingleton().GetPlayerInfo(_player->mTradeTarget);
		if(pinfo == NULL || GetAccountId() != pinfo->acct) // can't trade account-based items
		{ 
			sLog.outDebug("HandleSetTradeItem: item is account bound\n");
			return;
		}
	}
/*	if( pItem->IsContainer() )
	{
		if(_player->GetItemInterface()->IsBagSlot(SourceSlot))
		return;*/
		
	uint32 TradeStatus = TRADE_STATUS_STATE_CHANGED;
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	pTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

	pTarget->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;


	if( pItem->IsContainer() )
	{
		if( SafeContainerCast(pItem)->HasItems() )
		{
			_player->GetItemInterface()->BuildInventoryChangeError(pItem,0, INV_ERR_CAN_ONLY_DO_WITH_EMPTY_BAGS);

			//--trade cancel
			uint32 TradeStatus2 = TRADE_STATUS_CANCELLED;
			
			OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus2);
			
			Player * plr = _player->GetTradeTarget();
			if(plr)
			{
			    if(plr->m_session && plr->m_session->GetSocket())
					plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus2);
			    plr->ResetTradeVariables();
			}
			OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus2);
			_player->ResetTradeVariables();
			sLog.outDebug("HandleSetTradeItem: Container with items cannot be traded\n");
			return;
		}
	}			
		
	//well that covers all slots i think (6 is temporal slot)
	if(TradeSlot < 6)
	{
		if(pItem->IsSoulbound())
		{
			sCheatLog.writefromsession(this, "tried to cheat trade a soulbound item");
			sLog.outDebug("HandleSetTradeItem: Cannot trade soulbound item\n");
			Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
			if( ti )
			{
				ti->DeleteFromDB();
				ti->DeleteMe();
				ti = NULL;
			}
			string sReason = "Soulboundtrade 1";
			uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
			_player->SetBanned( uBanTime, sReason );
			sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
			return; //item is not valid anymore !
		}
		//this was checked earlier ? How the hack did we get here again ?
		else if(pItem->IsAccountbound() && pTarget->GetSession() && pTarget->GetSession()->GetAccountId() != GetAccountId() )
		{
			PlayerInfo* pinfo = ObjectMgr::getSingleton().GetPlayerInfo(_player->mTradeTarget);
			if(pinfo == NULL || GetAccountId() != pinfo->acct) // can't trade account-based items
			{
				sLog.outDebug("HandleSetTradeItem: Cannot trade account bound item\n");
				sCheatLog.writefromsession(this, "tried to cheat trade an accountbound item");
				Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
				if( ti )
				{
					ti->DeleteFromDB();
					ti->DeleteMe();
					ti = NULL;
				}
				string sReason = "Account bound itemtrade";
				uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
				_player->SetBanned( uBanTime, sReason );
				sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
				return; //item is not valid anymore !
			}
		}
	}

	for(uint32 i = 0; i < 8; ++i)
	{
		// duping little shits
		if(_player->mTradeItems[i] == pItem || pTarget->mTradeItems[i] == pItem)
		{
			sCheatLog.writefromsession(this, "tried to dupe an item through trade");
			Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
			if( ti )
			{
				ti->DeleteFromDB();
				ti->DeleteMe();
				ti = NULL;
			}
			GetPlayer()->SoftDisconnect();
			sLog.outDebug("HandleSetTradeItem: item dupe detected\n");
			return;
		}
	}

	if( SourceBag <= INVENTORY_SLOT_NOT_SET && //we are removing from our direct character slot and not from a bag
		SourceSlot >= INVENTORY_SLOT_BAG_START && SourceSlot < INVENTORY_SLOT_BAG_END)
	{
		//More duping woohoo
        Item *ti = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SourceBag, SourceSlot, false);
		if( ti )
		{
			ti->DeleteFromDB();
			ti->DeleteMe();
			ti = NULL;
		}
		sCheatLog.writefromsession(this, "tried to cheat trade a soulbound item");
		string sReason = "trading from bagslot";
		uint32 uBanTime = (uint32)UNIXTIME + 60*60; //60 minutes ban
		_player->SetBanned( uBanTime, sReason );
		sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 7000, 1, 0 );
		sLog.outDebug("HandleSetTradeItem: Cannot trade from bagslot\n");
		return; //item is not valid anymore !
	}

	_player->mTradeItems[TradeSlot] = pItem;
	_player->SendTradeUpdate();
}

void WorldSession::HandleSetTradeGold(WorldPacket & recv_data)
{
	if(_player->mTradeTarget == 0)
	{ 
		return;
	}
  // cebernic: TradeGold sameway.
	uint32 TradeStatus = TRADE_STATUS_STATE_CHANGED;
	Player * plr = _player->GetTradeTarget();
	if(!plr) 
	{ 
		return;
	}

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;


	uint32 Gold;
	recv_data >> Gold;

	if(_player->mTradeGold != Gold)
	{
		_player->mTradeGold = (Gold > _player->GetGold() ? _player->GetGold() : Gold);
		_player->SendTradeUpdate();
	}
}

void WorldSession::HandleClearTradeItem(WorldPacket & recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 1);
	if(_player->mTradeTarget == 0)
	{ 
		return;
	}

	uint8 TradeSlot = recv_data.contents()[0];
	if(TradeSlot > 6)
	{ 
		return;
	}

  // clean status
	Player * plr = _player->GetTradeTarget();
	if ( !plr ) 
	{ 
		return;
	}

	uint32 TradeStatus = TRADE_STATUS_STATE_CHANGED;

#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	swap32(&TradeStatus);
#else
	OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

	plr->mTradeStatus = TradeStatus;
	_player->mTradeStatus = TradeStatus;


	_player->mTradeItems[TradeSlot] = 0;
	_player->SendTradeUpdate();
}

void WorldSession::HandleAcceptTrade(WorldPacket & recv_data) 
{
	Player * plr = _player->GetTradeTarget();
	if(_player->mTradeTarget == 0 || !plr)
	{ 
		return;
	}

	uint32 TradeStatus = TRADE_STATUS_ACCEPTED;
	
	// Tell the other player we're green.
#ifdef USING_BIG_ENDIAN
	swap32(&TradeStatus);
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	_player->mTradeStatus = TradeStatus;
	swap32(&TradeStatus);
#else
	plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
	_player->mTradeStatus = TradeStatus;
#endif

	if(plr->mTradeStatus == TRADE_STATUS_ACCEPTED)
	{
		// Ready!
		uint32 ItemCount = 0;
		uint32 TargetItemCount = 0;
		Player * pTarget = plr;

/*		// Calculate Item Count
		for(uint32 Index = 0; Index < 7; ++Index)
		{
			if(_player->mTradeItems[Index] != 0)	++ItemCount;
			if(pTarget->mTradeItems[Index] != 0)	++TargetItemCount;
		}*/
		

		// Calculate Count
		for(uint32 Index = 0; Index < 6; ++Index) // cebernic: checking for 6items ,untradable item check via others func.
		{
			Item * pItem;

			// safely trade checking
			pItem = _player->mTradeItems[Index];
			if( pItem )
			{
				if( ( pItem->IsContainer() && SafeContainerCast(pItem)->HasItems() )   || ( pItem->GetProto() && pItem->GetProto()->Bonding==ITEM_BIND_ON_PICKUP) )
				{
					ItemCount = 0;
					TargetItemCount = 0;
					break;
				}
				else ++ItemCount;
			}					
			
			pItem = pTarget->mTradeItems[Index];
			if( pItem )
			{
				if( ( pItem->IsContainer() && SafeContainerCast(pItem)->HasItems() )   || ( pItem->GetProto() && pItem->GetProto()->Bonding==ITEM_BIND_ON_PICKUP) )
				{
					ItemCount = 0;
					TargetItemCount = 0;
					break;
				}
				else ++TargetItemCount;
			}					

			//if(_player->mTradeItems[Index] != 0)	++ItemCount;
			//if(pTarget->mTradeItems[Index] != 0)	++TargetItemCount;
		}

		

		if( (_player->m_ItemInterface->CalculateFreeSlots(NULL) + ItemCount) < TargetItemCount ||
			(pTarget->m_ItemInterface->CalculateFreeSlots(NULL) + TargetItemCount) < ItemCount ||
			(ItemCount==0 && TargetItemCount==0 && !pTarget->mTradeGold && !_player->mTradeGold) )	// ceberwow added it
		{
			// Not enough slots on one end.
			TradeStatus = TRADE_STATUS_CANCELLED;
		}
		else
		{
			TradeStatus = TRADE_STATUS_COMPLETE;
			uint64 Guid;
			Item * pItem;
			
			// Remove all items from the players inventory
			for(uint32 Index = 0; Index < 6; ++Index)
			{
				Guid = _player->mTradeItems[Index] ? _player->mTradeItems[Index]->GetGUID() : 0;
				if(Guid != 0)
				{
					if( _player->mTradeItems[Index]->GetProto()->Bonding == ITEM_BIND_ON_PICKUP ||
						_player->mTradeItems[Index]->GetProto()->Bonding  >=  ITEM_BIND_QUEST  )
					{
						_player->mTradeItems[Index] = NULL;
					}
					else
					{
						if(GetPermissionCount()>0)
							sGMLog.writefromsession(this, "traded item %s to %s", _player->mTradeItems[Index]->GetProto()->Name1, pTarget->GetName());
						pItem = _player->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);
					}
				}

				Guid = pTarget->mTradeItems[Index] ? pTarget->mTradeItems[Index]->GetGUID() : 0;
				if(Guid != 0)
				{
					if( pTarget->mTradeItems[Index]->GetProto()->Bonding == ITEM_BIND_ON_PICKUP ||  
						pTarget->mTradeItems[Index]->GetProto()->Bonding  >=  ITEM_BIND_QUEST )
					{
						pTarget->mTradeItems[Index] = NULL;
					}
					else
					{
						if(pTarget->GetSession() && pTarget->GetSession()->GetPermissionCount()>0)
							sGMLog.writefromsession(pTarget->GetSession(), "traded item %s to %s", pTarget->mTradeItems[Index]->GetProto()->Name1, _player->GetName());
						pTarget->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);
					}
				}
			}

			// Dump all items back into the opposite players inventory
			for(uint32 Index = 0; Index < 6; ++Index)
			{
				pItem = _player->mTradeItems[Index];
				if(pItem != 0 && pTarget)
				{
					pItem->SetOwner(pTarget); // crash fixed.
					if( !pTarget->m_ItemInterface->AddItemToFreeSlot(&pItem) )
					{
						pItem->DeleteMe();
						pItem = NULL;
					}
				}

				pItem = pTarget->mTradeItems[Index];
				if(pItem != 0 && _player)
				{
					pItem->SetOwner(_player);
					if( !_player->m_ItemInterface->AddItemToFreeSlot(&pItem) )
					{
						pItem->DeleteMe();
						pItem = NULL;
					}
				}
			}

			// Trade Gold
			if(pTarget->mTradeGold && pTarget->GetGold() >= pTarget->mTradeGold )
			{
				_player->ModGold(pTarget->mTradeGold);
				pTarget->ModGold(-(int32)pTarget->mTradeGold);
				if(GetPermissionCount()>0)
					sGMLog.writefromsession(this, "traded(gave) %u gold to %s", pTarget->mTradeGold, pTarget->GetName());
			}

			if(_player->mTradeGold && _player->GetGold() >= _player->mTradeGold)
			{
				pTarget->ModGold( _player->mTradeGold);
				_player->ModGold( -(int32)_player->mTradeGold);
				if(pTarget->GetSession() && pTarget->GetSession()->GetPermissionCount()>0)
					sGMLog.writefromsession(pTarget->GetSession(), "traded(gave) %u gold to %s", _player->mTradeGold, _player->GetName());
			}

			// Close Window
			TradeStatus = TRADE_STATUS_COMPLETE;
			
		}
					
#ifdef USING_BIG_ENDIAN
            swap32(&TradeStatus);
			OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
			plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
			swap32(&TradeStatus);
#else
			OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
			plr->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
#endif

			_player->mTradeStatus = TRADE_STATUS_COMPLETE;
			plr->mTradeStatus = TRADE_STATUS_COMPLETE;

			// Reset Trade Vars
			_player->ResetTradeVariables();		
			plr->ResetTradeVariables();

			//removed by zack. Hehe, just a way to spam db with saves. Not funny
			// added alternative way as item saves before 
			// Save for eachother
			//plr->SaveToDB(false);
			//_player->SaveToDB(false);
	}
}
