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
 * requires: libxml-2.0, yaml-0.1, libpjproject
 */

/**
 *  @file    functions.c
 *  @author  Wolfgang Kampichler (DEC112 2.0)
 *  @date    07-2020
 *  @version 1.0
 *
 *  @brief this file holds pjchat function definitions
 */

/******************************************************************* INCLUDE */

#include "functions.h"

/********************************************************************* CONST */

/***************************************************************** FUNCTIONS */

/*
 * rand_str(dest, length)
 * creates a random string used as temporary id in a findService request
 */
void rand_str(char *dest, size_t lgth) {
  size_t index;
  char charset[] = "0123456789"
                   "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  srand(time(NULL));
  while (lgth-- > 0) {
    index = (double)rand() / RAND_MAX * (sizeof charset - 1);
    *dest++ = charset[index];
  }
  *dest = '\0';
}

/*
 * replace_str(dest, length)
 * replaces pattern in a given string
 */
char *replace_str(const char *in, const char *pattern, const char *by) {
  size_t outsize = strlen(in) + 1;

  char *res = malloc(outsize);
  size_t resoffset = 0;

  char *needle;
  while ((needle = strstr(in, pattern))) {
    // copy everything up to the pattern
    memcpy(res + resoffset, in, needle - in);
    resoffset += needle - in;

    // skip the pattern in the input-string
    in = needle + strlen(pattern);

    // adjust space for replacement
    outsize = outsize - strlen(pattern) + strlen(by);
    res = realloc(res, outsize);

    // copy the pattern
    memcpy(res + resoffset, by, strlen(by));
    resoffset += strlen(by);
  }

  // copy the remaining input
  strcpy(res + resoffset, in);

  return res;
}

/*
 * from_hex(ch)
 * converts a hex character to its integer value
 */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/*
 * to_hex(code)
 * converts an integer value to its hex character
 */
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/*
 * url_encode(str)
 * returns a url-encoded version of str
 * IMPORTANT: be sure to free() the returned string after use
 */
char *url_encode(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' ||
        *pstr == '~')
      *pbuf++ = *pstr;
    else if (*pstr == ' ')
      *pbuf++ = '+';
    else
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/*
 * url_decode(str)
 * returns a url-decoded version of str
 * IMPORTANT: be sure to free() the returned string after use
 */
