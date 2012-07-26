/*
    HardwareSerial.cpp - Hardware serial library for Wiring
    Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Modified 23 November 2006 by David A. Mellis
    Modified 28 September 2010 by Mark Sproul
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "wiring_private.h"
//------------------------------------------------------------------------------
uint8_t const SERIAL_BUFFER_SIZE = 128;
//------------------------------------------------------------------------------
struct ring_buffer
{
    uint8_t buffer[SERIAL_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
};
//------------------------------------------------------------------------------
static ring_buffer rx_buffer0 = {{0}, 0, 0};
static ring_buffer rx_buffer1 = {{0}, 0, 0};
//------------------------------------------------------------------------------
static inline __attribute__((always_inline)) void store_char(uint8_t c,
    ring_buffer *buffer) {
    uint8_t i = (buffer->head + 1) % SERIAL_BUFFER_SIZE;

    if (i != buffer->tail) {
        buffer->buffer[buffer->head] = c;
        buffer->head = i;
    }
}
//------------------------------------------------------------------------------
ISR(USART0_RX_vect) {
    uint8_t c  =  UDR0;
    store_char(c, &rx_buffer0);
}
ISR(USART1_RX_vect) {
    uint8_t c = UDR1;
    store_char(c, &rx_buffer1);
}
//------------------------------------------------------------------------------
HardwareSerial::HardwareSerial(ring_buffer *rx_buffer0, volatile uint8_t *ubrrh,
    volatile uint8_t *ubrrl, volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
    volatile uint8_t *udr, uint8_t rxen, uint8_t txen, uint8_t rxcie,
    uint8_t u2x) {
    _rx_buffer = rx_buffer0;
    _ubrrh = ubrrh;
    _ubrrl = ubrrl;
    _ucsra = ucsra;
    _ucsrb = ucsrb;
    _udr = udr;
    _rxen = rxen;
    _txen = txen;
    _rxcie = rxcie;
    _u2x = u2x;
}
//------------------------------------------------------------------------------
void HardwareSerial::begin(unsigned long baud) {
    uint16_t baud_setting;
    bool use_u2x = true;

// hardcoded exception for compatibility with the bootloader shipped
// with the Duemilanove and previous boards and the firmware on the 8U2
// on the Uno and Mega 2560.
if (baud == 57600) {
    use_u2x = false;
}

try_again:

    if (use_u2x) {
        *_ucsra = 1 << _u2x;
        baud_setting = (F_CPU / 4 / baud - 1) / 2;
    }
    else {
        *_ucsra = 0;
        baud_setting = (F_CPU / 8 / baud - 1) / 2;
    }
    if ((baud_setting > 4095) && use_u2x) {
        use_u2x = false;
        goto try_again;
    }

    // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
    *_ubrrh = baud_setting >> 8;
    *_ubrrl = baud_setting;

    sbi(*_ucsrb, _rxen);
    sbi(*_ucsrb, _txen);
    sbi(*_ucsrb, _rxcie);
}
//------------------------------------------------------------------------------
int HardwareSerial::available() {
    return (uint8_t)(SERIAL_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) %
        SERIAL_BUFFER_SIZE;
}
//------------------------------------------------------------------------------
void HardwareSerial::clear() {
    _rx_buffer->head = _rx_buffer->tail;
}
//------------------------------------------------------------------------------
void HardwareSerial::end() {
    flush();

    cbi(*_ucsrb, _rxen);
    cbi(*_ucsrb, _txen);
    cbi(*_ucsrb, _rxcie);

    // clear any received data
    _rx_buffer->head = _rx_buffer->tail;
}
//------------------------------------------------------------------------------
void HardwareSerial::flush() {}
//------------------------------------------------------------------------------
int HardwareSerial::peek() {
    if (_rx_buffer->head == _rx_buffer->tail)
        return -1;

    return _rx_buffer->buffer[_rx_buffer->tail];
}
//------------------------------------------------------------------------------
int HardwareSerial::read() {
    if (_rx_buffer->head == _rx_buffer->tail)
        return -1;

    uint8_t c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (uint8_t)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
}
//------------------------------------------------------------------------------
size_t HardwareSerial::write(uint8_t ch) {
    while (!(*_ucsra & (1 << UDRE1)));
    *_udr = ch;
    return 1;
}
//------------------------------------------------------------------------------
HardwareSerial Serial(&rx_buffer0, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UDR0,
    RXEN0, TXEN0, RXCIE0, U2X0);
HardwareSerial Serial1(&rx_buffer1, &UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UDR1,
    RXEN1, TXEN1, RXCIE1, U2X1);
