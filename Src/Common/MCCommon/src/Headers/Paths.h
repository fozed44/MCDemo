#pragma once

namespace MC {
	
	/*
		Class MC::Paths

		Thread safe cached access to application directory paths.
	*/

#define __LOG_SUB_DIR__ "Log\\"
#define __ASSET_SUB_DIR__ "Asset\\"
#define __CONFIG_SUB_DIR__ "Config\\"
#define __SHADER_SUB_DIR__ "Asset\\Shaders\\"
#define __INIT_LOG_FILENAME__ "Init.log"
#define __CORE_CONFIG_FILENAME__ "Core.config"

	class Paths {
	public:
		static const char *AssetDirectory();
		static const char *ShaderDirectory();
		static const char *InstallDirectory();
		static const char *LogDirectory();
		static const char *ConfigDirectory();

		static const char *InitLogFilename();
		static const char *ExecutableFilename();
		static const char *CoreConfigFilename();
	};

}
