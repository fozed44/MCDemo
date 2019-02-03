
#include "../Headers/Paths.h"
#include <mutex>
#include <Windows.h>

namespace MC {

	// All directory methods share __pathsMutex
	std::recursive_mutex __pathsMutex;

	const char *Paths::AssetDirectory() {
		static std::string assetDirectory;

		if (assetDirectory.length() > 0)
			return assetDirectory.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (assetDirectory.length() == 0) {
			assetDirectory = std::string(InstallDirectory()) + __ASSET_SUB_DIR__;
		}

		return assetDirectory.c_str();
	}

	const char *Paths::ConfigDirectory() {
		static std::string configDirectory;
		
		if (configDirectory.length() > 0)
			return configDirectory.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (configDirectory.length() == 0) {
			configDirectory = std::string(InstallDirectory()) + __CONFIG_SUB_DIR__;
		}

		return configDirectory.c_str();
	}

	const char *Paths::LogDirectory() {
		static std::string logDirectory;

		if (logDirectory.length() > 0)
			return logDirectory.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (logDirectory.length() == 0) {
			logDirectory = std::string(InstallDirectory()) + __LOG_SUB_DIR__;
		}

		return logDirectory.c_str();
	}

	const char *Paths::ExecutableFilename() {
		static std::string exeFilename;

		if (exeFilename.length() > 0)
			return exeFilename.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (exeFilename.length() == 0) {
			std::string path(MAX_PATH, '\0');

			GetModuleFileNameA(NULL, &path[0], (DWORD)path.length());
			path.resize(strlen(&path[0]));

			std::string::size_type pos = path.find_last_of("\\/");

			exeFilename = std::move(path);
		}

		return exeFilename.c_str();
	}

	const char *Paths::InstallDirectory() {
		static std::string exeDirectory;

		if (exeDirectory.length() > 0)
			return exeDirectory.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (exeDirectory.length() == 0) {
			std::string path(MAX_PATH, '\0');

			GetModuleFileNameA(NULL, &path[0], (DWORD)path.length());
			path.resize(strlen(&path[0]));

			std::string::size_type pos = path.find_last_of("\\/");
			
			exeDirectory = path.substr(0, pos+1);
		}

		return exeDirectory.c_str();
	}

	const char *Paths::InitLogFilename() {
		static std::string logFilename;

		if (logFilename.length() > 0)
			return logFilename.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (logFilename.length() == 0) {
			logFilename = std::string(LogDirectory()) + __INIT_LOG_FILENAME__;
		}

		return logFilename.c_str();
	}

	const char *Paths::CoreConfigFilename() {
		static std::string coreConfigFilename;

		if (coreConfigFilename.length() > 0)
			return coreConfigFilename.c_str();

		std::lock_guard<std::recursive_mutex> lock(__pathsMutex);

		if (coreConfigFilename.length() == 0) {
			coreConfigFilename = std::string(ConfigDirectory()) + __CORE_CONFIG_FILENAME__;
		}

		return coreConfigFilename.c_str();
	}
}