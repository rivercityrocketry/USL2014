using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace ArduinoSerialInterface
{
    class Startup
    {
        public static void Main(string[] args)
        {
            Debug.Print("Running!");

            ArduinoController controller = new ArduinoController();
            controller.OpenArduinoConnection();
            controller.GetDataFromArduinoBoard();
        }
    }
}
