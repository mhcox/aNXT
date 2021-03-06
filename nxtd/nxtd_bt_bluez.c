/*
    nxtd_bt_bluez.c
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "nxtd.h"
#include "nxtd_bt_bluez.h"

#ifdef NXT_BT_IDLE_TIMEOUT
  #define nxtd_bt_timeout(n) (n)->nxt.conn_timeout = time(NULL)+NXT_USB_IDLE_TIMEOUT;
#else
  #define nxtd_bt_timeout(n) (n)->nxt.conn_timeout = 0;
#endif

static struct {
  int dev_id;
  int sock;
} bt_adapter;

/**
 * Identifies a NXT over Bluetooth
 *  @param class Device class
 *  @return Whether device is a NXT
 */
static int nxt_bt_identify(uint8_t class[3]) {
  return (class[0]==NXT_BT_DEVCLASS_BYTE0) && 
         (class[1]==NXT_BT_DEVCLASS_BYTE1) &&
         (class[2]==NXT_BT_DEVCLASS_BYTE2);
}

/**
 * Initializes Bluetooth
 *  @return Success
 */
int nxtd_bt_init() {
  if ((bt_adapter.dev_id = hci_get_route(NULL))<0) return -1;
  if ((bt_adapter.sock = hci_open_dev(bt_adapter.dev_id))<0) return -1;
  return 0;
}

/**
 * Shuts down Bluetooth
 */
void nxtd_bt_shutdown() {
  close(bt_adapter.sock);
}

/**
 * Closes a NXT over BT
 *  @param nxt NXT handle
 */
void nxtd_bt_close(struct nxtd_nxt_bt *nxt) {
  nxtd_bt_disconnect(nxt);
  free(nxt->nxt.name);
  free(nxt);
}

/**
 * Finds NXT with BT address
 *  @param bdaddr BT address
 *  @return NXT
 */
struct nxtd_nxt_bt *nxtd_bt_finddev(bdaddr_t *bdaddr) {
  struct nxtd_nxt_bt *nxt;
  size_t i;

  pthread_mutex_lock(&nxts.mutex);
  for (i=0;i<NXTD_MAXNUM;i++) {
    if (nxts.list[i]!=NULL && nxts.list[i]->conn_type==NXTD_BT) {
      nxt = (struct nxtd_nxt_bt*)nxts.list[i];
      if (bacmp(&(nxt->bt_addr),bdaddr)==0) {
        pthread_mutex_unlock(&nxts.mutex);
        return nxt;
      }
    }
  }
  pthread_mutex_unlock(&nxts.mutex);

  return NULL;
}

/**
 * Scans for NXTs on BT
 *  @return Success?
 */
int nxtd_bt_scan() {
  inquiry_info *ii;
  char bt_name[248]= { 0 };
  int dev_id,sock,max_rsp,num_rsp,i;
  bdaddr_t bt_addr;
  struct nxtd_nxt_bt *nxt;

  bacpy(&bt_addr,BDADDR_ANY);

  max_rsp = 255;
  ii = malloc(max_rsp*sizeof(inquiry_info));
  num_rsp = hci_inquiry(bt_adapter.dev_id,8,max_rsp,NULL,&ii,0); // IREQ_CACHE_FLUSH

  for (i=0;i<num_rsp;i++) {
    memset(bt_name,0,sizeof(bt_name));
    if (nxtd_bt_finddev(&(ii+i)->bdaddr)==NULL) {
      if (hci_read_remote_name(bt_adapter.sock,&(ii+i)->bdaddr,sizeof(bt_name),bt_name,NXT_BT_WAIT_TIMEOUT)!=-1) {
        if (nxt_bt_identify((ii+i)->dev_class)) {
          nxt = malloc(sizeof(struct nxtd_nxt_bt));
          memset(nxt,0,sizeof(struct nxtd_nxt_bt));
          nxt->nxt.name = strdup(bt_name);

          // don't know why BT address is reversed
          // memcpy(&nxt->nxt.id, &(ii+i)->bdaddr, sizeof(nxtd_id_t));
          nxt->nxt.id[0] = ii[i].bdaddr.b[5];
          nxt->nxt.id[1] = ii[i].bdaddr.b[4];
          nxt->nxt.id[2] = ii[i].bdaddr.b[3];
          nxt->nxt.id[3] = ii[i].bdaddr.b[2];
          nxt->nxt.id[4] = ii[i].bdaddr.b[1];
          nxt->nxt.id[5] = ii[i].bdaddr.b[0];

          nxt->nxt.conn_type = NXTD_BT;
          bacpy(&(nxt->bt_addr),&(ii+i)->bdaddr);
          nxtd_nxt_reg((struct nxtd_nxt*)nxt);
        }
      }
    }
    else {
      free(ii);
      return -1;
    }
  }

  free(ii);
  return 0;
}

