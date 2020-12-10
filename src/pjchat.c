/*
 * Copyright (C) 2020  <Wolfgang Kampichler>
 *
 * This file is part of pjchat
 *
 * qngin is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qngin is distributed in the hope that it will be useful, but
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
 *  @file    pjchat.c
 *  @author  Wolfgang Kampichler (DEC112 2.0)
 *  @date    08-2020
 *  @version 1.0
 *
 *  @brief this file holds the main function definition
 */

/******************************************************************* INCLUDE */

#include "functions.h"

/********************************************************************** MAIN */

int main(int argc, char *argv[]) {
  pjsua_acc_id acc_id;
  pj_status_t status;
  pjsua_transport_config tcfg;
  pjsua_transport_id transport_id = -1;
  pjsua_logging_config log_cfg;
  pjsua_acc_config acc_cfg;
  pjsua_config cfg;
  pj_str_t uri;
  pj_str_t text;
  pj_str_t urn;
  pj_pool_t *pool;

  int i;
  int len;
  int opt;
  int ret;
  int aflg;
  int sflg;
  int mflg;
  int tflg;
  int xflg;
  int cnt;
  int arg_mi;
  int arg_mn;
  int nRet;

  char *txt;
  char *rnd;
  char *res;
  char *api;
  char *arg_uri;
  char *arg_urn;
  char *arg_cfg;
  char *arg_cnt;
  char *arg_txt;
  char *buffer;
  char **gptr = malloc(sizeof(char *));
  char tmp[BUFFER_512 + 1];
  char tmptime[BUFFER_128 + 1];

  size_t bufsize = 32;
  size_t characters;
  size_t *t = malloc(0);

  time_t ltime;
  struct tm *info;

  FILE *fd;

  *gptr = NULL;
  ret = 0;
  aflg = 0;
  sflg = 0;
  mflg = 0;
  tflg = 0;
  xflg = 0;
  arg_mi = 0;
  arg_mn = 0;

  arg_uri = NULL;
  arg_urn = NULL;
  arg_cfg = NULL;
  arg_cnt = NULL;
  arg_txt = NULL;

  while ((opt = getopt(argc, argv, "asxhc:r:u:f:n:i:t:")) != -1) {
    switch (opt) {
    case 'r':
      arg_uri = optarg;
      break;
    case 'u':
      arg_urn = optarg;
      break;
    case 'f':
      arg_cfg = optarg;
      break;
    case 'n':
      arg_mn = atoi(optarg);
      break;
    case 'i':
      arg_mi = atoi(optarg);
      break;
    case 'a':
      aflg = 1;
      break;
    case 's':
      sflg = 1;
      break;
    case 'x':
      xflg = 1;
      break;
    case 't':
      arg_txt = optarg;
      tflg = 1;
      break;
    case 'h':
      printf("%s -r <sip-uri> [-u <service-urn>] [-f <yaml-cfg>] "
             "[-t <msg file>] [-n <number> -i <intervall>] "
             "[-a] ... auto message [-s] ... tls [-x] ...test header\n",
             THIS_FILE);
      return 0;
      break;
    case 'c':
      arg_cnt = optarg;
      mflg = 1;
      break;
    case '?':
      return 0;
      break;
    }
  }

  if (arg_uri == NULL) {
    printf("%s -r <sip-uri> [-u <service-urn>] [-f <yaml-cfg>] "
           "[-t <msg file>] [-n <number> -i <intervall>] "
           "[-a] ... auto message [-s] ... tls [-x] ...test header\n",
           THIS_FILE);
    return 0;
  }

  if (((arg_mn == 0) && (arg_mi > 0)) || ((arg_mi == 0) && (arg_mn > 0))) {
    printf("%s -r <sip-uri> [-u <service-urn>] [-f <yaml-cfg>] "
           "[-t <msg file>] [-n <number> -i <intervall>] "
           "[-a] ... auto message [-s] ... tls [-x] ...test header\n",
           THIS_FILE);
    return 0;
  }

  if (tflg == 1) {
    if ((fd = fopen(arg_txt, "r")) == NULL) {
      printf("Error opening file: %s\n", arg_txt);
      return 0;
    }
  }

  /* create pjsua first! */
  status = pjsua_create();
  if (status != PJ_SUCCESS)
    error_exit("error in pjsua_create()", status);
  /* create pjsua pool */
  pool = pjsua_pool_create("psip", BACKEND_POOL_INITIAL_SIZE,
                           BACKEND_POOL_INCREMENT);
  if (!pool)
    error_exit("error in pjsua_create()", -1);

  if (arg_cfg) {
    conf = readConf(arg_cfg, pool);
    PJ_LOG(3, (THIS_FILE, "reading config from %s\n", arg_cfg));
  } else {
    conf = readConf(CFG_FILE, pool);
    PJ_LOG(3, (THIS_FILE, "reading config from %s\n", arg_cfg));
  }
  conf->xhd = xflg;
  conf->reply = NULL;
  conf->ret = ERR_NON;

  /* if argument is specified, it's got to be a valid SIP URL */
  if (arg_uri) {
    status = pjsua_verify_url(arg_uri);
    if (status != PJ_SUCCESS)
      error_exit("invalid URL in argv", status);
  }

  if (arg_cnt) {
    conf->country = arg_cnt;
  } else {
    conf->country = "AT";
  }

  /* init pjsua */
  pjsua_config_default(&cfg);
  cfg.cb.on_incoming_call = &on_incoming_call;
  cfg.cb.on_call_media_state = &on_call_media_state;
  cfg.cb.on_call_state = &on_call_state;
  cfg.cb.on_pager2 = &on_pager2;
  cfg.cb.on_reg_state = &on_reg;

  pjsua_logging_config_default(&log_cfg);
  log_cfg.console_level = conf->dbg;

  status = pjsua_init(&cfg, &log_cfg, NULL);
  if (status != PJ_SUCCESS)
    error_exit("error in pjsua_init()", status);

  pjsua_transport_config_default(&tcfg);

  if (sflg == 1) {
    /* add TLS transport. */
    tcfg.port = SIP_PORT + 1;
    status = pjsua_transport_create(PJSIP_TRANSPORT_TLS, &tcfg, &transport_id);
    if (status != PJ_SUCCESS)
      error_exit("error creating transport", status);
  } else {
    /* add TCP transport. */
    // tcfg.port = SIP_PORT;
    status = pjsua_transport_create(PJSIP_TRANSPORT_TCP, &tcfg, &transport_id);
    if (status != PJ_SUCCESS)
      error_exit("error creating transport", status);
  }

  /* initialization is done, now start pjsua */
  status = pjsua_start();
  if (status != PJ_SUCCESS)
    error_exit("error starting pjsua", status);

  /* create unique call id */
  rnd = (char *)malloc((36 + 1) * sizeof(char));
  if (rnd == NULL)
    error_exit("malloc failed", -1);
  rand_str(rnd, 36);
  snprintf(
      tmp, BUFFER_512,
      "<urn:dec112:uid:callid:%s:service.dec112.at>;purpose=" DEC112_CALLID,
      rnd);
  conf->cid = (char *)pj_pool_alloc(pool, strlen(tmp) * sizeof(char) + 1);
  if (conf->cid == NULL)
    error_exit("malloc failed", -1);
  memset(conf->cid, 0, strlen(tmp) + 1);
  memcpy(conf->cid, tmp, strlen(tmp));
  free(rnd);

  PJ_LOG(3, (THIS_FILE, DEC112_CALLID " \n%s", conf->cid));

  /* create unique device id N*/
  snprintf(
      tmp, BUFFER_512,
      "<urn:dec112:uid:deviceid:%s:service.dec112.at>;purpose=" DEC112_DEVID,
      conf->device);
  conf->did = (char *)pj_pool_alloc(pool, strlen(tmp) * sizeof(char) + 1);
  if (conf->did == NULL)
    error_exit("malloc failed", -1);
  memset(conf->did, 0, strlen(tmp) + 1);
  memcpy(conf->did, tmp, strlen(tmp));

  PJ_LOG(3, (THIS_FILE, DEC112_DEVID " \n%s", conf->did));

  /* create sip uri */
  len = strlen("sip:") + strlen(conf->user) + strlen("@") +
        strlen(conf->domain) + 1;
  conf->uri = (char *)pj_pool_alloc(pool, len * sizeof(char));
  if (conf->uri == NULL)
    error_exit("malloc failed", -1);
  snprintf(conf->uri, len, "sip:%s@%s", conf->user, conf->domain);

  PJ_LOG(3, (THIS_FILE, "dec112 SIP URI \n%s", conf->uri));

  /* create id derference url */
  api = url_encode(conf->api);
  res = replace_str(conf->ref, "${device_id}", conf->device);
  res = replace_str(res, "${api_key}", api);
  snprintf(tmp, BUFFER_512, "<%s>;purpose=" DEC112_SUBINF, res);
  conf->url = (char *)pj_pool_alloc(pool, strlen(tmp) + 1);
  if (conf->url == NULL)
    error_exit("malloc failed", -1);
  memset(conf->url, 0, strlen(tmp) + 1);
  memcpy(conf->url, tmp, strlen(tmp));
  free(res);
  free(api);

  PJ_LOG(3, (THIS_FILE, "dec112-SubscriberInfo \n%s", conf->url));

  /* register to SIP server by creating SIP account. */
  pjsua_acc_config_default(&acc_cfg);
  acc_cfg.id = pj_str(conf->uri);
  acc_cfg.proxy_cnt = 1;
  acc_cfg.proxy[0] = pj_str(conf->proxy);
  acc_cfg.reg_uri = pj_str(conf->proxy);
  acc_cfg.cred_count = 1;
  acc_cfg.cred_info[0].realm = pj_str(conf->domain);
  acc_cfg.cred_info[0].scheme = pj_str("digest");
  acc_cfg.cred_info[0].username = pj_str(conf->user);
  acc_cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  acc_cfg.cred_info[0].data = pj_str(conf->passwd);

  status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &acc_id);
  if (status != PJ_SUCCESS)
    error_exit("error adding account", status);

  /* if URL is specified, send first message */
  txt = NULL;
  if (arg_uri) {
    uri = pj_str(arg_uri);
    if (mflg == 0) {
      text = pj_str((char *)"Ping");
    } else {
      // 02/24/2020, 10:26:01 PM
      time(&ltime);
      info = localtime(&ltime);
      strftime(tmptime, BUFFER_128, "%m/%d/%Y, %I:%M:%S %p", info);
      snprintf(tmp, BUFFER_512, START_MESSAGE,
               conf->surname ? conf->surname : USER_SURNAME,
               conf->given ? conf->given : USER_GIVEN,
               conf->phone ? conf->phone : USER_PHONE, tmptime, conf->lat,
               conf->lon);
      txt = (char *)pj_pool_alloc(pool, strlen(tmp) * sizeof(char) + 1);
      if (txt == NULL)
        error_exit("malloc failed", -1);
      memset(txt, 0, strlen(tmp) + 1);
      memcpy(txt, tmp, strlen(tmp));
      text.ptr = txt;
      text.slen = strlen(txt);
    }

    /* if urn is specified, use urn */
    if (arg_urn) {
      urn = pj_str(arg_urn);
    } else {
      urn.ptr = NULL;
      urn.slen = 0;
    }
    /* wait for registration or timeout*/
    cnt = 0;
    while ((cnt < TIMEOUT_CNT) && (conf->reg == 0)) {
      pj_thread_sleep(TIMEOUT_MS);
      cnt++;
    }

    if (cnt == TIMEOUT_CNT) {
      PJ_LOG(2, (THIS_FILE, "timeout on registration request\n"));
    }

    if (conf->reg == 0) {
      conf->ret = conf->ret | ERR_REG;

      PJ_LOG(2, (THIS_FILE, "Registration failed.\n"));
    }
  }

  if (conf->reg == 1) {
    /* send first message */
    if (tflg == 0) {
      conf->val = 1;
    }
    conf->req = 0;
    status = send_dec112_msg(&acc_id, &text, &uri, &urn, 21, pool);
    /* wait for first response message or timeout */
    cnt = 0;
    while ((cnt < TIMEOUT_CNT) && (!conf->reply)) {
      pj_thread_sleep(TIMEOUT_MS);
      cnt++;
    }

    if (cnt == TIMEOUT_CNT) {
      PJ_LOG(2, (THIS_FILE, "timeout on first request message\n"));
    }

    if (!conf->reply) {
      conf->ret = conf->ret | ERR_MSG;

      PJ_LOG(2, (THIS_FILE, "Reply-To header missing.\n"));

    } else {
      uri = pj_str(conf->reply);
      urn = pj_str(conf->reply);

      if ((aflg == 1) && (tflg == 0)) {
        if (arg_mn > 1) {
          for (i = 2; i <= arg_mn; i++) {
            pj_thread_sleep(arg_mi * 1000);
            printf("\t#### %i -> %s ####\n", i, text.ptr);
            conf->req = 0;
            status = send_dec112_msg(&acc_id, &text, &uri, &urn, 22, pool);
            PJ_LOG(3, (THIS_FILE, "message sent with status %i\n", status));
          }
          pj_thread_sleep(arg_mi * 1000);
        }
        conf->req = 0;
        status = send_dec112_msg(&acc_id, &text, &uri, &urn, 23, pool);

        PJ_LOG(2, (THIS_FILE, "exiting with (%i) ...\n", status));
      } else if ((aflg == 0) && (tflg == 1)) {
        if ((fd = fopen(arg_txt, "r")) == NULL) {
          PJ_LOG(2, (THIS_FILE, "Error opening file: %s\n", arg_txt));
          return EXIT_FAILURE;
        }
        while ((nRet = getline(gptr, t, fd)) > 0) {
          text.ptr = *gptr;
          text.slen = strlen(*gptr);
          PJ_LOG(4, (THIS_FILE, "message from file: %.*s\n", text.slen, text.ptr));
          if (text.slen > 2) {
            if (text.ptr[text.slen - 2] == '*') {
              conf->val = 1;
              text.ptr[text.slen - 2] = ' ';
            }
            printf("\t#### -> %.*s\n", (int)text.slen, text.ptr);
            conf->req = 0;
            status = send_dec112_msg(&acc_id, &text, &uri, &urn, 22, pool);
            PJ_LOG(3, (THIS_FILE, "message sent with status %i\n", status));
            cnt = 0;
            while ((cnt < TIMEOUT_CNT) && (!conf->req)) {
              pj_thread_sleep(TIMEOUT_MS);
              cnt++;
            }
            if (cnt == TIMEOUT_CNT) {
              conf->ret = conf->ret | ERR_TMR;
              PJ_LOG(3, (THIS_FILE, "timeout on remote message request\n"));
            }
          }
        }

        fclose(fd);

        time(&ltime);
        info = localtime(&ltime);
        strftime(tmptime, BUFFER_128, "%m/%d/%Y, %I:%M:%S %p", info);
        snprintf(tmp, BUFFER_512, STOP_MESSAGE,
                 conf->surname ? conf->surname : USER_SURNAME,
                 conf->given ? conf->given : USER_GIVEN,
                 conf->phone ? conf->phone : USER_PHONE, tmptime);
        txt = (char *)pj_pool_alloc(pool, strlen(tmp) * sizeof(char) + 1);
        if (txt == NULL)
          error_exit("malloc failed", -1);
        memset(txt, 0, strlen(tmp) + 1);
        memcpy(txt, tmp, strlen(tmp));
        text.ptr = txt;
        text.slen = strlen(txt);

        status = send_dec112_msg(&acc_id, &text, &uri, &urn, 23, pool);
        PJ_LOG(3, (THIS_FILE, "message sent with status %i\n", status));

      } else {
        printf("\n##### Type messages followed by RETURN or use 'exit' to "
               "unregister #####\n\n");
        fflush(stdout);

        /* wait until user sends "exit" to quit. */
        for (;;) {
          buffer = (char *)malloc(bufsize * sizeof(char));
          characters = getline(&buffer, &bufsize, stdin);

          PJ_LOG(3, (THIS_FILE, "sending %i characters ... \n", characters));

          if (conf->reg == 0) {

            PJ_LOG(2, (THIS_FILE, "remote close ... exiting ...\n"));

            break;
          }
          text = pj_str(buffer);
          if (strstr(buffer, "exit")) {
            if (mflg != 0) {
              // 02/24/2020, 10:26:01 PM
              time(&ltime);
              info = localtime(&ltime);
              strftime(tmptime, BUFFER_128, "%m/%d/%Y, %I:%M:%S %p", info);
              snprintf(tmp, BUFFER_512, STOP_MESSAGE,
                       conf->surname ? conf->surname : USER_SURNAME,
                       conf->given ? conf->given : USER_GIVEN,
                       conf->phone ? conf->phone : USER_PHONE, tmptime);
              txt = (char *)pj_pool_alloc(pool, strlen(tmp) * sizeof(char) + 1);
              if (txt == NULL)
                error_exit("malloc failed", -1);
              memset(txt, 0, strlen(tmp) + 1);
              memcpy(txt, tmp, strlen(tmp));
              text.ptr = txt;
              text.slen = strlen(txt);
            }

            status = send_dec112_msg(&acc_id, &text, &uri, &urn, 23, pool);

            PJ_LOG(2, (THIS_FILE, "exiting with (%i) ...\n", status));

            break;
          } else {
            status = send_dec112_msg(&acc_id, &text, &uri, &urn, 22, pool);
          }
        }
      }
    }
  }

  ret = conf->ret;

  /* destroy pjsua */
  free(buffer);
  free(conf->reply);
  pj_pool_release(pool);
  pjsua_destroy();

  return ret;
}
