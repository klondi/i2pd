#ifndef SSU_H__
#define SSU_H__

#include <inttypes.h>
#include <map>
#include <boost/asio.hpp>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include "I2PEndian.h"

namespace i2p
{
namespace ssu
{
#pragma pack(1)
	struct SSUHeader
	{
		uint8_t mac[16];
		uint8_t iv[16];
		uint8_t flag;
		uint32_t time;	
	};
#pragma pack()

	const int SSU_MTU = 1484;

	// payload types (4 bits)
	const uint8_t PAYLOAD_TYPE_SESSION_REQUEST = 0;
	const uint8_t PAYLOAD_TYPE_SESSION_CREATED = 1;
	const uint8_t PAYLOAD_TYPE_SESSION_CONFIRMED = 2;
	const uint8_t PAYLOAD_TYPE_RELAY_REQUEST = 3;
	const uint8_t PAYLOAD_TYPE_RELAY_RESPONSE = 4;
	const uint8_t PAYLOAD_TYPE_RELAY_INTRO = 5;
	const uint8_t PAYLOAD_TYPE_DATA = 6;
	const uint8_t PAYLOAD_TYPE_TEST = 7;

	enum SessionState
	{
		eSessionStateUnknown,
		eSessionStateRequestSent, 
		eSessionStateRequestReceived,
		eSessionStateCreatedSent,
		eSessionStateCreatedReceived,
		eSessionStateConfirmedSent,
		eSessionStateConfirmedReceived,
		eSessionStateEstablised
	};		

	class SSUSession
	{
		public:

			SSUSession ();
			void ProcessNextMessage (uint8_t * buf, size_t len);
		
		private:

			void Authenticate (uint8_t * buf, size_t len, uint8_t * aesKey, uint8_t * iv, uint8_t * macKey);

		private:
			
			SessionState m_State;	
			CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption m_Encryption;			
	};

	class SSUServer
	{
		public:

			SSUServer (boost::asio::io_service& service, int port);
			~SSUServer ();
			void Start ();
			void Stop ();

		private:

			void Receive ();
			void HandleReceivedFrom (const boost::system::error_code& ecode, std::size_t bytes_transferred);

		private:
			
			boost::asio::ip::udp::socket m_Socket;
			boost::asio::ip::udp::endpoint m_SenderEndpoint;
			uint8_t m_ReceiveBuffer[SSU_MTU];
			std::map<boost::asio::ip::udp::endpoint, SSUSession *> m_Sessions;
	};
}
}

#endif
