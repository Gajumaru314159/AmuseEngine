#include <Amuse/Input/Device/GamePadDevice.h>

namespace Amuse::Input {
    using namespace Amuse::Core;
#if defined(OS_LINUX)

	GamePadDevice::GamePadDevice() = default;
	GamePadDevice::~GamePadDevice() = default;

	void GamePadDevice::update() {
		// Linux向け実装は未対応
	}

#endif

}
