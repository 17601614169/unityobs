#pragma once

class MediasoupIncommintSourceInterface {
public:
	virtual ~MediasoupIncommintSourceInterface() = default;
	virtual void OnClose() = 0;
};
