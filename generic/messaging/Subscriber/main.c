/*
 * Copyright (C) 2025 Atym Incorporated. All rights reserved.
 */
#include <ocre_api.h>
#include <stdio.h>
#include <string.h>

#define TOPIC "test/topic"

void message_handler(const char *topic, const char *content_type, const void *payload, uint32_t payload_len);

// WASM entry point
int main(void)
{
  if (ocre_register_message_callback(TOPIC, message_handler) != OCRE_SUCCESS)
  {
    printf("Failed to register message callback for %s\n", TOPIC);
  }
  printf("Subscriber initialized: listening on %s\n", TOPIC); // YES
  while (1)
  {
    ocre_process_events();
    ocre_sleep(300);
  }
  return 0;
}

void message_handler(const char *topic, const char *content_type, const void *payload, uint32_t payload_len)
{
  if (topic && content_type && payload)
  {
    printf("Received message: topic=%s, content_type=%s, payload=%s, len=%u\n",
           topic, content_type, (const char *)payload, payload_len);
  }
  else
  {
    printf("Invalid message data received\n");
  }
}