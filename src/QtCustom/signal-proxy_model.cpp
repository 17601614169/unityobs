#include "signal-proxy_model.h"

SignalProxyModel::SignalProxyModel() {}

SignalProxyModel::~SignalProxyModel() {}

void SignalProxyModel::SetVedioSource(const std::string &id, const std::string &name, bool enable)
{
	m_tupleVedioEnable = std::make_tuple(id, name, enable);
}

bool SignalProxyModel::GetVedioSource(const std::string &id, const std::string &name, bool &enable)
{
	if (id == std::get<0>(m_tupleVedioEnable) && name == std::get<1>(m_tupleVedioEnable)) {
		enable = std::get<2>(m_tupleVedioEnable);
		return true;
	}

	return false;
}
