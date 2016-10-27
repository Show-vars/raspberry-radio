#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "sx1276.h"
#include "sx1276regs-fsk.h"

#define RF_FREQUENCY   433000000 // Hz

#define FSK_FDEV                          25e3      // Hz
#define FSK_DATARATE                      50e3      // bps
#define FSK_BANDWIDTH                     50e3      // Hz
#define FSK_AFC_BANDWIDTH                 83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH               5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON         false

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                  1000
#define TX_OUTPUT_POWER                   17        // dBm
#define BUFFER_SIZE                       32 // Define the payload size here

uint8_t buffer[BUFFER_SIZE];

radio_events_t radio_events;

void OnTxDone() {
  printf("GSV OUT\n");
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  printf("GSV IN\n");
}

void main() {
  printf("Cuddly radio node v0.3\n\n");
  //spi_init();

  printf("Init radio0: ");

  radio_events.TxDone = OnTxDone;
  radio_events.RxDone = OnRxDone;
  //radio_events.TxTimeout = OnTxTimeout;
  //radio_events.RxTimeout = OnRxTimeout;
  //radio_events.RxError = OnRxError;

  sx1276_init(&radio_events);
  sx1276_set_channel(RF_FREQUENCY);

/*
  sx1276_set_txconfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                                  FSK_DATARATE, 0,
                                  FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, 0, 3000);

  sx1276_set_rxconfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                                  0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                                  0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                                  0, 0, false, true);
*/
  sx1276_set_txconfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                  true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

  sx1276_set_rxconfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                  0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

  printf("done\n");


  buffer[0] = 'G';
  buffer[1] = 'S';
  buffer[2] = 'V';
  buffer[3] = '0';
  buffer[4] = '0';
  buffer[5] = '0';
  buffer[6] = '0';
  buffer[7] = '0';

  int counter = 0;
  while(1) {
    printf("SEND %d ", counter);
    sprintf(buffer + 3, "%05d", counter++);

    sx1276_send(buffer, 8);

    usleep(1000 * 1000 * 10);
  }

}
