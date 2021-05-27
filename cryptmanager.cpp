#include "StdAfx.h"
#include "cryptmanager.h"
//#include "dll.h"	// Inlcude this file before any other cryptopp header file

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "files.h"
using CryptoPP::FileSource;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::BlockPaddingSchemeDef;
using CryptoPP::HashFilter;
using CryptoPP::ArraySink;

#include "des.h"
using CryptoPP::DES;

#include "md5.h"
using CryptoPP::MD5;

#include "base64.h"
using CryptoPP::Base64Encoder;
using CryptoPP::Base64Decoder;

#include "sha.h"
using CryptoPP::SHA512;

#include "modes.h"
using CryptoPP::ECB_Mode;



namespace CryptManager
{

	// Des
	string EncodeDesString(string text, string key)
	{
		ECB_Mode<DES>::Encryption e;
		e.SetKey((const unsigned char *)key.c_str(), DES::KEYLENGTH);
		string cipher;

		try
		{
			StringSource(text, true,
				new CryptoPP::StreamTransformationFilter(e, new HexEncoder(new CryptoPP::StringSink(cipher), false), BlockPaddingSchemeDef::ZEROS_PADDING)
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			cipher.clear();
		}

		return cipher.c_str();
	}

	string EncodeDesString(string text)
	{
		string key = DES_KEY;

		return EncodeDesString(text, key);
	}

	string DecodeDesString(string text, string key)
	{
		ECB_Mode<DES>::Decryption d;
		d.SetKey((const unsigned char *)key.c_str(), DES::KEYLENGTH);
		string plain;

		try
		{
			StringSource(text, true, 
				new HexDecoder(new CryptoPP::StreamTransformationFilter(d, new CryptoPP::StringSink(plain), BlockPaddingSchemeDef::ZEROS_PADDING))
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			plain.clear();
		}

		return plain.c_str();
	}

	string DecodeDesString(string text)
	{

		string key = DES_KEY;

		return DecodeDesString(text, key);
	}

	// Base 64
	string EncodeBase64String(string text)
	{
		string cipher;

		try
		{
			StringSource(text, true,
				new Base64Encoder(new CryptoPP::StringSink(cipher))
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			cipher.clear();
		}

		return cipher;
	}

	string DecodeBase64String(string text)
	{
		string plain;

		try
		{
			StringSource(text, true,
				new Base64Decoder(new CryptoPP::StringSink(plain))
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			plain.clear();
		}

		return plain;
	}

	string GetFileBase64(string path)
	{
		string cipher;

		try
		{
			FileSource(path.c_str(), true,
				new Base64Encoder(new CryptoPP::StringSink(cipher))
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			cipher.clear();
		}

		return cipher;
	}

	// Md5
	string EncodeMd5String(string value)
	{
		CryptoPP::MD5 hash;
		byte digest[CryptoPP::MD5::DIGESTSIZE] = {0};

		hash.CalculateDigest(digest, (byte*) value.c_str(), value.length());

		CryptoPP::HexEncoder encoder;
		std::string cipher;
		encoder.Attach(new CryptoPP::StringSink(cipher));
		encoder.Put(digest, sizeof(digest));	// CryptoPP::MD5::DIGESTSIZE
		encoder.MessageEnd();

		transform(cipher.begin(), cipher.end(), cipher.begin(), tolower);
		return cipher;
	}

	string GetFileMd5(string path)
	{
		string cipher;

		CryptoPP::MD5 hash;

		const size_t size = MD5::DIGESTSIZE * 2;	// The output is encoded as hex which is why the output buffer must be two times the size of the MD5
		unsigned char buf[size+1] = {0};

		try
		{
			FileSource(path.c_str(), true,
				new HashFilter(hash, new HexEncoder(new StringSink(cipher)))
				);
		}
		catch(const CryptoPP::Exception& e)
		{
			cipher.clear();
		}

		transform(cipher.begin(), cipher.end(), cipher.begin(), tolower);
		return cipher;
	}

	// Sha
	string EncodeShaString(string value)
	{
		CryptoPP::SHA512 hash;
		byte digest[CryptoPP::SHA512::DIGESTSIZE] = {0};

		hash.CalculateDigest(digest, (byte*) value.c_str(), value.length());

		CryptoPP::HexEncoder encoder;
		std::string cipher;
		encoder.Attach(new CryptoPP::StringSink(cipher));
		encoder.Put(digest, sizeof(digest));	// CryptoPP::SHA512::DIGESTSIZE
		encoder.MessageEnd();

		transform(cipher.begin(), cipher.end(), cipher.begin(), tolower);
		return cipher;
	}


	// Url
	QString EncodeUrlString(QString text)
	{
		QString cipher;

		cipher = QString(QUrl(text).toEncoded());

		return cipher;
	}

	QString DecodeUrlString(QString text)
	{
		QString plain;

		plain = QUrl::fromPercentEncoding(text.toLocal8Bit());

		return plain;
	}

}