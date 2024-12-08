using System;
using System.IO.Ports;
using System.Collections.Generic;
using System.Threading;
using System.Management;
using System.Collections;
using System.Text;
using CenterSpace.NMath.Core;
using System.Threading.Tasks;

namespace I8Devices
{
    public class COMPORTS
    {
        public IList FindPortByDesc(string Desc)
        {
            IList suspect_coms = new ArrayList();
            foreach (ManagementObject obj in FindPorts())
            {
                try
                {
                    if (obj["Description"].ToString().ToLower().Equals(Desc.ToLower()))
                    {
                        string comName = ParseCOMName(obj);
                        if (comName != null)
                            suspect_coms.Add(comName);
                    }

                }
                catch (Exception)
                {
                    // Console.WriteLine(e);
                }
            }
            return suspect_coms;
        }

        static ManagementObject[] FindPorts()
        {
            try
            {
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("root\\CIMV2", "SELECT * FROM Win32_PnPEntity");
                List<ManagementObject> objects = new List<ManagementObject>();

                foreach (ManagementObject obj in searcher.Get())
                {
                    objects.Add(obj);
                }
                return objects.ToArray();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return new ManagementObject[] { };
            }
        }

        static string ParseCOMName(ManagementObject obj)
        {
            string name = obj["Name"].ToString();
            int startIndex = name.LastIndexOf("(");
            int endIndex = name.LastIndexOf(")");

            if (startIndex != -1 && endIndex != -1)
            {
                name = name.Substring(startIndex + 1, endIndex - startIndex - 1);
                return name;
            }
            return null;
        }
    }

    public class Settings
    {
        public bool linked_ear { get; set; }
        public bool test_signal { get; set; }
        public bool leadoff_mode { get; set; }

        private int _sampling_rate = 2000;  
        public int sampling_rate
        {
            get => _sampling_rate;
            set
            {
                if (value <= 250)  _sampling_rate = 250;
                else if ((value > 250) && (value <= 500))  _sampling_rate = 500;
                else if ((value > 500) && (value <= 1000))  _sampling_rate = 1000;
                else if ((value > 1000))  _sampling_rate = 2000;
            }
        }

        private int _gain = 24;
        private int _exgain = 24;
        public int gain
        {
            get => _gain;
            set
            {
                if (value <= 1) _gain = 1;
                else if ((value > 1) && (value <= 2)) _gain = 2;
                else if ((value > 2) && (value <= 4)) _gain = 4;
                else if ((value > 4) && (value <= 6)) _gain = 6;
                else if ((value > 6) && (value <= 8)) _gain = 8;
                else if ((value > 8) && (value <= 12)) _gain = 12;
                else if ((value > 12)) _gain = 24;
            }
        }

        public int exgain
        {
            get => _exgain;
            set
            {
                if (value <= 1) _exgain = 1;
                else if ((value > 1) && (value <= 2)) _exgain = 2;
                else if ((value > 2) && (value <= 4)) _exgain = 4;
                else if ((value > 4) && (value <= 6)) _exgain = 6;
                else if ((value > 6) && (value <= 8)) _exgain = 8;
                else if ((value > 8) && (value <= 12)) _exgain = 12;
                else if ((value > 12)) _exgain = 24;
            }
        }


        private const int _chanels_count = 21;
        public bool[] channels_on = new bool[_chanels_count];

        private const int _exchanels_count = 3;
        public bool[] exchannels_on = new bool[_exchanels_count];

        private const int _interaction_count = 4;
        public char[] interaction = new char[_interaction_count];

        public Settings()
        {
            test_signal = false;
            linked_ear = false;
            leadoff_mode = false;
            sampling_rate = 2000;
            gain = 24;
            for (int J = 0; J < _chanels_count; J++) channels_on[J] = true;
            for (int J = 0; J < _exchanels_count; J++) exchannels_on[J] = false;
            for (int J = 0; J < _interaction_count; J++)  interaction[J] = '0';
        }
    }

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

