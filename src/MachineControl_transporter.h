#include <micro_ros_arduino.h>
//#include "EthernetInterface.h"
//#include <EthernetUdp.h>

#include <SPI.h>
#include <Ethernet.h>



// struct micro_ros_agent_locator {
// 	IPAddress address;
// 	int port;
// };

extern "C"
{
  EthernetUDP eth_UDP;
  //UDPSocket socket;
  //EthernetClient client;

  bool MachineControl_eth_transport_open(struct uxrCustomTransport * transport)
  {
    ///struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;
    //udp_client.begin(locator->port);

    struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;


    eth_UDP.begin(locator->port);

    return true;
  }

  bool MachineControl_eth_transport_close(struct uxrCustomTransport * transport)
  {
    struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;
    eth_UDP.stop();
    return true;
  }

  size_t MachineControl_eth_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
  {
    (void)errcode;
    //(void)transport;
    struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;

    eth_UDP.beginPacket(locator->address, locator->port);
    size_t sent = eth_UDP.write(buf, len);
    eth_UDP.endPacket();
    eth_UDP.flush();

    return sent;
  }

  size_t MachineControl_eth_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
  {
    (void) errcode;

    uint32_t start_time = millis();
  

    while (millis() - start_time < timeout && eth_UDP.parsePacket() == 0) {
      delay(1);//delay until all data is in?
    }
    size_t readed = eth_UDP.read(buf, len);
    return (readed < 0) ? 0 : readed;
  }
}