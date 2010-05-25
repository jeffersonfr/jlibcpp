#include "jloggerlib.h"

using namespace std;

int main(int argc, char *argv[])
{
	jlogger::Logger *g_default_logger = NULL;

    try {
    	// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger(NULL, NULL);
			// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger(NULL, new jlogger::XMLFormatter());
			g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger((jlogger::Handler *)(new jlogger::FileHandler("teste.log")), new jlogger::XMLFormatter());
			// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger((jlogger::Handler *)(new jlogger::SocketHandler(1234)), new jlogger::XMLFormatter());

			// g_default_logger->SetOutput(jlogger::INFO_LOGGER);

			for (int i=0; i<100; i++) {
				g_default_logger->SendLogger(jlogger::INFO_LOGGER, "Logger Tested: %s:%u:%2d", __FUNCTION__, __LINE__, i);
				sleep(1);
			}

			g_default_logger->Release();
		} catch (...) {
		}

		if (g_default_logger != NULL) {
			delete g_default_logger;
		}

		return EXIT_SUCCESS;
}
