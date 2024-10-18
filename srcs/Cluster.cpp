#include "webserv.hpp"
#include "Cluster.hpp"

Cluster::Cluster(t_cluster_config config)
{
	for (size_t i = 0; i < config.servers.size(); i++) {
		Server server(config.servers[i]);
		_servers.push_back(server);
	}
}

Cluster::~Cluster()
{}

void Cluster::start() {
	for (size_t i = 0; i < _servers.size(); i++) {
		_servers[i].start();
	}
}
