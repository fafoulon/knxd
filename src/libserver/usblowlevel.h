/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2011 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef EIB_USB_H
#define EIB_USB_H

#include <libusb.h>
#include "lowlevel.h"
#include "usb.h"

typedef struct
{
  int bus;
  int device;
  int config;
  int altsetting;
  int interface;
} USBEndpoint;

typedef struct
{
  libusb_device *dev;
  int config;
  int altsetting;
  int interface;
  int sendep;
  int recvep;
} USBDevice;

USBEndpoint parseUSBEndpoint (const char *addr);
USBDevice detectUSBEndpoint (USBEndpoint e);

LOWLEVEL(USBLowLevelDriver, usb)
{
public:
  USBLowLevelDriver (const DriverPtr& p, IniSection& s);
  virtual ~USBLowLevelDriver ();
private:
  libusb_device_handle *dev;
  /* libusb event loop */
  USBLoop *loop;
  USBDevice d;
  bool running = false;

  ev::async trigger;
  void trigger_cb(ev::async &w, int revents);
  /** transmit queue */
  Queue < CArray > send_q;
  int state;
  bool connection_state = false;
  uint8_t sendbuf[64];
  uint8_t recvbuf[64];
  bool startUsbRecvTransferFailed = false;

  struct libusb_transfer *sendh = 0;
  struct libusb_transfer *recvh = 0;

  void StartUsbRecvTransfer();
  void FinishUsbRecvTransfer();
  void ReceiveUsb();

public:
  bool setup(DriverPtr master);
  void start();
  void stop();
  void CompleteReceive(struct libusb_transfer *recvh);
  void CompleteSend(struct libusb_transfer *recvh);

  void Send_Packet (CArray l);
  void SendReset ();
  EMIVer getEMIVer ();
};

#endif
