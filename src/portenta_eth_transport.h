#pragma once
#if defined(TARGET_PORTENTA_H7_M7)
#include <Arduino.h>
#include <micro_ros_arduino.h>

#include "EthernetInterface.h"

#include "MachineControl_transporter.h"

//I am following the wifi example from  https://github.com/arduino-libraries/Arduino_MachineControl
//Also following a wifi_transport example.



extern "C" bool MachineControl_eth_transport_open(struct uxrCustomTransport * transport);
extern "C" bool MachineControl_eth_transport_close(struct uxrCustomTransport * transport);
extern "C" size_t MachineControl_eth_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
extern "C" size_t MachineControl_eth_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

// struct micro_ros_agent_locator {
// 	IPAddress address;
// 	int port;
// };


static inline void set_MachineControl_eth_transports(char * agent_ip, uint agent_port){

	static struct micro_ros_agent_locator locator;
	locator.address.fromString(agent_ip);
	locator.port = agent_port;

	rmw_uros_set_custom_transport(
		false,
		(void *) &locator,
		MachineControl_eth_transport_open,
    MachineControl_eth_transport_close,
    MachineControl_eth_transport_write,
    MachineControl_eth_transport_read
	);
}

#endif

