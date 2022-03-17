#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <portenta_eth_transport.h>
#include <std_msgs/msg/int32.h>

#include <SPI.h>
#include <Ethernet.h>

EthernetClient client;

rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define LED_PIN 13
#define AGENT_IP "10.42.0.1"
#define AGENT_PORT 8888

#define RCCHECK(fn)              \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }
#define RCSOFTCHECK(fn)          \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
    }                            \
  }

void error_loop()
{
  while (1)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
  }
}

byte ip[] = { 10, 42, 0, 2 }; //porenta IP
byte server[] = { 64, 233, 187, 99 }; // Google
const char* mystring = "localhost";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
void setup(){

  Ethernet.begin(mac, ip);
  Serial.begin(9600);

  delay(2000);

  Serial.println("connecting...");



  // if (client.connect(AGENT_IP, 8080)) {
  //   Serial.println("connected");
  //   //client.println("GET /search?q=arduino HTTP/1.0");
  //   client.println();
  // } else {
  //   Serial.println("connection failed");
  // }

  set_MachineControl_eth_transports(AGENT_IP, AGENT_PORT);
  
  Serial.println("Passed set up");
  //set_microros_transports();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  delay(2000);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
      &publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "micro_ros_arduino_node_publisher"));

  // create timer,
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(timer_timeout),
      timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  msg.data = 0;
}

void loop()
{
  delay(100);
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));

  // if (client.available()) {
  //   char c = client.read();
  //   Serial.print(c);
  // }

  // if (!client.connected()) {
  //   Serial.println();
  //   Serial.println("disconnecting.");
  //   client.stop();
  //   for(;;)
  //     ;
  // }
}