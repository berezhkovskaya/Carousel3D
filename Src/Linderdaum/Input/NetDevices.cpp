#include "NetDevices.h"

#include "Core/Logger.h"

// true if last packet
bool NetDeviceClient::ProcessDeviceInfoPacket( LPacket* Packet )
{
	sInputDeviceInfo* Info = reinterpret_cast<sInputDeviceInfo*>( Packet->FData );

	FName = LString( &Info->ShortDeviceName[0] );

	NumKeys = Info->NumKeys;
	NumAxes = Info->NumAxes;

	for ( int i = Info->StartKey ; i <= Info->EndKey ; i++ )
	{
		KeyName[i] = LString( &Info->Keys[i].Name[0] );
	}

	for ( int i = Info->StartAxis ; i <= Info->EndAxis ; i++ )
	{
		AxisName[i] = LString( &Info->Axes[i].Name[0] );
	}

	return ( Info->NumPackets == Info->PacketIndex + 1 );
}

bool NetDeviceClient::ConnectTo( const LString& DevAddr, double ConnectTimeout )
{
	if ( !Disconnect() )
	{
		return false;
	}

	// now enter the 'connecting' state:
	DeviceAddress = DevAddr;

	Address.FromString( DeviceAddress );

	// a) open socket
	FSocket = new LUDPSocket;

	if ( !FSocket->Open() )
	{
		FLastErrorMsg = "Network error : " + FSocket->GetError();
		return false;
	}

	if ( !FSocket->SetNonBlocking( true ) )
	{
		FLastErrorMsg = "Network error (2) : " + FSocket->GetError();
		return false;
	}

	// b) send some CONNECT packets
	Msg.BeginWrite();
	Msg.WriteString( "CONNECT" );

	for ( int i = 0 ; i < 5 ; i++ )
	{
		FSocket->WriteTo( Address, Msg );
	}

	// c) wait for INFO packets
	double StartTime = Env->GetSeconds();

	Connected = false;

	LNetworkAddress From;

	while ( Env->GetSeconds() - StartTime < ConnectTimeout )
	{
		if ( FSocket->ReadFrom( From, InPacket ) )
		{
			if ( From.Compare( DeviceAddress ) )
			{
				InPacket.BeginRead();
//    LString S = Msg.ReadString();

//          Env->Logger->Log(L_LOG, "Trying to decode input device info packet");
				if ( ProcessDeviceInfoPacket( &InPacket ) )
				{
					// sequential packets by now
					Connected = true;

					Msg.BeginWrite();
					Msg.WriteString( "DONEINFO" );

					for ( int i = 0 ; i < 5 ; i++ )
					{
						FSocket->WriteTo( Address, Msg );
					}
				}
			}
		}
	}

	return Connected;
}

NetDeviceClient::NetDeviceClient()
{
	DeviceAddress = "127.0.0.1:12345";

	Msg.ReserveSpace( 4096 );
	InPacket.ReserveSpace( 4096 );

	FSocket = NULL;
	Connected = false;
}

bool NetDeviceClient::Disconnect()
{
	// 1. if we are connected, then disconnect
	if ( Connected )
	{
		FSocket->Close();
	}

	if ( FSocket )
	{
		delete FSocket;
		FSocket = NULL;
	}

	Connected = false;

	return true;
}

NetDeviceClient::~NetDeviceClient()
{
}

void NetDeviceClient::PollSockets()
{
	if ( Connected )
	{
		// try to read some new packet
		LNetworkAddress From;
		FSocket->ReadFrom( From, Msg );

		if ( Msg.FCurSize == sizeof( sInputDeviceState ) )
		{
//       Env->Logger->Log(L_LOG, "Trying to decode input device state");

			this->UnpackDeviceState( reinterpret_cast<sInputDeviceState*>( Msg.FData ) );
		}
	}
	else
	{
		// wait for connection ?
	}
}



NetDeviceServer::NetDeviceServer()
{
	Msg.ReserveSpace( DEVICE_PACKET_SIZE );

	DeviceInfoPacket.ReserveSpace( DEVICE_PACKET_SIZE );
	DeviceStatePacket.ReserveSpace( DEVICE_PACKET_SIZE );

	DeviceAddress = "192.168.0.35";
	DevicePort    = 4000;

	DisconnectTimeout = 5.0f;

	FServerSocket = NULL;

	Stop();
}

NetDeviceServer::~NetDeviceServer()
{
	Stop();
}

