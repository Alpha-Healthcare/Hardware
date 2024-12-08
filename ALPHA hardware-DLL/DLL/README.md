# DLL

Infinite8 device library for use in SDKs

# I8DeviceDLL

I8DeviceDLL is a library package for a 3rd party to be able make use of Infinite8 Devices without any desctop application.
There is some classes and methodes in this dll file for users to minimize complexiti of hardware protocols.
With this pakage users can easily connect to any Infinite8 devices from many programming languege including C++, matlab and python.

## Getting Started (python)

This dll requires the following packages to work fine.

### Prerequisites

Use pip script to install `pythonnet` . These package(s) are used to read .dll files writen in C# and calculate some anticipated data.
```
pip install pythonnet
```

### Import dll file

After installing the requrment packages, import ".net for python" (clr) and make a reference (copy) of the .dll file with thess commands: 

```
import clr
clr.AddReference(the absolute path of i8.dll file)
# for example: clr.AddReference('C:/Users/i8-tech/Desktop/csharp_dlls/infinite8dll/I8Library1/bin/Debug/I8Library1.dll')
```
### Instance from main class of i8devices
Just import `Device` class, And then you can use it by declaring an instance of `Device` class.
```
from I8Devices import Device
myAwsomeHolyShitImDyingForThisDevice = Device()

```
## How to Use

First of all, you should plug your device into your PC, and also install `ST micro COM Port Package` from `Device Manager` (it should be installed itself if your connected to internet).
After that type in the `Connect` method   

```
myAwsomeHolyShitImDyingForThisDevice.connect()

```

If device is successfully connected to the PC, you can use the following methods and properties to retrieve Device information, or send a command/ recieve data to/from the device:

```
# status
    myAwsomeHolyShitImDyingForThisDevice.getStatus()
    return:
            {"mode":dataGathering_flag, "cnt":sample_count, "buff_len":newData_count}

# info
    myAwsomeHolyShitImDyingForThisDevice.getInfo()
    returns:
           {"name":device_name, "version":firmware_version, "id":unic_ID, "flash":flash_data}
    

# get data
   myAwsomeHolyShitImDyingForThisDevice.start()
   myAwsomeHolyShitImDyingForThisDevice.getData()
   myAwsomeHolyShitImDyingForThisDevice.stop()
  
# write setting

        mysetting = Settings()
        
        mysetting.test_signal = 234
        mysetting.sampling_rate = 885.5
        mysetting.leadoff_mode = False
        mysetting.channels_on[7] = False
        mysetting.gain = 3
        
        myAwsomeHolyShitImDyingForThisDevice.writeSetting(mysetting)
        returns: True or False
   
```

## Built With
* [C# dll .net](https://docs.microsoft.com/en-us/dotnet/api/system.io.ports.serialport.pinchanged?view=netframework-4.8)
* [numpy](https://pypi.org/project/numpy/) - Used to calculate some shit
* [scipy](https://pypi.org/project/scipy/) - Used to calculate some real shit

## Contributing

How you fuckin' dare?

### Pull Request Process

1. Ensure any install or build dependencies are removed before the end of the layer when doing a 
   build.
2. Update the README.md with details of changes to the interface, this includes new environment 
   variables, exposed ports, useful file locations and container parameters.
3. Increase the version numbers in any examples files and the README.md to the new version that this
   Pull Request would represent. The versioning scheme we use is [SemVer](http://semver.org/).
4. You may merge the Pull Request in once you have the sign-off of two other developers, or if you 
   do not have permission to do that, you may request the second reviewer to merge it for you.

## Versioning

We use [SemVer](http://semver.org/) for versioning. The current released version is 0.2.5.

## Authors

* **Varnos** - *Initial work* - [Varnos](http://51.75.64.148/reza)

## License

This project is licensed under the I8 Co. License - see the [Infinite8 website](https://infinite8.tech) for details. (you wish there will be some information but I gotta tell you there is not )