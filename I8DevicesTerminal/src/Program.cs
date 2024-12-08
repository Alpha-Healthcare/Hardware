using System;
using System.Threading;
using I8Devices;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("I8Devices Terminal Interface");
        Console.WriteLine("---------------------------");
        
        var device = new Device();
        device.debug_mode = true; // Enable debug mode for detailed output
        
        while (true)
        {
            Console.WriteLine("\nMain Menu:");
            Console.WriteLine("1. Connect to device");
            Console.WriteLine("2. Show device info");
            Console.WriteLine("3. Configure settings");
            Console.WriteLine("4. Start data acquisition");
            Console.WriteLine("5. Show ADS registers");
            Console.WriteLine("6. Exit");
            
            Console.Write("\nSelect option (1-6): ");
            var choice = Console.ReadLine();
            
            switch (choice)
            {
                case "1":
                    Console.WriteLine("\nAttempting to connect to device...");
                    bool connected = device.connect();
                    if (!connected)
                    {
                        Console.WriteLine("Failed to connect to device. Please check connections and try again.");
                    }
                    else
                    {
                        Console.WriteLine("Successfully connected to device!");
                    }
                    break;

                case "2":
                    var info = device.getInfo("all") as Dictionary<string, string>;
                    if (info != null)
                    {
                        Console.WriteLine($"\nDevice Info:");
                        Console.WriteLine($"Name: {info["name"]}");
                        Console.WriteLine($"Version: {info["version"]}");
                        Console.WriteLine($"ID: {info["id"]}");
                        Console.WriteLine($"Flash: {info["flash"]}");
                    }
                    else
                    {
                        Console.WriteLine("No device info available. Please connect first.");
                    }
                    break;

                case "3":
                    Console.WriteLine("\nConfiguring device settings...");
                    var settings = new Settings()
                    {
                        sampling_rate = 2000,
                        test_signal = true,
                        linked_ear = false,
                        leadoff_mode = false,
                        gain = 24
                    };
                    
                    if (device.set(settings))
                    {
                        Console.WriteLine("Settings applied successfully!");
                    }
                    else
                    {
                        Console.WriteLine("Failed to apply settings.");
                    }
                    break;

                case "4":
                    Console.WriteLine("\nStarting data acquisition...");
                    if (device.start())
                    {
                        Console.WriteLine("Collecting data for 5 seconds...");
                        Thread.Sleep(5000);
                        
                        Console.WriteLine("Stopping data acquisition...");
                        int samples = device.stop();
                        Console.WriteLine($"Collected {samples} samples.");
                    }
                    else
                    {
                        Console.WriteLine("Failed to start data acquisition.");
                    }
                    break;

                case "5":
                    Console.WriteLine("\nShowing ADS registers (requires debug mode and active connection):");
                    var settings2 = new Settings()
                    {
                        sampling_rate = 2000,
                        test_signal = true,
                        linked_ear = false,
                        leadoff_mode = false,
                        gain = 24
                    };
                    device.set(settings2); // This will trigger register dump in debug mode
                    break;

                case "6":
                    Console.WriteLine("\nExiting...");
                    return;

                default:
                    Console.WriteLine("\nInvalid option. Please try again.");
                    break;
            }
            
            Thread.Sleep(1000); // Small delay before showing menu again
        }
    }
} 