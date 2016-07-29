using System;
using System.Linq;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Media;
using System.IO.Ports;
using System.Management;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace UploaderUI {
    class ItemCOMPort {
        public string COM { get; protected set; }
        public string Desc { get; protected set; }
        public ItemCOMPort(string COM, string desc) {
            this.COM = COM;
            this.Desc = desc;
        }
        public override string ToString() {
            return string.Format("{0}: {1}", COM, Desc);
        }
    }

    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {
        Communication communication;

        public MainWindow() {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e) {
            // загружаем список COM портов
            using (var searcher = new ManagementObjectSearcher("SELECT * FROM WIN32_SerialPort")) {
                string[] portnames = SerialPort.GetPortNames();
                var ports = searcher.Get().Cast<ManagementBaseObject>().ToList();
                var tList = (from n in portnames
                             join p in ports on n equals p["DeviceID"].ToString()
                             select new ItemCOMPort(n, (string)p["Caption"])).ToList();
                //n + " - " + p["Caption"]).ToList();

                cbCOMPorts.Items.Clear();
                foreach (var s in tList)
                    cbCOMPorts.Items.Add(s);
            }
            cbCOMPorts.SelectedIndex = cbCOMPorts.Items.Count - 1;


            communication = new Communication(this);
        }

        
        internal void Log(string message, Level level, params object[] args) {
            this.Dispatcher.Invoke(() => {
                BrushConverter bc = new BrushConverter();
                TextRange tr = new TextRange(rtbLog.Document.ContentStart, rtbLog.Document.ContentStart);
                tr.Text = string.Format("[{0}] - {1}\r",
                                        DateTime.Now,
                                        string.Format(message, args)
                                        );
                string color = "";
                switch (level) {
                    case Level.Error:
                        color = "red";
                        break;
                    case Level.Warning:
                        color = "yellow";
                        break;
                    case Level.Info:
                    default:
                        color = "black";
                        break;
                }
                try {
                    tr.ApplyPropertyValue(TextElement.ForegroundProperty,
                        bc.ConvertFromString(color));
                } catch (FormatException) { }
            });
        }

        Communication.FlashDesc flash;

        private void buttonOpen_Click(object sender, RoutedEventArgs e) {
            buttonOpen.IsEnabled = false;
            string port = (cbCOMPorts.SelectedItem as ItemCOMPort).COM;

            Task.Run(() => {
                Log("Trying open COM port...", Level.Info);
                communication.Open(port);
                Thread.Sleep(100);

                communication.Write("$V#");
                Thread.Sleep(100);

                communication.SetI2CAddress(0x94);
                Thread.Sleep(100);

                Log("Entering ISP mode...", Level.Info);
                communication.WriteReg(0x6f, 0x80);
                Thread.Sleep(100);

                byte b = communication.ReadReg(0x6f);
                if ((b & 0x80) == 0)
                    Log("Can`t enter in ISP mode", Level.Error);
                Thread.Sleep(100);

                uint jedec_id = communication.SPICommonCommand(CommondCommandType.E_CC_READ, 0x9f, 3, 0, 0);
                flash = Communication.FindFlashDesc(jedec_id);
                Log("JEDEC ID: {0:X}, Name: {1}, Manufacter: {2}, Size: {3}kb", Level.Info,
                    jedec_id, flash.device_name, Communication.JEDECManufacturer(jedec_id), flash.size_kb);
                
            });
        }

        private void btnCmd_Click(object sender, RoutedEventArgs e) {
            communication.Write(tbCmd.Text);
        }

        private void buttonClose_Click(object sender, RoutedEventArgs e) {
            communication.Close();
        }

        private void button_Click(object sender, RoutedEventArgs e) {
            communication.SaveFlash(@"c:\soft\Devices\ODROID_C2\Dev\MyRTD2662\flash.bin",
                                    flash.size_kb * 1024 );
        }

        private void button1_Click(object sender, RoutedEventArgs e) {
            communication.ProgramFlash(@"c:\soft\Devices\ODROID_C2\LCD\touch as fw uploader\PCB800099_firmware_without_blue_screen\PCB800099-LVDS1280X800-8bit.BIN",
                                       flash.size_kb * 1024 );
        }
    }
}
