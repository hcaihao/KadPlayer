//this file is part of eMule
//Copyright (C)2002-2008 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "../../QuickSearch/QuickSearch.h"
#include "ClientUDPSocket.h"
#include "../def/Packets.h"
#include "../../../include/zlib/zlib.h"
#include "../comm/Preferences.h"
#include "../comm/CommFun.h"
#include "../utils/SafeFile.h"
#include "../kademlia/Kademlia.h"
#include "../kademlia/UDPFirewallTester.h"
#include "../net/KademliaUDPListener.h"
#include "../io/IOException.h"
#include "../comm/Log.h"
#include "../socket/EncryptedDatagramSocket.h"
#include "../kademlia/prefs.h"
#include "../utils/KadUDPKey.h"
#include "../socket/ThrottledSocket.h"
#include "../socket/IPFilter.h"
#include "../../QuickSearch/QSKademlia.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CClientList::AddBannedClient(uint32 dwIP){
	m_bannedList.SetAt(dwIP, ::GetTickCount());
}

bool CClientList::IsBannedClient(uint32 dwIP) const
{
	uint32 dwBantime;
	if (m_bannedList.Lookup(dwIP, dwBantime)){
		if (dwBantime + CLIENTBANTIME > ::GetTickCount())
			return true;
	}
	return false; 
}

void CClientList::RemoveBannedClient(uint32 dwIP){
	m_bannedList.RemoveKey(dwIP);
}

void CClientList::RemoveAllBannedClients(){
	m_bannedList.RemoveAll();
}


void CClientList::AddKadFirewallRequest(uint32 dwIP){
	IPANDTICS add = {dwIP, ::GetTickCount()};
	listFirewallCheckRequests.AddHead(add);
	while (!listFirewallCheckRequests.IsEmpty()){
		if (::GetTickCount() - listFirewallCheckRequests.GetTail().dwInserted > SEC2MS(180))
			listFirewallCheckRequests.RemoveTail();
		else
			break;
	}
}

