using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using HID_SIMPLE.HID;

namespace HIDApp
{
    public partial class FormMain : Form
    {
        public Hid Device = new Hid();
        public FormMain()
        {
            InitializeComponent();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            List<string> DeviceList = new List<string>();
            //获取设备列表，将所有设备列举到界面上
            Hid.GetHidDeviceList(ref DeviceList);
            for (int i = 0; i < DeviceList.Count(); ++i) 
            {
                string ele = DeviceList[i];
                comboBoxDeviceList.Items.Add(ele);
            }
            comboBoxDeviceList.SelectedIndex = 0;
        }

        private void button_open_device_Click(object sender, EventArgs e)
        {
            string deviceString = comboBoxDeviceList.Text;
            if(Device.OpenDevice(deviceString) !=  0)
            {
                textBox_Info.AppendText("设备打开失败！\r\n");
            }
            else
            {
                textBox_Info.AppendText("设备打开成功！\r\n");
            }
        }

        private void button_led1_Click(object sender, EventArgs e)
        {
            //button_led1.Image = HIDApp.Properties.Resources.led_on;
        }

        private void button_led2_Click(object sender, EventArgs e)
        {
            //
        }

        private void button_led3_Click(object sender, EventArgs e)
        {
            //
        }

        private void button_led4_Click(object sender, EventArgs e)
        {
            //
        }
    }
}
