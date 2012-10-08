#ifndef __NetDevice__h__included__
#define __NetDevice__h__included__

#include "Platform.h"
#include "Network/Network.h"
#include "Input/Input.h"
#include "Environment.h"

/**
   Information about the device client
*/
struct NetDeviceClientRec
{
public:
	/// time the last signal from client was received
	double LastSignalTime;

	/// port and IP-adress of the client
	LNetworkAddress Address;

	/// connected or not - if not, send info packets
	bool Connected;

	int InfoPacketNum;

	NetDeviceClientRec()
	{
		LastSignalTime = 0.0;

		Connected = false;
	}
};

#define DEVICE_PACKET_SIZE (4096)

/**
   Usage:
      NetDeviceServer* Server = Env->Input->CreateNetworkDeviceServer();

      Server->DeviceAddress = "device_ip";
      Server->DevicePort    = 1234;

      Server->SetDevice( Env->Input->... );

      Server->Start();

      ...

      delete Server; // or Server->Stop()
*/

class netexportable scriptfinal NetDeviceServer : public iObject
{
public:
	LSocket* FServerSocket;

	std::vector<NetDeviceClientRec> FClients;

	virtual void ProcessClients();

	double DisconnectTimeout;

	iInputDevice* FDevice;
public:
	LPacket Msg;

	LPacket DeviceStatePacket;

	LPacket DeviceInfoPacket;

	int DevicePort;
	LString DeviceAddress;
public:
	NetDeviceServer();
	virtual ~NetDeviceServer();

	virtual void SetDevice( iInputDevice* SourceDevice ) { FDevice = SourceDevice; }

	virtual bool Stop();
	virtual bool Start();

	virtual void PollSockets();

	LString FLastError;
private:
	int FindClientByAddress( const LNetworkAddress& Addr );
};

/**

// to iInputDeviceList.h

   virtual NetInputDevice*    CreateNetworkInputDevice();
   virtual NetDeviceServer*   CreateNetworkDeviceServer();

// to iInputDeviceList.cpp

NetInputDevice* iInputDeviceList::CreateNetworkInputDevice()
{
   return Env->Linker->Instantiate<NetInputDevice>("NetInputDevice");
}

NetDeviceServer* iInputDeviceList::CreateNetworkDeviceServer()
{
   return Env->Linker->Instantiate<NetDeviceServer>("NetDeviceServer");
}

   Usage :
      NetInputDevice* Device = Env->Input->CreateNetworkInputDevice();

      Device->ConnectTo(DevAddr, DevPort, <Timeout>);

      ...

      Device->Disconnect(); // or delete Device;
*/
/*
class NetDeviceClient;

class NetDeviceConnectionThread : public iThread
{
public:
   NetDeviceConnectionThread(NetDeviceClient* AClient, double ATimeout)
   {
      Connecting = false;

      Timeout = ATimeout;
      Client = AClient;
   }

   // connection cycle
   virtual void Run()
   {
      while(true)
      {
      }
   }

   NetDeviceClient* Client;
   double Timeout;
public:
   // are we already connected or not
   bool Connecting;
}
*/

class netexportable scriptfinal NetDeviceClient : public iInputDevice
{
public:
	NetDeviceClient();

	virtual bool ConnectTo( const LString& DevAddr, double ConnectTimeout );

	virtual bool Disconnect();

	virtual bool ProcessDeviceInfoPacket( LPacket* Packet );

	virtual void PollSockets();

	virtual ~NetDeviceClient();

	virtual int       GetNumAxes() const { return NumAxes; }

	virtual bool      SetAxisRange( int AxisIdx, float MinVal, float MaxVal ) { return false; }
	virtual bool      SetAxisDeadZone( int AxisIdx, float DeadZone ) { return false; }
	virtual bool      SetAxisSaturation( int AxisIdx, float Saturation ) { return false; }
	virtual float     GetAxisValue( int AxisIdx ) const { return Axis[AxisIdx]; }
	virtual LString   GetAxisName( int AxisIdx ) const { return AxisName[AxisIdx]; }
	virtual void      SetAxisValue( int AxisIdx, float Val ) { Axis[AxisIdx] = Val; }

	virtual LString   GetKeyName( int KeyIdx ) const { return KeyName[KeyIdx]; }

	virtual bool      IsPressed( int KeyIdx ) const { return Keys[KeyIdx]; }
	virtual int       GetNumKeys() const { return NumKeys; }
	virtual void      SetKeyState( int KeyIdx, bool Pressed ) { Keys[KeyIdx] = Pressed; }

	virtual bool      Start() { return true; }
	virtual bool      IsStarted() const { return Connected; }
	virtual bool      Stop() { return Disconnect(); }

	virtual void      RunSystemSetup() {}
	virtual bool      SystemSetupAvailable() const { return false; }
	virtual LString   GetName() const { return FName; }
	virtual bool      Poll() { PollSockets(); return true; }

private:
	LPacket Msg;
	LPacket InPacket;

	// device address
	LString DeviceAddress;

	LNetworkAddress Address;

	// device port
	int DevicePort;

	// connection state
	bool Connected;

	// data socket
	LUDPSocket* FSocket;
private:
	LString FName;

	int NumAxes;
	int NumKeys;

	// internal state (keys, axes, names, limits)
	bool  Keys[MAX_KEYS_IN_DEVICE];
	float Axis[MAX_AXES_IN_DEVICE];

	LString KeyName[MAX_KEYS_IN_DEVICE];
	LString AxisName[MAX_AXES_IN_DEVICE];
};


#endif

/*
 * 07/10/2010
     Log section added
*/