/**
 * Connect to NXT over Bluetooth
 *  @param nxt NXT to connect to
 *  @return Success?
 */
int nxtd_bt_connect(struct nxtd_nxt_bt *nxt) {
  struct sockaddr_rc addr;
  int sock, flags;

  if (!nxt->connected) {
    sock = socket(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);
    memset(&addr, 0, sizeof(addr));
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = 1;
    bacpy(&addr.rc_bdaddr,&(nxt->bt_addr));
   
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))==0) {
      flags = fcntl(sock,F_GETFL,0);
      if (flags==-1) {
        flags = 0;
      }
      fcntl(sock,F_SETFL,flags|O_NONBLOCK);

      nxt->bt_sock = sock;
      nxt->connected = 1;
      nxtd_bt_timeout(nxt);
      return 0;
    }
    else {
      return -1;
    }
  }
  else {
    return 0;
  }
}

/**
 * Disconnect from NXT over Bluetooth
 *  @param nxt NXT to disconnect from
 *  @return Success?
 */
int nxtd_bt_disconnect(struct nxtd_nxt_bt *nxt) {
  if (nxt->connected) {
    close(nxt->bt_sock);
    nxt->connected = 0;
    nxt->nxt.conn_timeout = 0;
  }
  return 0;
}

static uint64_t useconds(void) {
  struct timeval current;
  gettimeofday(&current,NULL);
  return current.tv_sec*1000000+current.tv_usec;
}

static size_t write_timeout(int sock,const void *data,size_t size) {
  uint64_t timeout = useconds()+NXT_BT_WAIT_TIMEOUT*1000;
  size_t i = 0;
  ssize_t c;

  while (i<size && useconds()<timeout) {
    c = write(sock,data+i,size-i);
    if (c>0) {
      i += c;
    }
    usleep(1000);
  }

  return i;
}

static size_t read_timeout(int sock,void *data,size_t size) {
  uint64_t timeout = useconds()+NXT_BT_WAIT_TIMEOUT*1000;
  size_t i = 0;
  ssize_t c;

  while (i<size && useconds()<timeout) {
    c = read(sock,data+i,size-i);
    if (c>0) {
      i += c;
    }
    usleep(1000);
  }

  return i;
}

/**
 * Sends data over BT
 *  @param nxt NXT handle
 *  @param data Data to send
 *  @param size How many bytes to send
 *  @return How many bytes sent
 */
ssize_t nxtd_bt_send(struct nxtd_nxt_bt *nxt,const void *data,size_t size) {
  uint16_t sz = size;

  if (nxtd_bt_connect(nxt)==-1) {
    return -1;
  }

  nxtd_bt_timeout(nxt);
  if (write_timeout(nxt->bt_sock, &sz, 2)!=2) {
    return -1;
  }

  return write_timeout(nxt->bt_sock, data, size);
}

/**
 * Receives data over BT
 *  @param nxt NXT handle
 *  @param data Buffer for data
 *  @param size How many bytes to receive
 *  @return How many bytes received
 */
ssize_t nxtd_bt_recv(struct nxtd_nxt_bt *nxt,void *data,size_t size) {
  uint16_t sz;

  if (nxtd_bt_connect(nxt)==-1)  {
    return 0;
  }

  nxtd_bt_timeout(nxt);

  if (read_timeout(nxt->bt_sock, &sz, 2)!=2) {
    return 0;
  }

  return read_timeout(nxt->bt_sock, data, sz);
}

