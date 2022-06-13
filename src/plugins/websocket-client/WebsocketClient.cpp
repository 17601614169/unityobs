/* Copyright Dr. Alex. Gouaillard (2015, 2020) */

#include <obs-module.h>
#include <openssl/opensslv.h>
#include "MediasoupWebsocketClientImpl.h"

OBS_DECLARE_MODULE()

bool obs_module_load(void)
{
    OPENSSL_init_ssl(0, NULL);
    return true;
}

WEBSOCKETCLIENT_API WebsocketClient * createWebsocketClient()
{
	return new MediasoupWebsocketClientImpl();
}
