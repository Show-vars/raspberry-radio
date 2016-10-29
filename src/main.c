#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "sx1276.h"
#include "sx1276regs-fsk.h"

#define RF_FREQUENCY   434000000 // Hz

#define FSK_FDEV                          25e3      // Hz
#define FSK_DATARATE                      50e3      // bps
#define FSK_BANDWIDTH                     50e3      // Hz
#define FSK_AFC_BANDWIDTH                 83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH               5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON         false

#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       12        // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         3         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                  1000
#define TX_OUTPUT_POWER                   14        // dBm
#define BUFFER_SIZE                       32 // Define the payload size here

uint8_t buffer[BUFFER_SIZE];

radio_events_t radio_events;

int ackCounter = 0;

void SendAck() {
  buffer[0] = 'P';
  buffer[1] = 'O';
  buffer[2] = 'N';
  buffer[3] = 'G';
  buffer[4] = '0';
  buffer[5] = '0';
  buffer[6] = '0';
  buffer[7] = '0';

  printf("^SEND ACK%d\n", ackCounter);
  sprintf(buffer + 4, "%04d", ackCounter++);

  sx1276_send(buffer, 2);
}

void OnTxDone() {
  printf("$TXS\n");
  //usleep(1000);
  sx1276_set_rx(0);
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  printf("$RXS,%x,%x,%x,\n", rssi, snr, size);
  //usleep(1000);
  SendAck();
}

void OnRxError() {
  printf("$RXE\n");
}

void main() {
  radio_events.TxDone = OnTxDone;
  radio_events.RxDone = OnRxDone;
  //radio_events.TxTimeout = OnTxTimeout;
  //radio_events.RxTimeout = OnRxTimeout;
  radio_events.RxError = OnRxError;

  sx1276_init(&radio_events);
  sx1276_set_channel(RF_FREQUENCY);

  sx1276_set_txconfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

  sx1276_set_rxconfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                  0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  printf("$IND,%X,%X,%X,%X,%X,%X\n", sx1276_read(REG_VERSION), RF_FREQUENCY, TX_OUTPUT_POWER, LORA_BANDWIDTH, LORA_SPREADING_FACTOR, LORA_CODINGRATE);
  printf("^Time on air for 16 bytes: %d us\n", sx1276_get_timeonair(MODEM_LORA, 64));

  memset(buffer, 0, BUFFER_SIZE);

  sx1276_set_rx(0);

  while(1) {
    usleep(1000 * 1000 * 1);
  }

}
