using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Timers;
using System.IO;
using System.ServiceModel;

namespace ArduinoSerialInterface
{
    public partial class frmStartup : Form
    {
        private ArduinoController controller;

        private System.Timers.Timer timer;

        public List<string> dataEntries = new List<string>();

        private const string OUT_FILE_PATH = "C:\\Users\\Daniel\\Desktop\\USLI\\2013-2014\\codeRepository\\USL2014\\tiltometer\\Calibration Tool\\Calibration Tool\\log.txt";

        private ISimulationUpdater simUpdater;

        public frmStartup()
        {
            InitializeComponent();
            StartController();

            // TODO I can really get rid of all the HTTP functionality -
            //      I won't be using it.
            ChannelFactory<ISimulationUpdater> httpFactory =
                new ChannelFactory<ISimulationUpdater>(
                    new BasicHttpBinding(),
                    new EndpointAddress("http://localhost:8000/Reverse"));

            ChannelFactory<ISimulationUpdater> pipeFactory =
                new ChannelFactory<ISimulationUpdater>(
                    new NetNamedPipeBinding(),
                    new EndpointAddress("net.pipe://localhost/PipeReverse"));

            this.simUpdater = pipeFactory.CreateChannel();

            timer = new System.Timers.Timer(1);
            timer.Enabled = true;
            timer.Elapsed += new ElapsedEventHandler(ProcessData);
            timer.Start();

            this.ShowDialog();
        }

        public static void Main(string[] args)
        {
            frmStartup startup = new frmStartup();
        }

        private void StartController() {
            this.controller = new ArduinoController();
            controller.OpenArduinoConnection(this);
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            this.controller.CloseArduinoConnection();
            this.timer.Stop();
            this.Close();
            WriteDataToFile();
            return;
        }

        private void ProcessData(object sender, ElapsedEventArgs e)
        {
            controller.GetDataFromArduinoBoard();
        }

        private void WriteDataToFile()
        {
            string text = "";
            foreach (string dataEntry in dataEntries)
            {
                text += dataEntry;
                text += Environment.NewLine;
//                this.simUpdater.UpdateSim(0.0f, 0.0f, 0.0f);
            }

            File.AppendAllText(OUT_FILE_PATH, text);
        }
    }
}
