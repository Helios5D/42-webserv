#pragma once

#include "Server.hpp"

class Cluster {
	private:
		std::vector<Server> _servers;
	public:
				Cluster(t_cluster_config);
				~Cluster();
		void	start();
};
