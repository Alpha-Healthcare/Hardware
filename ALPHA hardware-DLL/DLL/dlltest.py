import clr
import time

DLL_PATH = r'C:/Program Files/Alpha/bin/I8Library1.dll'
from I8Devices import Device, Settings

def configure_channels(settings):
    """Configure main and extended channels with default settings
    Main channels: 21 channels across 3 ADS chips
    Extended channels: 3 channels (only on ADS chip 2)
    
    Derivation modes:
    - 0x00: No derivation
    - 0xF0: Just P (Positive)
    - 0x0F: Just N (Negative)
    - 0xFF: ALL (Both P and N) - Used for full bias derivation
    """
    # First, set all main channels to default state (enabled)
    for i in range(21):
        settings.channels_on[i] = True
    
    # Extended channels should be off by default (matching C# constructor)
    for i in range(3):
        settings.exchannels_on[i] = False
    
    # Set bias derivation mode to ALL (both P and N channels)
    settings.derivation = 0xFF  # Enable both P and N channels for bias

def main():
    device = Device()
    device.debug_mode = True
    
    if device.connect():
        print('Connected successfully')
        time.sleep(1)
        
        settings = Settings()
        # Default settings
        settings.test_signal = False
        settings.sampling_rate = 250  # Default in C# is 2000
        settings.leadoff_mode = False
        settings.gain = 24
        settings.exgain = 24
        
        # Configure channels and bias
        configure_channels(settings)
        
        # Apply settings
        success = device.set(settings)
        print(f"Settings applied: {success}")
        
        # Get status
        status = device.getStatus()
        print(f"Counter: {status['cnt']}")

if __name__ == "__main__":
    main()



