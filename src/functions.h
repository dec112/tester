/*
 * Copyright (C) 2020  <Wolfgang Kampichler>
 *
 * This file is part of pjchat
 *
 * pjchat is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pjchat is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 *  @file    functions.h
 *  @author  Wolfgang Kampichler (DEC112 2.0)
 *  @date    06-2020
 *  @version 1.0
 *
 *  @brief functions.c header file
 */

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

/******************************************************************* INCLUDE */

#include <ctype.h>
#include <getopt.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <pjsua-lib/pjsua.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <yaml.h>

/******************************************************************** DEFINE */

#define THIS_FILE "pjchat"
#define CFG_FILE "/home/cfg/config.yml"

#define BACKEND_POOL_INITIAL_SIZE 1000
#define BACKEND_POOL_INCREMENT 1000

#define SIP_CODE_OK 200
#define SIP_CODE_OK_END 299
#define SIP_CODE_BUSY_HERE 486
#define SIP_INTERNAL_ERROR 500

#define SIP_PORT 5060

#define BUFFER_2048 2048
#define BUFFER_1024 1024
#define BUFFER_512 512
#define BUFFER_128 128

#define TIMEOUT_MS 1000
#define TIMEOUT_CNT 32

#define DEC112_CALLID "dec112-CallId"
#define DEC112_MSGID "dec112-MessageId"
#define DEC112_DEVID "dec112-DeviceId"
#define DEC112_REGID "dec112-RegID"
#define DEC112_SUBINF "dec112-SubscriberInfo"
#define DEC112_MSGTYP "dec112-MessageTyp"
#define DEC112_DID "EmergencyCallData.DID"

#define DEC112_XHDR_N "X-DEC112-Test"
#define DEC112_XHDR_V "True"

#define DEC112_MSGTYP_19 "msgtype:19"

#define USER_SURNAME "Dow"
#define USER_GIVEN "John"
#define USER_PHONE "0012345555555"
#define USER_EMAIL "john.dow@mail.cc"
#define USER_STREET "Main Street 123"
#define USER_LOCALITY "Somewhere"
#define USER_CODE "2323"

#define VCARD_TEMPLATE                                                         \
  "<?xml version=\"1.0\" "                                                     \
  "encoding=\"UTF-8\"?><sub:EmergencyCallData.SubscriberInfo "                 \
  "xmlns:sub=\"urn:ietf:params:xml:ns:EmergencyCallData:SubscriberInfo\" "     \
  "xmlns:xc=\"urn:ietf:params:xml:ns:vcard-4.0\" "                             \
  "privacyRequested=\"false\"><sub:SubscriberData><xc:vcards><xc:vcard><xc:"   \
  "fn><xc:text>%s %s</xc:text></xc:fn><xc:n><xc:surname>%s</"                  \
  "xc:surname><xc:given>%s</xc:given><xc:prefix></xc:prefix><xc:suffix/"       \
  "></xc:n><xc:tel><xc:parameters><xc:type><xc:text>cell</"                    \
  "xc:text><xc:text>voice</xc:text><xc:text>text</xc:text></xc:type></"        \
  "xc:parameters><xc:text>%s</xc:text></"                                      \
  "xc:tel><xc:email><xc:parameters><xc:type><xc:text>home</xc:text></"         \
  "xc:type></xc:parameters><xc:text>%s</"                                      \
  "xc:text></xc:email><xc:adr><xc:parameters><xc:type><xc:text>home</"         \
  "xc:text></xc:type></xc:parameters><xc:street>%s</"                          \
  "xc:street><xc:locality>%s</xc:locality><xc:region/><xc:code>%s</"           \
  "xc:code><xc:country>%s</xc:country></xc:adr><xc:note><xc:text>{}</"         \
  "xc:text></xc:note></xc:vcard></xc:vcards></sub:SubscriberData></"           \
  "sub:EmergencyCallData.SubscriberInfo>"

#define START_MESSAGE                                                          \
  "%s %s (Phone: %s) has initiated an emergency "                              \
  "chat at %s. Current position is Lat: N %s; Lon: E %s"

#define STOP_MESSAGE "%s %s (Phone: %s) has ended the emergency chat at %s."

#define ERR_NON 0x00;
#define ERR_TMR 0x01;
#define ERR_REG 0x02;
#define ERR_MSG 0x04;
#define ERR_VAL 0x08;

/******************************************************************* TYPEDEF */

typedef unsigned char u_int8_t;

typedef struct conf {
  char *domain;
  char *user;
  char *passwd;
  char *device;
  char *country;
  char *proxy;
  char *lon;
  char *lat;
  char *uri;
  char *ref;
  char *dei;
  char *api;
  char *eval;
  char *url;
  char *reply;
  char *did;
  char *cid;
  char *rid;
  char *surname;
  char *given;
  char *phone;
  char *email;
  char *street;
  char *locality;
  char *code;
  int rad;
  int dbg;
  int reg;
  int req;
  int val;
  int xhd;
  u_int8_t ret;
} s_conf_t, *p_conf_t;

/****************************************************************** GLOBALS */

p_conf_t conf;

/*************************************************************** PROTOTYPES */

void rand_str(char *dest, size_t lgth);
char *replace_str(const char *in, const char *pattern, const char *by);
char from_hex(char ch);
char to_hex(char code);
char *url_encode(char *str);
char *url_decode(char *str);
void initConf(p_conf_t conf);
p_conf_t readConf(char *filename, pj_pool_t *pool);
char *create_vcard(long int *lgth, char *country, pj_pool_t *pool);
char *create_pidflo(long int *lgth, char *lat, char *lon, int rad, char *entity,
                    pj_pool_t *pool);
void error_exit(const char *title, pj_status_t status);
pj_status_t send_dec112_msg(pjsua_acc_id *acc_id, pj_str_t *text, pj_str_t *uri,
                            pj_str_t *surn, int mtype, pj_pool_t *pool);
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                      pjsip_rx_data *rdata);
void on_call_state(pjsua_call_id call_id, pjsip_event *e);
void on_call_media_state(pjsua_call_id call_id);
void on_reg(pjsua_call_id call_id);
void on_pager2(pjsua_call_id call_id, const pj_str_t *from, const pj_str_t *to,
               const pj_str_t *contact, const pj_str_t *mime_type,
               const pj_str_t *body, pjsip_rx_data *rdata, pjsua_acc_id acc_id);

#endif // FUNCTIONS_H_INCLUDED