char *url_decode(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') {
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/*
 * initConf(conf)
 * initialize config attributes
 */
void initConf(p_conf_t conf) {
  conf->domain = NULL;
  conf->user = NULL;
  conf->passwd = NULL;
  conf->device = NULL;
  conf->country = NULL;
  conf->proxy = NULL;
  conf->lon = NULL;
  conf->lat = NULL;
  conf->uri = NULL;
  conf->ref = NULL;
  conf->dei = NULL;
  conf->api = NULL;
  conf->eval = NULL;
  conf->url = NULL;
  conf->reply = NULL;
  conf->did = NULL;
  conf->cid = NULL;
  conf->rid = NULL;
  conf->surname = NULL;
  conf->given = NULL;
  conf->phone = NULL;
  conf->email = NULL;
  conf->street = NULL;
  conf->locality = NULL;
  conf->code = NULL;
  conf->rad = 0;
  conf->dbg = 0;
  conf->reg = 0;
  conf->req = 0;
  conf->val = 0;
  conf->xhd = 0;
}

/*
 * readConf(filename, pool)
 * reads YAML config file
 */
p_conf_t readConf(char *filename, pj_pool_t *pool) {

  int state = 0;
  char **datap;
  char *radstr;
  char *dbgstr;
  char *tk;

  FILE *fh = fopen(filename, "r");

  yaml_parser_t parser;
  yaml_token_t token;

  conf = pj_pool_alloc(pool, (sizeof(s_conf_t)));
  initConf(conf);

  if (!yaml_parser_initialize(&parser))
    fputs("failed to initialize parser!\n", stderr);
  if (fh == NULL)
    fputs("failed to open file!\n", stderr);

  yaml_parser_set_input_file(&parser, fh);

  do {
    yaml_parser_scan(&parser, &token);
    switch (token.type) {
    case YAML_KEY_TOKEN:
      state = 0;
      break;
    case YAML_VALUE_TOKEN:
      state = 1;
      break;
    case YAML_SCALAR_TOKEN:
      tk = (char *)token.data.scalar.value;
      if (state == 0) {
        if (!strcmp(tk, "domain")) {
          datap = &conf->domain;
        } else if (!strcmp(tk, "user")) {
          datap = &conf->user;
        } else if (!strcmp(tk, "passwd")) {
          datap = &conf->passwd;
        } else if (!strcmp(tk, "device")) {
          datap = &conf->device;
        } else if (!strcmp(tk, "proxy")) {
          datap = &conf->proxy;
        } else if (!strcmp(tk, "lon")) {
          datap = &conf->lon;
        } else if (!strcmp(tk, "lat")) {
          datap = &conf->lat;
        } else if (!strcmp(tk, "rad")) {
          datap = &radstr;
        } else if (!strcmp(tk, "ref")) {
          datap = &conf->ref;
        } else if (!strcmp(tk, "did")) {
          datap = &conf->dei;
        } else if (!strcmp(tk, "rid")) {
          datap = &conf->rid;
        } else if (!strcmp(tk, "api")) {
          datap = &conf->api;
        } else if (!strcmp(tk, "eval")) {
          datap = &conf->eval;
        } else if (!strcmp(tk, "debug")) {
          datap = &dbgstr;
        } else if (!strcmp(tk, "surname")) {
          datap = &conf->surname;
        } else if (!strcmp(tk, "given")) {
          datap = &conf->given;
        } else if (!strcmp(tk, "phone")) {
          datap = &conf->phone;
        } else if (!strcmp(tk, "email")) {
          datap = &conf->email;
        } else if (!strcmp(tk, "street")) {
          datap = &conf->street;
        } else if (!strcmp(tk, "locality")) {
          datap = &conf->locality;
        } else if (!strcmp(tk, "code")) {
          datap = &conf->code;
        } else {
          printf("unrecognised key: %s\n", tk);
        }
      } else {
        *datap = strdup(tk);
      }
      break;
    default:
      break;
    }
    if (token.type != YAML_STREAM_END_TOKEN)
      yaml_token_delete(&token);
  } while (token.type != YAML_STREAM_END_TOKEN);

  conf->rad = atoi(radstr);
  conf->dbg = atoi(dbgstr);

  yaml_token_delete(&token);
  yaml_parser_delete(&parser);

  fclose(fh);

  return conf;
}

/*
 * create_pidflo(lgth, lat, lon, rad, entity, pool)
 * creates pidf-lo; point or circle if rad > 0
 */
char *create_vcard(long int *lgth, char *country, pj_pool_t *pool) {

  char *doc = NULL;
  char buf[BUFFER_2048 + 1];

  snprintf(buf, BUFFER_2048, VCARD_TEMPLATE,
           conf->surname ? conf->surname : USER_SURNAME,
           conf->given ? conf->given : USER_GIVEN,
           conf->surname ? conf->surname : USER_SURNAME,
           conf->given ? conf->given : USER_GIVEN,
           conf->phone ? conf->phone : USER_PHONE,
           conf->email ? conf->email : USER_EMAIL,
           conf->street ? conf->street : USER_STREET,
           conf->locality ? conf->locality : USER_LOCALITY,
           conf->code ? conf->code : USER_CODE, country);

  doc = (char *)pj_pool_alloc(pool, strlen(buf));
  if (doc == NULL) {
    PJ_LOG(4, (THIS_FILE, "malloc failed\n"));
    return doc;
  }

  memset(doc, 0, strlen(buf) + 1);
  memcpy(doc, buf, strlen(buf));
  doc[strlen(buf)] = '\0';

  *lgth = strlen(doc);

  PJ_LOG(3, (THIS_FILE, "vcard\n%s\n", doc));

  return doc;
}

/*
 * create_pidflo(lgth, lat, lon, rad, entity, pool)
 * creates pidf-lo; point or circle if rad > 0
 */
char *create_pidflo(long int *lgth, char *lat, char *lon, int rad, char *entity,
                    pj_pool_t *pool) {
  int buffersize = 0;

  char buf[BUFFER_2048 + 1];
  char *doc = NULL;

  xmlChar *xmlbuff = NULL;
  xmlDocPtr request = NULL;

  xmlNodePtr ptrPresence = NULL;
  xmlNodePtr ptrTuple = NULL;
  xmlNodePtr ptrStatus = NULL;
  xmlNodePtr ptrGeoPriv = NULL;
  xmlNodePtr ptrLocInfo = NULL;
  xmlNodePtr ptrPoint = NULL;
  xmlNodePtr ptrCircle = NULL;
  xmlNodePtr ptrRadius = NULL;

  xmlKeepBlanksDefault(1);
  *lgth = 0;

  /* create xml */
  request = xmlNewDoc(BAD_CAST "1.0");
  if (!request) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewDoc() failed\n"));
    return doc;
  }
  /* presence - element */
  ptrPresence = xmlNewNode(NULL, BAD_CAST "presence");
  if (!ptrPresence) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewNode() failed\n"));
    return doc;
  }
  xmlDocSetRootElement(request, ptrPresence);
  /* set properties */
  xmlNewProp(ptrPresence, BAD_CAST "xmlns",
             BAD_CAST "urn:ietf:params:xml:ns:pidf");
  xmlNewProp(ptrPresence, BAD_CAST "xmlns:gp",
             BAD_CAST "urn:ietf:params:xml:ns:pidf:geopriv10");
  xmlNewProp(ptrPresence, BAD_CAST "xmlns:gs",
             BAD_CAST "http://www.opengis.net/pidflo/1.0");
  xmlNewProp(ptrPresence, BAD_CAST "entity", BAD_CAST entity);
  /* tuple - element */
  ptrTuple = xmlNewChild(ptrPresence, NULL, BAD_CAST "tuple", NULL);
  if (!ptrTuple) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
    return doc;
  }
  if (rad == 0) {
    xmlNewProp(ptrTuple, BAD_CAST "id", BAD_CAST "point");
  } else {
    xmlNewProp(ptrTuple, BAD_CAST "id", BAD_CAST "circle");
  }
  /* status - element */
  ptrStatus = xmlNewChild(ptrTuple, NULL, BAD_CAST "status", NULL);
  if (!ptrStatus) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
    return doc;
  }
  /* gp:geopriv - element */
  ptrGeoPriv = xmlNewChild(ptrStatus, NULL, BAD_CAST "gp:geopriv", NULL);
  if (!ptrGeoPriv) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
    return doc;
  }
  /* gp:loction-info - element */
  ptrLocInfo = xmlNewChild(ptrGeoPriv, NULL, BAD_CAST "gp:location-info", NULL);
  if (!ptrLocInfo) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
    return doc;
  }
  /* set pos */
  snprintf(buf, BUFFER_2048, "%s %s", lat, lon);
  /* Point */
  if (conf->rad == 0) {
    ptrPoint = xmlNewChild(ptrLocInfo, NULL, BAD_CAST "gs:Point", NULL);
#define USER_STREET "Main Street 123"
    if (!ptrPoint) {
      PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
      return doc;
    }
    xmlNewProp(ptrPoint, BAD_CAST "xmlns",
               BAD_CAST "http://www.opengis.net/gml");
    xmlNewProp(ptrPoint, BAD_CAST "srsName",
               BAD_CAST "urn:ogc:def:crs:EPSG::4326");
    /* pos */
    xmlNewChild(ptrPoint, NULL, BAD_CAST "pos", BAD_CAST buf);
  } else {
    /* circle - Point */
    ptrCircle = xmlNewChild(ptrLocInfo, NULL, BAD_CAST "gs:Circle", NULL);
    if (!ptrCircle) {
      PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
      return doc;
    }
    xmlNewProp(ptrCircle, BAD_CAST "xmlns:gml",
               BAD_CAST "http://www.opengis.net/gml");
    xmlNewProp(ptrCircle, BAD_CAST "srsName",
               BAD_CAST "urn:ogc:def:crs:EPSG::4326");
    /* pos */
    xmlNewChild(ptrCircle, NULL, BAD_CAST "gml:pos", BAD_CAST buf);
    /* circle - radius */
    snprintf(buf, BUFFER_2048, "%d", rad);
    ptrRadius =
        xmlNewChild(ptrCircle, NULL, BAD_CAST "gs:radius", BAD_CAST buf);
    if (!ptrRadius) {
      PJ_LOG(4, (THIS_FILE, "pidf-lo xmlNewChild() failed\n"));
      return doc;
    }
    xmlNewProp(ptrRadius, BAD_CAST "uom",
               BAD_CAST "urn:ogc:def:uom:EPSG::9001");
  }
  /* gp:usage-rules - element */
  xmlNewChild(ptrGeoPriv, NULL, BAD_CAST "gp:usage-rules", NULL);
  /* gp:method - element */
  xmlNewChild(ptrGeoPriv, NULL, BAD_CAST "gp:method", BAD_CAST "gps");

  xmlDocDumpFormatMemory(request, &xmlbuff, &buffersize, 0);
  if (!xmlbuff) {
    PJ_LOG(4, (THIS_FILE, "pidf-lo xmlDocDumpFormatMemory() failed\n"));
    return doc;
  }

  doc = (char *)pj_pool_alloc(pool, (buffersize + 1) * sizeof(char));
  if (doc == NULL) {
    PJ_LOG(4, (THIS_FILE, "malloc failed\n"));
    return doc;
  }

  memset(doc, 0, buffersize + 1);
  memcpy(doc, (char *)xmlbuff, buffersize);
  doc[buffersize] = '\0';

  *lgth = strlen(doc);

  xmlFree(xmlbuff);
  xmlFreeDoc(request);

  PJ_LOG(3, (THIS_FILE, "pidf-lo\n%s\n", doc));

  return doc;
}