bool CClientList::IsKadFirewallCheckIP(uint32 dwIP) const{
	for (POSITION pos = listFirewallCheckRequests.GetHeadPosition(); pos != NULL; listFirewallCheckRequests.GetNext(pos)){
		if (listFirewallCheckRequests.GetAt(pos).dwIP == dwIP && ::GetTickCount() - listFirewallCheckRequests.GetAt(pos).dwInserted < SEC2MS(180))
			return true;
	}
	return false;
}
void CClientList::Process()
{
#if _DEL_
	///////////////////////////////////////////////////////////////////////////
	// Cleanup banned client list
	//
	const uint32 cur_tick = ::GetTickCount();
	if (m_dwLastBannCleanUp + BAN_CLEANUP_TIME < cur_tick)
	{
		m_dwLastBannCleanUp = cur_tick;

		POSITION pos = m_bannedList.GetStartPosition();
		uint32 nKey;
		uint32 dwBantime;
		while (pos != NULL)
		{
			m_bannedList.GetNextAssoc( pos, nKey, dwBantime );
			if (dwBantime + CLIENTBANTIME < cur_tick )
				RemoveBannedClient(nKey);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Cleanup tracked client list
	//
	if (m_dwLastTrackedCleanUp + TRACKED_CLEANUP_TIME < cur_tick)
	{
		m_dwLastTrackedCleanUp = cur_tick;
		if (thePrefs.GetLogBannedClients())
			AddDebugLogLine(false, _T("Cleaning up TrackedClientList, %i clients on List..."), m_trackedClientsList.GetCount());
		POSITION pos = m_trackedClientsList.GetStartPosition();
		uint32 nKey;
		CDeletedClient* pResult;
		while (pos != NULL)
		{
			m_trackedClientsList.GetNextAssoc( pos, nKey, pResult );
			if (pResult->m_dwInserted + KEEPTRACK_TIME < cur_tick ){
				m_trackedClientsList.RemoveKey(nKey);
				delete pResult;
			}
		}
		if (thePrefs.GetLogBannedClients())
			AddDebugLogLine(false, _T("...done, %i clients left on list"), m_trackedClientsList.GetCount());
	}

	///////////////////////////////////////////////////////////////////////////
	// Process Kad client list
	//
	//We need to try to connect to the clients in m_KadList
	//If connected, remove them from the list and send a message back to Kad so we can send a ACK.
	//If we don't connect, we need to remove the client..
	//The sockets timeout should delete this object.
	POSITION pos1, pos2;

	// buddy is just a flag that is used to make sure we are still connected or connecting to a buddy.
	buddyState buddy = Disconnected;

	for (pos1 = m_KadList.GetHeadPosition(); (pos2 = pos1) != NULL; )
	{
		m_KadList.GetNext(pos1);
		CUpDownClient* cur_client =	m_KadList.GetAt(pos2);
		if( !Kademlia::CKademlia::IsRunning() )
		{
			//Clear out this list if we stop running Kad.
			//Setting the Kad state to KS_NONE causes it to be removed in the switch below.
			cur_client->SetKadState(KS_NONE);
		}
		switch(cur_client->GetKadState())
		{
		case KS_QUEUED_FWCHECK:
		case KS_QUEUED_FWCHECK_UDP:
			//Another client asked us to try to connect to them to check their firewalled status.
			cur_client->TryToConnect(true, true);
			break;
		case KS_CONNECTING_FWCHECK:
			//Ignore this state as we are just waiting for results.
			break;
		case KS_FWCHECK_UDP:
		case KS_CONNECTING_FWCHECK_UDP:
			// we want a UDP firewallcheck from this client and are just waiting to get connected to send the request
			break;
		case KS_CONNECTED_FWCHECK:
			//We successfully connected to the client.
			//We now send a ack to let them know.
			if (cur_client->GetKadVersion() >= KADEMLIA_VERSION7_49a){
				// the result is now sent per TCP instead of UDP, because this will fail if our intern UDP port is unreachable.
				// But we want the TCP testresult regardless if UDP is firewalled, the new UDP state and test takes care of the rest					
				ASSERT( cur_client->socket != NULL && cur_client->socket->IsConnected() );
				if (thePrefs.GetDebugClientTCPLevel() > 0)
					//DebugSend("OP_KAD_FWTCPCHECK_ACK", cur_client);
					;
				Packet* pPacket = new Packet(OP_KAD_FWTCPCHECK_ACK, 0, OP_EMULEPROT);
				if (!cur_client->SafeConnectAndSendPacket(pPacket))
					cur_client = NULL;

			}
			else {
				if (thePrefs.GetDebugClientKadUDPLevel() > 0)
					DebugSend("KADEMLIA_FIREWALLED_ACK_RES", cur_client->GetIP(), cur_client->GetKadPort());
				Kademlia::CKademlia::GetUDPListener()->SendNullPacket(KADEMLIA_FIREWALLED_ACK_RES, ntohl(cur_client->GetIP()), cur_client->GetKadPort(), 0, NULL);
			}
			//We are done with this client. Set Kad status to KS_NONE and it will be removed in the next cycle.
			if (cur_client != NULL)
				cur_client->SetKadState(KS_NONE);
			break;

		case KS_INCOMING_BUDDY:
			//A firewalled client wants us to be his buddy.
			//If we already have a buddy, we set Kad state to KS_NONE and it's removed in the next cycle.
			//If not, this client will change to KS_CONNECTED_BUDDY when it connects.
			if( m_nBuddyStatus == Connected )
				cur_client->SetKadState(KS_NONE);
			break;

		case KS_QUEUED_BUDDY:
			//We are firewalled and want to request this client to be a buddy.
			//But first we check to make sure we are not already trying another client.
			//If we are not already trying. We try to connect to this client.
			//If we are already connected to a buddy, we set this client to KS_NONE and it's removed next cycle.
			//If we are trying to connect to a buddy, we just ignore as the one we are trying may fail and we can then try this one.
			if( m_nBuddyStatus == Disconnected )
			{
				buddy = Connecting;
				m_nBuddyStatus = Connecting;
				cur_client->SetKadState(KS_CONNECTING_BUDDY);
				cur_client->TryToConnect(true, true);
				//	theApp.emuledlg->serverwnd->UpdateMyInfo();  //DEL LIPING
			}
			else if( m_nBuddyStatus == Connected )
				cur_client->SetKadState(KS_NONE);
			break;

		case KS_CONNECTING_BUDDY:
			//We are trying to connect to this client.
			//Although it should NOT happen, we make sure we are not already connected to a buddy.
			//If we are we set to KS_NONE and it's removed next cycle.
			//But if we are not already connected, make sure we set the flag to connecting so we know 
			//things are working correctly.
			if( m_nBuddyStatus == Connected )
				cur_client->SetKadState(KS_NONE);
			else
			{
				ASSERT( m_nBuddyStatus == Connecting );
				buddy = Connecting;
			}
			break;

		case KS_CONNECTED_BUDDY:
			//A potential connected buddy client wanting to me in the Kad network
			//We set our flag to connected to make sure things are still working correctly.
			buddy = Connected;

			//If m_nBuddyStatus is not connected already, we set this client as our buddy!
			if( m_nBuddyStatus != Connected )
			{
				m_pBuddy = cur_client;
				m_nBuddyStatus = Connected;
				//theApp.emuledlg->serverwnd->UpdateMyInfo();//DEL LIPING
			}
			if( m_pBuddy == cur_client && m_KadManager.IsFirewalled() && cur_client->SendBuddyPingPong() )
			{
				if (thePrefs.GetDebugClientTCPLevel() > 0)
					//	DebugSend("OP__BuddyPing", cur_client);
					Packet* buddyPing = new Packet(OP_BUDDYPING, 0, OP_EMULEPROT);
				//theStats.AddUpDataOverheadOther(buddyPing->size);
				//VERIFY( cur_client->SendPacket(buddyPing, true, true) );
				cur_client->SetLastBuddyPingPongTime();
			}
			break;

		default:
			RemoveFromKadList(cur_client);
		}
	}

	//We either never had a buddy, or lost our buddy..
	if( buddy == Disconnected )
	{
		if( m_nBuddyStatus != Disconnected || m_pBuddy )
		{
			if( Kademlia::CKademlia::IsRunning() && m_KadManager.IsFirewalled() && Kademlia::CUDPFirewallTester::IsFirewalledUDP(true))
			{
				//We are a lowID client and we just lost our buddy.
				//Go ahead and instantly try to find a new buddy.
				Kademlia::CKademlia::GetPrefs()->SetFindBuddy();
			}
			m_pBuddy = NULL;
			m_nBuddyStatus = Disconnected;
			//	theApp.emuledlg->serverwnd->UpdateMyInfo();//DEL LIPING
		}
	}

	if ( Kademlia::CKademlia::IsConnected() )
	{
		//we only need a buddy if direct callback is not available
		if( Kademlia::CKademlia::IsFirewalled() && Kademlia::CUDPFirewallTester::IsFirewalledUDP(true))
		{
			//TODO 0.49b: Kad buddies won'T work with RequireCrypt, so it is disabled for now but should (and will)
			//be fixed in later version
			// Update: Buddy connections itself support obfuscation properly since 0.49a (this makes it work fine if our buddy uses require crypt)
			// ,however callback requests don't support it yet so we wouldn't be able to answer callback requests with RequireCrypt, protocolchange intended for the next version
			if( m_nBuddyStatus == Disconnected && Kademlia::CKademlia::GetPrefs()->GetFindBuddy() && !thePrefs.IsClientCryptLayerRequired())
			{
				DEBUG_ONLY( DebugLog(_T("Starting Buddysearch")) );
				//We are a firewalled client with no buddy. We have also waited a set time 
				//to try to avoid a false firewalled status.. So lets look for a buddy..
				if( !Kademlia::CSearchManager::PrepareLookup(Kademlia::CSearch::FINDBUDDY, true, Kademlia::CUInt128(true).Xor(Kademlia::CKademlia::GetPrefs()->GetKadID())) )
				{
					//This search ID was already going. Most likely reason is that
					//we found and lost our buddy very quickly and the last search hadn't
					//had time to be removed yet. Go ahead and set this to happen again
					//next time around.
					Kademlia::CKademlia::GetPrefs()->SetFindBuddy();
				}
			}
		}
		else
		{
			if( m_pBuddy )
			{
				//Lets make sure that if we have a buddy, they are firewalled!
				//If they are also not firewalled, then someone must have fixed their firewall or stopped saturating their line.. 
				//We just set the state of this buddy to KS_NONE and things will be cleared up with the next cycle.
				if( !m_pBuddy->HasLowID() )
					m_pBuddy->SetKadState(KS_NONE);
			}
		}
	}
	else
	{
		if( m_pBuddy )
		{
			//We are not connected anymore. Just set this buddy to KS_NONE and things will be cleared out on next cycle.
			m_pBuddy->SetKadState(KS_NONE);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Cleanup client list
	//
	CleanUpClientList();

	///////////////////////////////////////////////////////////////////////////
	// Process Direct Callbacks for Timeouts
	//
	ProcessConnectingClientsList();

#endif
}

bool CClientList::IncomingBuddy(Kademlia::CContact* contact, Kademlia::CUInt128* buddyID )
{

	uint32 nContactIP = ntohl(contact->GetIPAddress());
	//If eMule already knows this client, abort this.. It could cause conflicts.
	//Although the odds of this happening is very small, it could still happen.

#if _DEL_
	if (FindClientByIP(nContactIP, contact->GetTCPPort()))
		return false;
	else if (IsKadFirewallCheckIP(nContactIP)){ // doing a kad firewall check with this IP, abort 
		DEBUG_ONLY( DebugLogWarning(_T("KAD tcp Firewallcheck / Buddy request collosion for IP %s"), ipstr(nContactIP)) );
		return false;
	}
	else if (m_KadManager.pSvrConnection->GetLocalIP() == nContactIP && thePrefs.GetPort() == contact->GetTCPPort())
		return false; // don't connect ourself

	//Add client to the lists to be processed.
	CUpDownClient* pNewClient = new CUpDownClient(0, contact->GetTCPPort(), contact->GetIPAddress(), 0, 0, false );
	pNewClient->SetKadPort(contact->GetUDPPort());
	pNewClient->SetKadState(KS_INCOMING_BUDDY);
	byte ID[16];
	contact->GetClientID().ToByteArray(ID);
	pNewClient->SetUserHash(ID); //??
	buddyID->ToByteArray(ID);
	pNewClient->SetBuddyID(ID);
	AddToKadList(pNewClient);
	AddClient(pNewClient);
#endif

	return true;
}
void CClientList::RequestBuddy(Kademlia::CContact* contact, uint8 byConnectOptions)
{
	/*
	uint32 nContactIP = ntohl(contact->GetIPAddress());
	// don't connect ourself
	if (m_KadManager.pSvrConnection->GetLocalIP() == nContactIP && thePrefs.GetPort() == contact->GetTCPPort())
	return;
	CUpDownClient* pNewClient = FindClientByIP(nContactIP, contact->GetTCPPort());
	if (!pNewClient)
	pNewClient = new CUpDownClient(0, contact->GetTCPPort(), contact->GetIPAddress(), 0, 0, false );
	else if (pNewClient->GetKadState() != KS_NONE)
	return; // already busy with this client in some way (probably fw stuff), don't mess with it
	else if (IsKadFirewallCheckIP(nContactIP)){ // doing a kad firewall check with this IP, abort 
	DEBUG_ONLY( DebugLogWarning(_T("KAD tcp Firewallcheck / Buddy request collosion for IP %s"), ipstr(nContactIP)) );
	return;
	}
	//Add client to the lists to be processed.
	pNewClient->SetKadPort(contact->GetUDPPort());
	pNewClient->SetKadState(KS_QUEUED_BUDDY);
	byte ID[16];
	contact->GetClientID().ToByteArray(ID);
	pNewClient->SetUserHash(ID);
	pNewClient->SetConnectOptions(byConnectOptions, true, false);
	AddToKadList(pNewClient);
	//This method checks if this is a dup already.
	AddClient(pNewClient);
	*/


}
bool CClientList::RequestTCP(Kademlia::CContact* contact, uint8 byConnectOptions)
{
	/*
	uint32 nContactIP = ntohl(contact->GetIPAddress());
	// don't connect ourself
	if (m_KadManager.pSvrConnection->GetLocalIP() == nContactIP && thePrefs.GetPort() == contact->GetTCPPort())
	return false;

	CUpDownClient* pNewClient = FindClientByIP(nContactIP, contact->GetTCPPort());

	if (!pNewClient)
	pNewClient = new CUpDownClient(0, contact->GetTCPPort(), contact->GetIPAddress(), 0, 0, false );
	else if (pNewClient->GetKadState() != KS_NONE)
	return false; // already busy with this client in some way (probably buddy stuff), don't mess with it

	//Add client to the lists to be processed.
	pNewClient->SetKadPort(contact->GetUDPPort());
	pNewClient->SetKadState(KS_QUEUED_FWCHECK);
	if (contact->GetClientID() != 0){
	byte ID[16];
	contact->GetClientID().ToByteArray(ID);
	pNewClient->SetUserHash(ID);
	pNewClient->SetConnectOptions(byConnectOptions, true, false);
	}
	m_KadList.AddTail(pNewClient);
	//This method checks if this is a dup already.
	AddClient(pNewClient);
	*/
	return true;
}

// CClientUDPSocket

CClientUDPSocket::CClientUDPSocket()
{
	m_bWouldBlock = false;
	m_port=0;
}

CClientUDPSocket::~CClientUDPSocket()
{
	//j.
	//theApp.m_KadImpl.m_UploadBandwidthThrottler.RemoveFromAllQueues(this); // ZZ:UploadBandWithThrottler (UDP)

	POSITION pos = controlpacket_queue.GetHeadPosition();
	while (pos){
		UDPPack* p = controlpacket_queue.GetNext(pos);
		delete p->packet;
		delete p;
	}
}

void CClientUDPSocket::OnReceive(int nErrorCode)
{
	if (nErrorCode)
	{
		if (thePrefs.GetVerbose()){

		}
	}
	BYTE buffer[5000];
	SOCKADDR_IN sockAddr = {0};
	int iSockAddrLen = sizeof sockAddr;
	int nRealLen = ReceiveFrom(buffer, sizeof buffer, (SOCKADDR*)&sockAddr, &iSockAddrLen);

	if (!(theApp.m_KadImpl.m_IPFilterImpl.IsFiltered(sockAddr.sin_addr.S_un.S_addr) || theApp.m_KadImpl.m_ClientListImpl.IsBannedClient(sockAddr.sin_addr.S_un.S_addr)))
	{
		BYTE* pBuffer;
		uint32 nReceiverVerifyKey;
		uint32 nSenderVerifyKey;
		int nPacketLen = DecryptReceivedClient(buffer, nRealLen, &pBuffer, sockAddr.sin_addr.S_un.S_addr, &nReceiverVerifyKey, &nSenderVerifyKey);
		if (nPacketLen >= 1)
		{
			CString strError;
			try
			{
				switch (pBuffer[0])
				{
				case OP_EMULEPROT:
					{
						if (nPacketLen >= 2)
							ProcessPacket(pBuffer+2, nPacketLen-2, pBuffer[1], sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
						else
							throw CString(_T("eMule packet too short"));
						break;
					}
				case OP_KADEMLIAPACKEDPROT:
					{
						////修改 
						//theStats.AddDownDataOverheadKad(nPacketLen);
						if (nPacketLen >= 2)
						{
							uint32 nNewSize = nPacketLen*10+300;
							BYTE* unpack = NULL;
							uLongf unpackedsize = 0;
							int iZLibResult = 0;

							do {
								delete[] unpack;
								unpack = new BYTE[nNewSize];
								unpackedsize = nNewSize-2;
								iZLibResult = uncompress(unpack+2, &unpackedsize, pBuffer+2, nPacketLen-2);
								nNewSize *= 2; // size for the next try if needed
							} while (iZLibResult == Z_BUF_ERROR && nNewSize < 250000);

							if (iZLibResult == Z_OK)
							{
								unpack[0] = OP_KADEMLIAHEADER;
								unpack[1] = pBuffer[1];
								try
								{
									//返回资源数据
									Kademlia::CKademlia::ProcessPacket(unpack, unpackedsize+2, ntohl(sockAddr.sin_addr.S_un.S_addr), ntohs(sockAddr.sin_port)
										, (Kademlia::CPrefs::GetUDPVerifyKey(sockAddr.sin_addr.S_un.S_addr) == nReceiverVerifyKey)
										, Kademlia::CKadUDPKey(nSenderVerifyKey, Kademlia::CKademlia::GetPublicIP(false)) );
								}
								catch(...)
								{
									delete[] unpack;
									throw;
								}
							}
							else
							{
								delete[] unpack;
								CString strError;
								strError.Format(_T("Failed to uncompress Kad packet: zip error: %d (%hs)"), iZLibResult, zError(iZLibResult));
								throw strError;
							}
							delete[] unpack;
						}
						else
							throw CString(_T("Kad packet (compressed) too short"));
						break;
					}
				case OP_KADEMLIAHEADER:
					{
						//修改 
						//theStats.AddDownDataOverheadKad(nPacketLen);
						if (nPacketLen >= 2)
							Kademlia::CKademlia::ProcessPacket(pBuffer, nPacketLen, ntohl(sockAddr.sin_addr.S_un.S_addr), ntohs(sockAddr.sin_port)
							, (Kademlia::CPrefs::GetUDPVerifyKey(sockAddr.sin_addr.S_un.S_addr) == nReceiverVerifyKey)
							, Kademlia::CKadUDPKey(nSenderVerifyKey, Kademlia::CKademlia::GetPublicIP(false)) );
						else
							throw CString(_T("Kad packet too short"));
						break;
					}
				default:
					{
						CString strError;
						strError.Format(_T("Unknown protocol 0x%02x"), pBuffer[0]);
						throw strError;
					}
				}
			}
			catch(CFileException* error)
			{
				error->Delete();
				strError = _T("Invalid packet received");
			}
			catch(CMemoryException* error)
			{
				error->Delete();
				strError = _T("Memory exception");
			}
			catch(CString error)
			{
				strError = error;
			}
			catch(Kademlia::CIOException* error)
			{
				error->Delete();
				strError = _T("Invalid packet received");
			}
			catch(CException* error)
			{
				error->Delete();
				strError = _T("General packet error");
			}
#ifndef _DEBUG
			catch(...)
			{
				strError = _T("Unknown exception");
				ASSERT(0);
			}
#endif

			if (thePrefs.GetVerbose() && !strError.IsEmpty())
			{
#if _DEL_
				CString strClientInfo;
				CUpDownClient* client;
				if (pBuffer[0] == OP_EMULEPROT)
					client = m_KadManager.pClientList->FindClientByIP_UDP(sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
				else
					client = m_KadManager.pClientList->FindClientByIP_KadPort(sockAddr.sin_addr.S_un.S_addr, ntohs(sockAddr.sin_port));
				if (client)
					strClientInfo = client->DbgGetClientInfo();
				else
					strClientInfo.Format(_T("%s:%u"), ipstr(sockAddr.sin_addr), ntohs(sockAddr.sin_port));

				DebugLogWarning(_T("Client UDP socket: prot=0x%02x  opcode=0x%02x  sizeaftercrypt=%u realsize=%u  %s: %s"), pBuffer[0], pBuffer[1], nPacketLen, nRealLen, strError, strClientInfo);
#endif
			}

		}
		else if (nPacketLen == SOCKET_ERROR)
		{
			DWORD dwError = WSAGetLastError();
			if (dwError == WSAECONNRESET)
			{
				// Depending on local and remote OS and depending on used local (remote?) router we may receive
				// WSAECONNRESET errors. According some KB articles, this is a special way of winsock to report 
				// that a sent UDP packet was not received by the remote host because it was not listening on 
				// the specified port -> no eMule running there.
				//
				// TODO: So, actually we should do something with this information and drop the related Kad node 
				// or eMule client...
				;
			}
			if (thePrefs.GetVerbose() && dwError != WSAECONNRESET)
			{
				CString strClientInfo;
				if (iSockAddrLen > 0 && sockAddr.sin_addr.S_un.S_addr != 0 && sockAddr.sin_addr.S_un.S_addr != INADDR_NONE)
					strClientInfo.Format(_T(" from %s:%u"), ipstr(sockAddr.sin_addr), ntohs(sockAddr.sin_port));
				//DebugLogError(_T("Error: Client UDP socket, failed to receive data%s: %s"), strClientInfo, GetErrorMessage(dwError, 1));
			}
		}
	}

}

bool CClientUDPSocket::ProcessPacket(const BYTE* packet, UINT size, uint8 opcode, uint32 ip, uint16 port)
{
#if _DEL_
	switch(opcode)
	{
	case OP_REASKCALLBACKUDP:
		{

			theStats.AddDownDataOverheadOther(size);
			CUpDownClient* buddy = m_KadManager.pClientList->GetBuddy();
			if( buddy )
			{
				if( size < 17 || buddy->socket == NULL )
					break;
				if (!md4cmp(packet, buddy->GetBuddyID()))
				{
					PokeUInt32(const_cast<BYTE*>(packet)+10, ip);
					PokeUInt16(const_cast<BYTE*>(packet)+14, port);
					Packet* response = new Packet(OP_EMULEPROT);
					response->opcode = OP_REASKCALLBACKTCP;
					response->pBuffer = new char[size];
					memcpy(response->pBuffer, packet+10, size-10);
					response->size = size-10;

					theStats.AddUpDataOverheadFileRequest(response->size);
					buddy->SendPacket(response, true);
				}
			}
			break;
		}
	case OP_REASKFILEPING:
		{

			theStats.AddDownDataOverheadFileRequest(size);
			CSafeMemFile data_in(packet, size);
			uchar reqfilehash[16];
			data_in.ReadHash16(reqfilehash);
			CKnownFile* reqfile = m_KadManager.pSharedFiles->GetFileByID(reqfilehash);

			bool bSenderMultipleIpUnknown = false;
			CUpDownClient* sender = m_KadManager.pUploadQueue->GetWaitingClientByIP_UDP(ip, port, true, &bSenderMultipleIpUnknown);
			if (!reqfile)
			{

				Packet* response = new Packet(OP_FILENOTFOUND,0,OP_EMULEPROT);
				theStats.AddUpDataOverheadFileRequest(response->size);
				if (sender != NULL)
					SendPacket(response, ip, port, sender->ShouldReceiveCryptUDPPackets(), sender->GetUserHash(), false, 0);
				else
					SendPacket(response, ip, port, false, NULL, false, 0);
				break;
			}
			if (sender)
			{


				//Make sure we are still thinking about the same file
				if (md4cmp(reqfilehash, sender->GetUploadFileID()) == 0)
				{
					sender->AddAskedCount();
					sender->SetLastUpRequest();
					//I messed up when I first added extended info to UDP
					//I should have originally used the entire ProcessExtenedInfo the first time.
					//So now I am forced to check UDPVersion to see if we are sending all the extended info.
					//For now on, we should not have to change anything here if we change
					//anything to the extended info data as this will be taken care of in ProcessExtendedInfo()
					//Update extended info. 
					if (sender->GetUDPVersion() > 3)
					{
						sender->ProcessExtendedInfo(&data_in, reqfile);
					}
					//Update our complete source counts.
					else if (sender->GetUDPVersion() > 2)
					{
						uint16 nCompleteCountLast= sender->GetUpCompleteSourcesCount();
						uint16 nCompleteCountNew = data_in.ReadUInt16();
						sender->SetUpCompleteSourcesCount(nCompleteCountNew);
						if (nCompleteCountLast != nCompleteCountNew)
						{
							reqfile->UpdatePartsInfo();
						}
					}
					CSafeMemFile data_out(128);
					if(sender->GetUDPVersion() > 3)
					{
						if (reqfile->IsPartFile())
							((CPartFile*)reqfile)->WritePartStatus(&data_out);
						else
							data_out.WriteUInt16(0);
					}
					data_out.WriteUInt16((uint16)(m_KadManager.pUploadQueue->GetWaitingPosition(sender)));

					Packet* response = new Packet(&data_out, OP_EMULEPROT);
					response->opcode = OP_REASKACK;
					theStats.AddUpDataOverheadFileRequest(response->size);
					SendPacket(response, ip, port, sender->ShouldReceiveCryptUDPPackets(), sender->GetUserHash(), false, 0);
				}
				else
				{
					DebugLogError(_T("Client UDP socket; ReaskFilePing; reqfile does not match"));
					TRACE(_T("reqfile:         %s\n"), DbgGetFileInfo(reqfile->GetFileHash()));
					TRACE(_T("sender->GetRequestFile(): %s\n"), sender->GetRequestFile() ? DbgGetFileInfo(sender->GetRequestFile()->GetFileHash()) : _T("(null)"));
				}

			}
			else
			{

				if (thePrefs.GetDebugClientUDPLevel() > 0){
				}

				//DebugRecv("OP_ReaskFilePing", NULL, reqfilehash, ip);
				// Don't answer him. We probably have him on our queue already, but can't locate him. Force him to establish a TCP connection
				if (!bSenderMultipleIpUnknown){
					if (((uint32)m_KadManager.pUploadQueue->GetWaitingUserCount() + 50) > thePrefs.GetQueueSize())
					{

						if (thePrefs.GetDebugClientUDPLevel() > 0){
						}
						//DebugSend("OP__QueueFull", NULL);
						Packet* response = new Packet(OP_QUEUEFULL,0,OP_EMULEPROT);
						theStats.AddUpDataOverheadFileRequest(response->size);
						SendPacket(response, ip, port, false, NULL, false, 0); // we cannot answer this one encrypted since we dont know this client
					}
				}
				else{
					DebugLogWarning(_T("UDP Packet received - multiple clients with the same IP but different UDP port found. Possible UDP Portmapping problem, enforcing TCP connection. IP: %s, Port: %u"), ipstr(ip), port); 
				}
				break;

			}

		}
	case OP_QUEUEFULL:
		{

			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = m_KadManager.pDownLoadQueue->GetDownloadClientByIP_UDP(ip, port, true);
			if (thePrefs.GetDebugClientUDPLevel() > 0){

			}
			//DebugRecv("OP_QueueFull", sender, NULL, ip);
			if (sender && sender->UDPPacketPending()){
				sender->SetRemoteQueueFull(true);
				sender->UDPReaskACK(0);
			}
			else if (sender != NULL)
				DebugLogError(_T("Received UDP Packet (OP_QUEUEFULL) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			break;


		}
	case OP_REASKACK:
		{

			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = m_KadManager.pDownLoadQueue->GetDownloadClientByIP_UDP(ip, port, true);
			if (thePrefs.GetDebugClientUDPLevel() > 0){

			}
			//DebugRecv("OP_ReaskAck", sender, NULL, ip);
			if (sender && sender->UDPPacketPending()){
				CSafeMemFile data_in(packet, size);
				if ( sender->GetUDPVersion() > 3 )
				{
					sender->ProcessFileStatus(true, &data_in, sender->GetRequestFile());
				}
				uint16 nRank = data_in.ReadUInt16();
				sender->SetRemoteQueueFull(false);
				sender->UDPReaskACK(nRank);
				sender->AddAskedCountDown();
			}
			else if (sender != NULL)
				DebugLogError(_T("Received UDP Packet (OP_REASKACK) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			break;


		}
	case OP_FILENOTFOUND:
		{

			theStats.AddDownDataOverheadFileRequest(size);
			CUpDownClient* sender = m_KadManager.pDownLoadQueue->GetDownloadClientByIP_UDP(ip, port, true);
			if (thePrefs.GetDebugClientUDPLevel() > 0){

			}
			//	DebugRecv("OP_FileNotFound", sender, NULL, ip);
			if (sender && sender->UDPPacketPending()){
				sender->UDPReaskFNF(); // may delete 'sender'!
				sender = NULL;
			}
			else if (sender != NULL)
				DebugLogError(_T("Received UDP Packet (OP_FILENOTFOUND) which was not requested (pendingflag == false); Ignored packet - %s"), sender->DbgGetClientInfo());
			break;


		}

	case OP_PORTTEST:
		{
			if (thePrefs.GetDebugClientUDPLevel() > 0)
			{

			}
			theStats.AddDownDataOverheadOther(size);
			if (size == 1){
				if (packet[0] == 0x12){
					bool ret = m_KadManager.pListenSocket->SendPortTestReply('1', true);
					AddDebugLogLine(true, _T("UDP Portcheck packet arrived - ACK sent back (status=%i)"), ret);
				}
			}
			break;
		}
	case OP_DIRECTCALLBACKREQ:
		{

			if (thePrefs.GetDebugClientUDPLevel() > 0)
			{

			}
			if (!m_KadManager.pClientList->AllowCalbackRequest(ip)){
				DebugLogWarning(_T("Ignored DirectCallback Request because this IP (%s) has sent too many request within a short time"), ipstr(ip));
				break;
			}
			// do we accept callbackrequests at all?
			if (Kademlia::CKademlia::IsRunning() && Kademlia::CKademlia::IsFirewalled())
			{
				m_KadManager.pClientList->AddTrackCallbackRequests(ip);
				CSafeMemFile data(packet, size);
				uint16 nRemoteTCPPort = data.ReadUInt16();
				uchar uchUserHash[16];
				data.ReadHash16(uchUserHash);
				uint8 byConnectOptions = data.ReadUInt8();
				CUpDownClient* pRequester = m_KadManager.pClientList->FindClientByUserHash(uchUserHash, ip, nRemoteTCPPort);
				if (pRequester == NULL) {
					pRequester = new CUpDownClient(NULL, nRemoteTCPPort, ip, 0, 0, true);
					pRequester->SetUserHash(uchUserHash);
					m_KadManager.pClientList->AddClient(pRequester);
				}
				pRequester->SetConnectOptions(byConnectOptions, true, false);
				pRequester->SetDirectUDPCallbackSupport(false);
				pRequester->SetIP(ip);
				pRequester->SetUserPort(nRemoteTCPPort);
				DEBUG_ONLY( DebugLog(_T("Accepting incoming DirectCallbackRequest from %s"), pRequester->DbgGetClientInfo()) );
				pRequester->TryToConnect();
			}
			else
				DebugLogWarning(_T("Ignored DirectCallback Request because we do not accept DirectCall backs at all (%s)"), ipstr(ip));

			break;
		}
	default:

		theStats.AddDownDataOverheadOther(size);
		if (thePrefs.GetDebugClientUDPLevel() > 0)
		{
			CUpDownClient* sender = m_KadManager.pDownLoadQueue->GetDownloadClientByIP_UDP(ip, port, true);
			Debug(_T("Unknown client UDP packet: host=%s:%u (%s) opcode=0x%02x  size=%u\n"), ipstr(ip), port, sender ? sender->DbgGetClientInfo() : _T(""), opcode, size);
		}

		return false;
	}
#endif
	return true;

}

void CClientUDPSocket::OnSend(int nErrorCode){


	if (nErrorCode){
		if (thePrefs.GetVerbose()){}
		//DebugLogError(_T("Error: Client UDP socket, error on send event: %s"), GetErrorMessage(nErrorCode, 1));
		return;
	}

	// ZZ:UploadBandWithThrottler (UDP) -->
	sendLocker.Lock();
	m_bWouldBlock = false;

	if(!controlpacket_queue.IsEmpty()) {
		theApp.m_KadImpl.m_UploadBandwidthThrottler.QueueForSendingControlPacket(this); 
	}
	sendLocker.Unlock();
	// <-- ZZ:UploadBandWithThrottler (UDP)

}

SocketSentBytes CClientUDPSocket::SendControlData(uint32 maxNumberOfBytesToSend, uint32 /*minFragSize*/){ // ZZ:UploadBandWithThrottler (UDP)

	// ZZ:UploadBandWithThrottler (UDP) -->
	// NOTE: *** This function is invoked from a *different* thread!


	sendLocker.Lock();

	uint32 sentBytes = 0;
	// <-- ZZ:UploadBandWithThrottler (UDP)

	while (!controlpacket_queue.IsEmpty() && !IsBusy() && sentBytes < maxNumberOfBytesToSend){ // ZZ:UploadBandWithThrottler (UDP)
		UDPPack* cur_packet = controlpacket_queue.GetHead();
		if( GetTickCount() - cur_packet->dwTime < UDPMAXQUEUETIME )
		{
			uint32 nLen = cur_packet->packet->size+2;
			uchar* sendbuffer = new uchar[nLen];
			memcpy(sendbuffer,cur_packet->packet->GetUDPHeader(),2);
			memcpy(sendbuffer+2,cur_packet->packet->pBuffer,cur_packet->packet->size);

			if (cur_packet->bEncrypt && (Kademlia::CKademlia::GetPublicIP(0) > 0 || cur_packet->bKad)){
				nLen = EncryptSendClient(&sendbuffer, nLen, cur_packet->pachTargetClientHashORKadID, cur_packet->bKad,  cur_packet->nReceiverVerifyKey, (cur_packet->bKad ? Kademlia::CPrefs::GetUDPVerifyKey(cur_packet->dwIP) : (uint16)0));
				//DEBUG_ONLY(  AddDebugLogLine(DLP_VERYLOW, false, _T("Sent obfuscated UDP packet to clientIP: %s, Kad: %s, ReceiverKey: %u"), ipstr(cur_packet->dwIP), cur_packet->bKad ? _T("Yes") : _T("No"), cur_packet->nReceiverVerifyKey) );
			}

			if (!SendTo((char*)sendbuffer, nLen, cur_packet->dwIP, cur_packet->nPort)){
				sentBytes += nLen; // ZZ:UploadBandWithThrottler (UDP)

				controlpacket_queue.RemoveHead();
				delete cur_packet->packet;
				delete cur_packet;
			}
			delete[] sendbuffer;
		}
		else
		{
			controlpacket_queue.RemoveHead();
			delete cur_packet->packet;
			delete cur_packet;
		}
	}

	// ZZ:UploadBandWithThrottler (UDP) -->
	if(!IsBusy() && !controlpacket_queue.IsEmpty()) {
		theApp.m_KadImpl.m_UploadBandwidthThrottler.QueueForSendingControlPacket(this); //
	}
	sendLocker.Unlock();

	SocketSentBytes returnVal = { true, 0, sentBytes };
	return returnVal;
	// <-- ZZ:UploadBandWithThrottler (UDP)
}

int CClientUDPSocket::SendTo(char* lpBuf,int nBufLen,uint32 dwIP, uint16 nPort)
{

	// NOTE: *** This function is invoked from a *different* thread!
	uint32 result = CAsyncSocket::SendTo(lpBuf,nBufLen,nPort,ipstr(dwIP));
	if (result == (uint32)SOCKET_ERROR){
		uint32 error = GetLastError();
		if (error == WSAEWOULDBLOCK){
			m_bWouldBlock = true;
			return -1;
		}
		//if (thePrefs.GetVerbose())
		//DebugLogError(_T("Error: Client UDP socket, failed to send data to %s:%u: %s"), ipstr(dwIP), nPort, GetErrorMessage(error, 1));
	}
	return 0;

}

bool CClientUDPSocket::SendPacket(Packet* packet, uint32 dwIP, uint16 nPort, bool bEncrypt, const uchar* pachTargetClientHashORKadID, bool bKad, uint32 nReceiverVerifyKey)
{
	UDPPack* newpending = new UDPPack;
	newpending->dwIP = dwIP;
	newpending->nPort = nPort;
	newpending->packet = packet;
	newpending->dwTime = GetTickCount();
	newpending->bEncrypt = bEncrypt && (pachTargetClientHashORKadID != NULL || (bKad && nReceiverVerifyKey != 0));
	newpending->bKad = bKad;
	newpending->nReceiverVerifyKey = nReceiverVerifyKey;

#ifdef _DEBUG
	if (newpending->packet->size > UDP_KAD_MAXFRAGMENT)
		DebugLogWarning(_T("Sending UDP packet > UDP_KAD_MAXFRAGMENT, opcode: %X, size: %u"), packet->opcode, packet->size);
#endif

	if (newpending->bEncrypt && pachTargetClientHashORKadID != NULL)
		md4cpy(newpending->pachTargetClientHashORKadID, pachTargetClientHashORKadID);
	else
		md4clr(newpending->pachTargetClientHashORKadID);
	// ZZ:UploadBandWithThrottler (UDP) -->
	sendLocker.Lock();
	controlpacket_queue.AddTail(newpending);
	sendLocker.Unlock();


	theApp.m_KadImpl.m_UploadBandwidthThrottler.QueueForSendingControlPacket(this); //

	return true;
	// <-- ZZ:UploadBandWithThrottler (UDP)
}

bool CClientUDPSocket::Create()
{
	bool ret = FALSE;
	if (thePrefs.GetUDPPort())
	{
		try
		{
			ret = CAsyncSocket::Create(thePrefs.GetUDPPort(), SOCK_DGRAM, FD_READ | FD_WRITE, thePrefs.GetBindAddrW()) != FALSE;
			if (ret)
			{
				m_port = thePrefs.GetUDPPort();
				// the default socket size seems to be not enough for this UDP socket
				// because we tend to drop packets if several flow in at the same time
				int val = 64 * 1024;
				if (!SetSockOpt(SO_RCVBUF, &val, sizeof(val)))
					DebugLogError(_T("Failed to increase socket size on UDP socket"));
			}
			else
			{

				int err =GetLastError() ;
				TRACE("%d\r\n",err);
			}
		}
		catch (CException* e)
		{
			//e->GetErrorMessage(L"error",10);
		}

	}

	if (ret)
		m_port = thePrefs.GetUDPPort();

	return ret;

}

bool CClientUDPSocket::Rebind()
{
	if (thePrefs.GetUDPPort() == m_port)
		return false;
	Close();

	return Create();
}
