/*
    include/anxt/nxt.h
    aNXT - a NXt Toolkit
    Libraries and tools for LEGO Mindstorms NXT robots
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _ANXT_NXT_H_
#define _ANXT_NXT_H_

#include <sys/types.h>

#include <anxt/net.h>

#define NXT_FAIL -1
#define NXT_SUCC 0

#define NXT_SENSOR1 0
#define NXT_SENSOR2 1
#define NXT_SENSOR3 2
#define NXT_SENSOR4 3

#define NXT_SENSOR_TYPE_NONE           0x00
#define NXT_SENSOR_TYPE_SWITCH         0x01
#define NXT_SENSOR_TYPE_TEMPERATURE    0x02
#define NXT_SENSOR_TYPE_REFLECTION     0x03
#define NXT_SENSOR_TYPE_ANGLE          0x04
#define NXT_SENSOR_TYPE_LIGHT_ACTIVE   0x05
#define NXT_SENSOR_TYPE_LIGHT_INACTIVE 0x06
#define NXT_SENSOR_TYPE_SOUND_DB       0x07
#define NXT_SENSOR_TYPE_SOUND_DBA      0x08
#define NXT_SENSOR_TYPE_CUSTOM         0x09
#define NXT_SENSOR_TYPE_LOWSPEED       0x0A
#define NXT_SENSOR_TYPE_LOWSPEED_9V    0x0B

#define NXT_SENSOR_MODE_RAW              0x00
#define NXT_SENSOR_MODE_BOOLEAN          0x20
#define NXT_SENSOR_MODE_TRANSITION_COUNT 0x40
#define NXT_SENSOR_MODE_PERIOD_COUNT     0x60
#define NXT_SENSOR_MODE_PERCENT          0x80
#define NXT_SENSOR_MODE_CELSIUS          0xA0
#define NXT_SENSOR_MODE_FAHRENHEIT       0xC0
#define NXT_SENSOR_MODE_ANGLE_STEP       0xE0
#define NXT_SENSOR_MODE_SLOPE_MASK       0x1F
#define NXT_SENSOR_MODE_MODE_MASK        0xE0

#define NXT_MOTORA   0
#define NXT_MOTORB   1
#define NXT_MOTORC   2
#define NXT_MOTORABC 0xFF

#define NXT_MOTORON   1
#define NXT_BRAKE     2
#define NXT_REGULATED 4

#define NXT_REGMODE_IDLE        0
#define NXT_REGMODE_MOTOR_SPEED 1
#define NXT_REGMODE_MOTOR_SYNC  2

#define NXT_RUN_STATE_IDLE     0
#define NXT_RUN_STATE_RAMPUP   0x10
#define NXT_RUN_STATE_RUNNING  0x20
#define NXT_RUN_STATE_RAMPDOWN 0x40

#define NXT_NUM_TYPES 12
#define NXT_NUM_MODES 8

#define nxt_setmotorrotation(nxt,motor,rot,power) nxt_motor(nxt,motor,rot,power,NXT_MOTORON,NXT_REGMODE_MOTOR_SPEED,0)
#define nxt_setmotorbrake(nxt,motor)              nxt_motor(nxt,motor,0,0,NXT_MOTORON|NXT_BRAKE|NXT_REGULATED,NXT_REGMODE_MOTOR_SPEED,0)
#define nxt_setmotorcoast(nxt,motor)              nxt_motor(nxt,motor,0,0,0,NXT_REGMODE_IDLE,0)

#define NXT_BUFFER_POLL      0x00
#define NXT_BUFFER_HIGHSPEED 0x01

#define VALID_SENSOR(s)  ((s)>=0 && (s)<=3)
#define VALID_MOTOR(m)   (((m)>=0 && (m)<=2) || (m)==NXT_MOTORABC)
#define VALID_MAILBOX(m) ((m)>=0 && (m)<=9)
#define VALID_VOLUME(v)  ((v)>=0 && (v)<=4)
#define VALID_BUTTON(b)  ((b)>=1 && (b)<=4)

#define NXT_CON_BUFFERSIZE 64

// NXT error numbers
#define NXT_ERR_SUCCESS                          0x00
#define NXT_ERR_TRANSACTION_IN_PROGRESS          0x20
#define NXT_ERR_MAILBOX_EMPTY                    0x40
#define NXT_ERR_NO_MORE_HANDLES                  0x81
#define NXT_ERR_NO_SPACE                         0x82
#define NXT_ERR_NO_MORE_FILES                    0x83
#define NXT_ERR_END_OF_FILE_EXPECTED             0x84
#define NXT_ERR_END_OF_FILE                      0x85
#define NXT_ERR_NOT_A_LINEAR_FILE                0x86
#define NXT_ERR_FILE_NOT_FOUND                   0x87
#define NXT_ERR_HANDLE_ALREADY_CLOSED            0x88
#define NXT_ERR_NO_LINEAR_SPACE                  0x89
#define NXT_ERR_UNDEFINED_ERROR                  0x8A
#define NXT_ERR_FILE_IS_BUSY                     0x8B
#define NXT_ERR_NO_WRITE_BUFFERS                 0x8C
#define NXT_ERR_APPEND_NOT_POSSIBLE              0x8D
#define NXT_ERR_FILE_IS_FULL                     0x8E
#define NXT_ERR_FILE_EXISTS                      0x8F
#define NXT_ERR_MODULE_NOT_FOUND                 0x90
#define NXT_ERR_OUT_OF_BOUNDARY                  0x91
#define NXT_ERR_ILLEGAL_FILE_NAME                0x92
#define NXT_ERR_ILLEGAL_HANDLE                   0x93
#define NXT_ERR_REQUEST_FAILED                   0xBD
#define NXT_ERR_UNKNOWN_OPCODE                   0xBE
#define NXT_ERR_INSANE_PACKET                    0xBF
#define NXT_ERR_OUT_OF_RANGE                     0xC0
#define NXT_ERR_BUS_ERROR                        0xDD
#define NXT_ERR_NO_FREE_MEMORY_IN_BUFFER         0xDE
#define NXT_ERR_CHANNEL_NOT_VALID                0xDF
#define NXT_ERR_CHANNEL_NOT_CONFIGURED_OR_BUSY   0xE0
#define NXT_ERR_NO_ACTIVE_PROGRAM                0xEC
#define NXT_ERR_ILLEGAL_SIZE                     0xED
#define NXT_ERR_ILLEGAL_MAILBOX_QUEUE_ID         0xEE
#define NXT_ERR_INVALID_FIELD_OF_STRUCTURE       0xEF
#define NXT_ERR_BAD_INPUT_OUTPUT                 0xF0
#define NXT_ERR_INSUFFICIENT_MEMORY              0xFB
#define NXT_ERR_BAD_ARGUMENTS                    0xFF
// aNXT error numbers
#define NXT_ERR_CONNECTION                       0x0100

#define NXT_COMMUNICATION_COMMAND_LATENCY   60000 /* microseconds */
#define NXT_DIRECT_COMMAND_LATENCY           6000 /* microseconds */