/*
 * error_exit(title, status)
 * display error and exit application
 */
void error_exit(const char *title, pj_status_t status) {

  pjsua_perror(THIS_FILE, title, status);
  pjsua_destroy();
  exit(1);
}

/*
 * send_dec112_msg(*acc_id, *text, *uri, *surn, mtype, *pool)
 * create multipart MIME body, add DEC112 Call-Info/Geolocation header
 * and send the message
 */
pj_status_t send_dec112_msg(pjsua_acc_id *acc_id, pj_str_t *text, pj_str_t *uri,
                            pj_str_t *surn, int mtype, pj_pool_t *pool) {
  pjsua_msg_data msg_data;
  pjsip_multipart_part *alt_part;
  pjsip_multipart_part *alt_partv;
  pj_status_t status;
  pjsip_hdr *hdr;

  pj_str_t type;
  pj_str_t subtype;
  pj_str_t content;
  pj_str_t hname;
  pj_str_t hvalue;
  pj_str_t typev;
  pj_str_t subtypev;
  pj_str_t contentv;

  pjsip_generic_string_hdr ci_cid;
  pjsip_generic_string_hdr ci_did;
  pjsip_generic_string_hdr ci_mtp;
  pjsip_generic_string_hdr ci_mid;
  pjsip_generic_string_hdr ci_url;
  pjsip_generic_string_hdr ci_dei;
  pjsip_generic_string_hdr ci_rid;

  pjsip_generic_string_hdr geortghdr;
  pjsip_generic_string_hdr geohdr;
  pjsip_generic_string_hdr xhdr;

  time_t now;
  struct tm *t;

  char tmp[BUFFER_128 + 1];
  char mtp[BUFFER_512 + 1];
  char dei[BUFFER_512 + 1];
  char rid[BUFFER_512 + 1];
  char mid[BUFFER_512 + 1];

  pjsua_msg_data_init(&msg_data);

  /* add DEC112 Call_Info header */
  hname = pj_str("Call-Info");

  // call id
  if (conf->cid != NULL) {
    hvalue = pj_str(conf->cid);
    pjsip_generic_string_hdr_init(pool, &ci_cid, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &ci_cid);
  }

  // devide id
  if (conf->did != NULL) {
    hvalue = pj_str(conf->did);
    pjsip_generic_string_hdr_init(pool, &ci_did, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &ci_did);
  }

  // url
  if (conf->url != NULL) {
    hvalue = pj_str(conf->url);
    pjsip_generic_string_hdr_init(pool, &ci_url, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &ci_url);
  }

  // rid
  if (conf->rid != NULL) {
    snprintf(
        rid, BUFFER_512,
        "<urn:dec112:uid:regid:%s:service.dec112.at>;purpose=" DEC112_REGID,
        conf->rid);
    hvalue = pj_str(rid);

    pjsip_generic_string_hdr_init(pool, &ci_rid, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &ci_rid);
  }

  // did
  if (conf->dei != NULL) {
    snprintf(dei, BUFFER_512, "<%s>;purpose=" DEC112_DID, conf->dei);
    hvalue = pj_str(dei);

    pjsip_generic_string_hdr_init(pool, &ci_dei, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &ci_dei);
  }

  // message type
  snprintf(
      mtp, BUFFER_512,
      "<urn:dec112:uid:msgtype:%i:service.dec112.at>;purpose=" DEC112_MSGTYP,
      mtype);
  hvalue = pj_str(mtp);

  pjsip_generic_string_hdr_init(pool, &ci_mtp, &hname, &hvalue);
  pj_list_push_back(&msg_data.hdr_list, &ci_mtp);

  // message id
  now = time(NULL);
  t = localtime(&now);

  strftime(tmp, BUFFER_128, "%Y%m%d%H%M", t);
  snprintf(mid, BUFFER_512,
           "<urn:dec112:uid:msgid:%s:service.dec112.at>;purpose=" DEC112_MSGID,
           tmp);
  hvalue = pj_str(mid);

  pjsip_generic_string_hdr_init(pool, &ci_mid, &hname, &hvalue);
  pj_list_push_back(&msg_data.hdr_list, &ci_mid);

  /* add Geolocation-Routing header */
  hname = pj_str("Geolocation-Routing");
  hvalue = pj_str("yes");

  pjsip_generic_string_hdr_init(pool, &geortghdr, &hname, &hvalue);
  pj_list_push_back(&msg_data.hdr_list, &geortghdr);

  /* add Geolocation header */
  hname = pj_str("Geolocation");
  hvalue = pj_str("<cid:DebhEr9UuGigk4nr@dec112.app>");

  pjsip_generic_string_hdr_init(pool, &geohdr, &hname, &hvalue);
  pj_list_push_back(&msg_data.hdr_list, &geohdr);

  /* add X-DEC112 header */
  if (conf->xhd == 1) {
    hname = pj_str(DEC112_XHDR_N);
    hvalue = pj_str(DEC112_XHDR_V);

    pjsip_generic_string_hdr_init(pool, &xhdr, &hname, &hvalue);
    pj_list_push_back(&msg_data.hdr_list, &xhdr);
  }

  /* add multipart MIME body */
  msg_data.multipart_ctype.type = pj_str("multipart");
  msg_data.multipart_ctype.subtype = pj_str("mixed");

  /* create pidf-lo */
  alt_part = NULL;
  alt_part = pjsip_multipart_create_part(pool);

  type = pj_str("application");
  subtype = pj_str("pidf+xml");

  hname = pj_str("Content-ID");
  hvalue = pj_str("<DebhEr9UuGigk4nr@dec112.app>");

  content.ptr = create_pidflo(&content.slen, conf->lat, conf->lon, conf->rad,
                              conf->uri, pool);

  alt_part->body = pjsip_msg_body_create(pool, &type, &subtype, &content);

  pj_list_push_back(&msg_data.multipart_parts, alt_part);

  hdr = (pjsip_hdr *)pjsip_generic_string_hdr_create(pool, &hname, &hvalue);
  pj_list_push_back(&alt_part->hdr, hdr);

  /* create vcard */
  if (mtype == 21) {
    alt_partv = NULL;
    alt_partv = pjsip_multipart_create_part(pool);

    typev = pj_str("application");
    subtypev = pj_str("addCallSub+xml");

    contentv.ptr = create_vcard(&contentv.slen, conf->country, pool);

    alt_partv->body = pjsip_msg_body_create(pool, &typev, &subtypev, &contentv);

    pj_list_push_back(&msg_data.multipart_parts, alt_partv);
  }
  /* send message */
  msg_data.target_uri = *surn;

  PJ_LOG(2, (THIS_FILE, "MESSAGE '%.*s' sending", text->slen, text->ptr));
  status = pjsua_im_send(*acc_id, uri, NULL, text, &msg_data, NULL);

  if (status != PJ_SUCCESS) {
    pj_strtrim(text);
    PJ_LOG(2,
           (THIS_FILE, "MESSAGE '%.*s' sending failed", text->slen, text->ptr));
  }

  return status;
}

