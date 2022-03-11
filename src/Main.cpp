#include <Arduino.h>
// #include <LwIP.h> // lightweight TCP/IP protocol suite
// #include <STM32Ethernet.h>
// #include <STM32FreeRTOS.h>
#include <micro_ros_arduino.h>
#include <micro_ros_utilities/string_utilities.h>
#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/int32.h>

// #include <EthernetClient.h>
// #include <Ethernet.h>
// #include <EthernetInterface.h> //This doesn't include the required tools

#define CLIENT_IP "192.168.1.177"
#define AGENT_IP "192.168.1.176"
#define AGENT_PORT 8888
#define NODE_NAME "node"

#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

IPAddress client_ip;
IPAddress agent_ip;
byte mac[] = {0x02, 0x47, 0x00, 0x00, 0x00, 0x01};

void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  client_ip.fromString(CLIENT_IP);
  agent_ip.fromString(AGENT_IP);

  Serial.print("Connecting to agent: ");
  Serial.println(agent_ip);

  set_microros_wifi_transports(mac, client_ip, agent_ip,
                                              AGENT_PORT);

  delay(2000);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_best_effort(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "topic_name"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  msg.data = 0; 
}

void loop() {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
}