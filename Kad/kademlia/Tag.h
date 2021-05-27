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
#include "../routing/Maps.h"
#include "../def/opcodes.h"
#include "../comm/commFun.h"

// forward declarations
namespace Kademlia { class CKadTagValueString; }
void KadTagStrMakeLower(Kademlia::CKadTagValueString &rstr);
int KadTagStrCompareNoCase(LPCWSTR dst, LPCWSTR src);

namespace Kademlia
{
	class CKadTagNameString : protected CStringA
	{
		public:
			CKadTagNameString()
			{}

			CKadTagNameString(LPCSTR psz)
					: CStringA(psz)
			{}

			CKadTagNameString(LPCSTR psz, int len)
					: CStringA(psz, len)
			{}

			// A tag name may include character values >= 0xD0 and therefor also >= 0xF0. to prevent those
			// characters be interpreted as multi byte character sequences we have to sensure that a binary
			// string compare is performed.
			int Compare(LPCSTR psz) const throw()
			{
				ATLASSERT( AfxIsValidString(psz) );
				// Do a binary string compare. (independant from any codepage and/or LC_CTYPE setting.)
				return strcmp(GetString(), psz);
			}

			int CompareNoCase(LPCSTR psz) const throw()
			{
				ATLASSERT( AfxIsValidString(psz) );

				// Version #1
				// Do a case-insensitive ASCII string compare.
				// NOTE: The current locale category LC_CTYPE *MUST* be set to "C"!
				//return stricmp(GetString(), psz);

				// Version #2 - independant from any codepage and/or LC_CTYPE setting.
				return _stricmp(GetString(), psz);
			}

			CKadTagNameString& operator=(LPCSTR pszSrc)
			{
				CStringA::operator=(pszSrc);
				return *this;
			}

			operator PCXSTR() const throw()
			{
				return CStringA::operator PCXSTR();
			}

			XCHAR operator[]( int iChar ) const throw()
			{
				return CStringA::operator [](iChar);
			}

			PXSTR GetBuffer()
			{
				return CStringA::GetBuffer();
			}

			PXSTR GetBuffer(int nMinBufferLength)
			{
				return CStringA::GetBuffer(nMinBufferLength);
			}

			int GetLength() const throw()
			{
				return CStringA::GetLength();
			}
	};


	class CKadTagValueString : public CStringW
	{
		public:
			CKadTagValueString()
			{}

			CKadTagValueString(const CStringW &rstr)
				: CStringW(rstr)
			{}

			CKadTagValueString(const wchar_t *psz)
				: CStringW(psz)
			{}

			CKadTagValueString(const wchar_t *psz, int iLen)
				: CStringW(psz, iLen)
			{}

			int CompareNoCase(LPCWSTR src) const throw()
			{
				return KadTagStrCompareNoCase(GetString(), src);
			}

			int Collate(PCXSTR psz) const throw()
			{
				// One *MUST NOT* call this function (see comments in 'KadTagStrMakeLower')
				ASSERT(0);
				return __super::Collate(psz);
			}

			int CollateNoCase(PCXSTR psz) const throw()
			{
				// One *MUST NOT* call this function (see comments in 'KadTagStrMakeLower')
				ASSERT(0);
				return __super::CollateNoCase(psz);
			}

			CKadTagValueString& MakeLower()
			{
				KadTagStrMakeLower(*this);
				return *this;
			}

			CKadTagValueString& MakeUpper()
			{
				// One *MUST NOT* call this function (see comments in 'KadTagStrMakeLower')
				ASSERT(0);
				__super::MakeUpper();
				return *this;
			}
	};


	class CKadTag
	{
		public:
			byte	m_type;
			CKadTagNameString m_name;

			CKadTag(byte type, LPCSTR name)
					: m_name(name)
			{
				m_type = type;
			}
			virtual ~CKadTag()
			{}
			virtual CKadTag* Copy() = 0;

			bool IsStr()  const
			{
				return m_type == TAGTYPE_STRING;
			}
			bool IsNum()  const
			{
				return m_type == TAGTYPE_UINT64 || m_type == TAGTYPE_UINT32 || m_type == TAGTYPE_UINT16 || m_type == TAGTYPE_UINT8 || m_type == TAGTYPE_BOOL || m_type == TAGTYPE_FLOAT32 || m_type == 0xFE;
			}
			bool IsInt()  const
			{
				return m_type == TAGTYPE_UINT64 || m_type == TAGTYPE_UINT32 || m_type == TAGTYPE_UINT16 || m_type == TAGTYPE_UINT8 || m_type == 0xFE;
			}
			bool IsFloat()const
			{
				return m_type == TAGTYPE_FLOAT32;
			}
			bool IsBsob() const
			{
				return m_type == TAGTYPE_BSOB;
			}
			bool IsHash() const
			{
				return m_type == TAGTYPE_HASH;
			}

			virtual CKadTagValueString GetStr() const
			{
				ASSERT(0);
				return L"";
			}
			virtual uint64 GetInt() const
			{
				ASSERT(0);
				return 0;
			}
			virtual float GetFloat() const
			{
				ASSERT(0);
				return 0.0F;
			}
			virtual const BYTE* GetBsob() const
			{
				ASSERT(0);
				return NULL;
			}
			virtual uint8 GetBsobSize() const
			{
				ASSERT(0);
				return 0;
			}
			virtual bool GetBool() const
			{
				ASSERT(0);
				return false;
			}
			virtual const BYTE* GetHash() const
			{
				ASSERT(0);
				return NULL;
			}

		protected:
			CKadTag()
			{}
	}
	;


