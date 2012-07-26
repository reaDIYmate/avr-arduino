/*
    HardwareSerial.h - Hardware serial library for Wiring
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

    Modified 28 September 2010 by Mark Sproul
*/
#ifndef HARDWARE_SERIAL_H
#define HARDWARE_SERIAL_H
#include <inttypes.h>
#include "Stream.h"
//------------------------------------------------------------------------------
struct ring_buffer;
//------------------------------------------------------------------------------
class HardwareSerial : public Stream {
public:
    HardwareSerial(ring_buffer *rx_buffer, volatile uint8_t *ubrrh,
        volatile uint8_t *ubrrl, volatile uint8_t *ucsra,
        volatile uint8_t *ucsrb, volatile uint8_t *udr, uint8_t rxen,
        uint8_t txen, uint8_t rxcie, uint8_t u2x);
        int available(void);
        void begin(unsigned long);
        void clear();
        void end();
        void flush(void);
        int peek(void);
        int read(void);
        size_t write(uint8_t);
        using Print::write;
//------------------------------------------------------------------------------
private:
    ring_buffer *_rx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _u2x;
};
//------------------------------------------------------------------------------
extern HardwareSerial Serial;
extern HardwareSerial Serial1;

#endif // HARDWARE_SERIAL_H
