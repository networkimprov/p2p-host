#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "wpa_ctrl.h"


void sendRequest(struct wpa_ctrl* iIf, char* iSend) {
  fprintf(stdout, ">>> %s\n", iSend);
  char aResp[64];
  size_t aRespN = sizeof aResp - 1;
  int aErr = wpa_ctrl_request(iIf, iSend, strlen(iSend), aResp, &aRespN, NULL);
  if (aErr)
    exit(3);
  aResp[aRespN] = '\0';
  fprintf(stdout, "    %s", aResp);
}


int main(int iArgc, char* iArgv[]) {

  const size_t kPds = strlen(P2P_EVENT_PROV_DISC_SHOW_PIN);
  const size_t kTrm = strlen(WPA_EVENT_TERMINATING);
  const size_t kDsc = strlen(WPA_EVENT_DISCONNECTED);

  if (iArgc < 5) {
    fprintf(stderr, "Usage: %s path-to-socket listen-period-ms listen-interval-ms 8-digit-pin\n", iArgv[0]);
    return 1;
  }

  const int kPeriod   = strtol(iArgv[2], NULL, 0);
  const int kInterval = strtol(iArgv[3], NULL, 0);
  if (kInterval < 100 || kPeriod < 10 || kPeriod > kInterval) {
    fprintf(stderr, "%s: invalid listen period and/or interval\n", iArgv[0]);
    return 1;
  }

  const int kPin = strtol(iArgv[4], NULL, 0);
  if (kPin < 10000000 || kPin > 99999999) {
    fprintf(stderr, "%s: invalid PIN\n", iArgv[0]);
    return 1;
  }

  struct wpa_ctrl *aIfRecv, *aIfSend;

  for (int i=0; i < 15; ++i) {
    aIfSend = wpa_ctrl_open(iArgv[1]);
    if (aIfSend)
      break;
    sleep(1);
  }
  if (!aIfSend) {
    fprintf(stderr, "%s: could not open %s\n", iArgv[0], iArgv[1]);
    return 1;
  }

  aIfRecv = wpa_ctrl_open(iArgv[1]);
  if (!aIfRecv)
    return 1;

  int aErr;
  aErr = wpa_ctrl_attach(aIfRecv);
  if (aErr)
    return 2;

  int aRfd = wpa_ctrl_get_fd(aIfRecv);
  fd_set aFds;
  FD_ZERO(&aFds);
  FD_SET(aRfd, &aFds);

  char aSend[256];
  char aResp[1024];
  size_t aRespN;

  sprintf(aSend, "P2P_EXT_LISTEN %d %d", kPeriod, kInterval);
  sendRequest(aIfSend, aSend);

  for (int aRun=1; aRun; ) {
    aErr = select(aRfd+1, &aFds, NULL, NULL, NULL);
    if (aErr == -1) {
      if (errno == EINTR)
        continue;
      return 4;
    }
    while (wpa_ctrl_pending(aIfRecv) > 0) {
      aRespN = sizeof aResp - 1;
      aErr = wpa_ctrl_recv(aIfRecv, aResp, &aRespN);
      if (aErr)
        return 5;
      aResp[aRespN] = '\0';
      fprintf(stdout, "    %s\n", aResp);

      if        (!strncmp(aResp+3, P2P_EVENT_PROV_DISC_SHOW_PIN, aRespN-3 < kPds ? aRespN-3 : kPds)) {
        char* aData = aResp + 3 + kPds;
        char* aSpc = strchr(aData, ' ');
        if (aSpc) *aSpc = '\0';
        sprintf(aSend, "P2P_CONNECT %s %d display go_intent=0", aData, kPin);
        sendRequest(aIfSend, aSend);
      } else if (!strncmp(aResp+3, WPA_EVENT_DISCONNECTED, aRespN-3 < kDsc ? aRespN-3 : kDsc)) {
        sprintf(aSend, "DISCONNECT");
      } else if (!strncmp(aResp+3, WPA_EVENT_TERMINATING, aRespN-3 < kTrm ? aRespN-3 : kTrm)) {
        aRun = 0;
        break;
      }
    }
  }

  aErr = wpa_ctrl_detach(aIfRecv);
  if (aErr)
    return 6;

  wpa_ctrl_close(aIfRecv);
  wpa_ctrl_close(aIfSend);

  return 0;
}
