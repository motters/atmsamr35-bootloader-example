/*
 * Contains register definitions for the SX1276 LoRa radio modem
 */

#ifndef SX1276_REGISTERS_H_
#define SX1276_REGISTERS_H_

#include <conf_board.h>
/**
 * The below should be defined by ASF framework, which they are
 * However for some reason the compiler can not fine them
 * Problem to solve
 */
#define PIN_PC19F_SERCOM4_PAD0    83L
#define MUX_PC19F_SERCOM4_PAD0    5L
#define PINMUX_PC19F_SERCOM4_PAD0 ((PIN_PC19F_SERCOM4_PAD0 << 16) | MUX_PC19F_SERCOM4_PAD0)

#define PIN_PB30F_SERCOM4_PAD2    62L
#define MUX_PB30F_SERCOM4_PAD2    5L
#define PINMUX_PB30F_SERCOM4_PAD2 ((PIN_PB30F_SERCOM4_PAD2 << 16) | MUX_PB30F_SERCOM4_PAD2)

#define PIN_PC18F_SERCOM4_PAD3    82L
#define MUX_PC18F_SERCOM4_PAD3    5L
#define PINMUX_PC18F_SERCOM4_PAD3 ((PIN_PC18F_SERCOM4_PAD3 << 16) | MUX_PC18F_SERCOM4_PAD3)

// SX1276 LoRa modem internal SPI info
#define SX_RF_SPI                  SERCOM4
#define SX_RF_RESET_PIN            PIN_PB15
#define SX_RF_SPI_CS               PIN_PB31
#define SX_RF_SPI_MOSI             PIN_PB30
#define SX_RF_SPI_MISO             PIN_PC19
#define SX_RF_SPI_SCK              PIN_PC18
#define SX_RF_SPI_SERCOM_MUX_SETTING   SPI_SIGNAL_MUX_SETTING_E
#define SX_RF_SPI_SERCOM_PINMUX_PAD0   PINMUX_PC19F_SERCOM4_PAD0
#define SX_RF_SPI_SERCOM_PINMUX_PAD1   PINMUX_UNUSED
#define SX_RF_SPI_SERCOM_PINMUX_PAD2   PINMUX_PB30F_SERCOM4_PAD2
#define SX_RF_SPI_SERCOM_PINMUX_PAD3   PINMUX_PC18F_SERCOM4_PAD3
#define SX_RF_RESET_HIGH		   true
#define SX_RF_RESET_LOW		       !SX_RF_RESET_HIGH
/* TODO - Change the SPI baudrate according to MCU clock frequency */
#define SX_RF_SPI_BAUDRATE 2000000



#define SX1276_FIFO                         0x00
#define SX1276_OP_MODE                      0x01

#define SX1276_FREQUENCY_HIGH               0x06
#define SX1276_FREQUENCY_MID                0x07
#define SX1276_FREQUENCY_LOW                0x08

#define SX1276_POWER_AMPLIFIER_CONFIG       0x09
#define SX1276_HIGH_POWER_SETTINGS          0x4D

#define SX1276_FIFO_ADDR_PTR                0x0D
#define SX1276_FIFO_TX_BASE_ADDR            0x0E
#define SX1276_FIFO_RX_BASE_ADDR            0x0F
#define SX1276_FIFO_RX_CURRENT_ADDR         0x10

#define SX1276_INTERRUPT_FLAGS_MASK         0x11
#define SX1276_INTERRUPT_FLAGS              0x12

#define SX1276_NUMBER_OF_BYTES_RECEIVED     0x13

#define SX1276_LAST_PACKET_SNR              0x19
#define SX1276_LAST_PACKET_RSSI             0x1A

#define SX1276_MODEM_CONFIG_1               0x1D
#define SX1276_MODEM_CONFIG_2               0x1E
#define SX1276_MODEM_CONFIG_3               0x26

#define SX1276_PREAMBLE_HIGH                0x20
#define SX1276_PREAMBLE_LOW                 0x21

#define SX1276_PAYLOAD_LENGTH               0x22

#define SX1276_HOP_PERIOD                   0x24

#define SX1276_DIO_MAPPING1                 0x40

#endif /* SX1276_REGISTERS_H_ */