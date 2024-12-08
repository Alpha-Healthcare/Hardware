using System;
using System.Collections;
using System.Management;

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
                    // Silently continue if there's an error with this port
                }
            }
            return suspect_coms;
        }

        private static ManagementObject[] FindPorts()
        {
            try
            {
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("root\\CIMV2", "SELECT * FROM Win32_PnPEntity");
                var objects = new System.Collections.Generic.List<ManagementObject>();

                foreach (ManagementObject obj in searcher.Get())
                {
                    objects.Add(obj);
                }
                return objects.ToArray();
            }
            catch (Exception)
            {
                return new ManagementObject[] { };
            }
        }

        private static string ParseCOMName(ManagementObject obj)
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
} 