// Bitmap Fileformats
#define NXT_JPEG 1
#define NXT_PNG  2

#define nxt_open(name) nxt_open_net(name,"localhost",NXTNET_DEFAULT_PORT,NULL)

typedef enum {
  NXT_CON_USB,
  NXT_CON_BT
} nxt_contype_t;

typedef char nxt_id_t[6];

typedef struct {
  char *name;
  char *buffer;
  char *ptr;
  int error;
  nxt_contype_t contype;
  nxtnet_cli_t *cli;
  int handle;
  nxt_id_t id;
} nxt_t;

struct nxt_sensor_values {
  int is_calibrated;
  int type;
  int mode;
  int raw;
  int normalized;
  int scaled;
  int calibrated;
};

void nxt_wait_after_direct_command(void);
void nxt_wait_after_communication_command(void);
void nxt_wait_extra_long_after_communication_command(void);

nxt_t *nxt_open_net(const char *name,const char *hostname,int port,const char *password);
void nxt_close(nxt_t *nxt);
int nxt_error(nxt_t *nxt);
char *nxt_strerror(unsigned int error);
void nxt_reset_error(nxt_t *nxt);
nxt_contype_t nxt_getcontype(nxt_t *nxt);
int nxt_sendmsg(nxt_t *nxt,int mailbox,char *data);
char *nxt_recvmsg(nxt_t *nxt,int mailbox,int clear);
int nxt_setname(nxt_t *nxt,char *name);
int nxt_getver(nxt_t *nxt,int *firmmaj,int *firmmin,int *protomaj,int *protomin);
int nxt_setsensormode(nxt_t *nxt,int sensor,int type,int mode);
int nxt_getsensorval(nxt_t *nxt,int sensor);
int nxt_getsensorvals(nxt_t *nxt,int sensor,struct nxt_sensor_values *values);
int nxt_resetsensor(nxt_t *nxt,int sensor);
int nxt_getbattery(nxt_t *nxt);
int nxt_motor(nxt_t *nxt,int motor,unsigned int rotation,int power,int mode,int regmode, int turnratio);
int nxt_tacho(nxt_t *nxt,int motor);
int nxt_resettacho(nxt_t *nxt,int motor,int relative);
int nxt_runprogram(nxt_t *nxt,char *filename);
int nxt_stopprogram(nxt_t *nxt);
char *nxt_getcurprog(nxt_t *nxt);
int nxt_getdevinfo(nxt_t *nxt,char **nxt_name,char bt_addr[6],unsigned int *bt_strength,unsigned int *free_flash);
char *nxt_getname(nxt_t *nxt);
int nxt_keepalive(nxt_t *nxt);
int nxt_beep(nxt_t *nxt,unsigned int freq,unsigned int dur);
int nxt_playsoundfile(nxt_t *nxt,char *filename,int loop);
int nxt_stopsound(nxt_t *nxt);
int nxt_resetbt(nxt_t *nxt);
ssize_t nxt_pollcmd(nxt_t *nxt,void **ptr,int buffer);
int nxt_deluserflash(nxt_t *nxt);

#endif /* _ANXT_NXT_H_ */
