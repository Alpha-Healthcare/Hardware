using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Threading;
using System.Management;
using System.Collections;
using System.Text;

namespace I8Devices
{
    public class Device
    {      
        private static string device_name = "No Device Found";
        private static int sample_count = 0;
        private static string firmware_version = "No Data";
        private static string unic_ID = "No Data";
        private static string flash_data = "No Data";
        
        static SerialPort _serialPort;
        static List<double[]> receiveBuffer = new List<double[]>() { };
        static byte[] sendBuffer = new byte[32];
        
        static bool dataGathering_flag = false;
        static bool answerToInit_flag = false;
        static bool settingMode_flag = false;
        static int newData_count = 0;
        public static bool debug_mode = false;

        public bool connect()
        {
            if (dataGathering_flag) return false;
            {
                sendBuffer[0] = Convert.ToByte('U');
                sendBuffer[1] = Convert.ToByte('8');
                sendBuffer[2] = Convert.ToByte('_');
                sendBuffer[3] = Convert.ToByte('s');
                sendBuffer[4] = Convert.ToByte('T');
                sendBuffer[5] = Convert.ToByte('8');
                sendBuffer[6] = Convert.ToByte(' ');
                sendBuffer[7] = Convert.ToByte('C');   // SRB1 connected  to all Nchs
                sendBuffer[8] = 0x00;  //Gain
                sendBuffer[9] = 0x00;  //Exgain
                sendBuffer[10] = 0x00;  //linked ear mode
                sendBuffer[11] = 0x0D;  //res
                sendBuffer[12] = 0x60;  //res
                sendBuffer[13] = 0x60;  //res
                sendBuffer[14] = 0x60;  //res
                sendBuffer[15] = 0x60;  //Channels
                sendBuffer[16] = 0x60;  //Channels
                sendBuffer[17] = 0x60;  //Channels
                sendBuffer[18] = 0x60;  //ExChannels
                sendBuffer[19] = 0x60;  //Derive
                sendBuffer[20] = Convert.ToByte(' '); //huse colors
                sendBuffer[21] = Convert.ToByte(' '); //huse colors
                sendBuffer[22] = Convert.ToByte(' '); //huse colors
                sendBuffer[23] = Convert.ToByte(' '); //huse colors
                sendBuffer[24] = Convert.ToByte(' '); 
                sendBuffer[25] = Convert.ToByte(' ');
                sendBuffer[26] = Convert.ToByte(' ');
                sendBuffer[27] = Convert.ToByte(' ');
                sendBuffer[28] = Convert.ToByte(' ');
                sendBuffer[29] = Convert.ToByte(' ');
                sendBuffer[30] = Convert.ToByte(' ');
                sendBuffer[31] = Convert.ToByte('V');
            }

            bool ATR_flag = false;
            COMPORTS device_coms = new COMPORTS();
            IList com_names = device_coms.FindPortByDesc("USB Serial Device");
            
            if (debug_mode) Console.WriteLine("DLL: Checking all COM ports ...");
            if (com_names.Count > 0)
            {
                foreach (string item in com_names)
                {
                    if (debug_mode) Console.WriteLine($"Found port: {item}");
                    _serialPort = new SerialPort(item);
                    _serialPort.ReadTimeout = 2000;
                    _serialPort.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);

                    if (debug_mode) Console.WriteLine("DLL: Send a reset command to device");
                    sendBuffer[3] = Convert.ToByte('R'); //reset the device
                    try
                    {
                        _serialPort.Open();
                    }
                    catch(Exception e)
                    {
                        if (debug_mode) Console.WriteLine($"Error: {e.Message}");
                        continue;
                    }
                    
                    _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
                    try { _serialPort.Close(); }
                    catch (Exception) { };
                    
                    Thread.Sleep(1000);
                    while (!ATR_flag)  //waiting for device to reinitialize...
                    {
                        try
                        {
                            _serialPort.Open();
                            ATR_flag = true;
                        }
                        catch (Exception)
                        {
                            Thread.Sleep(1000);
                            if (debug_mode) Console.WriteLine($"DLL: Waiting for device with {item} to restart...");
                            ATR_flag = false;
                        }
                    }
                    
                    if (debug_mode) Console.WriteLine("DLL: Sending handshake setting to device");
                    sendBuffer[3] = Convert.ToByte('s');    //setting mode
                    _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
                    
                    if (debug_mode) Console.WriteLine("DLL: Waiting for device to respond...");
                    while (!answerToInit_flag) Thread.Sleep(100);
                    
                    _serialPort.Close();
                    if (!answerToInit_flag)
                    {
                        if (debug_mode) Console.WriteLine("DLL: No response detected!");
                        return false;
                    }
                    else
                    {
                        if (debug_mode) Console.WriteLine($"DLL: Device {device_name} is Connected on {item}");
                        return true;
                    }
                }
                if (debug_mode) Console.WriteLine("DLL: No I8Devices found!");
                return false;
            }
            else
            { 
                if (debug_mode) Console.WriteLine("DLL: No COM port found!");
                return false;
            }
        }

        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            if (!answerToInit_flag)
            {
                byte[] rec_data = new byte[32];
                _serialPort.Read(rec_data, 0, 32);
                if (rec_data[31] == 144) // Indicate I8Device ID
                {
                    Thread.Sleep(500);
                    if (rec_data[27] == 48) // Indicate Fascin8 device
                    {
                        device_name = "Fascin8";
                        firmware_version = rec_data[24].ToString();
                        unic_ID = (256 * rec_data[25] + rec_data[26]).ToString();
                        flash_data = $"{rec_data[28]}{rec_data[29]}{rec_data[30]}";
                    }
                    else if (rec_data[27] == 47) // Indicate Ultim8 device
                    {
                        device_name = "Ultim8";
                        firmware_version = rec_data[24].ToString();
                        unic_ID = (256 * rec_data[25] + rec_data[26]).ToString();
                        flash_data = (65536 * rec_data[28] + 256 * rec_data[29] + rec_data[30]).ToString();
                    }
                    answerToInit_flag = true;
                }
            }
            else if (settingMode_flag)
            {
                byte[] rec_data = new byte[32];
                _serialPort.Read(rec_data, 0, 32);
                if (rec_data[31] == 144) // Indicate I8Device ID
                {
                    Thread.Sleep(500);
                    if (rec_data[27] == 48) // Indicate Fascin8 device
                    {
                        if (debug_mode)
                        {
                            Console.WriteLine("DLL: Fascin8 writing setting done successfully.");
                            Console.WriteLine("\nADS Register Dump:");
                            
                            // Print registers for all three ADS chips
                            for (int chip = 0; chip < 3; chip++)
                            {
                                Console.WriteLine($"\nChip {chip + 1} {(chip == 0 ? "(Main)" : "")}:");
                                int startIdx = chip * 24;
                                for (int i = 0; i < 24; i++)
                                {
                                    int regIdx = startIdx + i;
                                    if (regIdx < rec_data.Length)
                                    {
                                        Console.WriteLine($"Register 0x{i:X2}: 0x{rec_data[regIdx]:X2}");
                                    }
                                }
                            }
                        }
                    }
                    settingMode_flag = false;
                }
            }
            else if (_serialPort.IsOpen && _serialPort.BytesToRead > 0)
            {
                try
                {
                    if (device_name == "Fascin8" && _serialPort.BytesToRead > 73)
                    {
                        byte[] last_data_receive = new byte[80];
                        _serialPort.Read(last_data_receive, 0, 80);
                        
                        double[] data_converted = new double[30];
                        data_converted[0] = sample_count;  // dll sample count
                        data_converted[1] = (65536 * last_data_receive[0]) + (256 * last_data_receive[1]) + last_data_receive[2];  // firmware sample count
                        data_converted[2] = 0; //reserved
                        data_converted[3] = last_data_receive[3];  // Device ID
                        data_converted[4] = last_data_receive[4];  // user input keys
                        data_converted[5] = 0; //reserved
                        
                        int i = 6, p = 8;
                        for (int k = i; k < 24 + i; k++)
                        {
                            data_converted[k] = (65536 * last_data_receive[3 * (k - i) + p]) + 
                                              (256 * last_data_receive[3 * (k - i) + p + 1]) +
                                              last_data_receive[3 * (k - i) + p + 2];
                            
                            if (data_converted[k] > 8388607) 
                                data_converted[k] -= 16777216;
                            
                            data_converted[k] *= 0.536;
                        }
                        
                        receiveBuffer.Add(data_converted);
                        sample_count++;
                        newData_count++;
                    }
                }
                catch (Exception ex)
                {
                    if (debug_mode)
                        Console.WriteLine($"Error reading data: {ex.Message}");
                }
            }
        }

        public object getInfo(string val="all")
        {
            if (dataGathering_flag) return false;
            
            if (debug_mode)
            {
                Console.WriteLine($"DLL: device_name(name): {device_name}");
                Console.WriteLine($"DLL: firmware_version(version): {firmware_version}");
                Console.WriteLine($"DLL: unic_ID(id): {unic_ID}");
                Console.WriteLine($"DLL: flash_data(flash): {flash_data}");
            }

            if (val == "name") return device_name;
            if (val == "version") return firmware_version;
            if (val == "id") return unic_ID;
            if (val == "flash") return flash_data;
            if (val == "all")
            {
                return new Dictionary<string, string>
                {
                    { "name", device_name },
                    { "version", firmware_version },
                    { "id", unic_ID },
                    { "flash", flash_data }
                };
            }
            return false;
        }

        public bool set(Settings settings)
        {
            settingMode_flag = true;
            if (debug_mode)
            {
                Console.WriteLine($"DLL: Sampling rate= {settings.sampling_rate}");
                Console.WriteLine($"DLL: Test Mode= {settings.test_signal}");
                Console.WriteLine($"DLL: linked_ear Mode= {settings.linked_ear}");
                Console.WriteLine($"DLL: Lead off= {settings.leadoff_mode}");
                Console.WriteLine($"DLL: Gain= {settings.gain}");
            }

            sendBuffer[3] = Convert.ToByte('s'); // for enter in setting mode                                    
            sendBuffer[4] = settings.test_signal ? Convert.ToByte('T') : Convert.ToByte('n');
            sendBuffer[5] = Convert.ToByte(48 + (settings.sampling_rate / 250));
            sendBuffer[6] = settings.leadoff_mode ? Convert.ToByte('L') : Convert.ToByte(' ');
            sendBuffer[7] = Convert.ToByte('C');
            sendBuffer[8] = Convert.ToByte(settings.gain);
            sendBuffer[9] = 0x00;  // Extra gain
            sendBuffer[10] = settings.linked_ear ? Convert.ToByte('I') : Convert.ToByte('n');

            BitArray ch_arr = new BitArray(settings.channels_on);
            byte[] ch_data = new byte[80];
            ch_arr.CopyTo(ch_data, 0);
            
            sendBuffer[15] = ch_data[0];  // channels 1 to 8
            sendBuffer[16] = ch_data[1];  // channels 9 to 16
            sendBuffer[17] = ch_data[2];  // channels 17 to 24

            try
            {
                _serialPort.Open();
                _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
                
                if (debug_mode) 
                    Console.WriteLine("DLL: Waiting for device to send back acknowledge about write setting ...");
                
                while (settingMode_flag) 
                    Thread.Sleep(100);
                
                _serialPort.Close();
                return true;
            }
            catch (Exception ex)
            {
                if (debug_mode)
                    Console.WriteLine($"Error applying settings: {ex.Message}");
                return false;
            }
        }

        public bool start()
        {
            if (device_name == "No Device Found")
            {
                if (debug_mode) 
                    Console.WriteLine("DLL: No device connected to start data gathering!");
                return false;
            }

            if (dataGathering_flag)
            {
                if (debug_mode) 
                    Console.WriteLine("DLL: The device is already in data gathering mode!");
                return false;
            }

            receiveBuffer.Clear();
            newData_count = 0;
            dataGathering_flag = true;
            sample_count = 0;

            try
            {
                _serialPort.Open();
                sendBuffer[3] = Convert.ToByte('B'); //start mode
                _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
                
                if (debug_mode) 
                    Console.WriteLine("DLL: Starting data gathering ...");
                
                return true;
            }
            catch (Exception ex)
            {
                if (debug_mode)
                    Console.WriteLine($"Error starting data gathering: {ex.Message}");
                return false;
            }
        }

        public int stop()
        {
            if (device_name == "No Device Found")
            {
                if (debug_mode) 
                    Console.WriteLine("DLL: No device connected! Data gathering will remain unfinished :(");
                return -2;
            }

            if (!dataGathering_flag)
            {
                if (debug_mode) 
                    Console.WriteLine("DLL: The device was not in data gathering mode to stop it, please call 'start' method first.");
                return -1;
            }

            while (sample_count < 10) 
                Thread.Sleep(100);

            dataGathering_flag = false;
            sendBuffer[3] = Convert.ToByte('E'); //stop mode
            
            try
            {
                _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
                _serialPort.Close();
                
                if (debug_mode) 
                    Console.WriteLine("DLL: Data gathering has been finished.");
                
                return sample_count;
            }
            catch (Exception ex)
            {
                if (debug_mode)
                    Console.WriteLine($"Error stopping data gathering: {ex.Message}");
                return -1;
            }
        }
    }
} 