#if defined(TARGET_PORTENTA_H7_M7)
#include <Arduino.h>
#include <micro_ros_arduino.h>

#include "EthernetInterface.h"

//I am following the wifi example from  https://github.com/arduino-libraries/Arduino_MachineControl
//Also following a wifi_transport example.



extern "C" bool MachineControl_eth_transport_open(struct uxrCustomTransport * transport);
extern "C" bool MachineControl_eth_transport_close(struct uxrCustomTransport * transport);
extern "C" size_t MachineControl_eth_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
extern "C" size_t MachineControl_eth_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

/* // old way of passing ip and port. Will now use SocketAddress
struct micro_ros_agent_locator {
	IPAddress address;
	int port;
};*/

struct agent_locator {
  EthernetInterface net;
  SocketAddress addr;
};

//not sure what this if this is needed

static inline void set_MachineControl_eth_transports(char * agent_ip, uint agent_port){
  


	//WiFi.begin(ssid, pass);
  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
	static struct micro_ros_agent_locator locator;*/

  static struct agent_locator locator;
  locator.addr.set_ip_address(agent_ip);
	locator.addr.set_port(agent_port);

	rmw_uros_set_custom_transport(
		false,
		(void *) &locator,
		MachineControl_eth_transport_open,
    MachineControl_eth_transport_close,
    MachineControl_eth_transport_write,
    MachineControl_eth_transport_read
	);
}


extern "C"
{

  static WiFiUDP udp_client;

  UDPSocket socket;
  

  bool MachineControl_eth_transport_open(struct uxrCustomTransport * transport)
  {
    ///struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;
    //udp_client.begin(locator->port);

    agent_locator locator = locator;
    locator.net.connect(); 
    socket.open(&locator.net);
    socket.connect(locator.addr);

    return true;
  }

  bool MachineControl_eth_transport_close(struct uxrCustomTransport * transport)
  {
    agent_locator locator = locator;
    //udp_client.stop();
    socket.close();
    locator.net.disconnect(); //Not sure if this is needed here
    return true;
  }

  size_t MachineControl_eth_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
  {
    (void)errcode;
    struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;

    /*
    udp_client.beginPacket(locator->address, locator->port);
    size_t sent = udp_client.write(buf, len);
    udp_client.endPacket();
    udp_client.flush();*/


    //Unsure if I need to do the begin and end and flush stuff. The example does not do this.
    auto sent = socket.send(buf, len);
    return sent;
  }

  size_t Machine_Control_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
  {
    (void) errcode;

    uint32_t start_time = millis();

    /*
    while(millis() - start_time < timeout && udp_client.parsePacket() == 0){
      delay(1);
    }
    size_t readed  = udp_client.read(buf, len);
    */
   
    // Receive a simple HTTP response
    const size_t rlen { 64 };
    char rbuffer[rlen + 1] {};
    size_t rcount;
    size_t rec { 0 };
    String response;

    while (millis() - start_time < timeout && (rec = socket.recv(rbuffer, rlen)) > 0) {
        rcount += rec;
        response += rbuffer;
        memset(rbuffer, 0, rlen);
    }

    return (rcount <0) ? 0 : rcount;
    //return (readed < 0) ? 0 : readed;
  }
}

#endif

