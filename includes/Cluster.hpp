#pragma once

#include "Cluster.hpp"

class Cluster {
	private:
		std::vector<Server> servers;
	public:
				Cluster(t_cluster_config);
				~Cluster();
		void    start();
}