/*
 * on_incoming_call(acc_id, call_id, *rdata)
 * callback called by the library upon receiving incoming call
 */
void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata) {
  pjsua_call_info ci;

  PJ_UNUSED_ARG(acc_id);
  PJ_UNUSED_ARG(rdata);

  pjsua_call_get_info(call_id, &ci);

  PJ_LOG(3, (THIS_FILE, "incoming request from %.*s!!",
             (int)ci.remote_info.slen, ci.remote_info.ptr));

  pjsua_call_answer(call_id, 200, NULL, NULL);
}

/*
 * on_call_state(call_id, *e)
 * callback called by the library when call's state has changed
 */
void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
  pjsua_call_info ci;

  PJ_UNUSED_ARG(e);

  pjsua_call_get_info(call_id, &ci);

  PJ_LOG(3, (THIS_FILE, "dialog %d state=%.*s", call_id,
             (int)ci.state_text.slen, ci.state_text.ptr));
}

/*
 * on_call_media_state(call_id)
 * callback called by the library when call's media state has changed
 */
void on_call_media_state(pjsua_call_id call_id) {
  pjsua_call_info ci;

  pjsua_call_get_info(call_id, &ci);

  PJ_LOG(3, (THIS_FILE, "media state changed\n"));

  if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
    // When media is active, connect call to sound device.
    pjsua_conf_connect(ci.conf_slot, 0);
    pjsua_conf_connect(0, ci.conf_slot);
  }
}

