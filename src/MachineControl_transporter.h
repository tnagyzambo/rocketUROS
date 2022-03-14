#include <micro_ros_arduino.h>
#include "EthernetInterface.h"

struct agent_locator {
  EthernetInterface net;
  SocketAddress addr;
};

extern "C"
{

  UDPSocket socket;
  

  bool MachineControl_eth_transport_open(struct uxrCustomTransport * transport)
  {
    ///struct micro_ros_agent_locator * locator = (struct micro_ros_agent_locator *) transport->args;
    //udp_client.begin(locator->port);

    struct agent_locator * locator = (struct agent_locator *) transport->args;
    locator->net.connect();
    socket.open(&locator->net);
    socket.connect(locator->addr);

    return true;
  }

  bool MachineControl_eth_transport_close(struct uxrCustomTransport * transport)
  {
    struct agent_locator * locator = (struct agent_locator *) transport->args;
    socket.close();
    locator->net.disconnect();
    return true;
  }

  size_t MachineControl_eth_transport_write(struct uxrCustomTransport * transport, const uint8_t *buf, size_t len, uint8_t *errcode)
  {
    (void)errcode;
    (void)transport;
    //struct agent_locator * locator = (struct agent_locator *) transport->args;

    //Unsure if I need to do the begin and end and flush stuff. The example does not do this.
    auto sent = socket.send(buf, len);
    return sent;
  }

  size_t MachineControl_eth_transport_read(struct uxrCustomTransport * transport, uint8_t *buf, size_t len, int timeout, uint8_t *errcode)
  {
    (void) errcode;

    uint32_t start_time = millis();
   
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
  }
}