namespace I8Devices
{
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
                if (value <= 250) _sampling_rate = 250;
                else if ((value > 250) && (value <= 500)) _sampling_rate = 500;
                else if ((value > 500) && (value <= 1000)) _sampling_rate = 1000;
                else if ((value > 1000)) _sampling_rate = 2000;
            }
        }

        private int _gain = 24;
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

        private const int _channels_count = 21;
        public bool[] channels_on = new bool[_channels_count];

        public Settings()
        {
            test_signal = false;
            linked_ear = false;
            leadoff_mode = false;
            sampling_rate = 2000;
            gain = 24;
            for (int i = 0; i < _channels_count; i++) 
                channels_on[i] = true;
        }
    }
} 