        public bool writeSetting(Settings mysetting)
        {
            int bias_derivation = 0;
            settingMode_flag = true;
            if (debug_mode) Console.WriteLine("DLL: Sampling rate= " + mysetting.sampling_rate);
            if (debug_mode) Console.WriteLine("DLL: Test Mode= " + mysetting.test_signal);
            if (debug_mode) Console.WriteLine("DLL: linked_ear Mode= " + mysetting.linked_ear);
            if (debug_mode) Console.WriteLine("DLL: Lead off= " + mysetting.leadoff_mode);
            if (debug_mode) Console.WriteLine("DLL: Gain= " + mysetting.gain);
            if (debug_mode) Console.Write("DLL: Channels On/Off=");
            foreach (var chon in mysetting.channels_on)
            {
                if (debug_mode) Console.Write(" " + chon);
            }
            if (debug_mode) Console.WriteLine("");


            sendBuffer[3] = Convert.ToByte('S'); // for enter in setting mode
           // if (mysetting.test_signal) sendBuffer[4] = Convert.ToByte('T');
           // else sendBuffer[4] = Convert.ToByte('n');
            sendBuffer[4] = mysetting.test_signal ? Convert.ToByte('T') : Convert.ToByte('n');
            sendBuffer[5] = Convert.ToByte(48+ (mysetting.sampling_rate/250));  // '1'=250, '2'=500, '4'=1000, '8'=2000
            sendBuffer[6] = Convert.ToByte(' ');  // disable lead-off (enable = 'L')
            sendBuffer[6] = mysetting.leadoff_mode ? Convert.ToByte('L') : Convert.ToByte(' ');
            sendBuffer[7] = Convert.ToByte('C');   // SRB1 connected  to all Nchs
            sendBuffer[8] = 0x01;  //Bias Derivation to 3-0 Pchs 0xD Register ADS
            sendBuffer[9] = 0x03;  //Bias Derivation to 7-4 Pchs 0xE Register ADS
            sendBuffer[10] = 0x0E;  //Bias Derivation to 3-0 Nchs 0xD Register ADS
            sendBuffer[11] = 0x0D;  //Bias Derivation to 7-4 Nchs 0xE Register ADS
            sendBuffer[12] = 0x60;  //Channel 1 Setting
            sendBuffer[13] = 0x60;  //Channel 2 Setting
            sendBuffer[14] = 0x60;  //Channel 3 Setting
            sendBuffer[15] = 0x60;  //Channel 4 Setting
            sendBuffer[16] = 0x60;  //Channel 5 Setting
            sendBuffer[17] = 0x60;  //Channel 6 Setting
            sendBuffer[18] = 0x60;  //Channel 7 Setting
            sendBuffer[19] = 0x60;  //Channel 8 Setting

            for (int i = 0; i < mysetting.channels_on.Length; i++)
            {
               if (mysetting.channels_on[i])
               {
                         if (mysetting.gain == 1)  sendBuffer[i + 12] = 0x00;
                    else if (mysetting.gain == 2)  sendBuffer[i + 12] = 0x10;
                    else if (mysetting.gain == 4)  sendBuffer[i + 12] = 0x20;
                    else if (mysetting.gain == 6)  sendBuffer[i + 12] = 0x30;
                    else if (mysetting.gain == 8)  sendBuffer[i + 12] = 0x40;
                    else if (mysetting.gain == 12) sendBuffer[i + 12] = 0x50;
                    else if (mysetting.gain == 24) sendBuffer[i + 12] = 0x60;
                    bias_derivation = bias_derivation + (int)Math.Pow(2, i);
                   // if (debug_mode) Console.WriteLine("DLL: powe= " + (int)Math.Pow(2, i));
                }
               else
                {
                    sendBuffer[i + 12] = 0x81;  // Channel power-down + Input shorted
                }
            }
            if (debug_mode) Console.WriteLine("DLL: bias_derivation On/Off= " + bias_derivation.ToString("X"));
            sendBuffer[8] = (byte)(bias_derivation % 16);  //Bias Derivation to 3-0 Pchs 0xD Register ADS
            sendBuffer[9] = (byte)(bias_derivation / 16);  //Bias Derivation to 7-4 Pchs 0xE Register ADS
            sendBuffer[10] = (byte)(bias_derivation % 16);  //Bias Derivation to 3-0 Nchs 0xD Register ADS
            sendBuffer[11] = (byte)(bias_derivation / 16);  //Bias Derivation to 7-4 Nchs 0xE Register ADS
            

            _serialPort.Open();
            //_serialPort.Write(Encoding.UTF8.GetString(sendBuffer, 0, sendBuffer.Length));
            _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
            try
            {
                if (debug_mode) Console.WriteLine("DLL: Waiting for device to send back acknowledge about write setting ...");
                while (settingMode_flag) ;
                _serialPort.Close();
                return true;
            }
            catch (Exception)
            {
                return false;
            };
            
        }