/*
 * on_reg(call_id)
 * callback called by the library when registration has changed
 */
void on_reg(pjsua_call_id call_id) {
  pjsua_acc_info info;
  pjsua_acc_get_info(call_id, &info);

  PJ_LOG(3, (THIS_FILE, "registration state changed\n"));

  if (info.status >= SIP_CODE_OK && info.status <= SIP_CODE_OK_END) {
    PJ_LOG(3, (THIS_FILE, "registration ok\n"));
    conf->reg = 1;
  } else if (info.status > SIP_CODE_OK_END) {
    PJ_LOG(3, (THIS_FILE, "registration failed\n"));
    conf->reg = 0;
  }
}

/*
 * on_pager2(call_id, *from, *to, *contact, *mime_type, *body, *rdata, acc_id)
 * callback called by the library when MESSAGE request is received
 */
void on_pager2(pjsua_call_id call_id, const pj_str_t *from,
                      const pj_str_t *to, const pj_str_t *contact,
                      const pj_str_t *mime_type, const pj_str_t *body,
                      pjsip_rx_data *rdata, pjsua_acc_id acc_id) {

  PJ_UNUSED_ARG(call_id);
  PJ_UNUSED_ARG(from);
  PJ_UNUSED_ARG(to);
  PJ_UNUSED_ARG(contact);
  PJ_UNUSED_ARG(mime_type);
  PJ_UNUSED_ARG(acc_id);

  pjsip_generic_string_hdr *hdr;
  pj_str_t hdr_name;
  pj_str_t msg_content;

  char tmp[BUFFER_512 + 1];
  char *rto = NULL;
  char *cid = NULL;

  PJ_LOG(2, (THIS_FILE, "request received."));
  PJ_LOG(3, (THIS_FILE, "MESSAGE received \n%s\n", rdata->msg_info.msg_buf));

  conf->req = 1;

  printf("\033[0;31m\n"); // set the text to the color red
  printf("\n%.*s:", (int)from->slen, from->ptr);
  printf("\n%.*s\n", (int)body->slen, body->ptr);
  printf("\033[0m\n"); // resets the text to default color
  fflush(stdout);

  if (conf->val == 1) {
    msg_content = pj_str(conf->eval);
    if (pj_strncmp(body, &msg_content, msg_content.slen) != 0) {
      conf->ret = conf->ret | ERR_VAL;
      PJ_LOG(3, (THIS_FILE, "validation missmatch"));
      PJ_LOG(4, (THIS_FILE, "MESSAGE received \n%.*s\n", (int)body->slen,
                 body->ptr));
      PJ_LOG(4, (THIS_FILE, "MESSAGE expected \n%s\n", msg_content));
    }
    conf->val = 0;
  }

  /* get Reply-To header */
  hdr_name = pj_str("Reply-To");
  hdr = (pjsip_generic_string_hdr *)pjsip_msg_find_hdr_by_name(
      rdata->msg_info.msg, &hdr_name, NULL);
  if (hdr) {
    rto = (char *)malloc((int)hdr->hvalue.slen * sizeof(char) + 1);
    memset(rto, 0, (int)hdr->hvalue.slen + 1);
    memcpy(rto, hdr->hvalue.ptr, (int)hdr->hvalue.slen);
    PJ_LOG(3, (THIS_FILE, "Reply-To \n%s\n\n", rto));
    conf->reply = rto;
  } else {
    conf->reply = NULL;
  }

  /* get Call-Info header */
  hdr_name = pj_str("Call-Info");
  hdr = (pjsip_generic_string_hdr *)pjsip_msg_find_hdr_by_name(
      rdata->msg_info.msg, &hdr_name, NULL);
  if (hdr) {
    do {
      snprintf(tmp, BUFFER_512, "%.*s", (int)hdr->hvalue.slen, hdr->hvalue.ptr);
      if (strstr(tmp, DEC112_CALLID)) {
        cid = (char *)malloc((int)hdr->hvalue.slen * sizeof(char) + 1);
        memset(cid, 0, (int)hdr->hvalue.slen + 1);
        memcpy(cid, hdr->hvalue.ptr, (int)hdr->hvalue.slen);
        PJ_LOG(3, (THIS_FILE, DEC112_CALLID " \n%s\n\n", cid));
        free(cid);
      } else if (strstr(tmp, DEC112_MSGTYP)) {
        cid = (char *)malloc((int)hdr->hvalue.slen * sizeof(char) + 1);
        memset(cid, 0, (int)hdr->hvalue.slen + 1);
        memcpy(cid, hdr->hvalue.ptr, (int)hdr->hvalue.slen);
        PJ_LOG(3, (THIS_FILE, DEC112_MSGTYP " \n%s\n\n", cid));
        if (strstr(cid, DEC112_MSGTYP_19)) {
          conf->reg = 0;
        }
        free(cid);
      }

      PJ_LOG(4, (THIS_FILE, "Call-Info \n%.*s\n\n", hdr->hvalue.slen,
                 hdr->hvalue.ptr));

      hdr = (pjsip_generic_string_hdr *)pjsip_msg_find_hdr_by_name(
          rdata->msg_info.msg, &hdr_name, hdr->next);
    } while (hdr);
  }
}