	class CKadTagUnk : public CKadTag
	{
		public:
			CKadTagUnk(byte type, LPCSTR name)
					: CKadTag(type, name)
			{ }

			virtual CKadTagUnk* Copy()
			{
				return new CKadTagUnk(*this);
			}
	};


	class CKadTagStr : public CKadTag
	{
		public:
			CKadTagStr(LPCSTR name, LPCWSTR value, int len)
					: CKadTag(TAGTYPE_STRING, name)
					, m_value(value, len)
			{ }

			CKadTagStr(LPCSTR name, const CStringW& rstr)
					: CKadTag(TAGTYPE_STRING, name)
					, m_value(rstr)
			{ }

			virtual CKadTagStr* Copy()
			{
				return new CKadTagStr(*this);
			}

			virtual CKadTagValueString GetStr() const
			{
				return m_value;
			}

		protected:
			CKadTagValueString m_value;
	};

	class CKadTagUInt : public CKadTag
	{
		public:
			CKadTagUInt(LPCSTR name, uint64 value)
					: CKadTag(0xFE, name)
					, m_value(value)
			{ }

			virtual CKadTagUInt* Copy()
			{
				return new CKadTagUInt(*this);
			}

			virtual uint64 GetInt() const
			{
				return m_value;
			}

		protected:
			uint64 m_value;
	};

	class CKadTagUInt64 : public CKadTag
	{
		public:
			CKadTagUInt64(LPCSTR name, uint64 value)
					: CKadTag(TAGTYPE_UINT64, name)
					, m_value(value)
			{ }

			virtual CKadTagUInt64* Copy()
			{
				return new CKadTagUInt64(*this);
			}

			virtual uint64 GetInt() const
			{
				return m_value;
			}

		protected:
			uint64 m_value;
	};

	class CKadTagUInt32 : public CKadTag
	{
		public:
			CKadTagUInt32(LPCSTR name, uint32 value)
					: CKadTag(TAGTYPE_UINT32, name)
					, m_value(value)
			{ }

			virtual CKadTagUInt32* Copy()
			{
				return new CKadTagUInt32(*this);
			}

			virtual uint64 GetInt() const
			{
				return m_value;
			}

		protected:
			uint32 m_value;
	};


	class CKadTagFloat : public CKadTag
	{
		public:
			CKadTagFloat(LPCSTR name, float value)
					: CKadTag(TAGTYPE_FLOAT32, name)
					, m_value(value)
			{ }

			virtual CKadTagFloat* Copy()
			{
				return new CKadTagFloat(*this);
			}

			virtual float GetFloat() const
			{
				return m_value;
			}

		protected:
			float m_value;
	};


	class CKadTagBool : public CKadTag
	{
		public:
			CKadTagBool(LPCSTR name, bool value)
					: CKadTag(TAGTYPE_BOOL, name)
					, m_value(value)
			{ }

			virtual CKadTagBool* Copy()
			{
				return new CKadTagBool(*this);
			}

			virtual bool GetBool() const
			{
				return m_value;
			}

		protected:
			bool m_value;
	};


	class CKadTagUInt16 : public CKadTag
	{
		public:
			CKadTagUInt16(LPCSTR name, uint16 value)
					: CKadTag(TAGTYPE_UINT16, name)
					, m_value(value)
			{ }

			virtual CKadTagUInt16* Copy()
			{
				return new CKadTagUInt16(*this);
			}

			virtual uint64 GetInt() const
			{
				return m_value;
			}

		protected:
			uint16 m_value;
	};


	class CKadTagUInt8 : public CKadTag
	{
		public:
			CKadTagUInt8(LPCSTR name, uint8 value)
					: CKadTag(TAGTYPE_UINT8, name)
					, m_value(value)
			{ }

			virtual CKadTagUInt8* Copy()
			{
				return new CKadTagUInt8(*this);
			}

			virtual uint64 GetInt() const
			{
				return m_value;
			}

		protected:
			uint8 m_value;
	};


	class CKadTagBsob : public CKadTag
	{
		public:
			CKadTagBsob(LPCSTR name, const BYTE* value, uint8 nSize)
					: CKadTag(TAGTYPE_BSOB, name)
			{
				m_value = new BYTE[nSize];
				memcpy(m_value, value, nSize);
				m_size = nSize;
			}
			CKadTagBsob(const CKadTagBsob& rTag)
					: CKadTag(rTag)
			{
				m_value = new BYTE[rTag.m_size];
				memcpy(m_value, rTag.m_value, rTag.m_size);
				m_size = rTag.m_size;
			}
			~CKadTagBsob()
			{
				delete[] m_value;
			}

			virtual CKadTagBsob* Copy()
			{
				return new CKadTagBsob(*this);
			}

			virtual const BYTE* GetBsob() const
			{
				return m_value;
			}
			virtual uint8 GetBsobSize() const
			{
				return m_size;
			}

		protected:
			BYTE* m_value;
			uint8 m_size;
	};


	class CKadTagHash : public CKadTag
	{
		public:
			CKadTagHash(LPCSTR name, const BYTE* value)
					: CKadTag(TAGTYPE_HASH, name)
			{
				m_value = new BYTE[16];
				md4cpy(m_value, value);
			}
			CKadTagHash(const CKadTagHash& rTag)
					: CKadTag(rTag)
			{
				m_value = new BYTE[16];
				md4cpy(m_value, rTag.m_value);
			}
			~CKadTagHash()
			{
				delete[] m_value;
			}

			virtual CKadTagHash* Copy()
			{
				return new CKadTagHash(*this);
			}

			virtual const BYTE* GetHash() const
			{
				return m_value;
			}

		protected:
			BYTE* m_value;
	};
}
