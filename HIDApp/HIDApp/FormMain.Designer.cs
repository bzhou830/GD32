namespace HIDApp
{
    partial class FormMain
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.button_open_device = new System.Windows.Forms.Button();
            this.comboBoxDeviceList = new System.Windows.Forms.ComboBox();
            this.textBox_Info = new System.Windows.Forms.TextBox();
            this.button_led1 = new System.Windows.Forms.Button();
            this.button_led2 = new System.Windows.Forms.Button();
            this.button_led3 = new System.Windows.Forms.Button();
            this.button_led4 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // button_open_device
            // 
            this.button_open_device.Location = new System.Drawing.Point(514, 12);
            this.button_open_device.Name = "button_open_device";
            this.button_open_device.Size = new System.Drawing.Size(75, 23);
            this.button_open_device.TabIndex = 0;
            this.button_open_device.Text = "打开设备";
            this.button_open_device.UseVisualStyleBackColor = true;
            this.button_open_device.Click += new System.EventHandler(this.button_open_device_Click);
            // 
            // comboBoxDeviceList
            // 
            this.comboBoxDeviceList.FormattingEnabled = true;
            this.comboBoxDeviceList.Location = new System.Drawing.Point(12, 12);
            this.comboBoxDeviceList.Name = "comboBoxDeviceList";
            this.comboBoxDeviceList.Size = new System.Drawing.Size(496, 20);
            this.comboBoxDeviceList.TabIndex = 1;
            // 
            // textBox_Info
            // 
            this.textBox_Info.Location = new System.Drawing.Point(12, 228);
            this.textBox_Info.Multiline = true;
            this.textBox_Info.Name = "textBox_Info";
            this.textBox_Info.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_Info.Size = new System.Drawing.Size(577, 246);
            this.textBox_Info.TabIndex = 2;
            // 
            // button_led1
            // 
            this.button_led1.Image = global::HIDApp.Properties.Resources.led_off;
            this.button_led1.Location = new System.Drawing.Point(49, 60);
            this.button_led1.Name = "button_led1";
            this.button_led1.Size = new System.Drawing.Size(44, 43);
            this.button_led1.TabIndex = 3;
            this.button_led1.UseVisualStyleBackColor = true;
            this.button_led1.Click += new System.EventHandler(this.button_led1_Click);
            // 
            // button_led2
            // 
            this.button_led2.Image = global::HIDApp.Properties.Resources.led_off;
            this.button_led2.Location = new System.Drawing.Point(118, 60);
            this.button_led2.Name = "button_led2";
            this.button_led2.Size = new System.Drawing.Size(44, 43);
            this.button_led2.TabIndex = 3;
            this.button_led2.UseVisualStyleBackColor = true;
            this.button_led2.Click += new System.EventHandler(this.button_led2_Click);
            // 
            // button_led3
            // 
            this.button_led3.Image = global::HIDApp.Properties.Resources.led_off;
            this.button_led3.Location = new System.Drawing.Point(183, 60);
            this.button_led3.Name = "button_led3";
            this.button_led3.Size = new System.Drawing.Size(44, 43);
            this.button_led3.TabIndex = 3;
            this.button_led3.UseVisualStyleBackColor = true;
            this.button_led3.Click += new System.EventHandler(this.button_led3_Click);
            // 
            // button_led4
            // 
            this.button_led4.Image = global::HIDApp.Properties.Resources.led_off;
            this.button_led4.Location = new System.Drawing.Point(249, 60);
            this.button_led4.Name = "button_led4";
            this.button_led4.Size = new System.Drawing.Size(44, 43);
            this.button_led4.TabIndex = 3;
            this.button_led4.UseVisualStyleBackColor = true;
            this.button_led4.Click += new System.EventHandler(this.button_led4_Click);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(601, 486);
            this.Controls.Add(this.button_led4);
            this.Controls.Add(this.button_led3);
            this.Controls.Add(this.button_led2);
            this.Controls.Add(this.button_led1);
            this.Controls.Add(this.textBox_Info);
            this.Controls.Add(this.comboBoxDeviceList);
            this.Controls.Add(this.button_open_device);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.Text = "HIDApp";
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_open_device;
        private System.Windows.Forms.ComboBox comboBoxDeviceList;
        private System.Windows.Forms.TextBox textBox_Info;
        private System.Windows.Forms.Button button_led1;
        private System.Windows.Forms.Button button_led2;
        private System.Windows.Forms.Button button_led3;
        private System.Windows.Forms.Button button_led4;
    }
}

