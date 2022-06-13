/* Copyright Dr. Alex. Gouaillard (2015, 2020) */

#include<obs-module.h>

struct webrtc_mediasoup {
	char *server;
	char *room;
	char *password;
	char *codec;
	char *output;
	char *userid;
};

static const char *webrtc_mediasoup_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Mediasoup WebRTC Server";
}

static void webrtc_mediasoup_update(void *data, obs_data_t *settings)
{
	struct webrtc_mediasoup *service = data;

	bfree(service->server);
	bfree(service->room);
	bfree(service->password);
	bfree(service->codec);
	bfree(service->output);
	bfree(service->userid);

	service->server = bstrdup(obs_data_get_string(settings, "server"));
	service->room = bstrdup(obs_data_get_string(settings, "room"));
	service->password = bstrdup(obs_data_get_string(settings, "password"));
	service->codec = bstrdup(obs_data_get_string(settings, "codec"));
	service->userid = bstrdup(obs_data_get_string(settings, "userid"));
	service->output = bstrdup("mediasoup_output");
}

static void webrtc_mediasoup_destroy(void *data)
{
	struct webrtc_mediasoup *service = data;

	bfree(service->server);
	bfree(service->room);
	bfree(service->password);
	bfree(service->codec);
	bfree(service->output);
	bfree(service->userid);
	bfree(service);
}

static void *webrtc_mediasoup_create(obs_data_t *settings, obs_service_t *service)
{
	struct webrtc_mediasoup *data = bzalloc(sizeof(struct webrtc_mediasoup));
	webrtc_mediasoup_update(data, settings);

	UNUSED_PARAMETER(service);
	return data;
}

static bool use_auth_modified(obs_properties_t *ppts, obs_property_t *p,
			      obs_data_t *settings)
{
	p = obs_properties_get(ppts, "server");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "key");
	obs_property_set_visible(p, false);

	p = obs_properties_get(ppts, "room");
	obs_property_set_visible(p, true);
	p = obs_properties_get(ppts, "userid");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "username");
	obs_property_set_visible(p, false);

	p = obs_properties_get(ppts, "password");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "codec");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "protocol");
	obs_property_set_visible(p, false);

	return true;
}

static obs_properties_t *webrtc_mediasoup_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *ppts = obs_properties_create();
	obs_property_t *p;

	obs_properties_add_text(ppts, "server", "Server Name", OBS_TEXT_DEFAULT);
	obs_properties_add_text(ppts, "room", "Server Room", OBS_TEXT_DEFAULT);
	obs_properties_add_text(ppts, "userid", "Server userid", OBS_TEXT_DEFAULT);

	obs_properties_add_text(ppts, "username", "Username", OBS_TEXT_DEFAULT);
	obs_properties_add_text(ppts, "password", "Stream Key", OBS_TEXT_DEFAULT);

	obs_properties_add_text(ppts, "codec", "Codec", OBS_TEXT_DEFAULT);
	obs_properties_add_text(ppts, "protocol", "Protocol", OBS_TEXT_DEFAULT);

	// obs_properties_add_list(ppts, "codec", "Codec", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	// obs_property_list_add_string(obs_properties_get(ppts, "codec"), "Automatic", "");
	// obs_property_list_add_string(obs_properties_get(ppts, "codec"), "H264", "h264");
	// obs_property_list_add_string(obs_properties_get(ppts, "codec"), "VP8", "vp8");
	// obs_property_list_add_string(obs_properties_get(ppts, "codec"), "VP9", "vp9");

	p = obs_properties_get(ppts, "server");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "room");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "username");
	obs_property_set_visible(p, false);
	p = obs_properties_get(ppts, "userid");
	obs_property_set_visible(p, false);

	p = obs_properties_get(ppts, "password");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "codec");
	obs_property_set_visible(p, true);

	p = obs_properties_get(ppts, "protocol");
	obs_property_set_visible(p, false);

	// obs_property_set_modified_callback(p, use_auth_modified);

	return ppts;
}

static const char *webrtc_mediasoup_url(void *data)
{
	struct webrtc_mediasoup *service = data;
	return service->server;
}

static const char *webrtc_mediasoup_key(void *data)
{
	UNUSED_PARAMETER(data);
	return "";
}

static const char *webrtc_mediasoup_room(void *data)
{
	struct webrtc_mediasoup *service = data;
	return service->room;
}

static const char *webrtc_mediasoup_userid(void *data)
{
	struct webrtc_mediasoup *service = data;
	return service->userid;
}

static const char *webrtc_mediasoup_username(void *data)
{
	UNUSED_PARAMETER(data);
	return "";
}

static const char *webrtc_mediasoup_password(void *data)
{
	struct webrtc_mediasoup *service = data;
	return service->password;
}

static const char *webrtc_mediasoup_codec(void *data)
{
	struct webrtc_mediasoup *service = data;
	if (strcmp(service->codec, "Automatic") == 0)
		return "";
	return service->codec;
}

static const char *webrtc_mediasoup_protocol(void *data)
{
	// struct webrtc_mediasoup *service = data;
	// if (strcmp(service->protocol, "Automatic") == 0)
	// 	return NULL;
	// return service->protocol;

	UNUSED_PARAMETER(data);
	return "";
}

static const char *webrtc_mediasoup_get_output_type(void *data)
{
	struct webrtc_mediasoup *service = data;
	return service->output;
}

struct obs_service_info webrtc_mediasoup_service = {
	.id             = "webrtc_mediasoup",
	.get_name       = webrtc_mediasoup_name,
	.create         = webrtc_mediasoup_create,
	.destroy        = webrtc_mediasoup_destroy,
	.update         = webrtc_mediasoup_update,
	.get_properties = webrtc_mediasoup_properties,
	.get_url        = webrtc_mediasoup_url,
	.get_key        = webrtc_mediasoup_key,
	.get_room       = webrtc_mediasoup_room,
	.get_username   = webrtc_mediasoup_username,
	.get_password   = webrtc_mediasoup_password,
	.get_userid	= webrtc_mediasoup_userid,
	.get_codec      = webrtc_mediasoup_codec,
	.get_protocol   = webrtc_mediasoup_protocol,
	.get_output_type = webrtc_mediasoup_get_output_type
};