        public bool set(Settings mysetting)
        {
            if (debug_mode)
            {
                Console.WriteLine("DLL: Sampling rate= " + mysetting.sampling_rate);
                Console.WriteLine("DLL: Test Mode= " + mysetting.test_signal);
                Console.WriteLine("DLL: linked_ear Mode= " + mysetting.linked_ear);
                Console.WriteLine("DLL: Lead off= " + mysetting.leadoff_mode);
                Console.WriteLine("DLL: Gain= " + mysetting.gain);
                Console.WriteLine("DLL: Extera Gain= " + mysetting.exgain);
                Console.Write("DLL: Channels on/of (1 to 24): ");
                foreach (var channels in mysetting.channels_on)
                    Console.Write(" " + Convert.ToInt32(channels));
                Console.Write("\nDLL: Extra Channels on/of (1 to 3):");
                foreach (var channels in mysetting.exchannels_on)
                    Console.Write(" " + Convert.ToInt32(channels));
                Console.WriteLine("");

            }

            settingMode_flag = true;
       
            sendBuffer[3] = Convert.ToByte('s'); // for enter in setting mode                                    
            sendBuffer[4] = mysetting.test_signal ? Convert.ToByte('T') : Convert.ToByte('n');  // test signal enable/disable
            sendBuffer[5] = Convert.ToByte(48 + (mysetting.sampling_rate / 250));  // '1'=250, '2'=500, '4'=1000, '8'=2000
            sendBuffer[6] = mysetting.leadoff_mode ? Convert.ToByte('L') : Convert.ToByte(' '); // lead-off enable/disable
            sendBuffer[7] = Convert.ToByte('C'); // SRB1 connected  to all Nchs
            sendBuffer[8] = Convert.ToByte(mysetting.gain);
            sendBuffer[9] = Convert.ToByte(mysetting.exgain);
            sendBuffer[10] = mysetting.linked_ear ? Convert.ToByte('I') : Convert.ToByte('n');  // test signal enable/disable
            sendBuffer[11] = 0x00; //reserved
            sendBuffer[12] = 0x00; //reserved
            sendBuffer[13] = 0x00; //reserved
            sendBuffer[14] = 0x00; //reserved

            BitArray ch_arr = new BitArray(mysetting.channels_on);
            byte[] ch_data = new byte[80];
            ch_arr.CopyTo(ch_data, 0);
            
            sendBuffer[15] = ch_data[0];  // channels 1 to 8 on/off status
            sendBuffer[16] = ch_data[1];  // channels 9 to 16 on/off status
            sendBuffer[17] = ch_data[2];  // channels 17 to 24 on/off status

            BitArray Exch_arr = new BitArray(mysetting.exchannels_on);
            byte[] Exch_data = new byte[80];
            Exch_arr.CopyTo(Exch_data, 0);

            sendBuffer[18] = Exch_data[0];  // channels 1 to 8 on/off status
            sendBuffer[19] = 0x00; //reserved


            sendBuffer[20] = Convert.ToByte(mysetting.interaction[0]); // user interaction module settings
            sendBuffer[21] = Convert.ToByte(mysetting.interaction[1]); // user interaction module settings
            sendBuffer[22] = Convert.ToByte(mysetting.interaction[2]); // user interaction module settings
            sendBuffer[23] = Convert.ToByte(mysetting.interaction[3]); // user interaction module settings

            _serialPort.Open();
            _serialPort.Write(sendBuffer, 0, sendBuffer.Length);
            try
            {
                if (debug_mode) Console.WriteLine("DLL: Waiting for device to send back acknowledge about write setting ...");
                while (settingMode_flag) ;
                _serialPort.Close();
                return true;
            }
            catch (Exception)
            {
                return false;
            };

        }
        public bool connect() //string device_name
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
            StringComparer stringComparer = StringComparer.OrdinalIgnoreCase;
            IList com_names = new ArrayList();

