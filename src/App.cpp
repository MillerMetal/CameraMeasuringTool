#include "./controller/MyController.hpp"
#include "./AppComponent.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp-swagger/Controller.hpp"

#include <iostream>

void run() {

	// Register Components in scope of run() method 
	AppComponent components;

	//  Get router component 
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	// *********************************************
	// * Create MyController and add all of its 
	// * endpoints to the router.  Originally, the 
	// * following line was here:
	// *********************************************
	// router->addController(std::make_shared<MyController>());

	// *********************************************
	// * We had to break this apart to make it work 
	// *********************************************
	std::shared_ptr<MyController> myController = std::make_shared<MyController>();
	router->addController(myController);

	// Get connection handler component 
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	// Get connection provider component 
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	// Create server which takes provided TCP connections and passes them to HTTP connection handler 
	oatpp::network::Server server(connectionProvider, connectionHandler);

	// Print info about server port 
	OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());

	// *********************************************
	// * What they had on the site as example code.  
	// * Note, that myController did not exist
	// * originally at the beginning.
	// *********************************************
	// auto docEndpoints = oatpp::swagger::Controller::Endpoints::createShared();
	// docEndpoints->pushBackAll(myController->getEndpoints());
	//
	// auto swaggerController = oatpp::swagger::Controller::createShared(docEndpoints);
	// swaggerController->addEndpointsToRouter(router);

	// *********************************************
	// * Replaced the above with 2 lines.  The 
	// * addEndpointsToRouter function did not 
	// * exist, and there were no examples of how
	// * to do it.
	// *********************************************
	auto swaggerController = oatpp::swagger::Controller::createShared(myController->getEndpoints());
	router->addController(swaggerController);

	// Run server 
	server.run();
  
}

/**
 *  main
 */
int main(int argc, const char * argv[]) {

  oatpp::base::Environment::init();

  run();
  
  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";
  
  oatpp::base::Environment::destroy();
  
  return 0;
}