bool NetDeviceServer::Stop()
{
	if ( FServerSocket )
	{
		FServerSocket->Close();
		delete FServerSocket;
	}

	FServerSocket = NULL;

	return true;
}

bool NetDeviceServer::Start()
{
	FLastError = "";

	// открываем серверный сокет
	FServerSocket = new LUDPSocket();

	if ( !FServerSocket->Open() )
	{
		FLastError = LString( "Cannot open UDP server socket: " ) + FServerSocket->GetError();
		return false;
	}

	if ( !FServerSocket->SetNonBlocking( true ) )
	{
		FLastError = LString( "Cannot open (2) UDP server socket: " ) + FServerSocket->GetError();
		return false;
	}

	if ( !FServerSocket->Bind( DeviceAddress, DevicePort ) )
	{
		FLastError = LString( "Cannot bind server socket: " ) + FServerSocket->GetError();

		return false;
	}

	return true;
}

int NetDeviceServer::FindClientByAddress( const LNetworkAddress& Addr )
{
	for ( int i = 0 ; i < static_cast<int>( FClients.size() ) ; i++ )
	{
		if ( Addr.Compare( FClients[i].Address ) )
		{
			return i;
		}
	}

	return -1;
}

/*
void NetDeviceServer::RemoveClients(const list<int>& Numbers)
{
   list<int> NewClients;

   std::copy
}
*/

void NetDeviceServer::PollSockets()
{
	LNetworkAddress From;

	FDevice->Poll();

	// 1. save address of incomming packet
	if ( FServerSocket->ReadFrom( From, Msg ) )
	{
		Msg.BeginRead();
		LString Cmd = Msg.ReadString();

		// looking for a client with this address (-1 if none)
		int ClientIdx = FindClientByAddress( From );

		LString FromAddress = From.ToString();

		if ( Cmd == "CONNECT" )
		{
			if ( ClientIdx < 0 )
			{
				NetDeviceClientRec C;
				C.Address   = From;
				C.Connected = false; // send info packets from the beginning
				C.LastSignalTime = Env->GetEngineTime();
				C.InfoPacketNum = 0;

				FClients.push_back( C );

				// cout << "Client connected: " << FromAddress << endl;
				Env->Logger->Log( L_LOG, LString( "Client connected: " ) + FromAddress );
			}
		}

		if ( ClientIdx > -1 )
		{
			if ( Cmd == "DONEINFO" )
			{
				FClients[ClientIdx].Connected = true;
			}

			if ( Cmd == "KEEPALIVE" )
			{
				Env->Logger->Log( L_LOG, LString( "Keepalive packet from " ) + FromAddress );
				FClients[ClientIdx].LastSignalTime = Env->GetEngineTime();
			}

			if ( Cmd == "DISCONNECT" )
			{
//          FClients.erase();
			}
		}
	}

	// 2. check if someone has disconnected
	/*
	   list<int> ToDelete;

	   double ThisTime = DoGetTime();

	   for (int i = 0 ; i < static_cast<int>(FClients.size()) ; i++)
	   {
	      if (ThisTime - FClients[i].LastSignalTime > DisconnectTimeout)
	      {
	         ToDelete.push_back(i);
	      }
	   }

	   RemoveClients(ToDelete);
	*/

	DeviceStatePacket.BeginWrite();
	FDevice->PackDeviceState( reinterpret_cast<sInputDeviceState*>( DeviceStatePacket.FData ) );
	DeviceStatePacket.SkipWriteBytes( sizeof( sInputDeviceState ) );

	// 3. send packets to all registered clients
	for ( size_t i = 0 ; i < FClients.size() ; i++ )
	{
		if ( FClients[i].Connected )
		{
			// send data packet
			FServerSocket->WriteTo( FClients[i].Address, DeviceStatePacket );
		}
		else
		{
			DeviceInfoPacket.BeginWrite();
			FDevice->PackDeviceInfo( reinterpret_cast<sInputDeviceInfo*>( DeviceInfoPacket.FData ), FClients[i].InfoPacketNum );
			DeviceInfoPacket.SkipWriteBytes( sizeof( sInputDeviceInfo ) );
			FClients[i].InfoPacketNum++;

			// send info packet
			for ( int j = 0 ; j < 3 ; j++ )
			{
				FServerSocket->WriteTo( FClients[i].Address, DeviceInfoPacket );
			}
		}
	}
}

void NetDeviceServer::ProcessClients()
{
	//
}

/*
 * 07/10/2010
     Log section added
*/
