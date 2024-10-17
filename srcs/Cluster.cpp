#include "webserv.hpp"
#include "Cluster.hpp"

Cluster::Cluster(t_cluster_config config)
{
	for (size_t i, i < config.servers.size(); i++) {
		Server server(config.servers[i]);
		_servers.push_back(server);
	}
}

Cluster::~Cluster
{}

void Cluster::start() {
	for (size_t i, i < _servers.size(); i++) {
        _servers[i].start();
    }
}