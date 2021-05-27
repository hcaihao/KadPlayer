/*
Copyright (C)2003 Barry Dunne (http://www.emule-project.net)
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// Note To Mods //
/*
Please do not change anything here and release it..
There is going to be a new forum created just for the Kademlia side of the client..
If you feel there is an error or a way to improve something, please
post it in the forum first and let us look at it.. If it is a real improvement,
it will be added to the offical client.. Changing something without knowing
what all it does can cause great harm to the network if released in mass form..
Any mod that changes anything within the Kademlia side will not be allowed to advertise
there client on the eMule forum..
*/
#pragma once

namespace Kademlia
{
#define ERR_SUCCESS							0x0000
#define ERR_UNKNOWN							0xFFFF
#define ERR_WINSOCK							0x0001
#define	ERR_TCP_LISTENER_START_FAILURE		0x0002
#define	ERR_UDP_LISTENER_START_FAILURE		0x0003
#define	ERR_READ_ONLY						0x0004
#define	ERR_WRITE_ONLY						0x0005
#define	ERR_END_OF_FILE						0x0006
#define	ERR_BUFFER_TOO_SMALL				0x0007
#define ERR_CREATE_SOCKET_FAILED			0x0008
#define ERR_CREATE_THREAD_FAILED			0x0009
#define ERR_CREATE_HANDLE_FAILED			0x000A
#define ERR_LISTEN_FAILED					0x000B
#define ERR_QUEUE_FULL						0x000C
#define ERR_NO_CONTACTS						0x000D

	class CKademliaError
	{
		public:
			CKademliaError(int iErrorCode, LPCSTR szErrorDescription)
			{
				m_iErrorCode = iErrorCode;
				m_szErrorDescription = szErrorDescription;
			}

			int m_iErrorCode;
			LPCSTR m_szErrorDescription; // For internationalisation use error code to lookup description
	};
}