            com_names = device_coms.FindPortByDesc("USB Serial Device");
            if (debug_mode) Console.WriteLine("DLL: Checking all COM ports ...");
            if (com_names.Count > 0)
            {
                foreach (string item in com_names)
                {
                    Console.WriteLine(item);
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
                        Console.WriteLine(e.Message);
                        Console.WriteLine("cant connect ot this Comport");
                        return false;
                    }
                    _serialPort.Write(Encoding.UTF8.GetString(sendBuffer, 0, sendBuffer.Length));
                    try { _serialPort.Close(); }
                    catch (Exception) { };
                    Thread.Sleep(1000);
                    while (ATR_flag != true)  //wating for device to reinitializing...
                    {
                        try
                        {
                            _serialPort.Open();
                            ATR_flag = true;
                        }
                        catch (Exception )
                        {
                            Thread.Sleep(1000);
                            if (debug_mode) Console.WriteLine("DLL: Waiting for device with "+ item + " to restart...");
                            //if (debug_mode) Console.WriteLine(e);
                            ATR_flag = false;
                        }
                    }
                    if (debug_mode) Console.WriteLine("DLL: Sending handshake setting to device");
                    sendBuffer[3] = Convert.ToByte('s');    //setting mode
                    _serialPort.Write(Encoding.UTF8.GetString(sendBuffer, 0, sendBuffer.Length));
                    if (debug_mode) Console.WriteLine("DLL: Waiting for device to respond...");

                    //ulong i = 0;
                    //while (answerToInit_flag == false && i < 2000000000) i++; //wait for device to respond and call DataReceivedHandler to enable AnswerToInitFlag
                    while (answerToInit_flag == false); //wait for device to respond and call DataReceivedHandler to enable AnswerToInitFlag
                    _serialPort.Close();
                    if (answerToInit_flag == false)
                    {
                        if (debug_mode) Console.WriteLine("DLL: No respond detected!");
                        return false;
                    }
                    else
                    {
                        if (debug_mode) Console.WriteLine("DLL: Device " + device_name + " is Connected on " + item);
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
        public object getInfo(string val="all")
        {
            if (dataGathering_flag) return false;
            if (debug_mode) Console.WriteLine("DLL: device_name(name): " + device_name);
            if (debug_mode) Console.WriteLine("DLL: firmware_version(version): " + firmware_version);
            if (debug_mode) Console.WriteLine("DLL: unic_ID(id): " + unic_ID);
            if (debug_mode) Console.WriteLine("DLL: flash_data(flash): " + flash_data);

            if (val == "name") return device_name;
            if (val == "version") return firmware_version;
            if (val == "id") return unic_ID;
            if (val == "flash") return flash_data;
            if (val == "all")
            {
                Dictionary<string, string> infoList = new Dictionary<string, string>();
                infoList.Add("name", device_name);
                infoList.Add("version", firmware_version);
                infoList.Add("id", unic_ID);
                infoList.Add("flash", flash_data);
                return infoList;
            }
            return false;
        }
        public object getStatus(string val="all")
        {
            if (debug_mode) Console.WriteLine("DLL: Gathering Mode(mode): " + dataGathering_flag.ToString());
            if (debug_mode) Console.WriteLine("DLL: sample_count(cnt): " + sample_count.ToString());
            if (debug_mode) Console.WriteLine("DLL: Buffer Length(buff_len): " + newData_count.ToString());

            if (val == "mode") return dataGathering_flag;
            if (val == "cnt") return sample_count;
            if (val == "buff_len") return newData_count;
            if (val == "all")
            {
                Dictionary<string, object> status = new Dictionary<string, object>();
                status.Add("mode", dataGathering_flag);
                status.Add("cnt", sample_count);
                status.Add("buff_len", newData_count);
                return status;
            }
            if (debug_mode) Console.WriteLine("DLL: Not a valid input for getStatus");
            return "nothing to respond";
        }
        public bool start()
        {
            if (device_name != "No Device Found")
            {
                if (!dataGathering_flag)
                {
                    receiveBuffer.Clear();
                    newData_count = 0;
                    dataGathering_flag = true;
                    if (debug_mode) Console.WriteLine("DLL: Starting data gathering ...");
                    sample_count = 0;
                    _serialPort.Open();
                    sendBuffer[3] = Convert.ToByte('B'); //start mode
                    _serialPort.Write(Encoding.UTF8.GetString(sendBuffer, 0, sendBuffer.Length));
                    return true;
                }
                if (debug_mode) Console.WriteLine("DLL: The device is already in data gathering mode!");
                return false;
            }
            if (debug_mode) Console.WriteLine("DLL: No device connected to start data gathering!");
            return false;
        }
        public int stop()
        {
            if (device_name != "No Device Found")
            {
                if (dataGathering_flag)
                {
                    while (sample_count < 10);
                    dataGathering_flag = false;
                    sendBuffer[3] = Convert.ToByte('E'); //stop mode
                    _serialPort.Write(Encoding.UTF8.GetString(sendBuffer, 0, sendBuffer.Length));
                    _serialPort.Close();
                    if (debug_mode) Console.WriteLine("DLL: Data gathering has been finished.");
                    return sample_count;
                }
                if (debug_mode) Console.WriteLine("DLL: The device was not in data gathering mode to stop it, please call 'start' methode first.");
                return -1;
            }
            if (debug_mode) Console.WriteLine("DLL: No device connected! Data gathering will remain unfinished :(");
            return -2;


        }
        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            if (!answerToInit_flag)
            {
                byte[] rec_data = new byte[32];
                _serialPort.Read(rec_data, 0, 32);
                if (Convert.ToByte(rec_data[31]) == 144) // Indicate I8Device ID
                {
                    Thread.Sleep(500);
                    if (Convert.ToByte(rec_data[27]) == 48) // Indicate Fascin8 device
                    {
                        device_name = "Fascin8";
                        firmware_version = Convert.ToByte(rec_data[24]).ToString();
                        unic_ID = (256 * rec_data[25] + rec_data[26]).ToString();
                        flash_data = rec_data[28].ToString() + rec_data[29].ToString() + rec_data[30].ToString();
                    }
                    else if (Convert.ToByte(rec_data[27]) == 47) // Indicate Ultim8 device
                    {
                        device_name = "Ultim8";
                        firmware_version = Convert.ToByte(rec_data[24]).ToString();
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
                if (Convert.ToByte(rec_data[31]) == 144) // Indicate I8Device ID
                {
                    Thread.Sleep(500);
                   // Console.WriteLine("num_of_huges: ");
                    //Console.WriteLine(rec_data[23].ToString());
                    if (Convert.ToByte(rec_data[27]) == 48) // Indicate Fascin8 device
                    {
                        if (debug_mode) Console.WriteLine("DLL: Fascin8 writing setting done successfully.");
                        if (debug_mode)
                        {
                            if (Convert.ToByte(rec_data[22]) == 0xFF) Console.WriteLine("Full Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0xF0) Console.WriteLine("P Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0x0F) Console.WriteLine("N Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0x00) Console.WriteLine("NO Derivation");
                            else Console.WriteLine("unknown Derivation");

                            // Print registers for all three ADS chips
                            for (int chip = 0; chip < 3; chip++)
                            {
                                Console.WriteLine($"\nDLL: ADS Chip {chip} Registers (see datasheet):");
                                Console.WriteLine($"     Chip Role: {(chip == 1 ? "Main IC" : "Secondary IC")}");
                                
                                int startIdx = chip * 24;  // Each chip has registers up to 0x17 (23)
                                for (int i = 0; i < 24; i++)
                                {
                                    int regIdx = startIdx + i;
                                    if (regIdx < rec_data.Length)
                                    {
                                        Console.WriteLine($"     {i.ToString("X2")}- {((byte)rec_data[regIdx]).ToString("X2")}");
                                    }
                                }
                            }
                        }
                    }
                    else if (Convert.ToByte(rec_data[27]) == 47) // Indicate Ultim8 device
                    {
                        if (debug_mode) Console.WriteLine("DLL: Ultim8 writing setiing done succesfully.");
                        if (debug_mode)
                        {
                            // var sb = new StringBuilder("new byte[] { ");
                            int c = 0;
                            Console.WriteLine("Chip Settings:");
                            foreach (var b in rec_data)
                            {
                                //sb.Append(b + ", ");
                                Console.WriteLine(c.ToString("X") + "- " + ((byte)b).ToString("X"));
                                c++; if (c > 23) break;
                            }
                            if (Convert.ToByte(rec_data[22]) == 0xFF) Console.WriteLine("Full Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0xF0) Console.WriteLine("P Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0x0F) Console.WriteLine("N Derivation");
                            else if (Convert.ToByte(rec_data[22]) == 0x00) Console.WriteLine("NO Derivation");
                            else Console.WriteLine("unknown Derivation");
                            // sb.Append("}");
                            // Console.WriteLine(sb.ToString());
                        }
                        
                    }
                    settingMode_flag = false;
                }
            }
            else
            {
                //if (!_serialPort.IsOpen)
                //{
                //   Console.WriteLine(_serialPort.IsOpen);
                //}
                try
                {
                    if (device_name == "Fascin8" && _serialPort.IsOpen && _serialPort.BytesToRead > 73)
                {
                    /*
                    byte[] last_data_recieve = new byte[74];
                    _serialPort.Read(last_data_recieve, 0, 74);
                    sample_count++;

                    double[] data_converted = new double[26];
                    data_converted[0] = sample_count;
                    data_converted[1] = last_data_recieve[24];  // key inputs
                    int i = 2;
                    for (int k = i; k < 8 + i; k++)
                    {
                        data_converted[k] = (65536 * last_data_recieve[3 * (k - i) + 0]) + (256 * last_data_recieve[3 * (k - i) + 1]) + last_data_recieve[3 * (k - i) + 2];
                        if (data_converted[k] > 8388607) data_converted[k] = data_converted[k] - 16777216;
                        data_converted[k] = data_converted[k] * 0.536;
                    }
                    i = 10;
                    for (int k = i; k < 16 + i; k++)
                    {
                        data_converted[k] = (65536 * last_data_recieve[(3*k) - 4]) + (256 * last_data_recieve[(3*k) - 3]) + last_data_recieve[(3*k) - 2];
                        if (data_converted[k] > 8388607) data_converted[k] = data_converted[k] - 16777216;
                        data_converted[k] = data_converted[k] * 0.536;
                    }
                    receiveBuffer.Add(data_converted);
                    newData_count ++;
                   // if (debug_mode) Console.WriteLine("DLL: Sample number " + sample_count + " has been received :)");
                   */
                   
                        byte[] last_data_recieve = new byte[80];
                        _serialPort.Read(last_data_recieve, 0, 80);
                    

                        double[] data_converted = new double[30];
                        data_converted[0] = sample_count;  // dll sample count
                        data_converted[1] = (65536 * last_data_recieve[0]) + (256 * last_data_recieve[1]) + last_data_recieve[2];  // firmware sample count
                        data_converted[2] = 0; //reserverd
                        data_converted[3] = last_data_recieve[3];  // Device ID
                        data_converted[4] = last_data_recieve[4];  // user input keys
                        data_converted[5] = 0; //reserverd
                        int i = 6, p = 8;  // i =  data_convewrted position,  p = input data position
                        for (int k = i; k < 24 + i; k++)
                        {
                            data_converted[k] = (65536 * last_data_recieve[3 * (k - i) + p + 0]) + (256 * last_data_recieve[3 * (k - i) + p + 1]) +
                                                                                                          last_data_recieve[3 * (k - i) + p + 2];  // adding 3 byte as one data
                            if (data_converted[k] > 8388607) data_converted[k] = data_converted[k] - 16777216;  //two's compliment
                            data_converted[k] = data_converted[k] * 0.536;  //convert to uV
                            //data_converted[k] = sample_count;  //convert to uV
                            //data_converted[k] = data_converted[k] * 0.16125;  //convert to uV
                        }
                        receiveBuffer.Add(data_converted);
                        sample_count++;
                        newData_count++;

                }
                
                else if (device_name == "Ultim8" && _serialPort.IsOpen && _serialPort.BytesToRead > 25)
                {
                    byte[] last_data_recieve = new byte[26];
                    _serialPort.Read(last_data_recieve, 0, 26);
                    if (last_data_recieve[25] == 47)
                    {

                        double[] data_converted = new double[14];
                        data_converted[0] = sample_count;  // dll sample count
                        data_converted[1] = sample_count; // firmware sample count
                        data_converted[2] = 0; //reserverd
                        data_converted[3] = 47;  // Device ID
                        data_converted[4] = last_data_recieve[24];  // user input keys
                        data_converted[5] = 0; //reserverd
                        int i = 6, p = 0;
                        for (int k = i; k < 8 + i; k++)
                        {
                            data_converted[k] = (65536 * last_data_recieve[3 * (k - i) + p + 0]) + (256 * last_data_recieve[3 * (k - i) + p + 1]) +
                                                                                                          last_data_recieve[3 * (k - i) + p + 2];
                            if (data_converted[k] > 8388607) data_converted[k] = data_converted[k] - 16777216;
                            data_converted[k] = data_converted[k] * 0.536;
                        }
                        receiveBuffer.Add(data_converted);
                        sample_count++;
                        newData_count++;
                        // if (debug_mode) Console.WriteLine("DLL: Sample number " + sample_count + " has been received :)");
                    }
                    else
                      //  if (debug_mode) Console.WriteLine("DLL: Somthing is wrong!");
                        if (debug_mode) Console.WriteLine("DLL: Somthing is wrong!");
                }
                }
                catch (Exception)
                {
                    if (debug_mode)
                        Console.WriteLine("Port is not open?");
                }
            }

        }
        public object getData(int val=0)
        {
            double[][] last_data_list_temp;
            if (dataGathering_flag)
            {
                if (val == 0)
                {
                    while (newData_count == 0) ;
                    newData_count = 0;
                    last_data_list_temp = receiveBuffer.ToArray();
                    if (debug_mode) Console.WriteLine("DLL: Sending all buffered samples. " + newData_count);
                    receiveBuffer.Clear();
                    return last_data_list_temp;
                }
                else if (newData_count < val)
                {
                    while (newData_count < val) ;
                    newData_count = 0;
                    last_data_list_temp = receiveBuffer.ToArray();
                    receiveBuffer.Clear();
                    if (debug_mode) Console.WriteLine("DLL: Sending all buffered samples with size of '" + val.ToString() + "' that you asked for.");
                    return last_data_list_temp;
                }
                else
                {
                    last_data_list_temp = receiveBuffer.GetRange((int)(newData_count - val), val).ToArray();
                    receiveBuffer.Clear();
                    newData_count = 0;
                    if (debug_mode) Console.WriteLine("DLL: Sending last " + val.ToString() + " sample(s).");
                    return last_data_list_temp;
                }
            }
            else if (newData_count > 0)
            {
                if (val == 0)
                {
                    last_data_list_temp = receiveBuffer.ToArray();
                    receiveBuffer.Clear();
                    newData_count = 0;
                    if (debug_mode) Console.WriteLine("DLL: Sending all buffered samples.");
                    return last_data_list_temp;
                }
                else if (newData_count >= val)
                {
                    last_data_list_temp = receiveBuffer.GetRange((int)(newData_count - val), (int)val).ToArray();
                    receiveBuffer.Clear();
                    newData_count = 0;
                    if (debug_mode) Console.WriteLine("DLL: Sending last " + val.ToString() + "sample(s).");
                    return last_data_list_temp;
                }
                else
                {
                    if (debug_mode) Console.WriteLine("DLL: Device is not in gathering mode and receive buffer is less than number of samples you asked for.");
                    return false;
                }
            }
            else
            {
                if (debug_mode) Console.WriteLine("DLL: Device is not in gathering mode and receive buffer is empty too.");
                return false;
            }
        }
    }

}
