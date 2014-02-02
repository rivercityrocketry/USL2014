using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;

namespace ArduinoSerialInterface
{
    public class ArduinoController
    {
        /// <summary>
        /// Interface for the Serial Port at which an Arduino Board
        /// is connected.
        /// </summary>
        SerialPort arduinoBoard = new SerialPort();

        public List<string> DataItems = new List<string>();

        public event EventHandler NewDataReceived;

        private const char ENTRY_SEPARATOR = '$';

        private frmStartup MainForm;

        /// <summary>
        /// Closes the connection to an Arduino Board.
        /// </summary>
        public void CloseArduinoConnection()
        {
            arduinoBoard.Close();
        }
        /// <summary>
        /// Opens the connection to an Arduino board
        /// </summary>
        public void OpenArduinoConnection(frmStartup mainForm)
        {
            this.MainForm = mainForm;

            if (!arduinoBoard.IsOpen)
            {
                arduinoBoard.PortName = "COM10";
                arduinoBoard.Open();
                arduinoBoard.DataReceived += arduinoBoard_DataReceived;
                arduinoBoard.RtsEnable = true;
            }
            else
            {
                throw new InvalidOperationException("The Serial Port is already open!");
            }
        }
        /// <summary>
        /// Sends the command to the Arduino board which triggers the board
        /// to send the data it has internally stored.
        /// </summary>
        public void GetDataFromArduinoBoard()
        {
            if (arduinoBoard.IsOpen)
            {
                // Send command saying to read data.
                arduinoBoard.Write("1#");
            }
            else
            {
                throw new InvalidOperationException("Serial port is closed!");
            }
        }

        /// <summary>
        /// Reads data from the arduinoBoard serial port
        /// </summary>
        void arduinoBoard_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                if (arduinoBoard.IsOpen)
                {
                    string data = arduinoBoard.ReadTo(Environment.NewLine); //Read until the end of message code
                    string[] entries = data.Split(ENTRY_SEPARATOR);

                    if (NewDataReceived != null)  //If there is someone waiting for this event to be fired
                    {
                        NewDataReceived(this, new EventArgs()); //Fire the event, indicating that new WeatherData was added to the list.
                    }

                    foreach (string entry in entries)
                    {
                        MainForm.dataEntries.Add(entry);
                    }
                }
            }
            catch (Exception ex)
            {
            }
        }
    }
}

