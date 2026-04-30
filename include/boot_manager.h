// boot_manager.h — one-shot "do we need to enter AP mode at boot?" check.
//
// Detects back-to-back resets using a single NVS byte: on every boot we
// write "1", wait _resetWindowSeconds, then write "0". If we boot and
// find "1" still set, a second reset happened inside the window.
//
// The app is free to *also* call requestApMode() on the FSM from any
// hardware-specific trigger (e.g. a dedicated button).

#pragma once

class BootManager {
public:
    BootManager() = default;

    // Inspect the "recent boot" flag. Returns true if a double reset was
    // detected *and* the app should therefore force AP mode on this boot.
    bool checkForForceAPMode();

    // Clear the "recent boot" flag. Called by the FSM once the clock has
    // been running normally for a few seconds, so a subsequent single
    // reset is no longer seen as the 2nd of a pair.
    void markBootStable();

private:
    const int _resetWindowSeconds = 5;
};
